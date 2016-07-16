
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

DesktopMonitorManager mMan;

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	mMan = DesktopMonitorManager();

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
	m_slider = new wxSlider(this, 5800, mMan.GetBrightness(), 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
	sizerTop->Add(m_slider, wxSizerFlags().Expand());


    wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(new wxButton(this, wxID_OK, wxT("&Done")), flags);

    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
    SetSizerAndFit(sizerTop);
    Centre();

	m_taskBarIcon = new MyTaskBarIcon();

    // we should be able to show up to 128 characters on Windows
    if ( !m_taskBarIcon->SetIcon(wxICON(sample),
		"Desktop Monitor Brightness\n Current Brightness: "+ std::to_string(mMan.GetBrightness()) ) )
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

	// this sets the timer as used for automatically setting brightness based on time
	wxTimer* auto_brightness_timer = new wxTimer(this, 5801);
	auto_brightness_timer->Start(1000 * mMan.GetPollingTime() );
}

MyDialog::~MyDialog()
{
    delete m_taskBarIcon;
}

void MyDialog::OnTimer(wxTimerEvent& event) {
	if (AutoBrightness) {
		mMan.SetBasedOnTimeOfDay();
	}
}

void MyDialog::OnSlider(wxScrollEvent& event) {
	// we must turn off auto brightness to allow manual control first
	AutoBrightness = false;

	int slider_value = event.GetPosition();

	mMan.SetAllMonitorsBrightness(slider_value);
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
	gs_dialog->m_slider->SetValue(mMan.GetBrightness());
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
	gs_dialog->m_slider->SetValue(mMan.GetBrightness());
    gs_dialog->Show(!gs_dialog->IsShown());

}