//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Dialog for viewing and editing properties of a node
//

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif  // WX_PRECOMP
#include "game/efg.h"
#include "dleditnode.h"

//======================================================================
//                      class dialogEditNode
//======================================================================

dialogEditNode::dialogEditNode(wxWindow *p_parent, Node *p_node)
  : wxDialog(p_parent, -1, "Node properties"), m_node(p_node)
{
  SetAutoLayout(true);

  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *labelSizer = new wxBoxSizer(wxHORIZONTAL);
  labelSizer->Add(new wxStaticText(this, wxID_STATIC, "Node label"),
		  0, wxALL | wxCENTER, 5);
  m_nodeName = new wxTextCtrl(this, -1, (const char *) m_node->GetName());
  labelSizer->Add(m_nodeName, 1, wxALL | wxCENTER | wxEXPAND, 5);
  topSizer->Add(labelSizer, 0, wxALL | wxEXPAND, 5);

  wxBoxSizer *outcomeSizer = new wxBoxSizer(wxHORIZONTAL);
  outcomeSizer->Add(new wxStaticText(this, wxID_STATIC, "Outcome"),
		    0, wxALL | wxCENTER, 5);
  m_outcome = new wxChoice(this, -1);
  m_outcome->Append("(null)");
  m_outcome->SetSelection(0);
  const FullEfg &efg = *p_node->Game();
  for (int outc = 1; outc <= efg.NumOutcomes(); outc++) {
    Efg::Outcome outcome = efg.GetOutcome(outc);
    gText item = ToText(outc) + ": " + efg.GetOutcomeName(outcome);
    if (item == "") {
      item = "Outcome" + ToText(outc);
    }

    item += (" (" + ToText(efg.Payoff(outcome, efg.Players()[1])) + ", " +
	     ToText(efg.Payoff(outcome, efg.Players()[2])));
    if (efg.NumPlayers() > 2) {
      item += ", " + ToText(efg.Payoff(outcome, efg.Players()[3]));
      if (efg.NumPlayers() > 3) {
	item += ",...)";
      }
      else {
	item += ")";
      }
    }
    else {
      item += ")";
    }

    m_outcome->Append((const char *) item);
    if (efg.GetOutcome(m_node) == outcome) {
      m_outcome->SetSelection(outc);
    }
  }
  outcomeSizer->Add(m_outcome, 1, wxALL | wxEXPAND, 5);
  topSizer->Add(outcomeSizer, 0, wxALL | wxEXPAND, 5);

  wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  wxButton *okButton = new wxButton(this, wxID_OK, "OK");
  okButton->SetDefault();
  buttonSizer->Add(okButton, 0, wxALL, 5);
  buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0, wxALL, 5);
  buttonSizer->Add(new wxButton(this, wxID_HELP, "Help"), 0, wxALL, 5);
  topSizer->Add(buttonSizer, 0, wxALL | wxCENTER, 5);

  SetSizer(topSizer);
  topSizer->Fit(this);
  topSizer->SetSizeHints(this);

  Layout();
}
