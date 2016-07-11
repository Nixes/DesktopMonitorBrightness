
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//#ifndef WX_PRECOMP
    #include "wx/wx.h"
//#endif

#include "wx/taskbar.h"
#include "wx/slider.h"

#include "wxui.h"

// a global include file
#include "stdafx.h"

// include brightness setting functions
#include "DesktopMonitorBrightness.h"

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static MyDialog *gs_dialog = NULL;

// this toggles the running of the SetBasedOnTimeOFDay loop
static bool AutoBrightness = true;



wxIMPLEMENT_APP(MyApp);

settings current_settings;

bool MyApp::OnInit()
{

	current_settings = RestoreSettings("settings.json");

	GetMonitorHandles();

    if ( !wxApp::OnInit() )
        return false;

    if ( !wxTaskBarIcon::IsAvailable() )
    {
        wxMessageBox
        (
            "There appears to be no system tray support in your current environment. This sample may not behave as expected.",
            "Warning",
            wxOK | wxICON_EXCLAMATION
        );
    }

    // Create the Brightness change window
    gs_dialog = new MyDialog(wxT("Monitor Brightness"));
	// but don't show it

    return true;
}


// ----------------------------------------------------------------------------
// MyDialog implementation
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyDialog, wxDialog)
	//EVT_COMMAND_KILL_FOCUS(5802 ,MyDialog::OnKillFocus)
	//EVT_ACTIVATE(MyDialog::OnKillFocus)
	EVT_TIMER(5801, MyDialog::OnTimer)
	EVT_COMMAND_SCROLL(5800, MyDialog::OnSlider)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_CLOSE(MyDialog::OnCloseWindow)
wxEND_EVENT_TABLE()


MyDialog::MyDialog(const wxString& title)
        : wxDialog(NULL, wxID_ANY, title)
{
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
	sizerTop->SetMinSize(300,-1); // make the dialog a fixed width

	this->SetWindowStyle(wxSYSTEM_MENU); // remove window border

    wxSizerFlags flags;
    flags.Border(wxALL, 5);

	sizerTop->Add(new wxStaticText(this,wxID_ANY,wxT("Monitor Brightness")), flags);

	// add slider
	sizerTop->Add(new wxSlider(this, 5800, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL), wxSizerFlags().Expand() );


    wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(new wxButton(this, wxID_OK, wxT("&Done")), flags);

    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
    SetSizerAndFit(sizerTop);
    Centre();

	m_taskBarIcon = new MyTaskBarIcon();

    // we should be able to show up to 128 characters on Windows
    if ( !m_taskBarIcon->SetIcon(wxICON(sample),
		"Desktop Monitor Brightness") )
    {
        wxLogError(wxT("Could not set icon."));
    }

#if defined(__WXOSX__) && wxOSX_USE_COCOA
    m_dockIcon = new MyTaskBarIcon(wxTBI_DOCK);
    if ( !m_dockIcon->SetIcon(wxICON(sample)) )
    {
        wxLogError(wxT("Could not set icon."));
    }
#endif

	// this stuff here tries to position the window just above the taskbar
	wxRect display_area;
	display_area = wxGetClientDisplayRect();
	int window_width;
	int window_height;
	this->GetClientSize(&window_width,&window_height);
	int xpos = display_area.GetWidth() - window_width;
	int ypos = display_area.GetHeight() - window_height;
	this->SetPosition(wxPoint(xpos, ypos));

	// this should hide the brightness setting dialog when it looses focus
	//this->OnKillFocus( wxFocusEvent(wxEVT_KILL_FOCUS, 5802) );
	//this->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(MyFrame::OnKillFocus), NULL, this);
	//this->Connect(wxEVT_CHILD_FOCUS, wxFocusEventHandler(MyDialog::OnKillFocus));
	Bind(wxEVT_KILL_FOCUS, &MyDialog::OnKillFocus,this);

	// this sets the timer as used for automatically setting brightness based on time
	wxTimer* auto_brightness_timer = new wxTimer(this, 5801);
	auto_brightness_timer->Start(1000 * current_settings.polling_time);
}

MyDialog::~MyDialog()
{
    delete m_taskBarIcon;
}

void MyDialog::OnKillFocus(wxFocusEvent& WXUNUSED(event)) {
	wxMessageBox(wxT("Killfocus called"));
	//Show(false);
}

void MyDialog::OnTimer(wxTimerEvent& event) {
	if (AutoBrightness) {
		SetBasedOnTimeOfDay(current_settings);
	}
}

void MyDialog::OnSlider(wxScrollEvent& event) {
	// we must turn off auto brightness to allow manual control first
	AutoBrightness = false;

	int slider_value = event.GetPosition();

	SetAllMonitorsBrightness(slider_value);
}

void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void MyDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}


// ----------------------------------------------------------------------------
// MyTaskBarIcon implementation
// ----------------------------------------------------------------------------

enum
{
    PU_RESTORE = 10001,
    PU_EXIT,
    PU_CHECKMARK
};


wxBEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    MyTaskBarIcon::OnMenuExit)
    EVT_MENU(PU_CHECKMARK,MyTaskBarIcon::OnMenuCheckmark)
    EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
    EVT_TASKBAR_LEFT_DOWN  (MyTaskBarIcon::OnLeftButtonClick)
wxEND_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
    gs_dialog->Show(true);
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
    gs_dialog->Close(true);
}

void MyTaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
{
	AutoBrightness = !AutoBrightness;
}

void MyTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
{
    event.Check(AutoBrightness);
}


// Overridables
wxMenu *MyTaskBarIcon::CreatePopupMenu()
{
    wxMenu *menu = new wxMenu;
    menu->Append(PU_RESTORE, wxT("&Set Brightness"));
    menu->AppendSeparator();
    menu->AppendCheckItem(PU_CHECKMARK, wxT("AutoBrightness"));
    menu->AppendSeparator();

    /* OSX has built-in quit menu for the dock menu, but not for the status item */
#ifdef __WXOSX__ 
    if ( OSXIsStatusItem() )
#endif
    {
        menu->AppendSeparator();
        menu->Append(PU_EXIT,    wxT("E&xit"));
    }
    return menu;
}

void MyTaskBarIcon::OnLeftButtonClick(wxTaskBarIconEvent &event)
{
	// should place dialog at bottom right of primary monitor
    gs_dialog->Show(!gs_dialog->IsShown());

}
