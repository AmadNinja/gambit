//
// FILE: eliap.cc -- Extensive Form Liapunov module
//
// $Id$
//

#include "eliap.h"

#include "gfunc.h"
#include "gmatrix.h"

EFLiapParams::EFLiapParams(gStatus &s)
  : trace(0), maxits1(100), maxitsN(20), tol1(2.0e-10), tolN(1.0e-10),
    tracefile(0), status(s)
{ }



class EFLiapFunc : public gFunction<double>  {
  private:
    long _nevals;
    const Efg<double> &_efg;
    BehavProfile<double> _p;
    gDPVector<double> _cpay;

    double Value(const gVector<double> &x);

  public:
    EFLiapFunc(const Efg<double> &, const BehavProfile<double> &);
    virtual ~EFLiapFunc();
    
    long NumEvals(void) const  { return _nevals; }
};


EFLiapFunc::EFLiapFunc(const Efg<double> &E,
		       const BehavProfile<double> &start)
  : _nevals(0L), _efg(E), _p(start), _cpay(E.Dimensionality())
{ }

EFLiapFunc::~EFLiapFunc()
{ }


double EFLiapFunc::Value(const gVector<double> &v)
{
  static const double BIG1 = 10000.0;
  static const double BIG2 = 100.0;

  _nevals++;


  ((gVector<double> &) _p).operator=(v);
  BehavProfile<double> tmp(_p);
  double x, result = 0.0, avg, sum;

      // Ted -- only reason for this is because you 
      // got rid of CondPayoff ( . , . )
  gPVector<double> probs(_efg.Dimensionality().Lengths());  
  tmp.CondPayoff(_cpay, probs);

  for (int i = 1; i <= _efg.NumPlayers(); i++) {
    EFPlayer *player = _efg.PlayerList()[i];
    for (int j = 1; j <= player->NumInfosets(); j++) {
      avg = sum = 0.0;
      Infoset *s = player->InfosetList()[j];
      int k;
      for (k = 1; k <= s->NumActions(); k++) {
	x = _p(i, j, k); 
	avg += x * _cpay(i, j, k);
	sum += x;
	if (x > 0.0)  x = 0.0;
	result += BIG1 * x * x;         // add penalty for neg probabilities
      }
      for (k = 1; k <= s->NumActions(); k++) {
	x = _cpay(i, j, k) - avg;
	if (x < 0.0) x = 0.0;
	result += x * x;          // add penalty if not best response
      }
      x = sum - 1.0;
      result += BIG2 * x * x;       // add penalty for sum not equal to 1
    }
  }

  return result;
}


extern bool Powell(gVector<double> &p,
		   gMatrix<double> &xi,
		   gFunction<double> &func,
		   double &fret, int &iter,
		   int maxits1, double tol1, int maxitsN, double tolN,
		   gOutput &tracefile);


bool Liap(const Efg<double> &E, EFLiapParams &params,
	  const BehavProfile<double> &start,
	  gList<BehavProfile<double> > &solutions,
	  long &nevals, long &niters)
{
  EFLiapFunc F(E, start);

  BehavProfile<double> p(start);
  
  gMatrix<double> xi(p.Length(), p.Length());
  xi.MakeIdent();

  double value;
  int iter;
  bool found;

  if (found = Powell(p, xi, F, value, iter,
		     params.maxits1, params.tol1, params.maxitsN, params.tolN,
		     (params.tracefile) ? *params.tracefile : gnull))
    solutions.Append(p);

  nevals = F.NumEvals();
  niters = 0L;

  return found;
}


