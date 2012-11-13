///////////////////////////////////////
// Copyright (C) 2008 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include <stdio.h>

#include "mac_midi.h"
#include "midi_stream.h"
#include "midi_command_prompt.h"

mac_midi_service midi_service;

buffered_midi_stream line (8192);
buffered_midi_stream in_line (8192);
class internal_info_midi_reader : public midi_reader {
public:
	virtual void midi_system_exclusive (midi_stream * line) {
		line -> mark ();
		if (line -> check_system_exclusive ()) {
			if (line -> get () == 0x2a) {
				AREA area;
				line -> get (area);
				printf (area);
				return;
			}
		}
		line -> restore ();
		line -> get_f7 ();
	}
};
static internal_info_midi_reader info;

MidiCommandPrompt command_console (& line, 40);

int main (int args, char * * argv) {
	char * name;
	int ind;
	for (ind = 0; ind < midi_service . getNumberOfInputs (); ind++) {
		name = midi_service . getInputInfo (ind);
		if (name != NULL) {
			if (strcmp (name, "HERCs Main MIDI OUT") == 0) {
				printf ("input [%s]\n", name);
				midi_service . set_input_line (& in_line);
				midi_service . set_reader (& info);
				midi_service . setInputPort (ind);
			}
		}
	}
	for (ind = 0; ind < midi_service . getNumberOfOutputs (); ind++) {
		name = midi_service . getOutputInfo (ind);
		if (name != NULL) {
			if (strcmp (name, "HERCs Main MIDI IN") == 0) {
				printf ("output [%s]\n", name);
				midi_service . setOutputPort (ind);
			}
		}
	}

//	command_console . open ();
//	while (true) {
//		midi_service . send (& line);
//		sleep (1000);
//	}




	AREA command;
	int ch;
	while (true) {
		ind = 0;
		ch = getchar ();
		while (ch >= ' ') {
			ind = area_cat (command, ind, ch);
			ch = getchar ();
		}
		if (strcmp (command, "cexit") == 0) break;
		area_cat (command, ind, "\n");
		line . open_system_exclusive ();
		line . insert (0x28);
		line . insert (command);
		line . close_system_exclusive ();
		midi_service . send (& line);
	}
}
