
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//#ifndef WX_PRECOMP
    #include "wx/wx.h"
//#endif

#include "wx/taskbar.h"
#include "wx/slider.h"

#include <wx/valnum.h> // required for wxFloatingPointValidator

#include "wxui.h"

// a global include file
#include "stdafx.h"

// include brightness setting functions
#include "DesktopMonitorBrightness.h"

//#define testing

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static MyDialog *gs_dialog = NULL;
static SettingsDialog *settings_dialog = NULL;

// this toggles the running of the SetBasedOnTimeOFDay loop
static bool AutoBrightness = true;

// this contains the timer for running, SetBasedOnTimeOfDay
wxTimer* auto_brightness_timer;

DesktopMonitorManager mMan;


void UpdateTimer() {
	if (AutoBrightness) {
		auto_brightness_timer->Start(1000 * mMan.GetPollingTime());
	}
	else {
		auto_brightness_timer->Stop();
	}
}


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

	#ifdef testing
		std::string error_string = "";
		if ( mMan.Tests(error_string) == false ) {
			wxLogError( wxT("Tests FAILED with: "+error_string) );
			exit(EXIT_FAILURE);
		}
		else {
			wxLogError(wxT("Tests PASSED with: " + error_string));
			exit(EXIT_SUCCESS);
		}
	#endif

    // Create the Brightness change window
    gs_dialog = new MyDialog(wxT("Monitor Brightness"));
	// but don't show it

    return true;
}

// settingsDialog Implementation

wxBEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
	EVT_CLOSE(SettingsDialog::OnCloseWindow)
	EVT_BUTTON(wxID_SAVE,SettingsDialog::onSave)
wxEND_EVENT_TABLE()

// constructor
SettingsDialog::SettingsDialog(const wxString& title)
	: wxDialog(NULL, wxID_ANY, title)
{
	wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags flags;
	flags.Border(wxALL, 10);

	wxFloatingPointValidator<float>floatTimeValidator(2, 0, wxNUM_VAL_ZERO_AS_BLANK);
	floatTimeValidator.SetRange(0, 24);

	wxFloatingPointValidator<float>timeValidator(2, 0, wxNUM_VAL_ZERO_AS_BLANK);
	timeValidator.SetRange(1, 3600); // between 1 second and 1 hour

	// start location specific settings
	sizerTop->Add(new wxStaticText(this, wxID_ANY, wxT("Longitude")), flags);
	longitude_text = new wxTextCtrl(this, wxID_ANY, std::to_string(mMan.GetLongitude()), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	sizerTop->Add(longitude_text, flags);

	sizerTop->Add(new wxStaticText(this, wxID_ANY, wxT("Latitude")), flags);
	latitude_text = new wxTextCtrl(this, wxID_ANY, std::to_string(mMan.GetLatitude()), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	sizerTop->Add(latitude_text, flags);
	// end location specific settings

	sizerTop->Add(new wxStaticText(this, wxID_ANY, wxT("AutoBrightness Update Interval (seconds)")), flags);
	update_interval = new wxTextCtrl(this, wxID_ANY, std::to_string(mMan.GetPollingTime()), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, timeValidator);
	sizerTop->Add(update_interval, flags);


	sizerTop->Add(new wxStaticText(this, wxID_ANY, wxT("AutoBrightness Sunrise time")), flags);
	sunrise_time = new wxTextCtrl(this, wxID_ANY, std::to_string(mMan.GetSunrisetime()), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, floatTimeValidator);
	sizerTop->Add(sunrise_time, flags);


	sizerTop->Add(new wxStaticText(this, wxID_ANY, wxT("AutoBrightness Sunset time")), flags);
	sunset_time = new wxTextCtrl(this, wxID_ANY, std::to_string(mMan.GetSunsettime()), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, floatTimeValidator);
	sizerTop->Add(sunset_time, flags);

	// these two should ideally be the same control that allows you to set both the minimum and the maximum on the same slider
	sizerTop->Add(new wxStaticText(this, wxID_ANY, wxT("Minimum Brightness")), flags);
	min_slider = new wxSlider(this, 5800, mMan.GetMinBrightness(), 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
	sizerTop->Add(min_slider, wxSizerFlags().Expand());

	sizerTop->Add(new wxStaticText(this, wxID_ANY, wxT("Maximum Brightness")), flags);
	max_slider = new wxSlider(this, 5800, mMan.GetMaxBrightness(), 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
	sizerTop->Add(max_slider, wxSizerFlags().Expand());

	autobrightness_checkbox = new wxCheckBox(this, wxID_ANY, wxT("AutoBrightness on by default"));
	sizerTop->Add(autobrightness_checkbox, flags);

	wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerBtns->Add(new wxButton(this, wxID_SAVE, wxT("Save")), flags);
	sizerBtns->Add(new wxButton(this, wxID_CANCEL, wxT("Discard")), flags);
	sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));

	// actually initiate the sizer
	SetSizerAndFit(sizerTop);
	Centre();
}

SettingsDialog::~SettingsDialog() {

}

void SettingsDialog::onSave(wxCommandEvent& WXUNUSED(event)) {
	// read settings from dialog

	// read interval
	double update_interval_double;
	update_interval->GetValue().ToDouble(&update_interval_double);
	mMan.SetPollingTime(update_interval_double);

	// read sunrise time
	double sunrise_double;
	sunrise_time->GetValue().ToDouble(&sunrise_double);
	mMan.SetSunrisetime( sunrise_double);

	// read sunset time
	double sunset_double;
	sunset_time->GetValue().ToDouble(&sunset_double);
	mMan.SetSunsettime(sunset_double);

	// read min and max brightness
	mMan.SetMinBrightness(min_slider->GetValue());
	mMan.SetMaxBrightness(max_slider->GetValue());

	// we save the settings to file
	if (!mMan.SaveSettings()) {
		wxLogError(wxT("Failed to write settings to disk, is the program directory writable?"));
	}
	
	// then we close the dialog
	Close(true);
}


void SettingsDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void SettingsDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	Destroy();
}

// ----------------------------------------------------------------------------
// MyDialog implementation
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyDialog, wxDialog)
	EVT_TIMER(5801, MyDialog::OnTimer)
	EVT_COMMAND_SCROLL(5800, MyDialog::OnSlider)
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

    SetSizerAndFit(sizerTop);
    Centre();

	m_taskBarIcon = new MyTaskBarIcon();

    // we should be able to show up to 128 characters on Windows
    if ( !m_taskBarIcon->SetIcon(wxICON(sample),
		"Current Brightness: "+ std::to_string(mMan.GetBrightness()) ) )
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
	this->GetSize(&window_width,&window_height);
	int xpos = display_area.GetWidth() - window_width;
	int ypos = display_area.GetHeight() - window_height;
	this->SetPosition(wxPoint(xpos, ypos));

	// this sets the timer as used for automatically setting brightness based on time
	auto_brightness_timer = new wxTimer(this, 5801);
	UpdateTimer();
}

MyDialog::~MyDialog()
{
    delete m_taskBarIcon;
}

void MyDialog::OnTimer(wxTimerEvent& event) {
	std::ostringstream stream;
	stream << mMan.SetBasedOnTimeOfDay();
	std::string debug_string = stream.str();
	//wxLogError(wxT( "Target brightness was: " + debug_string ));
}

void MyDialog::OnSlider(wxScrollEvent& event) {
	// we must turn off auto brightness to allow manual control first
	AutoBrightness = false;
	UpdateTimer();

	int slider_value = event.GetPosition();

	mMan.SetAllMonitorsBrightness(slider_value);
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
	PU_SETTINGS = 10002,
    PU_EXIT,
    PU_CHECKMARK
};


wxBEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    MyTaskBarIcon::OnMenuExit)
    EVT_MENU(PU_CHECKMARK,MyTaskBarIcon::OnMenuCheckmark)
	EVT_MENU(PU_SETTINGS, MyTaskBarIcon::OnSettingsRestore)
    EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
    EVT_TASKBAR_LEFT_DOWN  (MyTaskBarIcon::OnLeftButtonClick)
wxEND_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
	gs_dialog->m_slider->SetValue(mMan.GetBrightness());
    gs_dialog->Show(true);
	gs_dialog->Raise();
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
    gs_dialog->Close(true);
}

void MyTaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
{
	AutoBrightness = !AutoBrightness;
	UpdateTimer();
}

void MyTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
{
    event.Check(AutoBrightness);
	UpdateTimer();
}


// Overridables
wxMenu *MyTaskBarIcon::CreatePopupMenu()
{
    wxMenu *menu = new wxMenu;
    menu->Append(PU_RESTORE, wxT("&Set Brightness"));
	menu->Append(PU_SETTINGS, wxT("&Change Settings"));
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
	gs_dialog->Raise();

}

void MyTaskBarIcon::OnSettingsRestore(wxCommandEvent&)
{
	// Create the settings window
	settings_dialog = new SettingsDialog(wxT("Settings"));
	settings_dialog->Show(true);
}
