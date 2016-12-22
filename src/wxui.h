/////////////////////////////////////////////////////////////////////////////
// Name:        tbtest.h
// Purpose:     wxTaskBarIcon sample
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

class MyTaskBarIcon : public wxTaskBarIcon
{
public:
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    MyTaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE)
    :   wxTaskBarIcon(iconType)
#else
    MyTaskBarIcon()
#endif
    {}

    void OnLeftButtonClick(wxTaskBarIconEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuSetNewIcon(wxCommandEvent&);
    void OnMenuCheckmark(wxCommandEvent&);
    void OnMenuUICheckmark(wxUpdateUIEvent&);
    void OnMenuSub(wxCommandEvent&);
	void OnSettingsRestore(wxCommandEvent&);
    virtual wxMenu *CreatePopupMenu() wxOVERRIDE;

    wxDECLARE_EVENT_TABLE();
};


// Define a new application
class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
};


// provides an easy to use interface to modify settings
class SettingsDialog : public wxPropertySheetDialog
{
public:
	SettingsDialog(const wxString& title);
	virtual ~SettingsDialog();

protected:
	wxTextCtrl* longitude_text;
	wxTextCtrl* latitude_text;
	wxTextCtrl* update_interval;
	wxTextCtrl* sunrise_time;
	wxTextCtrl* sunset_time;
	wxSlider* min_slider;
	wxSlider* max_slider;
	wxCheckBox* autobrightness_checkbox;
	wxCheckBox* auto_suntime_calc_checkbox;

	void OnExit(wxCommandEvent& event);
	void OnCloseWindow(wxCloseEvent& event);
	void onSave(wxCommandEvent & WXUNUSED);

	MyTaskBarIcon   *m_taskBarIcon;
#if defined(__WXOSX__) && wxOSX_USE_COCOA
	MyTaskBarIcon   *m_dockIcon;
#endif

	wxDECLARE_EVENT_TABLE();
};


class MyDialog: public wxDialog
{
public:
    MyDialog(const wxString& title);
    virtual ~MyDialog();
	wxSlider *m_slider;

protected:
	void OnTimer(wxTimerEvent& event);
	void OnSlider(wxScrollEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    MyTaskBarIcon   *m_taskBarIcon;
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    MyTaskBarIcon   *m_dockIcon;
#endif

    wxDECLARE_EVENT_TABLE();
};
