/** \file
	\brief Contains the FindSequenceDialog class members
*/
#include "FindSequenceDialog.h"

BEGIN_EVENT_TABLE(FindSequenceDialog, wxDialog )
    EVT_BUTTON(SH_SEARCH,FindSequenceDialog::OnSearch)
    EVT_BUTTON(SH_CANCEL,FindSequenceDialog::OnCancel)
    EVT_BUTTON(FD_ADD_HIGHLIGHTS,FindSequenceDialog::OnAddHighlights)
    EVT_BUTTON(FD_SET_HIGHLIGHT_COLOR,FindSequenceDialog::OnSetHighlightColor)
    EVT_BUTTON(FD_RESET_HIGHLIGHTS,FindSequenceDialog::OnResetHighlights)
    EVT_CHECKBOX(SH_CB_SEQUENCE,FindSequenceDialog::OnTextChange)
    EVT_CHECKBOX(SH_CB_ITEMS,FindSequenceDialog::OnTextChange)
    EVT_CHECKBOX(SH_CB_ENZYMES,FindSequenceDialog::OnTextChange)
    EVT_CHECKBOX(SH_CB_TRANSLATION,FindSequenceDialog::OnTextChange)
    EVT_LISTBOX(SH_LB,FindSequenceDialog::OnLB)
    EVT_LISTBOX_DCLICK(SH_LB,FindSequenceDialog::OnLBdclick)
    EVT_TEXT(SH_TEXT,FindSequenceDialog::OnTextChange)
    EVT_CHAR_HOOK(FindSequenceDialog::OnCharHook)
END_EVENT_TABLE()


#define FIND_MAX 1000


FindSequenceDialog::FindSequenceDialog ( wxWindow *parent, const wxString& title )
	: wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 410 , 400 ) )
	{ 
    myapp()->frame->push_help ( _T("GENtle:Find") ) ;
	highlight = *wxBLUE ;
	wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
	wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
	wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
	p = 0 ;
	c = (ChildBase*) parent ;
	allowed_chars = _T("AGCT") ; // For DNA search; currently ignored
	
	t = new wxTextCtrl ( this , SH_TEXT , _T("") ) ;
	status = new wxTextCtrl ( this , -1 , txt("t_find_hint") , 
		wxDefaultPosition , wxDefaultSize , wxTE_READONLY|wxSTATIC_BORDER ) ;
	status->SetBackgroundColour ( GetBackgroundColour() ) ;
	
	find_button = new wxButton ( this , SH_SEARCH , txt("b_find") ) ;	
	h0->Add ( find_button , 1 , wxALL|wxEXPAND , 2 ) ;
	h0->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , wxALL|wxEXPAND , 2 ) ;
	h0->Add ( new wxButton ( this , SH_CANCEL , txt("b_cancel") ) , 1 , wxALL|wxEXPAND , 2 ) ;

    // Highlight stuff
    highlight_display = new wxStaticText ( this , -1 , _T("      ") ) ;
    do_highlight = new wxButton ( this , FD_ADD_HIGHLIGHTS , txt("b_find_highlight") ) ;
	h1->Add ( do_highlight , 0 , wxALL|wxEXPAND , 2 ) ;
	h1->Add ( highlight_display , 0 , wxLEFT|wxEXPAND , 5 ) ;
	h1->Add ( new wxButton ( this , FD_SET_HIGHLIGHT_COLOR , txt("b_find_highlight_color") ) , 0 , wxALL|wxEXPAND , 2 ) ;
	h1->Add ( new wxButton ( this , FD_RESET_HIGHLIGHTS , txt("b_find_remove_highlights") ) , 0 , wxALL|wxEXPAND , 2 ) ;
	highlight_display->SetBackgroundColour ( highlight ) ;

    // Options
    cb_sequence = cb_items = cb_enzymes = cb_translation = NULL ;
    cb_sequence = new wxCheckBox ( this , SH_CB_SEQUENCE , txt("t_sh_cb_sequence") ) ;
    h2->Add ( cb_sequence , 0 , wxALL|wxEXPAND , 2 ) ;
    cb_sequence->SetValue ( true ) ;
    if ( c->def != _T("ABIviewer") )
       {
       cb_items = new wxCheckBox ( this , SH_CB_ITEMS , txt("t_sh_cb_items") ) ;
       h2->Add ( cb_items , 0 , wxALL|wxEXPAND , 2 ) ;
       cb_items->SetValue ( true ) ;
       }
    if ( c->def == _T("dna") || c->def == _T("PrimerDesign") )
       {
       cb_enzymes = new wxCheckBox ( this , SH_CB_ENZYMES , txt("t_sh_cb_enzymes") ) ;
       cb_translation = new wxCheckBox ( this , SH_CB_TRANSLATION , txt("t_sh_cb_translation") ) ;
       h2->Add ( cb_enzymes , 0 , wxALL|wxEXPAND , 2 ) ;
       h2->Add ( cb_translation , 0 , wxALL|wxEXPAND , 2 ) ;
       cb_enzymes->SetValue ( true ) ;
       cb_translation->SetValue ( true ) ;
       }
    

	lb = new wxListBox ( this , SH_LB ) ;
	
	v0->Add ( t , 0 , wxALL|wxEXPAND , 2 ) ;
	v0->Add ( h0 , 0 , wxALL|wxEXPAND , 2 ) ;
	v0->Add ( h1 , 0 , wxALL|wxEXPAND , 2 ) ;
	v0->Add ( h2 , 0 , wxALL|wxEXPAND , 2 ) ;
	v0->Add ( status , 0 , wxALL|wxEXPAND , 2 ) ;
	v0->Add ( lb , 1 , wxALL|wxEXPAND , 2 ) ;
	
	do_highlight->Disable() ;
	find_button->SetDefault () ;
	t->SetFocus () ;
	
	SetSizer ( v0 ) ;
	v0->Layout () ;
	Center () ;
	
	// Move the dialog so it won't hide the sequence
	int x , y , w , h , x1 , y1 ;
	GetClientSize ( &w , &h ) ;
	GetPosition ( &x , &y ) ;
	myapp()->frame->GetPosition ( &x1, &y1 ) ;
	x -= w ;
	y -= h/2 ;
	x -= 30 ;
	if ( x < x1+5 ) x = x1+5 ;
	if ( y < y1+5 ) y = y1+5 ;
	Move ( x , y ) ;	
	}

FindSequenceDialog::~FindSequenceDialog ()
	{
    myapp()->frame->pop_help () ;
	}
    
wxString FindSequenceDialog::getQuery ()
	{
	wxString s = t->GetValue().Upper() ;
	wxString t ;
	for ( int a = 0 ; a < s.length() ; a++ )
		{
		if ( ( s.GetChar(a) >= 'A' && s.GetChar(a) <= 'Z' ) ||
			  ( s.GetChar(a) >= '0' && s.GetChar(a) <= '9' ) || 
			  s.GetChar(a) == '?' || s.GetChar(a) == '*' )
		    t += s.GetChar ( a ) ;
		}
	return t ;
	}

void FindSequenceDialog::OnLB ( wxCommandEvent &ev )
    {
    doAction ( false ) ;
    }
    
void FindSequenceDialog::OnLBdclick ( wxCommandEvent &ev )
    {
    doAction ( true ) ;
    wxDialog::OnOK ( ev ) ;
    }    
	
void FindSequenceDialog::getFromTo ( wxString s , long &from , long &to , int idx )
	{
	from = to = -1 ;
    wxString type = s.BeforeFirst ( ':' ) ;
    wxString data = s.AfterFirst ( ':' ) ;
	if ( type == txt("sequence") )
		{
        data.BeforeFirst('-').ToLong ( &from ) ;
        data.AfterFirst('-').ToLong ( &to ) ;        
		}
    else if ( type == txt("amino_acid") )
        {
        data = data.AfterLast ( '(' ) ;
        data.BeforeFirst('-').ToLong ( &from ) ;
        data.AfterFirst('-').ToLong ( &to ) ;
		}
    else if ( type == txt("t_vec_item") )
        {
        from = c->vec->items[vi[idx]].from ;
        to = c->vec->items[vi[idx]].to ;
		}
    else if ( type == txt("m_restriction") )
        {
        int a = vi[idx] ;
        from = c->vec->rc[a].getPos() - c->vec->rc[a].e->getCut() + 1 ;
        to = c->vec->rc[a].getPos() - c->vec->rc[a].e->getCut() + c->vec->rc[a].e->getSequence().length() + 1 ;
		}
	}

SequenceCanvas *FindSequenceDialog::getMarkSequence ( wxString &mark )
	{
	SequenceCanvas *canvas = NULL ;
    if ( c->def == _T("dna") )
        {
        mark = _T("DNA") ;
        canvas = ( (MyChild*) c ) -> cSequence ;
        }    
    else if ( c->def == _T("PrimerDesign") )
        {
        mark = _T("PRIMER_UP") ;
        canvas = ( (TPrimerDesign*) c ) -> sc ;
        }    
    else if ( c->def == _T("AminoAcids") )
        {
        mark = _T("AA") ;
        canvas = ( (TAminoAcids*) c ) -> sc ;
        }    
    else if ( c->def == _T("ABIviewer") )
        {
        mark = _T("ABI") ;
        canvas = ( (TABIviewer*) c ) -> sc ;
        }    
	return canvas ;
	}
    
void FindSequenceDialog::doAction ( bool doubleclick )
    {
    int idx = lb->GetSelection () ;
    wxString s = lb->GetStringSelection () ;
    wxString type = s.BeforeFirst ( ':' ) ;
    wxString data = s.AfterFirst ( ':' ) ;
    
    wxString mark ;
    SequenceCanvas *canvas = getMarkSequence ( mark ) ;        
    if ( !canvas ) return ; // Just a safety
    
	long from , to ;
    if ( type == txt("sequence") )
        {
		getFromTo ( s , from , to ) ;
        canvas->mark ( mark , from , to ) ;
        canvas->ensureVisible ( canvas->getBatchMark() ) ;
        }
    else if ( type == txt("amino_acid") )
        {
		getFromTo ( s , from , to ) ;
        canvas->mark ( mark , from , to ) ;
        canvas->ensureVisible ( canvas->getBatchMark() ) ;
        }    
    else if ( type == txt("t_vec_item") )
        {
		getFromTo ( s , from , to , idx ) ;
        canvas->mark ( mark , from , to ) ;
        canvas->ensureVisible ( canvas->getBatchMark() ) ;
        if ( doubleclick )
           {
           Hide () ;
           if ( c->def == _T("dna") ) ( (MyChild*) c ) -> cPlasmid -> invokeVectorEditor ( _T("item") , vi[idx] ) ;
           if ( c->def == _T("AminoAcids") ) ( (TAminoAcids*) c ) -> invokeVectorEditor ( _T("item") , vi[idx] ) ;
           }    
        }    
    else if ( type == txt("m_restriction") )
        {
        int a = vi[idx] ;
		getFromTo ( s , from , to , idx ) ;
        canvas->mark ( mark , from , to ) ;
        canvas->ensureVisible ( canvas->getBatchMark() ) ;
        if ( doubleclick && c->def == _T("dna") ) // Only is DNA
           {
           Hide () ;
           ( (MyChild*) c ) -> cPlasmid -> invokeVectorEditor ( _T("enzyme") , a ) ;
           }
        }    
    }    
    
void FindSequenceDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else if ( k == WXK_F1 ) myapp()->frame->OnHelp ( ev ) ;
    else event.Skip() ;
    }

bool FindSequenceDialog::doesMatch ( char a , char b )
    {
    if ( a == b ) return true ;
    if ( a == '?' || b == '?' ) return true ;
    return false ;
    }
    
int FindSequenceDialog::subsearch ( const wxString &s , const wxString &sub , int start )
    {
    int a , b ;
    if ( s.length() < sub.length() ) return -1 ;
    for ( a = start ; a < s.length() - sub.length() + 1 ; a++ )
        {
        for ( b = 0 ; b < sub.length() ; b++ )
           {
           if ( sub[b] == '*' )
              {
              if ( subsearch ( s , sub.substr ( b+1 ) , a ) == -1 ) break ;
              b = sub.length()-1 ;
              }
           else
              {
              if ( !doesMatch ( sub[b] , s[a+b] ) ) break ;
              last = a+b ;
              }
           }
        if ( b == sub.length() ) return a ;
        }
    return -1 ;
    }

void FindSequenceDialog::OnSearch ( wxCommandEvent &ev )
	{
	status->SetLabel ( txt("t_searching") ) ;
	wxBeginBusyCursor() ;
	wxSafeYield() ;
	Freeze () ;
	lb->Clear () ;
	vi.Clear () ;
	if ( cb_sequence && cb_sequence->GetValue() )
       {
       sequenceSearch() ;
       if ( c->def == _T("dna") || c->def == _T("ABIviewer") || c->def == _T("PrimerDesign") ) sequenceSearch ( true ) ;
       }
	//if ( c->def == _T("dna") || c->def == _T("PrimerDesign") ) 
	if ( cb_translation && cb_translation->GetValue() ) aaSearch () ;
	if ( cb_items && cb_items->GetValue() ) itemSearch() ;
	if ( cb_enzymes && cb_enzymes->GetValue() ) restrictionSearch() ;
	if ( !vi.IsEmpty() )
		{
		lb->SetSelection ( 0 ) ;
		OnLB ( ev ) ;
		}

	// Status text
	if ( lb->GetCount() > FIND_MAX )
	    {
		status->SetLabel ( wxString::Format ( txt("t_too_many_search_results") , FIND_MAX ) ) ;
		do_highlight->Enable() ;
        }
	else if ( lb->GetCount() == 0 )
	     {
	     status->SetLabel ( txt("t_no_matches_found") ) ;
         }
	else
	    {
		status->SetLabel ( wxString::Format ( txt("t_matches_found") , lb->GetCount() ) ) ;
		do_highlight->Enable() ;
        }
	
	Thaw () ;
	wxEndBusyCursor() ;
	}

void FindSequenceDialog::aaSearch ()
    {
    int a , b ;
    TVector *v = c->vec ;
    if ( v->getType() != TYPE_VECTOR && 
    	 v->getType() != TYPE_SEQUENCE && 
    	 v->getType() != TYPE_PRIMER )
    	 return ;
    	 
    wxString s = getQuery() ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        wxString ls = v->items[a].getAminoAcidSequence() ;
        if ( ls.IsEmpty() ) continue ;
        int dir = v->items[a].getDirection() ;
        int off = v->items[a].getOffset() ;
        vector <Tdna2aa> dna2aa ;
        p = 0 ;
        b = subsearch ( ls , s , p ) ;
        while ( b != -1 )
           {
           if ( lb->GetCount() > FIND_MAX ) return ;
           if ( dna2aa.size() == 0 )
              v->items[a].translate ( v , NULL , dna2aa ) ;
           int from = dir==1?b:last ;
           int to = dir==1?last:b ;
           wxString msg ;
           if ( off != -1 )
              msg = wxString::Format ( _T(" [%d-%d]") , from+off , to+off ) ;
           from = dna2aa[from].dna[0] + 1 ;
           to = dna2aa[to].dna[2] + 1 ;
           if ( from > to ) { int x = from ; from = to ; to = x ; }
           lb->Append ( wxString::Format ( _T("%s: %s (%d-%d)%s") ,
                                 txt("amino_acid").c_str() ,
                                 v->items[a].name.c_str() ,
                                 from , to , msg.c_str() ) ) ;
           vi.Add ( -1 ) ;
           p = b + 1 ;
           b = subsearch ( ls , s , p ) ;
           }
        }   
        
    // Fixed reading frames
    wxString ss ;
    ss = v->getSequence() ;
    if ( v->isCircular() ) ss += ss.Left ( 2 ) ;
    else ss += _T("  ") ;
    aaSubSearch ( ss , 0 , 1 , txt("m_aa_1") ) ; // Reading frame +1
    aaSubSearch ( ss , 1 , 1 , txt("m_aa_2") ) ; // Reading frame +2     
    aaSubSearch ( ss , 2 , 1 , txt("m_aa_3") ) ; // Reading frame +3
    ss = v->transformSequence ( true , false ) ;
    if ( v->isCircular() ) ss = ss.Right ( 2 ) + ss ;
    else ss = _T("  ") + ss ;
    aaSubSearch ( ss , -1 , -1 , txt("m_aa_m1") ) ; // Reading frame -1     
    aaSubSearch ( ss , -2 , -1 , txt("m_aa_m2") ) ; // Reading frame -2     
    aaSubSearch ( ss , -3 , -1 , txt("m_aa_m3") ) ; // Reading frame -3    
    }    
    
void FindSequenceDialog::aaSubSearch ( const wxString &s , int start , int dir , wxString rf )
	{
	int a ;
	wxChar codon[4] ;
	codon[3] = 0 ;
	wxString res ;
	wxArrayInt ai ;
    TVector *v = c->vec ;
    wxString sub = getQuery() ;
    int ostart = start < 0 ? 2 : 0 ;
    
    res.Alloc ( s.length() / 3 + 10 ) ;
    ai.Alloc ( s.length() / 3 + 10 ) ;
    
    if ( start < 0 )
    	{
	    for ( start = -start ; start + 3 < s.length() ; start += 3 ) ;
    	}
   	
	for ( a = start ; a + dir * 2 >= 0 && a + dir * 2 < s.length() ; a += dir * 3 )
		{
 		codon[0] = s.GetChar ( a ) ;
 		codon[1] = s.GetChar ( a + dir ) ;
 		codon[2] = s.GetChar ( a + dir * 2 ) ;

        wxChar c2 = codonhash[codon] ;
        if ( c2 < 'A' ) c2 = codonhash[codon] = v->dna2aa ( codon ) . GetChar ( 0 ) ;
        res += c2 ;

		ai.Add ( a ) ;
		}    
		
	a = subsearch ( res , sub , 0 ) ;
	//a = res.Find ( sub ) ;
	while ( a != -1 )
		{
  		if ( lb->GetCount() > FIND_MAX ) return ;
		int from = ai[a] + 1 - ostart ;
		int to = ai[a+sub.length()-1] + dir * 2 + 1 - ostart ;
		if ( from > to ) { int z = from ; from = to ; to = z ; }
		wxString msg = rf.BeforeFirst ( '\t' ) ;
      lb->Append ( wxString::Format ( _T("%s: %s (%d-%d)") ,
                          txt("amino_acid").c_str() ,
                          msg.c_str() ,
                          from , to ) ) ;
      vi.Add ( -1 ) ;
		res.SetChar ( a , '_' ) ; // Invalidating
		a = subsearch ( res , sub , a+1 ) ;
//		a = res.Find ( sub ) ;
		}    
	}    
    
void FindSequenceDialog::itemSearch ()
    {
    int a ;
	 TVector *v = c->vec ;
    wxString s = t->GetValue().Upper() ; // Using original input instead of filtered query
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        if ( lb->GetCount() > FIND_MAX ) return ;
        if ( v->items[a].name.Upper().Find(s) != -1 ||
             v->items[a].desc.Upper().Find(s) != -1 )
           {
           lb->Append ( wxString::Format ( _T("%s: %s (%d-%d)") ,
                                 txt("t_vec_item").c_str() , 
                                 v->items[a].name.c_str() ,
                                 v->items[a].from , 
                                 v->items[a].to ) ) ;
           vi.Add ( a ) ;           
           }    
        }    
    }    
    
void FindSequenceDialog::restrictionSearch ()
    {
    int a ;
    TVector *v = c->vec ;
    wxString s = getQuery() ;
    for ( a = 0 ; a < v->rc.size() ; a++ )
        {
        if ( lb->GetCount() > FIND_MAX ) return ;
        wxString en = v->rc[a].getDisplayName() ;
        if ( en.Upper().Find(s) != -1 )
           {
           lb->Append ( wxString::Format ( _T("%s: %s (%d)") ,
                                 txt("m_restriction").c_str() , 
                                 en.c_str() ,
                                 v->rc[a].getPos()+1 ) ) ;
           vi.Add ( a ) ;
           }    
        }    
    }    
    
void FindSequenceDialog::sequenceSearch ( bool invers )
    {
    bool cont = true ;
    int a ;
    wxString sub = getQuery() ;
    p = 0 ;
    if ( sub.IsEmpty() ) return ;

    // Preparing sequence    
    wxString s = c->vec->getSequence() ;
    if ( invers ) { s = c->vec->transformSequence ( true , true ) ; }
    if ( c->vec->isCircular() )
        {
        wxString t ;
        t = s ;
        while ( t.length() < sub.length() ) t += _T(" ") ;
        s += t.substr ( 0 , sub.length()-1 ) ;
        }
        

    while ( cont )
        {
        // Now we search...
        a = subsearch ( s , sub , p ) ;
        if ( a > -1 )
            {
            if ( lb->GetCount() > FIND_MAX ) return ;
            p = a+1 ;
            int from = a + 1 ;
            int to = last + 1 ;
            if ( to > c->vec->getSequenceLength() ) to -= c->vec->getSequenceLength() ;
            wxString msg ;
            if ( invers )
               {
               msg = _T(" (3'->5')") ;
               int nt = c->vec->getSequenceLength() - from + 1 ;
               int nf = c->vec->getSequenceLength() - to + 1 ;
               from = nf ;
               to = nt ;
               }    
            lb->Append ( wxString::Format ( _T("%s: %d-%d%s") , txt("sequence").c_str() , from , to , msg.c_str() ) ) ;
            vi.Add ( -1 ) ;
            }
        else
            {
            cont = false ;
            }
        }    
    }
    
void FindSequenceDialog::OnTextChange ( wxCommandEvent &ev )
    {
	do_highlight->Disable() ;
    find_button->Enable() ;
    if ( c->vec->getGenomeMode() ) return ;
	int ql = getQuery().length() ;
    if ( ql < 3 )
       {
       lb->Clear () ;
       vi.Clear () ;
       return ;
       }    
    find_button->Disable () ;
    OnSearch ( ev ) ;
#ifdef __WXMAC__
	t->SetFocus () ;
	t->SetSelection ( ql , ql ) ;
#endif
    }    

void FindSequenceDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }



void FindSequenceDialog::OnAddHighlights ( wxCommandEvent &ev )
	{
    wxString mark ;
    SequenceCanvas *canvas = getMarkSequence ( mark ) ;        
    if ( !canvas ) return ; // Just a safety

	SeqBasic *seq = canvas->findID ( mark ) ;
	if ( !seq ) return ; // Just a safety
	
	for ( int idx = 0 ; idx < lb->GetCount() ; idx++ )
		{
		wxString s = lb->GetString ( idx ) ;
		long from , to ;
		getFromTo ( s , from , to , idx ) ;
		if ( from < 0 ) continue ; // Something's wrong, ignore
		seq->addHighlight ( from-1 , to-1 , highlight ) ;
		}
	canvas->unmark () ;
	canvas->SilentRefresh () ;
	}

void FindSequenceDialog::OnSetHighlightColor ( wxCommandEvent &ev )
	{
	wxColour col = highlight ;
	col = wxGetColourFromUser ( this , col ) ;
    if ( !col.Ok() ) return ;
	highlight = col ;
	highlight_display->SetBackgroundColour ( highlight ) ;
	}

void FindSequenceDialog::OnResetHighlights ( wxCommandEvent &ev )
	{
    wxString mark ;
    SequenceCanvas *canvas = getMarkSequence ( mark ) ;        
    if ( !canvas ) return ; // Just a safety
	
	SeqBasic *seq = canvas->findID ( mark ) ;
	if ( !seq ) return ; // Just a safety

	seq->clearHighlights () ;
	canvas->SilentRefresh () ;
	}
