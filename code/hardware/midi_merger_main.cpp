///////////////////////////////////////
// Copyright (C) 2009 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////


#include "wx/wx.h"
#include "wx/spinctrl.h"
#include "wx/checkbox.h"


#include <CoreMIDI/MIDIServices.h>

MIDIClientRef midi_client = NULL;
MIDIEndpointRef virtual_endpoint = NULL;

static wxString cf_to_wx (CFStringRef name) {
	wxString ret = _T ("");
	int ind = 0;
	while (CFStringGetCharacterAtIndex (name, ind) != '\0') ret += CFStringGetCharacterAtIndex (name, ind++);
	ret += wxChar ('\0');
	return ret;
}

static void main_midi_in_proc (const MIDIPacketList * pktlist, void * readProcRefCon, void * srcConnRefCon) {MIDIReceived (virtual_endpoint, pktlist);}

typedef wxSpinCtrl * spinner_pointer;
typedef wxCheckBox * checkbox_pointer;
class wxMidiMergerFrame : public wxFrame {
public:
	int number_of_sources;
	wxCheckBox * * checkboxes;
	bool * actives;
	MIDIPortRef * in_ports;
	MIDIEndpointRef * in_endpoints;
	wxMidiMergerFrame (wxWindow * parent, wxWindowID id, const wxString & title) : wxFrame (parent, id, title, wxPoint (80, 180), wxSize (-1, -1), wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX)) {
		number_of_sources = 0;
		if (midi_client == NULL) return;
		number_of_sources = MIDIGetNumberOfSources ();
		CFStringRef name;
		wxFlexGridSizer * grid = new wxFlexGridSizer (number_of_sources + 1, 2, 0, 0);
		checkboxes = new checkbox_pointer [number_of_sources];
		actives = new bool [number_of_sources];
		in_ports = new MIDIPortRef [number_of_sources];
		in_endpoints = new MIDIEndpointRef [number_of_sources];
		grid -> Add (new wxStaticText (this, -1, _T ("DEVICE")));
		grid -> Add (new wxStaticText (this, -1, _T ("ACTIVE")));
		for (int ind = 0; ind < number_of_sources; ind++) {
			in_endpoints [ind] = MIDIGetSource (ind);
			MIDIObjectGetStringProperty (in_endpoints [ind], kMIDIPropertyName, & name);
			grid -> Add (new wxStaticText (this, -1, cf_to_wx (name)));
			checkboxes [ind] = new wxCheckBox (this, 301, _T (""));
			checkboxes [ind] -> SetValue (1.0);
			grid -> Add (checkboxes [ind]);
			MIDIInputPortCreate (midi_client, CFSTR (""), main_midi_in_proc, NULL, & in_ports [ind]);
			MIDIPortConnectSource (in_ports [ind], in_endpoints [ind], NULL);
			actives [ind] = true;
		}
		MIDISourceCreate (midi_client, CFSTR ("Midi Merger End Point"), & virtual_endpoint);
		SetSizer (grid);
		grid -> SetSizeHints (this);
	}
	void OnCheckbox (wxCommandEvent & event) {
		for (int ind = 0; ind < number_of_sources; ind++) {
			bool active = checkboxes [ind] -> GetValue ();
			if (actives [ind] != active) {
				if (active) MIDIPortConnectSource (in_ports [ind], in_endpoints [ind], NULL);
				else MIDIPortDisconnectSource (in_ports [ind], in_endpoints [ind]);
				actives [ind] = active;
			}
		}
	}
	~ wxMidiMergerFrame (void) {
		delete [] checkboxes;
		delete [] actives;
		delete [] in_ports;
		delete [] in_endpoints;
	}
private: DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxMidiMergerFrame, wxFrame)
EVT_CHECKBOX(301, wxMidiMergerFrame :: OnCheckbox)
END_EVENT_TABLE()

class wxMidiMergerApplication : public wxApp {
public:
	bool OnInit (void) {
		if (MIDIClientCreate (CFSTR ("HERCs Midi Merger"), NULL, 0, & midi_client)) return false;
		(new wxMidiMergerFrame (NULL, -1, _T ("HERCs MIDI MERGER"))) -> Show ();
		return true;
	}
	int OnExit (void) {return wxApp :: OnExit ();}
};

IMPLEMENT_APP(wxMidiMergerApplication);
