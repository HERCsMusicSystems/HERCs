
#include "wx_audio_dialog.h"

static wxString convertTowxString (char * original) {
	if (original == NULL) return wxString :: Format (_T ("NULL"));
	wxString ret = _T ("");
	int length = (int) strlen (original);
	for (int ind = 0; ind < length; ind++) ret += wxChar (original [ind]);
	ret += wxChar ('\0');
	return ret;
}

enum {AUDIO_INPUT = 101, AUDIO_OUTPUT};

wxAudioDialog :: wxAudioDialog (wxWindow * parent, MultiplatformAudio * audio) : wxDialog (parent, -1, wxString :: Format (_T ("Audio Hardware"))) {
	this -> audio = audio;
//	if (audio == NULL) return;
	wxFlexGridSizer * grid = new wxFlexGridSizer (2, 2, 0, 0);
	grid -> Add ((new wxStaticText (this, -1, _T ("Audio Inputs"))));
	audio_in = new wxComboBox (this, AUDIO_INPUT, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
	audio_in -> Append (_T ("NONE"));
	if (audio != NULL) {
		for (int ind = 0; ind < audio -> getNumberOfInputDevices (); ind++) audio_in -> Append (convertTowxString (audio -> getInputDeviceName (ind)));
		audio_in -> SetSelection (audio -> getSelectedInputDevice () + 1);
	} else audio_in -> SetSelection (0);
	grid -> Add (audio_in);
	grid -> Add ((new wxStaticText (this, -1, _T ("Audio Outputs"))));
	audio_out = new wxComboBox (this, AUDIO_OUTPUT, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
	audio_out -> Append (_T ("NONE"));
	if (audio != NULL) {
		for (int ind = 0; ind < audio -> getNumberOfOutputDevices (); ind++) audio_out -> Append (convertTowxString (audio -> getOutputDeviceName (ind)));
		audio_out -> SetSelection (audio -> getSelectedOutputDevice () + 1);
	} else audio_out -> SetSelection (0);
	grid -> Add (audio_out);
	SetSizer (grid);
	grid -> SetSizeHints (this);
	Centre ();
}

void wxAudioDialog :: audio_in_action (wxCommandEvent & event) {if (audio == NULL) return; audio -> selectInputDevice (audio_in -> GetSelection () - 1);}
void wxAudioDialog :: audio_out_action (wxCommandEvent & event) {if (audio == NULL) return; audio -> selectOutputDevice (audio_out -> GetSelection () - 1);}

BEGIN_EVENT_TABLE(wxAudioDialog, wxDialog)
EVT_COMBOBOX(AUDIO_INPUT, wxAudioDialog :: audio_in_action)
EVT_COMBOBOX(AUDIO_OUTPUT, wxAudioDialog :: audio_out_action)
END_EVENT_TABLE()
