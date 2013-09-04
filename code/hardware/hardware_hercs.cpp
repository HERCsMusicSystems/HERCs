///////////////////////////////////////
// Copyright (C) 2008 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "hercs_sdk.h"

#include "wx/wx.h"
#include "controls.h"
#include "front_panel.h"

#define MULTI_CORE_HARDWARE
#define SWITCHABLE_MULTICORE

config * cfg = NULL;

orthogonal_core core;
buffered_midi_stream console_feedback (2048);

static bool oscilloscope_visible = false;

/*
class OscilloscopeDialogClass;
OscilloscopeDialogClass * oscilloscope_dialog = NULL;
class OscilloscopeDialogClass : public wxDialog {
public:
	wxOscilloscopeController * oscilloscope;
	OscilloscopeDialogClass (wxWindow * parent, int * wave) : wxDialog (parent, -1, wxString :: Format (_T ("Oscilloscope"))) {
		wxBoxSizer * grid = new wxBoxSizer (wxHORIZONTAL);
		oscilloscope = new wxOscilloscopeController (this, -1, wave);
		grid -> Add (oscilloscope, wxEXPAND | wxALL);
		SetSizer (grid);
		grid -> SetSizeHints (this);
		Centre ();
	}
	~ OscilloscopeDialogClass (void) {oscilloscope_visible = false; oscilloscope_dialog = NULL;}
};
*/

////////////////////////////////////////
// Multiplatform Audio Hardware Start //
////////////////////////////////////////

#include "wx_audio_dialog.h"

// Transmissions:
// 1. From console interface to console input line
// 2. From main interface to main input line
// 3. From main output line to main interface
// 4. From console output line to console interface

static double input_audio_buffer [32768];
static int read_pointer = 0;
static int write_pointer = 0;
void core_audio_input_callback (int frames, AudioBuffers * buffers) {
	while (frames-- > 0) {
		input_audio_buffer [write_pointer++] = buffers -> getStereoLeftRight ();
		input_audio_buffer [write_pointer++] = buffers -> getStereoLeftRight ();
		if (write_pointer >= 32768) write_pointer = 0;
	}
}
extern void console_midi_transmission (void);
extern void external_midi_transmission (void);
extern void general_quit (void);
static int oscilloscope_monitor_wave [256];
static int oscilloscope_wave_pointer = 0;
static int previous_osc_out = 0;
static int oscilloscope_gap_sentinel = 20000;
static int oscilloscope_gap = 20000;
static int oscilloscope_shift_sentinel = 0;
static int oscilloscope_shift = -1;
static wxOscilloscopeController * oscilloscope_controller = NULL;
void core_audio_output_callback (int nframes, AudioBuffers * buffers) {
	if (nframes >= 2048) return;
	// transmission from console interface to console input line here....
	// transmission from console output line to console interface here....
	console_midi_transmission ();
	// crash check here....
	float left_out, right_out; int osc_out;
	#ifdef MULTI_CORE_HARDWARE
	#ifdef SWITCHABLE_MULTICORE
	if (cfg -> processors > 0) {
	#endif
	for (int ind = 0; ind < nframes; ind++) {
		core . input_left (input_audio_buffer [read_pointer++], ind);
		core . input_right (input_audio_buffer [read_pointer++], ind);
		if (read_pointer >= 32768) read_pointer = 0;
	}
	if (core . multi_move (nframes)) general_quit ();
	#ifdef SWITCHABLE_MULTICORE
	}
	#endif
	#endif
	int sample = 0;
	while (nframes-- > 0) {
		// transmission from main interface to main input line here....
		#ifndef MULTI_CORE_HARDWARE
		core . input_left (input_audio_buffer [read_pointer++]);
		core . input_right (input_audio_buffer [read_pointer++]);
		if (read_pointer >= 32768) read_pointer = 0;
		if (core . move ()) general_quit (); //callback_return = -1;
		left_out = core . left_out (); right_out = core . right_out ();
		#endif
		#ifdef MULTI_CORE_HARDWARE
		#ifdef SWITCHABLE_MULTICORE
		if (cfg -> processors > 0) {
			left_out = core . left_out (sample); right_out = core . right_out (sample); sample++;
		} else {
			core . input_left (input_audio_buffer [read_pointer++]);
			core . input_right (input_audio_buffer [read_pointer++]);
			if (read_pointer >= 32768) read_pointer = 0;
			if (core . move ()) general_quit (); //callback_return = -1;
			left_out = core . left_out (); right_out = core . right_out ();
		}
		#else
		left_out = core . left_out (sample); right_out = core . right_out (sample); sample++;
		#endif
		#endif
		buffers -> insertStereo (left_out, right_out);
		//--------------------------------------
		if (oscilloscope_visible) {
			osc_out = (int) ((left_out + right_out) * 128.0F);
			if (oscilloscope_gap > 0) oscilloscope_gap--;
			if (oscilloscope_gap == 0) {
				if (osc_out >= 0 && previous_osc_out <= 0) {oscilloscope_shift = oscilloscope_shift_sentinel; oscilloscope_gap--;}
			}
				if (oscilloscope_shift > 0) oscilloscope_shift--;
			if (oscilloscope_shift == 0) {oscilloscope_wave_pointer = 0; oscilloscope_shift--;}
			if (oscilloscope_wave_pointer < 256) {oscilloscope_monitor_wave [oscilloscope_wave_pointer++] = osc_out;}
			previous_osc_out = osc_out;
		}
		//--------------------------------------
//		buffers -> insertStereo (core . left_out (), core . right_out ());
		// transmission from main output line to main interface here....
		external_midi_transmission ();
	}
	if (oscilloscope_controller == NULL) return;
	if (oscilloscope_gap >= 0) return;
	if (oscilloscope_wave_pointer < 256) return;
//	if (! oscilloscope_dialog -> IsShown ()) {oscilloscope_visible = false; return;}
	oscilloscope_controller -> Refresh ();
	oscilloscope_gap = oscilloscope_gap_sentinel;
}

MultiplatformAudio * audio = NULL;

//////////////////////////////////////
// Multiplatform Audio Hardware End //
//////////////////////////////////////

#include "prolog_neural.h"
#ifdef WINDOWS_OPERATING_SYSTEM
#include "resource.h"
class resource_loader_class : public PrologResourceLoader {
public:
	char * load (char * name) {
		HRSRC resource = NULL;
		if (strcmp (name, "studio.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STUDIO_PRC), RT_RCDATA);
		if (strcmp (name, "store.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STORE_PRC), RT_RCDATA);
		if (strcmp (name, "help.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HELP_PRC), RT_RCDATA);
		if (strcmp (name, "keyboard.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (KEYBOARD_PRC), RT_RCDATA);
		if (strcmp (name, "record.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (RECORD_PRC), RT_RCDATA);
		if (strcmp (name, "scala_reader.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (SCALA_READER_PRC), RT_RCDATA);
		if (strcmp (name, "neural.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (NEURAL_PRC), RT_RCDATA);
		if (strcmp (name, "f1.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (F1_PRC), RT_RCDATA);
		if (! resource) return NULL;
		HGLOBAL loader = LoadResource (NULL, resource);
		if (! loader) return NULL;
		return (char *) LockResource (loader);
	}
} resource_loader;
#endif

#ifdef LINUX_OPERATING_SYSTEM
extern char resource_0 [];
extern char resource_1 [];
extern char resource_2 [];
extern char resource_3 [];
extern char resource_4 [];
extern char resource_5 [];
extern char resource_6 [];
extern char resource_7 [];
extern char resource_8 [];
class resource_loader_class : public PrologResourceLoader {
public:
	char * load (char * name) {
		char * ret = NULL;
		if (strcmp (name, "studio") == 0) ret = resource_0;
		if (strcmp (name, "http") == 0) ret = resource_1;
		if (strcmp (name, "store") == 0) ret = resource_2;
		if (strcmp (name, "f1") == 0) ret = resource_3;
		if (strcmp (name, "help") == 0) ret = resource_4;
		if (strcmp (name, "record") == 0) ret = resource_5;
		if (strcmp (name, "neural") == 0) ret = resource_6;
		if (strcmp (name, "keyboard") == 0) ret = resource_7;
		if (strcmp (name, "sql") == 0) ret = resource_8;
		if (strcmp (name, "studio.prc") == 0) ret = resource_0;
		if (strcmp (name, "http.prc") == 0) ret = resource_1;
		if (strcmp (name, "store.prc") == 0) ret = resource_2;
		if (strcmp (name, "f1.prc") == 0) ret = resource_3;
		if (strcmp (name, "help.prc") == 0) ret = resource_4;
		if (strcmp (name, "record.prc") == 0) ret = resource_5;
		if (strcmp (name, "neural.prc") == 0) ret = resource_6;
		if (strcmp (name, "keyboard.prc") == 0) ret = resource_7;
		if (strcmp (name, "sql.prc") == 0) ret = resource_8;
		return ret;
	}
} resource_loader;
#endif

class service_class_loader_class : public PrologServiceClassLoader {
public:
	PrologServiceClass * load (char * name) {
		if (strcmp (name, "prolog.neural") == 0) return new PrologNeuralServiceClass ();
		return NULL;
	}
} service_class_loader;


#ifdef WINDOWS_OPERATING_SYSTEM
#include "windows_midi.h"
windows_midi_service midi_service (NULL);
windows_midi_service console_midi_service;
MidiCommandPrompt * command_console = NULL;
#define menu_height 36
#endif

#ifdef LINUX_OPERATING_SYSTEM
#include "midi_command_prompt.h"
MidiCommandPrompt * command_console = NULL;

#ifdef MAC_OPERATING_SYSTEM
#include "mac_midi.h"
mac_midi_service midi_service ("HERCs CORE");
#else
//#include "linux_midi.h"
//linux_midi_service midi_service;
#endif

#endif

void build_synthesizer (void) {
	core . build_synthesizer (cfg, & resource_loader, & service_class_loader);
	core . conn_midi_out -> connect_thru (& console_feedback);
	// to do
//	core . root -> setMidiPortServiceClass (& midi_service);
	command_console = new MidiCommandPrompt (core . conn_midi_in, cfg -> prolog_console_horizontal);
	command_console -> open ();
}

void destroy_synthesizer (void) {
	if (command_console != NULL) {command_console -> close (); delete command_console;}
	core . destroy_synthesizer ();
	printf ("Synthesiser deallocated.\n");
}


wxString towxstring (char * text) {
	wxString ret = _T ("");
	while (* text != '\0') {
		ret += * text;
		text++;
	}
	return ret;
}

static wxFrame * open_close_frame = NULL;

class wx_panel : public hercs_dx_type_panel {
public:
	wxAnimatedButton * part_buttons [10];
	wxAnimatedButton * parameter_buttons [18];
	wxAnimatedButton * type_buttons [4];
	wxAnimatedButton * sensitivity_buttons [3];
	wxAnimatedButton * mode_buttons [12];
	wxAnimatedButton * store_button, * reset_button, * enter_button;
	wxStaticText * parameter_names [18];
	wxAnimatedButton * inc_button, * dec_button;
	wxAnimatedButton * times_buttons [3];
	wxStaticText * parameter_display;
	wxStaticText * status_display;
	// Controllers
	wxTravelingBitmapKnob * volume_knob, * pan_knob, * dry_wet_knob;
	wxTravelingBitmapKnob * attack_knob, * decay_knob, * sustain_knob, * release_knob;
	wxTravelingBitmapKnob * speed_knob, * vibrato_knob, * tremolo_knob, * wah_wah_knob, * lfo_pan_knob;
	wxTravelingBitmapKnob * freq_knob, * amp_knob, * index_knob, * porta_time_knob;
	wxAnimatedVector * vector, * pitch_wheel, * modulation_wheel;
	wxAnimatedButton * mono_poly_switch, * porta_switch;
	double vector_x_backup;
	virtual void update_controllers (int nrpn_lsb, int data_msb, int data_lsb, bool separate_vector_coordinates) {
		//double data = (double) ((data_msb << 7) + data_lsb);
		//if (data_msb == 0x7f && data_msb == 0x7f) data = 1.0;
		//else data /= 16384.0;
		double data = (double) data_msb;
		if (data < 0.0) data = 0.0; if (data > 127.0) data = 127.0;
		if (data < 64.0) data /= 128.0;
		else data = 0.5 + 0.5 * (data - 64.0) / 63.0;
		switch (nrpn_lsb) {
		case 0: pitch_wheel -> setY (data); break;
		case 1: freq_knob -> setValue (data); break;
		case 2: amp_knob -> setValue (data); break;
		case 3: index_knob -> setValue (data); break;
		case 4: attack_knob -> setValue (data); break;
		case 5: decay_knob -> setValue (data); break;
		case 6: sustain_knob -> setValue (data); break;
		case 7: release_knob -> setValue (data); break;
		case 8: speed_knob -> setValue (data); break;
		case 9: vibrato_knob -> setValue (data); break;
		case 10: tremolo_knob -> setValue (data); break;
		case 11: wah_wah_knob -> setValue (data); break;
		case 12: lfo_pan_knob -> setValue (data); break;
		case 13: vector_x_backup = data; if (separate_vector_coordinates) vector -> setX (data); break;
		case 14: if (separate_vector_coordinates) vector -> setY (data); else vector -> set (vector_x_backup, data); break;
		case 15: volume_knob -> setValue (data); break;
		case 16: pan_knob -> setValue (data); break;
		case 17: break; // hold
		case 18: porta_switch -> setValue (data_msb == 0 && data_lsb == 0 ? 0.0 : 1.0); break;
		case 19: porta_time_knob -> setValue (data); break;
		case 20: break; // porta control
		case 21: dry_wet_knob -> setValue (data); break;
		case 22: break; // global volume
		case 23: mono_poly_switch -> setValue (data_msb == 0 && data_lsb == 0 ? 0.0 : 1.0); break;
		default: break;
		}
	}
	virtual void set_button (int button, double value) {
		if (button >= mode_button_program && button <= mode_button_midi) {mode_buttons [button - mode_button_program] -> setValue (value); return;}
		if (button >= part_button_0 && button <= part_button_9) {part_buttons [button - part_button_0] -> setValue (value); return;}
		if (button >= pb0 && button <= pb17) {parameter_buttons [button - pb0] -> setValue (value); return;}
		if (button >= type_button_freq && button <= type_button_pan) {type_buttons [button - type_button_freq] -> setValue (value); return;}
		if (button >= sensitivity_button_lc && button <= sensitivity_button_rc) {sensitivity_buttons [button - sensitivity_button_lc] -> setValue (value); return;}
		if (button >= extra_1_button && button <= extra_128_button) {times_buttons [button - extra_1_button] -> setValue (value); return;}
		if (button == extra_reset_button) {reset_button -> setValue (value); return;}
		if (button == extra_enter_button) {enter_button -> setValue (value); return;}
		if (button == extra_store_button) {store_button -> setValue (value); return;}
	}
	virtual void change_parameter_name (int parameter, char * name) {
		if (parameter < 0 || parameter >= 18) return;
		parameter_names [parameter] -> SetLabel (towxstring (name));
	}
	virtual void change_parameter_text (void) {parameter_display -> SetLabel (towxstring (parameter_text));}
	virtual void change_status_text (void) {status_display -> SetLabel (towxstring (status_text));}
	virtual void open_file_for_input (char * file_name) {if (audio != NULL) audio -> selectInputFile (file_name);}
	virtual void open_file_for_record (int seconds, char * file_name) {if (audio != NULL) audio -> selectOutputFile (seconds, file_name);}
	virtual void console_print (char * text) {if (command_console != NULL) command_console -> print (text);}
#ifdef WINDOWS_OPERATING_SYSTEM
	virtual void console_operations (int cc, int mm, int ll) {
		if (command_console == NULL) return;
		switch (cc) {
		case 0xb: command_console -> setColors (mm, ll); break;
		default: break;
		}
	}
#endif
#ifdef LINUX_OPERATING_SYSTEM
	virtual void console_operations (int cc, int mm, int ll) {
		if (command_console == NULL) return;
		switch (cc) {
		case 0x01: case 0x02: case 0x03: if (open_close_frame != NULL) open_close_frame -> Show (); break;
		case 0x05: case 0x06: case 0x07: if (open_close_frame != NULL) open_close_frame -> Show (false); break;
		case 0xb: command_console -> setColors (mm, ll); break;
		default: break;
		}
	}
	virtual void text_operations (int selector, char * text) {
		char command [256];
		int fg = core . root -> current_foreground;
		int bg = core . root -> current_background;
		core . root -> setColors (0xff, 0);
		switch (selector) {
		//PROGRAMATICALLY DEFINED MESSAGES:
		case 0x20: core . root -> print ("MANUFACTURER: "); break;
		case 0x21: core . root -> print ("MANUFACTURER'S ADDRESS: "); break;
		case 0x22: core . root -> print ("MANUFACTURER'S POSTAL ADDRESS: "); break;
		case 0x23: core . root -> print ("MANUFACTURER'S PHONE: "); break;
		case 0x24: core . root -> print ("MANUFACTURER'S WEBSITE: "); break;
		case 0x25: core . root -> print ("MANUFACTURER'S EMAIL: "); break;
		case 0x2B: core . root -> print ("PRODUCT: "); break;
		case 0x2C: core . root -> print ("MODEL: "); break;
		case 0x2D: core . root -> print ("VERSION: "); break;
		case 0x2E: core . root -> print ("SERIAL NUMBER: "); break;
		case 0x2F: core . root -> print ("KEY: "); break;
		//USER DEFINED MESSAGES:
		case 0x30: core . root -> print ("OWNER: "); break;
		case 0x31: core . root -> print ("OWNER'S ADDRESS: "); break;
		case 0x32: core . root -> print ("OWNER'S POSTAL ADDRESS: "); break;
		case 0x33: core . root -> print ("OWNER'S PHONE: "); break;
		case 0x34: core . root -> print ("OWNER'S WEBSITE: "); break;
		case 0x35: core . root -> print ("OWNER'S EMAIL: "); break;
		case 0x36: core . root -> print ("WELCOME SCREEN: "); break;
		default: sprintf (command, "CUSTOM %i: ", selector); core . root -> print (command); break;
		}
		core . root -> setColors (0xff00, 0);
		core . root -> print (text);
		core . root -> setColors (fg, bg);
		core . root -> print ("\n");
	}
	virtual void identity_operations (int * identity) {
		if (identity [4] != 2) return;
		int fg = core . root -> current_foreground;
		int bg = core . root -> current_background;
		core . root -> setColors (0xff, 0);
		core . root -> print ("IDENTITY REPLY:");
		core . root -> setColors (0xff00, 0);
		char command [32];
		while (* identity != 0xf7) {
			sprintf (command, " %02X", (int) * identity++);
			core . root -> print (command);
		}
		core . root -> print (" F7");
		core . root -> setColors (fg, bg);
		core . root -> print ("\n");
	}
	virtual void error_operations (int channel, int code, int msb, int lsb, char * error) {
		int fg = core . root -> current_foreground;
		int bg = core . root -> current_background;
		char command [256];
		switch (code) {
		case 0: sprintf (command, "ERROR %i [%i %i %i] general fault: ", code, channel, msb, lsb); break;
		case 1: sprintf (command, "ERROR %i [%i %i %i] wrong parameter: ", code, channel, msb, lsb); break;
		case 2: sprintf (command, "ERROR %i [%i %i %i] file not found: ", code, channel, msb, lsb); break;
		case 3: sprintf (command, "ERROR %i [%i %i %i] subscript wrong: ", code, channel, msb, lsb); break;
		case 4: sprintf (command, "ERROR %i [%i %i %i] not implemented: ", code, channel, msb, lsb); break;
		case 5: sprintf (command, "ERROR %i [%i %i %i] unknown request: ", code, channel, msb, lsb); break;
		case 6: sprintf (command, "ERROR %i [%i %i %i] unknown data: ", code, channel, msb, lsb); break;
		case 7: sprintf (command, "ERROR %i [%i %i %i] not installed: ", code, channel, msb, lsb); break;
		default: sprintf (command, "ERROR %i [%i %i %i] unknown: ", code, channel, msb, lsb); break;
		}
		core . root -> setColors (0xff0000, 0);
		core . root -> print (command);
		core . root -> setColors (0xff00ff, 0);
		core . root -> print (error);
		core . root -> setColors (fg, bg);
		core . root -> print ("\n");
	}
#endif
} panel;

class keyboard_controller : public wxPictureKeyboard {
public:
	virtual void keyon (int key) {
		core . external_midi_in -> insert_keyon (panel . get_transmission_channel (panel . get_channel_extension ()), key, 100);
	}
	virtual void keyoff (int key) {
		core . external_midi_in -> insert_keyoff (panel . get_transmission_channel (panel . get_channel_extension ()), key);
	}
	keyboard_controller (wxWindow * parent, wxWindowID id, wxBitmap * bitmap, int size_selector) : wxPictureKeyboard (parent, id, bitmap, size_selector) {}
};

#ifdef MAC_OPERATING_SYSTEM_BAK
#define menu_height 24

////////////////
// midi start //
////////////////

#include <CoreMIDI/MIDIServices.h>

MIDIClientRef midi_client = NULL;

MIDIEndpointRef external_main_midi_in_endpoint = NULL;
MIDIEndpointRef external_main_midi_out_endpoint = NULL;
MIDIEndpointRef external_console_midi_in_endpoint = NULL;
MIDIEndpointRef external_console_midi_out_endpoint = NULL;

MIDIEndpointRef internal_main_midi_in_endpoint = NULL;
MIDIEndpointRef internal_main_midi_out_endpoint = NULL;
MIDIEndpointRef internal_console_midi_in_endpoint = NULL;
MIDIEndpointRef internal_console_midi_out_endpoint = NULL;

MIDIPortRef main_midi_in_port = NULL;
MIDIPortRef main_midi_out_port = NULL;
MIDIPortRef console_midi_in_port = NULL;
MIDIPortRef console_midi_out_port = NULL;

bool console_external = false;
bool main_external = false;

void main_midi_in_proc (const MIDIPacketList * pktlist, void * readProcRefCon, void * srcConnRefCon) {
	const MIDIPacket * packet = pktlist -> packet;
	for (int ind = 0; ind < pktlist -> numPackets; ind++) {
		switch (packet -> length) {
			case 1: core . external_midi_in -> insert (packet -> data [0]); break;
			case 2: core . external_midi_in -> insert (packet -> data [0], packet -> data [1]); break;
			case 3: core . external_midi_in -> insert (packet -> data [0], packet -> data [1], packet -> data [2]); break;
			default: for (int sub = 0; sub < packet -> length; sub++) core . external_midi_in -> insert (packet -> data [sub]); break;
		}
		packet = MIDIPacketNext (packet);
	}
}

void console_midi_in_proc (const MIDIPacketList * pktlist, void * readProcRefCon, void * srcConnRefCon) {
	const MIDIPacket * packet = pktlist -> packet;
	for (int ind = 0; ind < pktlist -> numPackets; ind++) {
		switch (packet -> length) {
			case 1: core . conn_midi_in -> insert (packet -> data [0]); break;
			case 2: core . conn_midi_in -> insert (packet -> data [0], packet -> data [1]); break;
			case 3: core . conn_midi_in -> insert (packet -> data [0], packet -> data [1], packet -> data [2]); break;
			default:
				wxMessageBox (_T ("sonda"), _T ("INFO"), wxOK, NULL);
				printf ("incoming <= [");
				for (int sub = 0; sub < packet -> length; sub++) {
					core . conn_midi_in -> insert (packet -> data [sub]);
					printf ("%02x ", packet -> data [sub]);
				}
				printf ("]\n");
				break;
		}
		packet = MIDIPacketNext (packet);
	}
}

void prolog_print (midi_stream * line) {AREA area; line -> get (area); printf (area);}

class dx_out_class : public midi_reader {
private:
	bool console;
	void emit (MIDIPacketList * packet) {
		if (console) {
			if (console_external) MIDISend (console_midi_out_port, external_console_midi_out_endpoint, packet);
			else MIDIReceived (internal_console_midi_out_endpoint, packet);
			//			MIDISend (console_midi_out_port, console_external ? external_console_midi_out_endpoint : internal_console_midi_out_endpoint, packet);
			//			printf ("I");
		} else {
			if (main_external) MIDISend (main_midi_out_port, external_main_midi_out_endpoint, packet);
			else MIDIReceived (internal_main_midi_out_endpoint, packet);
		}
	}
	void send_two (Byte one, Byte two) {
		MIDIPacketList packet;
		packet . numPackets = 1;
		packet . packet [0] . timeStamp = 0;
		packet . packet [0] . length = 2;
		packet . packet [0] . data [0] = one;
		packet . packet [0] . data [1] = two;
		emit (& packet);
	}
	void send_three (Byte one, Byte two, Byte three) {
		MIDIPacketList packet;
		packet . numPackets = 1;
		packet . packet [0] . timeStamp = 0;
		packet . packet [0] . length = 3;
		packet . packet [0] . data [0] = one;
		packet . packet [0] . data [1] = two;
		packet . packet [0] . data [2] = three;
		emit (& packet);
	}
public:
	virtual void midi_keyoff (int channel, int key) {send_three (0x80 + channel, key, 0);}
	virtual void midi_keyon (int channel, int key, int velocity) {send_three (0x90 + channel, key, velocity);}
	virtual void midi_pat (int channel, int key, int value) {send_three (0xa0 + channel, key, value);}
	virtual void midi_control (int channel, int controller, int value) {send_three (0xb0 + channel, controller, value);}
	virtual void midi_programchange (int channel, int program) {send_two (0xc0 + channel, program);}
	virtual void midi_cat (int channel, int value) {send_two (0xd0 + channel, value);}
	virtual void midi_pitchbend (int channel, int v1, int v2) {send_three (0xe0 + channel, v1, v2);}
	virtual void midi_parameter_request (int channel) {send_two (0xf1, channel);}
	virtual void midi_timing_clock (void) {
		MIDIPacketList packet;
		packet . numPackets = 1;
		packet . packet [0] . timeStamp = 0;
		packet . packet [0] . length = 1;
		packet . packet [0] . data [0] = 0xf8;
		emit (& packet);
	}
	virtual void midi_system_exclusive (midi_stream * line) {
		if (console) {
			line -> mark ();
			if (line -> check_system_exclusive ()) {
				if (line -> get () == 0x2a) {
					prolog_print (line);
					//					return;
				}
			}
			line -> restore ();
		}
		MIDIPacketList packet;
		packet . numPackets = 1;
		packet . packet [0] . timeStamp = 0;
		int ind = 0;
		packet . packet [0] . data [ind++] = 0xf0;
		int data = line -> get ();
		printf ("outgoing => [F0 ");
		while (data != 0xf7) {
			packet . packet [0] . data [ind++] = (Byte) data;
			printf ("%02X ", data);
			if (ind > 250) return;
			data = line -> get ();
		}
		packet . packet [0] . data [ind++] = 0xf7;
		packet . packet [0] . length = ind;
		printf ("F7]\n");
		emit (& packet);
	}
	dx_out_class (bool console) {this -> console = console;}
};
dx_out_class dx_console_transmitter (true);
dx_out_class dx_transmitter (false);

static int active_main_midi_input = -1;
static int active_main_midi_output = -1;
static int active_console_midi_input = -1;
static int active_console_midi_output = -1;

void set_main_midi_input (int ind) {
	if (ind < 0) ind = -1;
	if (main_midi_in_port == NULL) return;
	active_main_midi_input = ind;
	if (ind < 0) {
		if (internal_main_midi_in_endpoint == NULL) return;
		MIDIPortConnectSource (main_midi_in_port, internal_main_midi_in_endpoint, NULL);
		return;
	}
	external_main_midi_in_endpoint = MIDIGetSource (ind);
	if (external_main_midi_in_endpoint == NULL) return;
	MIDIPortConnectSource (main_midi_in_port, external_main_midi_in_endpoint, NULL);
}

void set_main_midi_output (int ind) {
	if (ind < 0) ind = -1;
	active_main_midi_output = ind;
	if (ind < 0) {main_external = false; return;}
	main_external = true;
	external_main_midi_out_endpoint = MIDIGetDestination (ind);
}

void set_console_midi_input (int ind) {
	if (ind < 0) ind = -1;
	if (console_midi_in_port == NULL) return;
	active_console_midi_input = ind;
	if (ind < 0) {
		if (internal_console_midi_in_endpoint == NULL) return;
		MIDIPortConnectSource (console_midi_in_port, internal_console_midi_in_endpoint, NULL);
		return;
	}
	external_console_midi_in_endpoint = MIDIGetSource (ind);
	if (external_console_midi_in_endpoint == NULL) return;
	MIDIPortConnectSource (console_midi_in_port, external_console_midi_in_endpoint, NULL);
}

void set_console_midi_output (int ind) {
	if (ind < 0) ind = -1;
	active_console_midi_output = ind;
	if (ind < 0) {console_external = false; return;}
	console_external = true;
	external_main_midi_out_endpoint = MIDIGetDestination (ind);
}

void midi_setup (config * cfg) {
	if (MIDIClientCreate (CFSTR ("HERCs CORE"), NULL, 0, & midi_client)) return;
	MIDIInputPortCreate (midi_client, CFSTR ("HERCs main midi in port"), main_midi_in_proc, NULL, & main_midi_in_port);
	MIDIOutputPortCreate (midi_client, CFSTR ("HERCs main midi out port"), & main_midi_out_port);
	MIDIInputPortCreate (midi_client, CFSTR ("HERCs console midi in port"), console_midi_in_proc, NULL, & console_midi_in_port);
	MIDIOutputPortCreate (midi_client, CFSTR ("HERCs console midi out port"), & console_midi_out_port);
	MIDIDestinationCreate (midi_client, CFSTR ("HERCs Main MIDI IN"), main_midi_in_proc, NULL, & internal_main_midi_in_endpoint);
	MIDISourceCreate (midi_client, CFSTR ("HERCs Main MIDI OUT"), & internal_main_midi_out_endpoint);
	MIDIDestinationCreate (midi_client, CFSTR ("HERCs Console MIDI IN"), console_midi_in_proc, NULL, & internal_console_midi_in_endpoint);
	MIDISourceCreate (midi_client, CFSTR ("HERCs Console MIDI OUT"), & internal_console_midi_out_endpoint);
	set_main_midi_input (cfg -> midi_in);
	set_main_midi_output (cfg -> midi_out);
	set_console_midi_input (cfg -> console_midi_in);
	set_console_midi_output (cfg -> console_midi_out);
}

static wxString cf_to_wx (CFStringRef name) {
	wxString ret = _T ("");
	int ind = 0;
	while (CFStringGetCharacterAtIndex (name, ind) != '\0') ret += CFStringGetCharacterAtIndex (name, ind++);
	ret += wxChar ('\0');
	return ret;
}

class MidiHardwareDialogClass : public wxDialog {
public:
	wxComboBox * main_input_combo;
	wxComboBox * main_output_combo;
	wxComboBox * console_input_combo;
	wxComboBox * console_output_combo;
	MidiHardwareDialogClass (wxWindow * parent) : wxDialog (parent, -1, wxString :: Format (_T ("MIDI Hardware"))) {
		int ind, sentinel;
		CFStringRef name;
		wxFlexGridSizer * grid = new wxFlexGridSizer (2, 4, 0, 0);
		grid -> Add (new wxStaticText (this, -1, _T ("Main Input")));
		main_input_combo = new wxComboBox (this, 101, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		main_input_combo -> Append (_T ("Internal"));
		sentinel = MIDIGetNumberOfSources ();
		for (ind = 0; ind < sentinel; ind++) {
			MIDIObjectGetStringProperty (MIDIGetSource (ind), kMIDIPropertyName, & name);
			main_input_combo -> Append (cf_to_wx (name));
		}
		main_input_combo -> SetSelection (active_main_midi_input + 1);
		grid -> Add (main_input_combo);
		grid -> Add (new wxStaticText (this, -1, _T ("Main Output")));
		main_output_combo = new wxComboBox (this, 102, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		main_output_combo -> Append (_T ("Internal"));
		sentinel = MIDIGetNumberOfDestinations ();
		for (ind = 0; ind < sentinel; ind++) {
			MIDIObjectGetStringProperty (MIDIGetDestination (ind), kMIDIPropertyName, & name);
			main_output_combo -> Append (cf_to_wx (name));
		}
		main_output_combo -> SetSelection (active_main_midi_output + 1);
		grid -> Add (main_output_combo);
		grid -> Add (new wxStaticText (this, -1, _T ("Console Input")));
		console_input_combo = new wxComboBox (this, 103, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		console_input_combo -> Append (_T ("Internal"));
		sentinel = MIDIGetNumberOfSources ();
		for (ind = 0; ind < sentinel; ind++) {
			MIDIObjectGetStringProperty (MIDIGetSource (ind), kMIDIPropertyName, & name);
			console_input_combo -> Append (cf_to_wx (name));
		}
		console_input_combo -> SetSelection (active_console_midi_input + 1);
		grid -> Add (console_input_combo);
		grid -> Add (new wxStaticText (this, -1, _T ("Console Output")));
		console_output_combo = new wxComboBox (this, 104, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		console_output_combo -> Append (_T ("Internal"));
		sentinel = MIDIGetNumberOfDestinations ();
		for (ind = 0; ind < sentinel; ind++) {
			MIDIObjectGetStringProperty (MIDIGetDestination (ind), kMIDIPropertyName, & name);
			console_output_combo -> Append (cf_to_wx (name));
		}
		console_output_combo -> SetSelection (active_console_midi_output + 1);
		grid -> Add (console_output_combo);
		SetSizer (grid);
		grid -> SetSizeHints (this);
		Centre ();
	}
	bool midi_check (int ind, bool out) {
		if (ind < 0) return true;
		CFStringRef name;
		MIDIObjectGetStringProperty (out ? MIDIGetDestination (ind) : MIDIGetSource (ind), kMIDIPropertyName, & name);
		if (CFStringFind (name, CFSTR ("HERCs"), 0) . location != kCFNotFound) {
			wxMessageBox (_T ("Non-selectable port"), _T ("INFO"), wxOK, this);
			return false;
		}
		return true;
	}
	void OnMainInput (wxCommandEvent & event) {
		int ind = main_input_combo -> GetSelection () - 1;
		if (midi_check (ind, false)) set_main_midi_input (ind);
		else main_input_combo -> SetSelection (active_main_midi_input + 1);
	}
	void OnMainOutput (wxCommandEvent & event) {
		int ind = main_output_combo -> GetSelection () - 1;
		if (midi_check (ind, true)) set_main_midi_output (ind);
		else main_output_combo -> SetSelection (active_main_midi_output + 1);
	}
	void OnConsoleInput (wxCommandEvent & event) {
		int ind = console_input_combo -> GetSelection () - 1;
		if (midi_check (ind, false)) set_console_midi_input (ind);
		else console_input_combo -> SetSelection (active_console_midi_input + 1);
	}
	void OnConsoleOutput (wxCommandEvent & event) {
		int ind = console_output_combo -> GetSelection () - 1;
		if (midi_check (ind, true)) set_console_midi_output (ind);
		else console_output_combo -> SetSelection (active_console_midi_output + 1);
	}
private: DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(MidiHardwareDialogClass, wxDialog)
EVT_COMBOBOX(101, MidiHardwareDialogClass :: OnMainInput)
EVT_COMBOBOX(102, MidiHardwareDialogClass :: OnMainOutput)
EVT_COMBOBOX(103, MidiHardwareDialogClass :: OnConsoleInput)
EVT_COMBOBOX(104, MidiHardwareDialogClass :: OnConsoleOutput)
END_EVENT_TABLE()

//////////////
// midi end //
//////////////

////////////////
// jack start //
////////////////

int callback_return = 0;
/*
#include <jack/jack.h>

jack_client_t * jack_client = NULL;
jack_port_t * jack_left_out = NULL;
jack_port_t * jack_right_out = NULL;

typedef jack_default_audio_sample_t sample_t;
int jack_callback (jack_nframes_t nframes, void * args) {
	if (callback_return != 0) return callback_return;
	sample_t * left_buffer = (sample_t *) jack_port_get_buffer (jack_left_out, nframes);
	sample_t * right_buffer = (sample_t *) jack_port_get_buffer (jack_right_out, nframes);
	//	core . conn_move (& info);
	core . conn_move (& dx_console_transmitter);
	double dv;
	while (nframes-- > 0) {
		if (core . move ()) callback_return = -1;
		dv = core . left_out ();
		if (dv > 1.0) dv = 1.0;
		if (dv < -1.0) dv = -1.0;
		* left_buffer++ = dv;
		dv = core . right_out ();
		if (dv > 1.0) dv = 1.0;
		if (dv < -1.0) dv = -1.0;
		* right_buffer++ = dv;
		dx_transmitter . read (core . external_midi_out);
	}
	//	dx_console_transmitter . read (core . conn_midi_out);
//	panel . read (& console_feedback);
	return callback_return;
}

bool jack_start (config * cfg) {
	jack_client = jack_client_new ("HERCs CORE");
	if (jack_client == NULL) {
		if (audio == NULL) audio = new MultiplatformAudio (NULL, 2, 44100, 256);
		audio -> installOutputCallback (core_audio_callback);
		audio -> selectOutputDevice (0);
		return true;
	}
	jack_set_process_callback (jack_client, jack_callback, 0);
	jack_left_out = jack_port_register (jack_client, "left out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	jack_right_out = jack_port_register (jack_client, "right out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	jack_activate (jack_client);
	return true;
}

void jack_stop (void) {
	if (jack_client != NULL) jack_deactivate (jack_client);
	if (audio != NULL) delete audio; audio = NULL;
}
*/
//////////////
// jack end //
//////////////

void console_midi_transmission (void) {core . conn_move (& dx_console_transmitter);}
void external_midi_transmission (void) {dx_transmitter . read (core . external_midi_out);}
void general_quit (void) {callback_return = -1;}

//#define open_audio jack_start
bool open_audio (config * cfg) {
	if (audio == NULL) audio = new MultiplatformAudio (NULL, 2, cfg -> sampling_freq, cfg -> latency_block);
	audio -> installOutputCallback (core_audio_output_callback);
	audio -> selectOutputDevice (0);
	audio -> installInputCallback (core_audio_input_callback);
	audio -> selectInputDevice (0);
	return true;
}
//#define close_audio jack_stop
void close_audio (void) {if (audio != NULL) delete audio; audio = NULL;}

#endif

#ifdef LINUX_OPERATING_SYSTEM

#define menu_height 24

//prolog_midi_reader * midi_reader = NULL;
static int active_main_midi_input = -1;
static int active_main_midi_output = -1;
static int active_console_midi_input = -1;
static int active_console_midi_output = -1;
void set_main_midi_input (int ind) {
	printf ("setting midi input to %i.\n", ind);
	if (ind < 0) ind = -1;
	// to do
//	midi_service . setInputPort (ind);
	active_main_midi_input = ind;
}
void set_main_midi_output (int ind) {
	printf ("setting midi output to %i.\n", ind);
	if (ind < 0) ind = -1;
	// to do
//	midi_service . setOutputPort (ind);
	active_main_midi_output = ind;
	// to do
//	core . conn_midi_feed -> connect_thru (midi_service . getTransmissionLine ());
}

void set_console_midi_input (int ind) {}
void set_console_midi_output (int ind) {}

class MidiHardwareDialogClass : public wxDialog {
public:
	wxComboBox * main_input_combo;
	wxComboBox * main_output_combo;
	wxComboBox * console_input_combo;
	wxComboBox * console_output_combo;
	MidiHardwareDialogClass (wxWindow * parent) : wxDialog (parent, -1, wxString :: Format (_T ("MIDI Hardware"))) {
		int ind, sentinel;
		wxFlexGridSizer * grid = new wxFlexGridSizer (2, 4, 0, 0);
		grid -> Add (new wxStaticText (this, -1, _T ("Main Input")));
		main_input_combo = new wxComboBox (this, 101, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		main_input_combo -> Append (_T ("Internal"));
		// to do
//		sentinel = midi_service . getNumberOfInputs ();
//		for (ind = 0; ind < sentinel; ind++) {
//			main_input_combo -> Append (wxString :: From8BitData (midi_service . getInputInfo (ind)));
//		}
		main_input_combo -> SetSelection (active_main_midi_input + 1);
		grid -> Add (main_input_combo);
		grid -> Add (new wxStaticText (this, -1, _T ("Main Output")));
		main_output_combo = new wxComboBox (this, 102, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		main_output_combo -> Append (_T ("Internal"));
		// to do
//		sentinel = midi_service . getNumberOfOutputs ();
//		for (ind = 0; ind < sentinel; ind++) {
//			main_output_combo -> Append (wxString :: From8BitData (midi_service . getOutputInfo (ind)));
//		}
		main_output_combo -> SetSelection (active_main_midi_output + 1);
		grid -> Add (main_output_combo);
		grid -> Add (new wxStaticText (this, -1, _T ("Console Input")));
		console_input_combo = new wxComboBox (this, 103, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		console_input_combo -> Append (_T ("Internal"));
		// to do
//		sentinel = midi_service . getNumberOfInputs ();
//		for (ind = 0; ind < sentinel; ind++) {
//			console_input_combo -> Append (wxString :: From8BitData (midi_service . getInputInfo (ind)));
//		}
		console_input_combo -> SetSelection (active_console_midi_input + 1);
		grid -> Add (console_input_combo);
		grid -> Add (new wxStaticText (this, -1, _T ("Console Output")));
		console_output_combo = new wxComboBox (this, 104, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		console_output_combo -> Append (_T ("Internal"));
		// to do
//		sentinel = midi_service . getNumberOfOutputs ();
//		for (ind = 0; ind < sentinel; ind++) {
//			console_output_combo -> Append (wxString :: From8BitData (midi_service . getOutputInfo (ind)));
//		}
		console_output_combo -> SetSelection (active_console_midi_output + 1);
		grid -> Add (console_output_combo);
		SetSizer (grid);
		grid -> SetSizeHints (this);
		Centre ();
	}
	bool midi_check (int ind, bool out) {return true;}
	void OnMainInput (wxCommandEvent & event) {
		int ind = main_input_combo -> GetSelection () - 1;
		if (midi_check (ind, false)) set_main_midi_input (ind);
		else main_input_combo -> SetSelection (active_main_midi_input + 1);
	}
	void OnMainOutput (wxCommandEvent & event) {
		int ind = main_output_combo -> GetSelection () - 1;
		if (midi_check (ind, true)) set_main_midi_output (ind);
		else main_output_combo -> SetSelection (active_main_midi_output + 1);
	}
	void OnConsoleInput (wxCommandEvent & event) {
		int ind = console_input_combo -> GetSelection () - 1;
		if (midi_check (ind, false)) set_console_midi_input (ind);
		else console_input_combo -> SetSelection (active_console_midi_input + 1);
	}
	void OnConsoleOutput (wxCommandEvent & event) {
		int ind = console_output_combo -> GetSelection () - 1;
		if (midi_check (ind, true)) set_console_midi_output (ind);
		else console_output_combo -> SetSelection (active_console_midi_output + 1);
	}
private: DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(MidiHardwareDialogClass, wxDialog)
EVT_COMBOBOX(101, MidiHardwareDialogClass :: OnMainInput)
EVT_COMBOBOX(102, MidiHardwareDialogClass :: OnMainOutput)
EVT_COMBOBOX(103, MidiHardwareDialogClass :: OnConsoleInput)
EVT_COMBOBOX(104, MidiHardwareDialogClass :: OnConsoleOutput)
END_EVENT_TABLE()

bool open_audio (config * cfg) {
	audio = new MultiplatformAudio (NULL, 2, cfg -> sampling_freq, cfg -> latency_block);
	audio -> installOutputCallback (core_audio_output_callback);
	audio -> selectOutputDevice (0);
	audio -> installInputCallback (core_audio_input_callback);
	audio -> selectInputDevice (0);
	return true;
}

void midi_setup (config * cfg) {
	set_main_midi_input (cfg -> midi_in);
	set_main_midi_output (cfg -> midi_out);
	set_console_midi_input (cfg -> console_midi_in);
	set_console_midi_output (cfg -> console_midi_out);
	//midi_reader = new prolog_midi_reader (core . root);
	//midi_service . set_reader (midi_reader);
}


void close_audio (void) {if (audio != NULL) delete audio; audio = NULL;}

void external_midi_transmission (void) {}

void console_midi_transmission (void) {core . conn_move ();}

#endif

#ifdef WINDOWS_OPERATING_SYSTEM

class MidiHardwareDialogClass : public wxDialog {
public:
	wxComboBox * main_input_combo;
	wxComboBox * main_output_combo;
	wxComboBox * console_input_combo;
	wxComboBox * console_output_combo;
	MidiHardwareDialogClass (wxWindow * parent) : wxDialog (parent, -1, wxString :: Format (_T ("MIDI Hardware"))) {
		int ind, sentinel;
//		CFStringRef name;
		wxFlexGridSizer * grid = new wxFlexGridSizer (2, 4, 0, 0);
		grid -> Add (new wxStaticText (this, -1, _T ("Main Input")));
		main_input_combo = new wxComboBox (this, 101, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		main_input_combo -> Append (_T ("Internal"));
		sentinel = midi_service . getNumberOfInputs ();
		for (ind = 0; ind < sentinel; ind++) main_input_combo -> Append (wxString :: Format (_T ("%s"), midi_service . getInputInfo (ind)));
		main_input_combo -> SetSelection (midi_service . getInputPort () + 1);
		grid -> Add (main_input_combo);
		grid -> Add (new wxStaticText (this, -1, _T ("Main Output")));
		main_output_combo = new wxComboBox (this, 102, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		main_output_combo -> Append (_T ("Internal"));
		sentinel = midi_service . getNumberOfOutputs ();
		for (ind = 0; ind < sentinel; ind++) main_output_combo -> Append (wxString :: Format (_T ("%s"), midi_service . getOutputInfo (ind)));
		main_output_combo -> SetSelection (midi_service . getOutputPort () + 1);
		grid -> Add (main_output_combo);
		grid -> Add (new wxStaticText (this, -1, _T ("Console Input")));
		console_input_combo = new wxComboBox (this, 103, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		console_input_combo -> Append (_T ("Internal"));
		sentinel = console_midi_service . getNumberOfInputs ();
		for (ind = 0; ind < sentinel; ind++) console_input_combo -> Append (wxString :: Format (_T ("%s"), console_midi_service . getInputInfo (ind)));
		console_input_combo -> SetSelection (console_midi_service . getInputPort () + 1);
		grid -> Add (console_input_combo);
		grid -> Add (new wxStaticText (this, -1, _T ("Console Output")));
		console_output_combo = new wxComboBox (this, 104, _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
		console_output_combo -> Append (_T ("Internal"));
		sentinel = console_midi_service . getNumberOfOutputs ();
		for (ind = 0; ind < sentinel; ind++) console_output_combo -> Append (wxString :: Format (_T ("%s"), console_midi_service . getOutputInfo (ind)));
		console_output_combo -> SetSelection (console_midi_service . getOutputPort () + 1);
		grid -> Add (console_output_combo);
		SetSizer (grid);
		grid -> SetSizeHints (this);
		Centre ();
	}
	void OnMainInput (wxCommandEvent & event) {midi_service . setInputPort (main_input_combo -> GetSelection () - 1);}
	void OnMainOutput (wxCommandEvent & event) {midi_service . setOutputPort (main_output_combo -> GetSelection () - 1);}
	void OnConsoleInput (wxCommandEvent & event) {console_midi_service . setInputPort (console_input_combo -> GetSelection () - 1);}
	void OnConsoleOutput (wxCommandEvent & event) {console_midi_service . setOutputPort (console_output_combo -> GetSelection () - 1);}
private: DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(MidiHardwareDialogClass, wxDialog)
EVT_COMBOBOX(101, MidiHardwareDialogClass :: OnMainInput)
EVT_COMBOBOX(102, MidiHardwareDialogClass:: OnMainOutput)
EVT_COMBOBOX(103, MidiHardwareDialogClass :: OnConsoleInput)
EVT_COMBOBOX(104, MidiHardwareDialogClass :: OnConsoleOutput)
END_EVENT_TABLE()

#endif

void button_callbackx (int controller_id, double value) {
	if (controller_id >= part_button_0 && controller_id <= part_button_9) {panel . press_part_button_dx (controller_id - part_button_0); return;}
	if (controller_id >= mode_button_program && controller_id <= mode_button_midi) {panel . press_mode_button (controller_id - mode_button_program); return;}
	if (controller_id >= type_button_freq && controller_id <= type_button_pan) {panel . press_type_button (controller_id - type_button_freq); return;}
	if (controller_id >= sensitivity_button_lc && controller_id <= sensitivity_button_rc) {panel . press_sensitivity_button (controller_id - sensitivity_button_lc); return;}
	if (controller_id >= pb0 && controller_id <= pb17) {panel . press_parameter_button_dx (controller_id - pb0); return;}
	if (controller_id >= extra_1_button && controller_id <= extra_128_button) {panel . press_zoom_button (controller_id - extra_1_button); return;}
	if (controller_id == extra_inc_button) {if (value != 0) panel . encoder_change (core . conn_midi_in, 1); return;}
	if (controller_id == extra_dec_button) {if (value != 0) panel . encoder_change (core . conn_midi_in, -1); return;}
	if (controller_id >= extra_reset_button && controller_id <= extra_store_button) {panel . press_command_button (controller_id - extra_reset_button); return;}
}

void button_callback (int controller_id, double value) {
	button_callbackx (controller_id, value);
	if (controller_id >= extra_button) return;
	panel . calculate_nrpn_from_mode ();
	//if (controller_id < part_button_0 || controller_id > part_button_9) panel . send_parameter_request ();
	panel . send_parameter_request ();
}

void encoder_callback (int controller_id, double value) {panel . encoder_change (core . conn_midi_in, (int) value);}

void controller_callback (int controller_id, double value) {panel . controller_change (controller_id, value);}

#define ROW_1 10
#define ROW_2 30
#define ROW_3 50
#define ROW_4 70
#define ROW_5 90
#define ROW_6 110
#define ROW_7 130
#define ROW_8 150
#define ROW_9 170
#define ROW_10 190

#define ROW_START 30


#include "core_layout.h"

enum {
	showmicrokeyboardaction = 101,
	showminikeyboardaction,
	showkeyboardaction,
	showcontrollersaction,
	showoscilloscopeaction,
	showaudiohardwareaction,
	showmidihardwareaction,
	showexit
};

class KeyboardDialogClass : public wxDialog {
public:
	keyboard_controller * keyboard;
	KeyboardDialogClass (wxWindow * parent, wxBitmap * bitmap, int size_selector) : wxDialog (parent, -1, wxString :: Format (_T ("Keyboard"))) {
		wxBoxSizer * grid = new wxBoxSizer (wxHORIZONTAL);
		keyboard = new keyboard_controller (this, -1, bitmap, size_selector);
		grid -> Add (keyboard, wxEXPAND | wxALL);
		SetSizer (grid);
		grid -> SetSizeHints (this);
		Centre ();
	}
};
KeyboardDialogClass * micro_keyboard_dialog = NULL;
KeyboardDialogClass * mini_keyboard_dialog = NULL;
KeyboardDialogClass * keyboard_dialog = NULL;


/*
class ControllersWindowClass : public wxWindow {
public:
	ControllersWindowClass (wxWindow * parent, wxWindowID id) : wxWindow (parent, id) {// (parent, -1, wxString :: Format (_T ("Controllers"))) {
		keyboard_controller * keyboard = new keyboard_controller (this, -1, & picture_keyboard, 2);
		keyboard -> SetSize (20, picture_controls . GetHeight () - 140, -1, -1);
		wxAnimatedVector * v = new wxAnimatedVector (this, -1, & vector_handle, 143, 143);
		v -> setMotionRatio (0.2) -> setX (0.5) -> setY (0.5) -> SetSize (picture_controls . GetWidth () - 180, 20, -1, -1);
//		SetSize (300, 200);
//		Centre ();
	}
	void OnEraseBackground (wxEraseEvent & event) {}
	void OnPaint (wxPaintEvent & event) {wxPaintDC dc (this); dc . DrawBitmap (picture_controls, 0, 0, true);}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(ControllersWindowClass, wxWindow)
EVT_ERASE_BACKGROUND(ControllersWindowClass :: OnEraseBackground)
EVT_PAINT(ControllersWindowClass :: OnPaint)
END_EVENT_TABLE()

class ControllersDialogClass : public wxDialog {
public:
	ControllersDialogClass (wxWindow * parent) : wxDialog (parent, -1, wxString :: Format (_T ("Controllers"))) {
		new ControllersWindowClass (this, -1);
		SetSize (picture_controls . GetWidth (), picture_controls . GetHeight ());
		Centre ();
	}
};

ControllersDialogClass * control_dialog = NULL;
*/

class EditorFrame;

#include "wx/dnd.h"

class FileReceiver : public wxFileDropTarget {
public:
	virtual bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString & files) {
		for (int ind = 0; ind < (int) files . GetCount (); ind++) {
			char file_name [1024];
			char command [1024];
			int sub = 0;
			wxString wx_file_name = files [ind];
			wx_file_name . Replace (_T ("\\"), _T ("\\\\"));
			while (sub < (int) wx_file_name . length ()) {
				file_name [sub] = wx_file_name [sub];
				sub++;
			}
			file_name [sub++] = '\0';
			sprintf (command, "[drag_and_drop \"%s\"]", file_name);
			core . root -> insertCommand (command);
		}
		return true;
	}
};

class EditorFrame : public wxFrame {
public:
	wxMenuBar * bar;
	EditorFrame (wxWindow * parent, wxWindowID id, const wxString & title) :
	wxFrame (parent, id, title, wxPoint (10, 180), wxDefaultSize, wxDEFAULT_FRAME_STYLE) {// & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX)) {
//	wxFrame (parent, id, title, wxPoint (1710, 180), wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX)) {
		//wxMenuBar * bar = new wxMenuBar ();
		/*bar = new wxMenuBar ();
		wxMenu * viewMenu = new wxMenu;
//		viewMenu -> Append (showmicrokeyboardaction, _T ("Micro Keyboard"), _T (""));
//		viewMenu -> Append (showminikeyboardaction, _T ("Mini Keyboard"), _T (""));
//		viewMenu -> Append (showkeyboardaction, _T ("Keyboard"), _T (""));
//		viewMenu -> Append (showcontrollersaction, _T ("Controllers"), _T (""));
//		viewMenu -> Append (showoscilloscopeaction, _T ("Oscilloscope"), _T (""));
		viewMenu -> Append (showaudiohardwareaction, _T ("Audio Hardware"), _T (""));
		viewMenu -> Append (showmidihardwareaction, _T ("MIDI Hardware"), _T (""));
		viewMenu -> Append (showexit, _T ("EXIT"), _T (""));
		bar -> Append (viewMenu, _T ("Options"));
		SetMenuBar (bar);*/
		main_panel * mp = new main_panel (this); mp -> SetDropTarget (new FileReceiver ()); mp -> SetFocus ();
		SetSize (wxSize (mp -> front_panel . GetWidth (), mp -> front_panel . GetHeight () + menu_height));
//		SetSize (wxSize (100, 10 + menu_height));
#ifdef WINDOWS_OPERATING_SYSTEM
		wxAcceleratorEntry accelerators [84];
		accelerators [0] . Set (wxACCEL_NORMAL, WXK_F1, 5001);
		accelerators [1] . Set (wxACCEL_NORMAL, WXK_F2, 5002);
		accelerators [2] . Set (wxACCEL_NORMAL, WXK_F3, 5003);
		accelerators [3] . Set (wxACCEL_NORMAL, WXK_F4, 5004);
		accelerators [4] . Set (wxACCEL_NORMAL, WXK_F5, 5005);
		accelerators [5] . Set (wxACCEL_NORMAL, WXK_F6, 5006);
		accelerators [6] . Set (wxACCEL_NORMAL, WXK_F7, 5007);
		accelerators [7] . Set (wxACCEL_NORMAL, WXK_F8, 5008);
		accelerators [8] . Set (wxACCEL_NORMAL, WXK_F9, 5009);
		accelerators [9] . Set (wxACCEL_NORMAL, WXK_F10, 5010);
		accelerators [10] . Set (wxACCEL_NORMAL, WXK_F11, 5011);
		accelerators [11] . Set (wxACCEL_NORMAL, WXK_F12, 5012);
		accelerators [12] . Set (wxACCEL_NORMAL, (int) '0', 5022);
		accelerators [13] . Set (wxACCEL_NORMAL, (int) '`', 5022);
		accelerators [14] . Set (wxACCEL_NORMAL, (int) '1', 5013);
		accelerators [15] . Set (wxACCEL_NORMAL, (int) '2', 5014);
		accelerators [16] . Set (wxACCEL_NORMAL, (int) '3', 5015);
		accelerators [17] . Set (wxACCEL_NORMAL, (int) '4', 5016);
		accelerators [18] . Set (wxACCEL_NORMAL, (int) '5', 5017);
		accelerators [19] . Set (wxACCEL_NORMAL, (int) '6', 5018);
		accelerators [20] . Set (wxACCEL_NORMAL, (int) '7', 5019);
		accelerators [21] . Set (wxACCEL_NORMAL, (int) '8', 5020);
		accelerators [22] . Set (wxACCEL_NORMAL, (int) '9', 5021);
		accelerators [23] . Set (wxACCEL_CTRL, WXK_F5, 5023);
		accelerators [24] . Set (wxACCEL_CTRL, WXK_F6, 5024);
		accelerators [25] . Set (wxACCEL_CTRL, WXK_F7, 5025);
		accelerators [26] . Set (wxACCEL_CTRL, WXK_F8, 5026);
		accelerators [27] . Set (wxACCEL_CTRL, WXK_F9, 5027);
		accelerators [28] . Set (wxACCEL_CTRL, WXK_F10, 5028);
		accelerators [29] . Set (wxACCEL_CTRL, WXK_F11, 5029);
		accelerators [30] . Set (wxACCEL_NORMAL, (int) 'Q', 5030);
		accelerators [31] . Set (wxACCEL_NORMAL, (int) 'W', 5031);
		accelerators [32] . Set (wxACCEL_NORMAL, (int) 'E', 5032);
		accelerators [33] . Set (wxACCEL_NORMAL, (int) 'R', 5033);
		accelerators [34] . Set (wxACCEL_NORMAL, (int) 'T', 5034);
		accelerators [35] . Set (wxACCEL_NORMAL, (int) 'Y', 5035);
		accelerators [36] . Set (wxACCEL_NORMAL, (int) 'U', 5036);
		accelerators [37] . Set (wxACCEL_NORMAL, (int) 'I', 5037);
		accelerators [38] . Set (wxACCEL_NORMAL, (int) 'O', 5038);
		accelerators [39] . Set (wxACCEL_NORMAL, (int) 'A', 5039);
		accelerators [40] . Set (wxACCEL_NORMAL, (int) 'S', 5040);
		accelerators [41] . Set (wxACCEL_NORMAL, (int) 'D', 5041);
		accelerators [42] . Set (wxACCEL_NORMAL, (int) 'F', 5042);
		accelerators [43] . Set (wxACCEL_NORMAL, (int) 'G', 5043);
		accelerators [44] . Set (wxACCEL_NORMAL, (int) 'H', 5044);
		accelerators [45] . Set (wxACCEL_NORMAL, (int) 'J', 5045);
		accelerators [46] . Set (wxACCEL_NORMAL, (int) 'K', 5046);
		accelerators [47] . Set (wxACCEL_NORMAL, (int) 'L', 5047);
		accelerators [48] . Set (wxACCEL_NORMAL, (int) '-', 5048);
		accelerators [49] . Set (wxACCEL_NORMAL, (int) '=', 5049);
		accelerators [50] . Set (wxACCEL_NORMAL, WXK_NUMPAD_SUBTRACT, 5048);
		accelerators [51] . Set (wxACCEL_NORMAL, WXK_NUMPAD_ADD, 5049);
		accelerators [52] . Set (wxACCEL_NORMAL, WXK_PAGEDOWN, 5048);
		accelerators [53] . Set (wxACCEL_NORMAL, WXK_PAGEUP, 5049);
		accelerators [54] . Set (wxACCEL_NORMAL, WXK_LEFT, 5048);
		accelerators [55] . Set (wxACCEL_NORMAL, WXK_RIGHT, 5049);
		accelerators [56] . Set (wxACCEL_NORMAL, WXK_DOWN, 5048);
		accelerators [57] . Set (wxACCEL_NORMAL, WXK_UP, 5049);
		accelerators [58] . Set (wxACCEL_NORMAL, WXK_NUMPAD7, 5050);
		accelerators [59] . Set (wxACCEL_NORMAL, WXK_NUMPAD8, 5051);
		accelerators [60] . Set (wxACCEL_NORMAL, WXK_NUMPAD9, 5052);
		accelerators [61] . Set (wxACCEL_NORMAL, WXK_NUMPAD4, 5050);
		accelerators [62] . Set (wxACCEL_NORMAL, WXK_NUMPAD5, 5051);
		accelerators [63] . Set (wxACCEL_NORMAL, WXK_NUMPAD6, 5052);
		accelerators [64] . Set (wxACCEL_NORMAL, WXK_NUMPAD1, 5053);
		accelerators [65] . Set (wxACCEL_NORMAL, WXK_NUMPAD2, 5054);
		accelerators [66] . Set (wxACCEL_NORMAL, WXK_NUMPAD3, 5055);

		accelerators [67] . Set (wxACCEL_NORMAL, (int) 'Z', 5023);
		accelerators [68] . Set (wxACCEL_NORMAL, (int) 'X', 5024);
		accelerators [69] . Set (wxACCEL_NORMAL, (int) 'C', 5025);
		accelerators [70] . Set (wxACCEL_NORMAL, (int) 'V', 5026);
		accelerators [71] . Set (wxACCEL_NORMAL, (int) 'B', 5027);
		accelerators [72] . Set (wxACCEL_NORMAL, (int) 'N', 5028);
		accelerators [73] . Set (wxACCEL_NORMAL, (int) 'M', 5029);
		accelerators [74] . Set (wxACCEL_NORMAL, WXK_RETURN, 5054);
		accelerators [75] . Set (wxACCEL_NORMAL, WXK_NUMPAD_ENTER, 5054);
		accelerators [76] . Set (wxACCEL_NORMAL, (int) ',', 5050);
		accelerators [77] . Set (wxACCEL_NORMAL, (int) '.', 5051);
		accelerators [78] . Set (wxACCEL_NORMAL, (int) '/', 5052);
		accelerators [79] . Set (wxACCEL_NORMAL, (int) ';', 5053);
		accelerators [80] . Set (wxACCEL_NORMAL, (int) '\'', 5055);
		accelerators [81] . Set (wxACCEL_CTRL, (int) ';', 5056);
		accelerators [82] . Set (wxACCEL_CTRL, (int) '\'', 5057);
		accelerators [83] . Set (wxACCEL_NORMAL, WXK_ESCAPE, 5058);
		wxAcceleratorTable accel (84, accelerators);
		SetAcceleratorTable (accel);
#endif
	}
	void Idle (wxIdleEvent & event) {
		panel . read (& console_feedback);
		panel . read (core . conn_midi_feed);
		event . RequestMore ();
	}
//	void OnEraseBackground (wxEraseEvent & event) {wxPaintDC dc (this); dc . DrawBitmap (front_panel, 0, 0, true);}
//	void OnShowKeyboard (wxCommandEvent & event) {
//		switch (event . GetId () + 1 - showmicrokeyboardaction) {
//		case 1: if (micro_keyboard_dialog == NULL) micro_keyboard_dialog = new KeyboardDialogClass (this, & picture_keyboard_small, 1); micro_keyboard_dialog -> Show (true); break;
//		case 3: if (keyboard_dialog == NULL) keyboard_dialog = new KeyboardDialogClass (this, & picture_keyboard_big, 3); keyboard_dialog -> Show (true); break;
//		default: if (mini_keyboard_dialog == NULL) mini_keyboard_dialog = new KeyboardDialogClass (this, & picture_keyboard, 2); mini_keyboard_dialog -> Show (true); break;
//		}
//	}
//	void OnShowControllers (wxCommandEvent & event) {
//		if (control_dialog == NULL) control_dialog = new ControllersDialogClass (this);
//		control_dialog -> Show (true);
//	}
//	void OnShowOscilloscope (wxCommandEvent & event) {
//		if (oscilloscope_dialog == NULL) oscilloscope_dialog = new OscilloscopeDialogClass (this, oscilloscope_monitor_wave);
//		oscilloscope_dialog -> Show (true);
//		oscilloscope_visible = true;
//	}
	void OnShowAudioHardware (wxCommandEvent & event) {wxAudioDialog dialog (this, audio); dialog . ShowModal ();}
	void OnShowMidiHardware (wxCommandEvent & event) {MidiHardwareDialogClass dialog (this); dialog . ShowModal ();}
	void OnExit (wxCommandEvent & event) {
//		oscilloscope_controller = NULL;
		Close ();
	}
	void OnEscape (wxCommandEvent & event) {
		if (wxYES == wxMessageBox (_T ("CONFIRM EXIT"), _T ("INFO"), wxYES_NO | wxICON_EXCLAMATION, this)) Close ();
	}
	void OnAccelerator (wxCommandEvent & event) {button_callback (event . GetId () - 5001, 1.0);}
	~ EditorFrame (void) {
		oscilloscope_controller = NULL;
#ifdef WINDOWS_OPERATING_SYSTEM
		if (audio != NULL) delete audio; audio = NULL;
#endif
		#ifdef LINUX_OPERATING_SYSTEM
		close_audio ();
		#endif
		printf ("Control panel closed.\n");
	}
private:
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
EVT_IDLE(EditorFrame :: Idle)
//EVT_ERASE_BACKGROUND(EditorFrame :: OnEraseBackground)
//EVT_MENU(showmicrokeyboardaction, EditorFrame :: OnShowKeyboard)
//EVT_MENU(showminikeyboardaction, EditorFrame :: OnShowKeyboard)
//EVT_MENU(showkeyboardaction, EditorFrame :: OnShowKeyboard)
//EVT_MENU(showcontrollersaction, EditorFrame :: OnShowControllers)
//EVT_MENU(showoscilloscopeaction, EditorFrame :: OnShowOscilloscope)
EVT_MENU(showaudiohardwareaction, EditorFrame :: OnShowAudioHardware)
EVT_MENU(showmidihardwareaction, EditorFrame :: OnShowMidiHardware)
EVT_MENU(showexit, EditorFrame :: OnExit)
#ifdef WINDOWS_OPERATING_SYSTEM
EVT_MENU(5001, EditorFrame :: OnAccelerator)
EVT_MENU(5002, EditorFrame :: OnAccelerator)
EVT_MENU(5003, EditorFrame :: OnAccelerator)
EVT_MENU(5004, EditorFrame :: OnAccelerator)
EVT_MENU(5005, EditorFrame :: OnAccelerator)
EVT_MENU(5006, EditorFrame :: OnAccelerator)
EVT_MENU(5007, EditorFrame :: OnAccelerator)
EVT_MENU(5008, EditorFrame :: OnAccelerator)
EVT_MENU(5009, EditorFrame :: OnAccelerator)
EVT_MENU(5010, EditorFrame :: OnAccelerator)
EVT_MENU(5011, EditorFrame :: OnAccelerator)
EVT_MENU(5012, EditorFrame :: OnAccelerator)
EVT_MENU(5013, EditorFrame :: OnAccelerator)
EVT_MENU(5014, EditorFrame :: OnAccelerator)
EVT_MENU(5015, EditorFrame :: OnAccelerator)
EVT_MENU(5016, EditorFrame :: OnAccelerator)
EVT_MENU(5017, EditorFrame :: OnAccelerator)
EVT_MENU(5018, EditorFrame :: OnAccelerator)
EVT_MENU(5019, EditorFrame :: OnAccelerator)
EVT_MENU(5020, EditorFrame :: OnAccelerator)
EVT_MENU(5021, EditorFrame :: OnAccelerator)
EVT_MENU(5022, EditorFrame :: OnAccelerator)
EVT_MENU(5023, EditorFrame :: OnAccelerator)
EVT_MENU(5024, EditorFrame :: OnAccelerator)
EVT_MENU(5025, EditorFrame :: OnAccelerator)
EVT_MENU(5026, EditorFrame :: OnAccelerator)
EVT_MENU(5027, EditorFrame :: OnAccelerator)
EVT_MENU(5028, EditorFrame :: OnAccelerator)
EVT_MENU(5029, EditorFrame :: OnAccelerator)
EVT_MENU(5030, EditorFrame :: OnAccelerator)
EVT_MENU(5031, EditorFrame :: OnAccelerator)
EVT_MENU(5032, EditorFrame :: OnAccelerator)
EVT_MENU(5033, EditorFrame :: OnAccelerator)
EVT_MENU(5034, EditorFrame :: OnAccelerator)
EVT_MENU(5035, EditorFrame :: OnAccelerator)
EVT_MENU(5036, EditorFrame :: OnAccelerator)
EVT_MENU(5037, EditorFrame :: OnAccelerator)
EVT_MENU(5038, EditorFrame :: OnAccelerator)
EVT_MENU(5039, EditorFrame :: OnAccelerator)
EVT_MENU(5030, EditorFrame :: OnAccelerator)
EVT_MENU(5040, EditorFrame :: OnAccelerator)
EVT_MENU(5041, EditorFrame :: OnAccelerator)
EVT_MENU(5042, EditorFrame :: OnAccelerator)
EVT_MENU(5043, EditorFrame :: OnAccelerator)
EVT_MENU(5044, EditorFrame :: OnAccelerator)
EVT_MENU(5045, EditorFrame :: OnAccelerator)
EVT_MENU(5046, EditorFrame :: OnAccelerator)
EVT_MENU(5047, EditorFrame :: OnAccelerator)
EVT_MENU(5048, EditorFrame :: OnAccelerator)
EVT_MENU(5049, EditorFrame :: OnAccelerator)
EVT_MENU(5050, EditorFrame :: OnAccelerator)
EVT_MENU(5051, EditorFrame :: OnAccelerator)
EVT_MENU(5052, EditorFrame :: OnAccelerator)
EVT_MENU(5053, EditorFrame :: OnAccelerator)
EVT_MENU(5054, EditorFrame :: OnAccelerator)
EVT_MENU(5055, EditorFrame :: OnAccelerator)
EVT_MENU(5056, EditorFrame :: OnShowAudioHardware)
EVT_MENU(5057, EditorFrame :: OnShowMidiHardware)
EVT_MENU(5058, EditorFrame :: OnEscape)
#endif
END_EVENT_TABLE()

EditorFrame * main_frame = NULL;

#ifdef LINUX_OPERATING_SYSTEM
void general_quit (void) {if (main_frame != NULL) main_frame -> Close (); main_frame = NULL;}
#endif

class wxStartApp : public wxApp {
public:
	int previous_key_down;
	bool OnInit (void) {
		wxInitAllImageHandlers ();
		previous_key_down = -1;
#ifdef LINUX_OPERATING_SYSTEM
		cfg = new config ();
		build_synthesizer ();
		// to do
//		midi_service . reader_line = core . external_midi_in;
#endif
		panel . preset_bank_msb = cfg -> default_preset_sound_msb;
		panel . preset_bank_lsb = cfg -> default_preset_sound_lsb;
		panel . user_bank_msb = cfg -> default_user_sound_msb;
		panel . user_bank_lsb = cfg -> default_user_sound_lsb;
		panel . algo_bank_msb = cfg -> default_algo_msb;
		panel . algo_bank_lsb = cfg -> default_algo_lsb;
		panel . arp_bank_msb = cfg -> default_pattern_msb;
		panel . arp_bank_lsb = cfg -> default_pattern_lsb;
		panel . dsp_algo_bank_msb = cfg -> default_dsp_algo_msb;
		panel . dsp_algo_bank_lsb = cfg -> default_dsp_algo_lsb;
		panel . preset_dsp_bank_msb = cfg -> default_preset_dsp_msb;
		panel . preset_dsp_bank_lsb = cfg -> default_preset_dsp_lsb;
		panel . user_dsp_bank_msb = cfg -> default_user_dsp_msb;
		panel . user_dsp_bank_lsb = cfg -> default_user_dsp_lsb;
		panel . style_bank_msb = cfg -> default_style_msb;
		panel . style_bank_lsb = cfg -> default_style_lsb;
		panel . hercules_number = cfg -> stripes;
		panel . hercules_stereo_number = cfg -> stereo;
		panel . total_dsp = cfg -> local_dsp + cfg -> global_dsp;
		panel . set_midi_out (core . conn_midi_in);
		open_close_frame = main_frame = new EditorFrame (NULL, -1, _T ("HERCs CORE"));
		main_frame -> Show ();
		panel . send_panel_controllers_request ();
		button_callback (pb0, 1.0);
#ifdef LINUX_OPERATING_SYSTEM
		if (! open_audio (cfg)) {destroy_synthesizer (); delete cfg; wxMessageBox (_T ("Jack server is not running."), _T ("INFO"), wxOK, NULL); return false;}
		midi_setup (cfg);
#endif
		return true;
	}
	int OnExit (void) {
		if (panel . control_store_required) {
			panel . send_control_store_request ();
			core . conn_move ();
		}
		destroy_synthesizer ();
		delete cfg;
		printf ("Application exit.\n");
		return wxApp :: OnExit ();
	}
#ifdef LINUX_OPERATING_SYSTEM
#ifndef MAC_OPERATING_SYSTEM_BAK
	int FilterEvent (wxEvent & event) {
		if (event . GetEventType () != wxEVT_KEY_DOWN) return -1;
		int key = ((wxKeyEvent &) event) . GetKeyCode ();
		if (key == previous_key_down) {previous_key_down = -1; return -1;}
		previous_key_down = key;
		bool sub;
		switch (key) {
		case WXK_BACK: break;
		case WXK_F1: button_callback (0, 1.0); break;
		case WXK_F2: button_callback (1, 1.0); break;
		case WXK_F3: button_callback (2, 1.0); break;
		case WXK_F4: button_callback (3, 1.0); break;
		case WXK_F5: button_callback (4, 1.0); break;
		case WXK_F6: button_callback (5, 1.0); break;
		case WXK_F7: button_callback (6, 1.0); break;
		case WXK_F8: button_callback (7, 1.0); break;
		case WXK_F9: button_callback (8, 1.0); break;
		case WXK_F10: button_callback (9, 1.0); break;
		case WXK_F11: button_callback (10, 1.0); break;
		case WXK_F12: button_callback (11, 1.0); break;
		case (int) '0': button_callback (21, 1.0); break;
		case (int) '`': button_callback (21, 1.0); break;
		case (int) '1': button_callback (12, 1.0); break;
		case (int) '2': button_callback (13, 1.0); break;
		case (int) '3': button_callback (14, 1.0); break;
		case (int) '4': button_callback (15, 1.0); break;
		case (int) '5': button_callback (16, 1.0); break;
		case (int) '6': button_callback (17, 1.0); break;
		case (int) '7': button_callback (18, 1.0); break;
		case (int) '8': button_callback (19, 1.0); break;
		case (int) '9': button_callback (20, 1.0); break;
		case (int) 'Z': button_callback (22, 1.0); break;
		case (int) 'X': button_callback (23, 1.0); break;
		case (int) 'C': button_callback (24, 1.0); break;
		case (int) 'V': button_callback (25, 1.0); break;
		case (int) 'B': button_callback (26, 1.0); break;
		case (int) 'N': button_callback (27, 1.0); break;
		case (int) 'M': button_callback (28, 1.0); break;
		case (int) 'Q': button_callback (29, 1.0); break;
		case (int) 'W': button_callback (30, 1.0); break;
		case (int) 'E': button_callback (31, 1.0); break;
		case (int) 'R': button_callback (32, 1.0); break;
		case (int) 'T': button_callback (33, 1.0); break;
		case (int) 'Y': button_callback (34, 1.0); break;
		case (int) 'U': button_callback (35, 1.0); break;
		case (int) 'I': button_callback (36, 1.0); break;
		case (int) 'O': button_callback (37, 1.0); break;
		case (int) 'A': button_callback (38, 1.0); break;
		case (int) 'S': button_callback (39, 1.0); break;
		case (int) 'D': button_callback (40, 1.0); break;
		case (int) 'F': button_callback (41, 1.0); break;
		case (int) 'G': button_callback (42, 1.0); break;
		case (int) 'H': button_callback (43, 1.0); break;
		case (int) 'J': button_callback (44, 1.0); break;
		case (int) 'K': button_callback (45, 1.0); break;
		case (int) 'L': button_callback (46, 1.0); break;
		case (int) '-':
		case WXK_NUMPAD_SUBTRACT:
		case WXK_PAGEDOWN:
		case WXK_LEFT:
		case WXK_DOWN: button_callback (47, 1.0); break;
		case (int) '=':
		case WXK_NUMPAD_ADD:
		case WXK_PAGEUP:
		case WXK_RIGHT:
		case WXK_UP: button_callback (48, 1.0); break;
		case WXK_NUMPAD7:
		case WXK_NUMPAD4:
		case (int) ',': button_callback (49, 1.0); break;
		case WXK_NUMPAD8:
		case WXK_NUMPAD5:
		case (int) '.': button_callback (50, 1.0); break;
		case WXK_NUMPAD9:
		case WXK_NUMPAD6:
		case (int) '/':
		case (int) '\\': button_callback (51, 1.0); break;
		case WXK_NUMPAD1:
		case (int) ';':
			if (((wxKeyEvent &) event) . ControlDown ()) {
				wxAudioDialog dialog (main_frame, audio);
				dialog . ShowModal ();
				break;
			}
			button_callback (52, 1.0);
			break;
		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
		case WXK_NUMPAD2: button_callback (53, 1.0); break;
		case WXK_NUMPAD3:
		case (int) '\'':
			if (((wxKeyEvent &) event) . ControlDown ()) {
				MidiHardwareDialogClass dialog (main_frame);
				dialog . ShowModal ();
				break;
			}
			button_callback (54, 1.0);
			break;
		case (int) ']':
			sub = panel . porta_switch -> GetValue () == 0.0;
			panel . porta_switch -> setValue (sub ? 1.0 : 0.0);
			controller_callback (18, sub ? 1.0 : 0.0);
			panel . data = sub ? 1 : 0;
			break;
		case (int) '[':
			sub = panel . mono_poly_switch -> GetValue () == 0.0;
			panel . mono_poly_switch -> setValue (sub ? 1.0 : 0.0);
			controller_callback (23, sub ? 1.0 : 0.0);
			panel . data = sub ? 1 : 0;
			break;
		case WXK_ESCAPE:
			if (wxYES == wxMessageBox (_T ("CONFIRM EXIT"), _T ("INFO"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION, main_frame)) general_quit ();
			break;
		default: break;
		}
		return -1;
	}
#endif
#endif
};

#ifdef LINUX_OPERATING_SYSTEM
IMPLEMENT_APP(wxStartApp)
#endif

#ifdef WINDOWS_OPERATING_SYSTEM

IMPLEMENT_APP_NO_MAIN(wxStartApp)

#include "resource.h"

HINSTANCE hinstance = NULL;

HWND main_console = NULL;
void console_midi_transmission (void) {
//	console_midi_service . receive (core . conn_midi_in);
	//core . conn_move (console_midi_service . get_transmitter ());
	// # getTransmitter ???
	core . conn_move ();
}
//void external_midi_transmission (void) {midi_service . send (core . external_midi_out);}
void external_midi_transmission (void) {} // # most likely redundant
//void general_quit (void) {SendMessage (main_console, WM_COMMAND, IDCANCEL, NULL);}
void general_quit (void) {if (main_frame != 0) main_frame -> Close (); main_frame = NULL;}

int CALLBACK main_console_window_proc (HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
	case WM_INITDIALOG:
		SendMessage (hwnd, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon (hinstance, MAKEINTRESOURCE (IDI_ICON1)));
		if (audio == NULL) audio = new MultiplatformAudio ((void *) hwnd, 2, cfg -> sampling_freq, cfg -> latency_block);
		audio -> installOutputCallback (core_audio_output_callback);
		audio -> selectOutputDevice (0);
		audio -> installInputCallback (core_audio_input_callback);
		audio -> selectInputDevice (0);
		return true;
	case WM_COMMAND:
		switch (LOWORD (wparam)) {
		case IDOK:
		case IDCANCEL:
			if (audio != NULL) delete audio; audio = NULL;
			PostQuitMessage (0);
			EndDialog (hwnd, IDCANCEL);
			return true;
		}
		return false;
	default: return false;
	}
	return false;
}

void build_main_console (void) {
	main_console = CreateDialog (hinstance, MAKEINTRESOURCE (IDD_MAIN_CONSOLE), NULL, main_console_window_proc);
}


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	hinstance = hInstance;

	WNDCLASS wn;
	wn . lpszClassName = TEXT ("HERCs CORE [hardware]");
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

	cfg = new config ();
	build_synthesizer ();
	midi_service . set_external_midi_in_line (core . external_midi_in);

	build_main_console ();

	wxEntry (hInstance, hPrevInstance, lpCmdLine, nShowCmd);

	return IDCANCEL;
}

#endif
