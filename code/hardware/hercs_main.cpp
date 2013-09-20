///////////////////////////////////////////////////////////////////////////////////
//                       Copyright (C) 2013 Robert P. Wolf                       //
//                                                                               //
// Permission is hereby granted, free of charge, to any person obtaining a copy  //
// of this software and associated documentation files (the "Software"), to deal //
// in the Software without restriction, including without limitation the rights  //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell     //
// copies of the Software, and to permit persons to whom the Software is         //
// furnished to do so, subject to the following conditions:                      //
//                                                                               //
// The above copyright notice and this permission notice shall be included in    //
// all copies or substantial portions of the Software.                           //
//                                                                               //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE   //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER        //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     //
// THE SOFTWARE.                                                                 //
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "prolog.h"
#include "prolog_conductor.h"
#include "prolog_midi.h"
#include "prolog_neural.h"
#include "prolog_windows_console.h"
#include "midi_command_prompt.h"
#include "config.h"
#include "core.h"
#include "multiplatform_audio.h"

orthogonal_core core;
config cfg;

#ifdef WINDOWS_OPERATING_SYSTEM
#include "resource.h"
class resource_loader_class : public PrologResourceLoader {
public:
	char * load (char * name) {
		if (strcmp (name, "hercs") == 0 || strcmp (name, "hercs.prc") == 0) return hercs_resource;
		HRSRC resource = NULL;
		if (strcmp (name, "studio") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STUDIO_PRC), RT_RCDATA);
		if (strcmp (name, "conductor") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (CONDUCTOR_PRC), RT_RCDATA);
		if (strcmp (name, "midi") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (MIDI_PRC), RT_RCDATA);
		if (strcmp (name, "http") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HTTP_PRC), RT_RCDATA);
		if (strcmp (name, "store") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STORE_PRC), RT_RCDATA);
		if (strcmp (name, "f1") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (F1_PRC), RT_RCDATA);
		if (strcmp (name, "help") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HELP_PRC), RT_RCDATA);
		if (strcmp (name, "record") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (RECORD_PRC), RT_RCDATA);
		if (strcmp (name, "neural") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (NEURAL_PRC), RT_RCDATA);
		if (strcmp (name, "keyboard") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (KEYBOARD_PRC), RT_RCDATA);
		if (strcmp (name, "sql") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (SQL_PRC), RT_RCDATA);
		if (strcmp (name, "studio.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STUDIO_PRC), RT_RCDATA);
		if (strcmp (name, "conductor.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (CONDUCTOR_PRC), RT_RCDATA);
		if (strcmp (name, "midi.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (MIDI_PRC), RT_RCDATA);
		if (strcmp (name, "http.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HTTP_PRC), RT_RCDATA);
		if (strcmp (name, "store.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STORE_PRC), RT_RCDATA);
		if (strcmp (name, "f1.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (F1_PRC), RT_RCDATA);
		if (strcmp (name, "help.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HELP_PRC), RT_RCDATA);
		if (strcmp (name, "record.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (RECORD_PRC), RT_RCDATA);
		if (strcmp (name, "neural.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (NEURAL_PRC), RT_RCDATA);
		if (strcmp (name, "keyboard.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (KEYBOARD_PRC), RT_RCDATA);
		if (strcmp (name, "sql.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (SQL_PRC), RT_RCDATA);
		if (! resource) return NULL;
		HGLOBAL loader = LoadResource (NULL, resource);
		if (! loader) return NULL;
		return (char *) LockResource (loader);
	}
} resource_loader;
#endif

class service_class_loader_class : public PrologServiceClassLoader {
public:
	PrologServiceClass * load (char * name) {
		if (strcmp (name, "prolog.conductor") == 0) return new PrologConductorServiceClass ();
		if (strcmp (name, "prolog.midi") == 0) return new PrologMidiServiceClass ();
		if (strcmp (name, "hercs") == 0) return new HERCsServiceClass (& core);
#ifdef LINUX_OPERATING_SYSTEM
		if (strcmp (name, "prolog.http") == 0) return new PrologHttpServiceClass ();
#endif
		if (strcmp (name, "prolog.neural") == 0) return new PrologNeuralServiceClass ();
		return NULL;
	}
} service_class_loader;

MultiplatformAudio audio (GetConsoleWindow (), 2, cfg . sampling_freq, cfg . latency_block);
MidiCommandPrompt command_prompt (& core . conn_midi_source);

class Editor : public midi_reader {
public:
	virtual void midi_system_exclusive (midi_stream * line) {
		line -> mark ();
		if (! line -> check_system_exclusive ()) {line -> restore (); line -> get_f7 (); return;}
		int selector = line -> get ();
		AREA area; int ap = 0;
		switch (selector) {
		case 0x2a:
			while ((selector = line -> get ()) != 0xf7) ap = area_cat (area, ap, (char) selector);
			printf ("%s", area);
			break;
		default: line -> get_f7 (); break;
		}
	}
} editor;

static double input_audio_buffer [32768];
static int read_pointer = 0;
static int write_pointer = 0;

static void input_callback (int frames, AudioBuffers * buffers) {
	while (frames-- > 0) {
		input_audio_buffer [write_pointer++] = buffers -> getStereoLeftRight ();
		input_audio_buffer [write_pointer++] = buffers -> getStereoLeftRight ();
		if (write_pointer >= 32768) write_pointer = 0;
	}
}

static void multicore_output_callback (int nframes, AudioBuffers * buffers) {
	if (nframes >= 2048) return;
	core . conn_move (& editor);
	for (int ind = 0; ind < nframes; ind++) {
		core . input_left ((float) input_audio_buffer [read_pointer++], ind);
		core . input_right ((float) input_audio_buffer [read_pointer++], ind);
		if (read_pointer >= 32768) read_pointer = 0;
	}
	core . multi_move (nframes);
	for (int ind = 0; ind < nframes; ind++) {
		buffers -> insertStereo (core . left_out (ind), core . right_out (ind));
	}
}

static void singlecore_output_callback (int nframes, AudioBuffers * buffers) {
	if (nframes >= 2048) return;
	core . conn_move (& editor);
	int sample = 0;
	while (nframes-- > 0) {
		core . input_left ((float) input_audio_buffer [read_pointer++]);
		core . input_right ((float) input_audio_buffer [read_pointer++]);
		if (read_pointer >= 32768) read_pointer = 0;
		core . move ();
		buffers -> insertStereo (core . left_out (), core . right_out ());
	}
}

void main (int args, char * * argv) {
	core . build_synthesiser (& cfg, & resource_loader, & service_class_loader);
	printf ("Serial Number [%s]\n", cfg . serial_number);
	for (int ind = 0; ind < audio . getNumberOfInputDevices (); ind++) {
		printf ("Input Device [%s]\n", audio . getInputDeviceName (ind));
	}
	for (int ind = 0; ind < audio . getNumberOfOutputDevices (); ind++) {
		printf ("Output device [%s]\n", audio . getOutputDeviceName (ind));
	}
	audio . installInputCallback (input_callback);
#ifdef MULTI_CORE_HARDWARE
#ifdef SWITCHABLE_MULTICORE
	audio . installOutputCallback (cfg . processors > 0 ? multicore_output_callback : singlecore_output_callback);
#else
	audio . installOutputCallback (multicore_output_callback);
#endif
#else
	audio . installOutputCallback (singlecore_output_callback);
#endif
	//audio . selectInputDevice (0);
	audio . selectOutputDevice (0);
	command_prompt . open ();
	core . resolution (& cfg);
	audio . selectInputDevice (-1);
	audio . selectOutputDevice (-1);
	core . destroy_synthesiser ();
	drop_object_counter ();
	getchar ();
	command_prompt . close ();
}

