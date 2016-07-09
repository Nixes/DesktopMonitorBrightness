
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//#ifndef WX_PRECOMP
    #include "wx/wx.h"
//#endif

// the application icon (under Windows it is in resources)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// another alternative application icon
#include "smile.xpm"

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

	settings current_settings = RestoreSettings("settings.json");

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

	// setup some kind of wxTimer event and run the below within


	//auto_brightness_timer.Start(1000 * current_settings.polling_time);
	//SetBasedOnTimeOfDay(current_settings);

    return true;
}


// ----------------------------------------------------------------------------
// MyDialog implementation
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyDialog, wxDialog)
	EVT_TIMER(5801, MyDialog::OnTimer)
	EVT_COMMAND_SCROLL(5800, MyDialog::OnSlider)
    EVT_BUTTON(wxID_ABOUT, MyDialog::OnAbout)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_CLOSE(MyDialog::OnCloseWindow)
wxEND_EVENT_TABLE()


MyDialog::MyDialog(const wxString& title)
        : wxDialog(NULL, wxID_ANY, title)
{
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

    wxSizerFlags flags;
    flags.Border(wxALL, 10);

    sizerTop->Add(new wxStaticText
                      (
                        this,
                        wxID_ANY,
                        wxT("Move this slider to set brightness")
                      ), flags);

	// add slider
	sizerTop->Add(new wxSlider(this, 5800,50,0,100) , wxSizerFlags().Expand() );

    wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(new wxButton(this, wxID_ABOUT, wxT("&About")), flags);
    sizerBtns->Add(new wxButton(this, wxID_OK, wxT("&Hide")), flags);

    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
    SetSizerAndFit(sizerTop);
    Centre();

	m_taskBarIcon = new MyTaskBarIcon();

    // we should be able to show up to 128 characters on Windows
    if ( !m_taskBarIcon->SetIcon(wxICON(sample),
		"Desktop Monitor Brightness\n"
		"This tool allows you to change your desktop monitor brightness like a laptop.") )
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

	wxTimer* auto_brightness_timer = new wxTimer(this, 5801);

	auto_brightness_timer->Start(1000 * current_settings.polling_time);
}

MyDialog::~MyDialog()
{
    delete m_taskBarIcon;
}

void MyDialog::OnTimer(wxTimerEvent& event) {
	if (AutoBrightness) {
		SetBasedOnTimeOfDay(current_settings);
	}
}

void MyDialog::OnSlider(wxScrollEvent& event) {
	wxEventType eventType = event.GetEventType();

	int slider_value = event.GetPosition();

	SetAllMonitorsBrightness(slider_value);
	//wxLogMessage(wxT("Slider event: Scroll Changed: %d", event.GetValue() ));
	//std::cout << "Event Type";
	//wxLogMessage(wxT("Slider event occured %d"),eventType);
}

void MyDialog::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    static const char * const title = "About wxWidgets Taskbar Sample";
    static const char * const message
        = "wxWidgets sample showing wxTaskBarIcon class\n"
          "\n"
          "(C) 1997 Julian Smart\n"
          "(C) 2007 Vadim Zeitlin";

#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
    m_taskBarIcon->ShowBalloon(title, message, 15000, wxICON_INFORMATION);
#else // !__WXMSW__
    wxMessageBox(message, title, wxICON_INFORMATION|wxOK, this);
#endif // __WXMSW__/!__WXMSW__
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
    PU_NEW_ICON,
    PU_EXIT,
    PU_CHECKMARK,
    PU_SUB1,
    PU_SUB2,
    PU_SUBMAIN
};


wxBEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    MyTaskBarIcon::OnMenuExit)
    EVT_MENU(PU_CHECKMARK,MyTaskBarIcon::OnMenuCheckmark)
    EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
    EVT_TASKBAR_LEFT_DCLICK  (MyTaskBarIcon::OnLeftButtonDClick)
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

void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
    gs_dialog->Show(true);
}
