#ifndef _MAIN_H_
#define _MAIN_H_

// This will turn the myass error logging on/off
// Place a comment in front of the following line when compiling releases
//#define MYDEBUG

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
    #include "wx/notebook.h"
#endif

#include <wx/toolbar.h>
#include <wx/snglinst.h>
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/file.h>
#include <wx/dialup.h>
#include <wx/utils.h>

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
    #include "bitmaps/new.xpm"
    #include "bitmaps/open.xpm"
    #include "bitmaps/save.xpm"
    #include "bitmaps/copy.xpm"
    #include "bitmaps/cut.xpm"
    #include "bitmaps/paste.xpm"
    #include "bitmaps/print.xpm"
    #include "bitmaps/help.xpm"
#endif

#include <math.h>
#include <string>
#include <vector>

using namespace std ;

#define MYPEN(_a) (wxThePenList->FindOrCreatePen ( _a , 1 , wxSOLID ) )
#define MYBRUSH(_a) (wxTheBrushList->FindOrCreateBrush ( _a , wxSOLID ) )
#define MYFONT(_a,_b,_c,_d) (wxTheFontList->FindOrCreateFont(_a,_b,_c,_d) )
#define DEL_POINTERS(_x) {while(_x.size()){if(_x[_x.size()-1])delete _x[_x.size()-1]; _x.pop_back();}}

// Don't you *EVER* turn this on again!!!!
//#define TIXML_USE_STL
#include "tinyxml/tinyxml.h"

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/snglinst.h>
#include <wx/laywin.h>

#define TYPE_VECTOR 0
#define TYPE_FRAGMENT 1
#define TYPE_SEQUENCE 2
#define TYPE_PRIMER 3
#define TYPE_ALIGNMENT 4
#define TYPE_AMINO_ACIDS 5
#define TYPE_MISC 6

#define FEAT_NONE 0
#define FEAT_ALPHA 1
#define FEAT_BETA 2
#define FEAT_MUT 3
#define FEAT_NOLINE 4
#define FEAT_VLINE 5

class MyFrame ;

// Define a new application
class MyApp : public wxApp
    {
    public:
    bool OnInit();
    int OnExit () ;
    string getHTMLCommand ( string command ) ;
    string getFileFormatApplication ( string type ) ;
    MyFrame *frame;
    wxString homedir , bmpdir , slash ;
    wxSingleInstanceChecker *m_checker ;
    int programVersion ;
    int dbWarningIssued ;
    };

class TUndo ;

#include "enums.h"
#include "TVector.h"
#include "TUReadSeq.h"
#include "ABItype.h"
#include "MiscDialogs.h"
#include "TStorage.h"
#include "TPrimer.h"
#include "TIMGreader.h"
#include "PlasmidCanvas.h"
#include "SequenceCanvas.h"
#include "MyFrame.h"
#include "MyChild.h"
#include "TRestrictionEditor.h"
#include "TClone.h"
#include "GenBank.h"
#include "TXMLfile.h"
#include "TVectorTree.h"
#include "TMainTree.h"
#include "RestrictionEnzymes.h"
#include "TImageDisplay.h"
#include "TCalculator.h"
#include "Alignment.h"
#include "AminoAcids.h"
#include "ABIviewer.h"
#include "TItemEditDialog.h"
#include "PrimerDesign.h"

#define PI 3.14
#define IUPAC_A 1
#define IUPAC_C 2
#define IUPAC_G 4
#define IUPAC_T 8

#define MINI_DISPLAY_ORIGINAL 0
#define MINI_DISPLAY_REAL 1
#define MINI_DISPLAY_CONDENSED 2

#include "ureadseq.h"


// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

#ifdef MYDEBUG
void myass ( bool b , wxString msg = "" ) ;
#else
//#define myass(a)
#define myass(a,b)
#endif

vector <string> explode ( string sep , string s ) ;
char* txt ( string item ) ;
void init_txt ( string lang ) ;
MyApp *myapp () ;
string toupper ( string s ) ;

#endif
// _MAIN_H__

