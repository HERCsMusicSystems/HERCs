/////////////////////////////////////
// Copyright (C) 2008 In The Chair //
//       ALL RIGHTS RESERVED       //
/////////////////////////////////////


#include "wx_audio_dialog.h"
#include <wx/spinctrl.h>

MultiplatformAudio * audio = NULL;

#define DELAY_LINE_SIZE 131072
double delay_line [DELAY_LINE_SIZE];
int read_pointer = 0;
int write_pointer = 0;
int delay_time = 17;
int delay_feedback = 80;
int delay_pan = 39;

void input_callback (int frames, AudioBuffers * buffers) {
	int sentinel = (int) ((double) DELAY_LINE_SIZE * (double) delay_time / 100.0);
	sentinel &= 0xfffffe;
	if (sentinel > DELAY_LINE_SIZE) sentinel = DELAY_LINE_SIZE;
	if (write_pointer >= sentinel) write_pointer = 0;
	double left_fraction = 1.0 - (double) delay_pan / 50.0;
	double right_fraction = 1.0 + (double) delay_pan / 50.0;
	double feed = (double) delay_feedback / 100.0;
	while (frames -- > 0) {
		double value = buffers -> getMono ();
		double left = delay_line [write_pointer++];
		double right = delay_line [write_pointer--];
		delay_line [write_pointer++] = value * left_fraction + right * feed;
		delay_line [write_pointer++] = value * right_fraction + left * feed;
		if (write_pointer >= sentinel) write_pointer = 0;
	}
	read_pointer = write_pointer;
}

void new_input_callback (int frames, AudioBuffers * buffers) {
	int sentinel = (int) ((double) DELAY_LINE_SIZE * (double) delay_time / 100.0);
	sentinel &= 0xfffffe;
	if (sentinel > DELAY_LINE_SIZE) sentinel = DELAY_LINE_SIZE;
	if (write_pointer >= sentinel) write_pointer = 0;
	while (frames-- > 0) {
		double value = buffers -> getMono ();
		delay_line [write_pointer++] = value;
		delay_line [write_pointer++] = value;
		if (write_pointer >= sentinel) write_pointer = 0;
	}
}

void output_callback (int frames, AudioBuffers * buffers) {
	int sentinel = (int) ((double) DELAY_LINE_SIZE * (double) delay_time / 100.0);
	sentinel &= 0xfffffe;
	if (sentinel > DELAY_LINE_SIZE) sentinel = DELAY_LINE_SIZE;
	if (read_pointer >= sentinel) read_pointer = 0;
	while (frames -- > 0) {
		double left = delay_line [read_pointer++];
		double right = delay_line [read_pointer++];
		if (read_pointer >= sentinel) read_pointer = 0;
		buffers -> insertStereo (left, right);
	}
}

class AudioDelayFrame : public wxFrame {
public:
	wxSpinCtrl * time_spinner, * feedback_spinner, * pan_spinner;
public:
	AudioDelayFrame (wxWindow * parent, wxWindowID id, const wxString & title) : wxFrame (parent, id, title) {
		wxMenuBar * bar = new wxMenuBar ();
		wxMenu * options = new wxMenu ();
		options -> Append (104, _T ("Audio Hardware"), _T (""));
		bar -> Append (options, _T ("Options"));
		SetMenuBar (bar);
		wxFlexGridSizer * grid = new wxFlexGridSizer (2, 3, 0, 0);
		time_spinner = new wxSpinCtrl (this, 101);
		time_spinner -> SetRange (0, 100); time_spinner -> SetValue (delay_time);
		grid -> Add (time_spinner);
		feedback_spinner = new wxSpinCtrl (this, 102);
		feedback_spinner -> SetRange (0, 100); feedback_spinner -> SetValue (delay_feedback);
		grid -> Add (feedback_spinner);
		pan_spinner = new wxSpinCtrl (this, 103);
		pan_spinner -> SetRange (-50, 50); pan_spinner -> SetValue (delay_pan);
		grid -> Add (pan_spinner);
		grid -> Add (new wxStaticText (this, -1, _T ("Time")));
		grid -> Add (new wxStaticText (this, -1, _T ("Feedback")));
		grid -> Add (new wxStaticText (this, -1, _T ("Pan")));
		SetSizer (grid);
		grid -> SetSizeHints (this);
		Centre ();
	}
	void OnShowAudioHardware (wxCommandEvent & event) {
		wxAudioDialog audio_dialog (this, audio);
		audio_dialog . ShowModal ();
	}
	void OnDelayTime (wxSpinEvent & event) {delay_time = time_spinner -> GetValue ();}
	void OnDelayFeedback (wxSpinEvent & event) {delay_feedback = feedback_spinner -> GetValue ();}
	void OnDelayPan (wxSpinEvent & event) {delay_pan = pan_spinner -> GetValue ();}
private: DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(AudioDelayFrame, wxFrame)
EVT_MENU(104, AudioDelayFrame :: OnShowAudioHardware)
EVT_SPINCTRL(101, AudioDelayFrame :: OnDelayTime)
EVT_SPINCTRL(102, AudioDelayFrame :: OnDelayFeedback)
EVT_SPINCTRL(103, AudioDelayFrame :: OnDelayPan)
END_EVENT_TABLE()

class AudioDelayApplication : public wxApp {
private:
	AudioDelayFrame * frame;
public:
	bool OnInit (void) {
#ifdef MAC_OS_X_CORE_AUDIO
		if (audio == NULL) audio = new MultiplatformAudio (NULL, 2, 44100, 256);
		audio -> installInputCallback (input_callback);
		audio -> installOutputCallback (output_callback);
		audio -> selectInputDevice (0);
		audio -> selectOutputDevice (0);
#endif
		frame = new AudioDelayFrame (NULL, -1, _T ("Audio Delay Effect"));
		frame -> Show ();
		return true;
	}
	int OnExit (void) {
#ifdef MAC_OS_X_CORE_AUDIO
		if (audio != NULL) delete audio; audio = NULL;
#endif
		return wxApp :: OnExit ();
	}
};

#ifdef MAC_OS_X_CORE_AUDIO
IMPLEMENT_APP(AudioDelayApplication)
#endif

#ifdef WINDOWS_DIRECT_X

IMPLEMENT_APP_NO_MAIN(AudioDelayApplication)
#include "resource.h"
HINSTANCE hinstance = NULL;
HWND main_console = NULL;
int CALLBACK main_console_window_proc (HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
	case WM_INITDIALOG:
		SendMessage (hwnd, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon (hinstance, MAKEINTRESOURCE (IDI_ICON1)));
		if (audio == NULL) audio = new MultiplatformAudio ((void *) hwnd, 2, 44100, 1024);
		audio -> installInputCallback (input_callback);
		audio -> installOutputCallback (output_callback);
		audio -> selectInputDevice (0);
		audio -> selectOutputDevice (0);
		return true;
	case WM_COMMAND:
		switch (LOWORD (wparam)) {
		case IDOK:
		case IDCANCEL:
			if (audio != NULL) delete audio; audio = NULL;
			PostQuitMessage (0);
			EndDialog (hwnd, IDCANCEL);
			return true;
		default: return false;
		}
		return false;
	default: return false;
	}
	return false;
}

void build_main_console (void) {main_console = CreateDialog (hinstance, MAKEINTRESOURCE (IDD_MAIN_CONSOLE), NULL, main_console_window_proc);}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	hinstance = hInstance;
	WNDCLASS wn;
	wn . lpszClassName = TEXT ("Audio Delay for testing Multiplatform Audio Acquisition and Transmission");
	wn . lpfnWndProc = DefWindowProc;
	wn . style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wn . hInstance = hInstance;
	wn . hIcon = LoadIcon (NULL, MAKEINTRESOURCE (IDI_ICON1));
	wn . hCursor = LoadCursor (NULL, IDC_ARROW);
	wn . hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
	wn . lpszMenuName = NULL;
	wn . cbClsExtra = 0;
	wn . cbWndExtra = 0;
	if (! RegisterClass (& wn)) return 0;
	CoInitialize (NULL);

	build_main_console ();
	wxEntry (hInstance, hPrevInstance, lpCmdLine, nShowCmd);
	return IDCANCEL;
}

#endif

