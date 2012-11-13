///////////////////////////////////////
// Copyright (C) 2009 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////


#include "wx/wx.h"
#include "wx/spinctrl.h"
#include "multiplatform_audio.h"

MultiplatformAudio audio (NULL, 2, 44100, 1024);

static FILE * output_file = NULL;
static int record_frames = 0;
static int record_sampling_freq = 44100;
static void insert_long (FILE * tc, int ind) {
	fputc (ind & 0xff, tc);
	ind >>= 8;
	fputc (ind & 0xff, tc);
	ind >>= 8;
	fputc (ind & 0xff, tc);
	ind >>= 8;
	fputc (ind & 0xff, tc);
}
static void insert_short (FILE * tc, int ind) {
	fputc (ind & 0xff, tc);
	ind >>= 8;
	fputc (ind & 0xff, tc);
}
static void selectOutputFile (int seconds, char * file_name) {
	if (* file_name == '\0') return;
	output_file = fopen (file_name, "wb");
	if (output_file == NULL) return;
	record_frames = seconds * record_sampling_freq;
	fprintf (output_file, "RIFF");
	insert_long (output_file, 36 + (record_frames << 2));
	fprintf (output_file, "WAVEfmt ");
	insert_long (output_file, 16);
	insert_short (output_file, 1);                          // some kind of tag = 1
	insert_short (output_file, 2);                          // channels (stereo)
	insert_long (output_file, record_sampling_freq);        // samples per second
	insert_long (output_file, record_sampling_freq << 2);   // bytes per second
	insert_short (output_file, 4);                          // bytes per sample
	insert_short (output_file, 16);                         // bits per sample
	fprintf (output_file, "data");
	insert_long (output_file, record_frames << 2);
}

static void input_callback_procedure (int frames, AudioBuffers * buffers) {
	if (output_file == NULL) return;
	for (int ind = 0; ind < frames; ind++) {
		double left = 32768.0 * buffers -> getStereoLeftRight ();
		double right = 32768.0 * buffers -> getStereoLeftRight ();
		if (left > 32767.0) left = 32767.0;
		if (left < -32768.0) left = -32768.0;
		if (right > 32767.0) right = 32767.0;
		if (right < -32768.0) right = -32768.0;
		short int value = (short int) left;
		fputc (value & 0xff, output_file);
		fputc (value >> 8, output_file);
		value = (short int) right;
		fputc (value & 0xff, output_file);
		fputc (value >> 8, output_file);
		if (--record_frames <= 0) {
			fclose (output_file);
			output_file = NULL;
			return;
		}
	}
}

static wxString convertTowxString (char * original) {
	if (original == NULL) return wxString :: Format (_T ("NULL"));
	wxString ret = _T ("");
	int length = (int) strlen (original);
	for (int ind = 0; ind < length; ind++) ret += wxChar (original [ind]);
	ret += wxChar ('\0');
	return ret;
}

class wxAudioRecorderFrame : public wxFrame {
public:
	wxButton * record_button;
	wxTextCtrl * file_name_edit;
	wxSpinCtrl * seconds_spinner;
	wxComboBox * audio_device_selector;
	wxAudioRecorderFrame (wxWindow * parent, wxWindowID id, const wxString & title) : wxFrame (parent, id, title, wxPoint (80, 180), wxSize (-1, -1), wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX)) {
		audio . installInputCallback (input_callback_procedure);
		audio . selectInputDevice (0);
		wxFlexGridSizer * grid = new wxFlexGridSizer (1, 5, 0, 0);
		file_name_edit = new wxTextCtrl (this, 302); file_name_edit -> SetMinSize (wxSize (300, -1));
		grid -> Add (file_name_edit);
		seconds_spinner = new wxSpinCtrl (this, 303);
		seconds_spinner -> SetRange (1, 10000); seconds_spinner -> SetValue (10.0); seconds_spinner -> SetMinSize (wxSize (100, -1));
		grid -> Add (seconds_spinner);
		record_button = new wxButton (this, 301);
		record_button -> SetLabel (_T ("RECORD"));
		grid -> Add (record_button);
		audio_device_selector = new wxComboBox (this, 304, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		audio_device_selector -> Append (_T ("NONE"));
		for (int ind = 0; ind < audio . getNumberOfInputDevices (); ind++) audio_device_selector -> Append (convertTowxString (audio . getInputDeviceName (ind)));
		audio_device_selector -> SetSelection (audio . getSelectedInputDevice () + 1);
		grid -> Add (audio_device_selector);
		SetSizer (grid);
		grid -> SetSizeHints (this);
	}
	void OnRecord (wxCommandEvent & event) {
		wxMessageBox (wxString :: Format (_T ("Record %i seconds into %s file"), (int) seconds_spinner -> GetValue (), file_name_edit -> GetValue () . c_str ()), _T ("INFO"), wxOK, NULL);
		selectOutputFile ((int) seconds_spinner -> GetValue (), file_name_edit -> GetValue () . char_str ());
	}
	void OnAudioDevice (wxCommandEvent & event) {audio . selectInputDevice (audio_device_selector -> GetSelection () - 1);}
private: DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxAudioRecorderFrame, wxFrame)
EVT_BUTTON(301, wxAudioRecorderFrame :: OnRecord)
EVT_COMBOBOX(304, wxAudioRecorderFrame :: OnAudioDevice)
END_EVENT_TABLE()

class wxAudioRecorderApplication : public wxApp {
public:
	bool OnInit (void) {
		(new wxAudioRecorderFrame (NULL, -1, _T ("HERCs AUDIO RECORDER"))) -> Show ();
		return true;
	}
	int OnExit (void) {return wxApp :: OnExit ();}
};

IMPLEMENT_APP(wxAudioRecorderApplication);
