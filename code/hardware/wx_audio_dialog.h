
#include "wx/wx.h"
#include "multiplatform_audio.h"

class wxAudioDialog : public wxDialog {
public:
	MultiplatformAudio * audio;
	wxComboBox * audio_in;
	wxComboBox * audio_out;
public:
	wxAudioDialog (wxWindow * parent, MultiplatformAudio * audio);
	void audio_in_action (wxCommandEvent & event);
	void audio_out_action (wxCommandEvent & event);
private: DECLARE_EVENT_TABLE()
};
