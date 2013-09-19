///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "core.h"
#include "prolog_midi_command.h"
#include "prolog_midi.h"

#define MIDI_STREAM_SIZE 65536


/////////////////////
// ORTHOGONAL CORE //
/////////////////////

char * hercs_resource = "\
\
import studio \
import conductor \
import midi \
\
program hercs #machine := \"hercs\" [insertSource connSource connDestination] \
#machine insertSource := \"insertSource\" \
#machine connSource := \"connSource\" \
#machine connDestination := \"connDestination\" \
end .\
";

class InsertSourceClass : public PrologNativeCode {
private:
	orthogonal_core * core;
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isAtom ()) return false;
		PrologNativeCode * machine = parameters -> getAtom () -> getMachine ();
		if (machine == 0) return false;
		if (machine -> codeName () != PrologMidiNativeCode :: name ()) return false;
		PrologMidiNativeCode * midi = (PrologMidiNativeCode *) machine;
		core -> insertMidiSource (midi -> getLine ());
		return true;
	}
	InsertSourceClass (orthogonal_core * core) {this -> core = core;}
};

class ConnSource : public PrologMidiNativeCode {
private:
	orthogonal_core * core;
public:
	virtual bool connectThru (PrologMidiNativeCode * destination) {core -> conn_midi_source . connect_thru (destination != 0 ? destination -> getLine () : 0); return true;}
	virtual midi_stream * getLine (void) {return & core -> conn_midi_source;}
	bool code (PrologElement * parameters, PrologRoot * root) {
		if (parameters -> isEarth ()) return connectThru (0);
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isAtom ()) return false;
		PrologNativeCode * machine = parameters -> getAtom () -> getMachine ();
		if (machine != 0 && machine -> codeName () == PrologMidiNativeCode :: name ()) return connectThru ((PrologMidiNativeCode *) machine);
		return false;
	}
	ConnSource (orthogonal_core * core) {this -> core = core;}
};

class ConnDestination : public PrologMidiNativeCode {
private:
	orthogonal_core * core;
public:
	virtual bool connectThru (PrologMidiNativeCode * destination) {core -> conn_midi_feed . connect_thru (destination != 0 ? destination -> getLine () : 0); return true;}
	virtual midi_stream * getLine (void) {return & core -> conn_midi_feed;}
	bool code (PrologElement * parameters, PrologRoot * root) {
		if (parameters -> isEarth ()) return connectThru (0);
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isAtom ()) return false;
		PrologNativeCode * machine = parameters -> getAtom () -> getMachine ();
		if (machine != 0 && machine -> codeName () == PrologMidiNativeCode :: name ()) return connectThru ((PrologMidiNativeCode *) machine);
		return false;
	}
	ConnDestination (orthogonal_core * core) {this -> core = core;}
};

PrologNativeCode * HERCsServiceClass :: getNativeCode (char * name) {
	if (strcmp (name, "insertSource") == 0) return new InsertSourceClass (core);
	if (strcmp (name, "connSource") == 0) return new ConnSource (core);
	if (strcmp (name, "connDestination") == 0) return new ConnDestination (core);
	return 0;
}

HERCsServiceClass :: HERCsServiceClass (orthogonal_core * core) {this -> core = core;}

void orthogonal_core :: set_security_crash_after (int ind) {
	security_crash_after = ind;
	protection_counter = horizontal * security_crash_after;
}

int orthogonal_core :: get_security_crash_after (void) {return security_crash_after;}

void orthogonal_core :: set_volume_id (unsigned long int volume_id) {root -> set_volume_id (volume_id);}
void orthogonal_core :: set_serial_shift (int shift) {root -> set_serial_shift (shift);}

void orthogonal_core :: insertMidiSource (midi_stream * source) {
	source -> next = this -> lines;
	this -> lines = source;
}

void orthogonal_core :: removeMidiSource (midi_stream * source) {
	if (source == 0) return;
	if (lines == source) {lines = lines -> next; return;}
	midi_stream * ptr = lines;
	while (ptr -> next != 0) {
		if (ptr -> next == source) {ptr -> next = source -> next; return;}
		ptr = ptr -> next;
	}
}

void orthogonal_core :: insertMidiDestination (midi_stream * destination) {
	destination -> thru = this -> line;
	this -> line = destination;
}

void orthogonal_core :: removeMidiDestination (midi_stream * destination) {
	if (line == 0) return;
	if (line == destination) {line = line -> thru; return;}
	midi_stream * ptr = line;
	while (ptr -> thru != 0) {
		if (ptr -> thru == destination) {ptr -> thru = destination -> thru; return;}
		ptr = ptr -> thru;
	}
}

orthogonal_core :: orthogonal_core (void)
	: conn_midi_source (MIDI_STREAM_SIZE)
	, conn_midi_feed (MIDI_STREAM_SIZE)
{}

void orthogonal_core :: build_synthesizer (config * cfg, PrologResourceLoader * resource_loader, PrologServiceClassLoader * service_loader) {
	horizontal = cfg -> horizontal;
	set_security_crash_after (72);
//	prolog_ctrl = 4;
	resolution_finished = false;
	prolog_sample_counter = 0;
	prolog_sample_sentinel = 50;
	arp_sample_counter = 0;
	arp_sample_sentinel = 100;
	vector_sample_counter = 0;
	vector_sample_sentinel = 100;

	if (cfg -> prolog_horizontal > 0) prolog_sample_sentinel = cfg -> horizontal / cfg -> prolog_horizontal;
	if (cfg -> arranger_horizontal > 0) arp_sample_sentinel = cfg -> horizontal / cfg -> arranger_horizontal;
	if (cfg -> vector_horizontal > 0) vector_sample_sentinel = cfg -> horizontal / cfg -> vector_horizontal;

	lines = line = 0;

	root = new PrologRoot ();
	root -> get_search_directories_from_environment ("HRCS_MODULE_SEARCH_PATHS");
	root -> setResourceLoader (resource_loader);
	root -> setServiceClassLoader (service_loader);
//	root -> setMidi (external_midi_in, internal_midi_line);
	root -> set_serial_number (cfg -> serial_number);
	root -> set_key (cfg -> key);
	root -> set_volume_id (get_volume_id (NULL));
	root -> set_serial_shift (0);
	root -> set_uap32_captions ();
//	root -> greenThreads (cfg -> prolog_horizontal);
//	root -> setRootDirectory (cfg -> prolog_root_directory);
	if (strlen (cfg -> prolog_root_directory) > 0) root -> addSearchDirectory (cfg -> prolog_root_directory);

	prolog_console = new PrologMidiCommand (& conn_midi_feed);
	root -> insertCommander (prolog_console);

//	root -> resolutionHead (cfg -> prolog_library_load);
//	root -> setQuery ();
//	prolog_reader = new prolog_midi_reader (root);
//	pool = root -> getResolutionPool ();

	sth = new synthesizer (cfg);
	arps = new arpeggiator_pool_cpp (cfg, sth);
	vects = new vector_pool_cpp (cfg, sth);
	dsp = new float_stereo_dsp_line_pool (cfg, sth);
	osc = new oscillator_pool_cpp (cfg, sth, dsp);
	sth -> connect_prolog_root (root);

	sth -> set_message (0x20, "HERCs Music Systems ABN 19 078 307 346 BN04318407");
	sth -> set_message (0x21, "24/12-26 Willcox Street; Adelaide SA 5000; AUSTRALIA");
	sth -> set_message (0x22, "P.O. Box 10027; Adelaide BC SA 5000; AUSTRALIA");
	sth -> set_message (0x23, "04 0004 4096");
	sth -> set_message (0x24, "http://www.hercsmusicsystems.com");
	sth -> set_message (0x25, "info@hercsmusicsystems.com");
	sth -> set_message (0x2b, "HERCs");
	sth -> set_message (0x2c, "CORE");
	sth -> set_message (0x2d, "6 stripes console");
	sth -> set_message (0x2e, cfg -> serial_number);
	sth -> set_message (0x2f, cfg -> key);

	sth -> connect_midi_out (& conn_midi_feed);
	sth -> configure ();
	sth -> voice_init ();

	sth -> active_sensing_delay = cfg -> horizontal >> 1;
}

void orthogonal_core :: destroy_synthesizer (void) {
	delete sth;
	delete arps;
	delete vects;


	delete dsp;
	delete osc;
//	delete prolog_console;
//	if (prolog_ctrl != 4) root -> removeMainQuery ();
//	root -> removeThreads ();
	delete root;
	delete prolog_console;
//	delete prolog_reader;
}

void orthogonal_core :: conn_move (midi_reader * conn) {
	sth -> read (& conn_midi_source);
	conn -> read (& conn_midi_feed);
	if (lines) sth -> read (lines);
}

void orthogonal_core :: conn_move (void) {sth -> read (& conn_midi_source); if (lines) sth -> read (lines);}

bool orthogonal_core :: move (void) {
//	if (prolog_reader -> is_ready ()) prolog_reader -> read (external_midi_in);
//	pool -> main_shift ();
//	if (--prolog_sample_counter <= 0) {
//		if (prolog_ctrl == 4) {
//			prolog_ctrl = root -> move ();
//			if (prolog_ctrl != 4) root -> removeMainQuery ();
//		}
//		prolog_sample_counter = prolog_sample_sentinel;
//	}
//	sth -> read (internal_midi_line);
	if (lines) sth -> read (lines);
	if (--arp_sample_counter <= 0) {arps -> move (); arp_sample_counter = arp_sample_sentinel;}
	if (--vector_sample_counter <= 0) {vects -> move (); vector_sample_counter = vector_sample_sentinel;}
	osc -> move ();
//	return prolog_ctrl != 4;
	return resolution_finished;
}

bool orthogonal_core :: multi_move (int samples) {
//	if (prolog_reader -> is_ready ()) prolog_reader -> read (external_midi_in);
//	pool -> main_shift ();
//	prolog_sample_counter -= samples;
//	while (prolog_sample_counter <= 0) {
//		if (prolog_ctrl == 4) {
//			prolog_ctrl = root -> move ();
//			if (prolog_ctrl != 4) root -> removeMainQuery ();
//		}
//		prolog_sample_counter += prolog_sample_sentinel;
//	}
//	sth -> read (internal_midi_line);
	if (lines) sth -> read (lines, samples);
	arp_sample_counter -= samples;
	while (arp_sample_counter <= 0) {arps -> move (); arp_sample_counter += arp_sample_sentinel;}
	vector_sample_counter -= samples;
	while (vector_sample_counter <= 0) {vects -> move (); vector_sample_counter += vector_sample_sentinel;}

	osc -> multi_move_parts (samples);
	osc -> multi_move_oscillators (samples);
	osc -> multi_move_dsp (samples);

//	return prolog_ctrl != 4;
	return resolution_finished;
}

void orthogonal_core :: crash (void) {
	sth -> destroy_banks ();
	sth -> voice_init ();
}

void orthogonal_core :: crash_check (unsigned int samples) {
	if (protection_counter < samples) {
		if (! e . check_serial (root -> serial_number, root -> volume_id, root -> key, root -> serial_shift)) {
			crash ();
		}
	}
	protection_counter -= samples;
}

float * orthogonal_core :: left_out_pointer (void) {return & dsp -> left_out;}
float * orthogonal_core :: right_out_pointer (void) {return & dsp -> right_out;}
float orthogonal_core :: left_out (void) {return dsp -> left_out;}
float orthogonal_core :: right_out (void) {return dsp -> right_out;}
float orthogonal_core :: left_out (int sample) {return dsp -> left_outs [sample];}
float orthogonal_core :: right_out (int sample) {return dsp -> right_outs [sample];}
void orthogonal_core :: input_left (float left) {osc -> left_in = left;}
void orthogonal_core :: input_right (float right) {osc -> right_in = right;}
void orthogonal_core :: input_left (float left, int sample) {osc -> left_ins [sample] = left;}
void orthogonal_core :: input_right (float right, int sample) {osc -> right_ins [sample] = right;}




//////////////////////
// SYNTHESIZER CORE //
//////////////////////

void synthesizer_core :: set_security_crash_after (int ind) {
	security_crash_after = ind;
	protection_counter = horizontal * security_crash_after;
}

int synthesizer_core :: get_security_crash_after (void) {return security_crash_after;}

void synthesizer_core :: set_volume_id (unsigned long int volume_id) {this -> volume_id = volume_id;}
void synthesizer_core :: set_serial_shift (int shift) {this -> shift = shift;}

void synthesizer_core :: build_synthesizer (config * cfg) {

	horizontal = cfg -> horizontal;
	set_security_crash_after (72);
	arp_sample_counter = 0;
	arp_sample_sentinel = 100;
	vector_sample_counter = 0;
	vector_sample_sentinel = 100;

	if (cfg -> arranger_horizontal > 0) arp_sample_sentinel = cfg -> horizontal / cfg -> arranger_horizontal;
	if (cfg -> vector_horizontal > 0) vector_sample_sentinel = cfg -> horizontal / cfg -> vector_horizontal;

	external_midi_in = new delayed_buffered_midi_stream (MIDI_STREAM_SIZE);
	external_midi_out = new buffered_midi_stream (MIDI_STREAM_SIZE);
	conn_midi_in = new buffered_midi_stream (MIDI_STREAM_SIZE);
	conn_midi_out = new buffered_midi_stream (MIDI_STREAM_SIZE);
	conn_midi_feed = new buffered_midi_stream (MIDI_STREAM_SIZE);
	external_midi_in -> connect_thru (conn_midi_feed);

	sth = new synthesizer (cfg);
	arps = new arpeggiator_pool_cpp (cfg, sth);
	vects = new vector_pool_cpp (cfg, sth);
	dsp = new float_stereo_dsp_line_pool (cfg, sth);
	osc = new oscillator_pool_cpp (cfg, sth, dsp);

	sth -> set_message (0x20, "HERCs Music Systems ABN 19 078 307 346 BN04318407");
	sth -> set_message (0x21, "1/39 Stonehouse Avenue; Camden Park SA 5038; AUSTRALIA");
	sth -> set_message (0x22, "P.O. Box 10027; Adelaide BC SA 5000; AUSTRALIA");
	sth -> set_message (0x23, "04 0004 4096");
	sth -> set_message (0x24, "http://www.hercsmusicsystems.com");
	sth -> set_message (0x25, "info@hercsmusicsystems.com");
	sth -> set_message (0x2b, "HERCs");
	sth -> set_message (0x2c, "CORE");
	sth -> set_message (0x2d, "6 stripes console");
	sth -> set_message (0x2e, cfg -> serial_number);
	sth -> set_message (0x2f, cfg -> key);

	sth -> connect_midi_out (conn_midi_out);
	sth -> configure ();
	sth -> voice_init ();
}

void synthesizer_core :: destroy_synthesizer (void) {
	delete sth;
	delete arps;
	delete vects;

	delete external_midi_in;
	delete external_midi_out;
	delete conn_midi_feed;
	delete conn_midi_out;
	delete conn_midi_in;

	delete dsp;
	delete osc;
}

void synthesizer_core :: conn_move (midi_reader * conn) {
	sth -> read (conn_midi_in);
	conn -> read (conn_midi_out);
	conn -> read (conn_midi_feed);
}

void synthesizer_core :: conn_move (void) {sth -> read (conn_midi_in);}

bool synthesizer_core :: move (void) {
	sth -> read (external_midi_in);
	if (--arp_sample_counter <= 0) {arps -> move (); arp_sample_counter = arp_sample_sentinel;}
	if (--vector_sample_counter <= 0) {vects -> move (); vector_sample_counter = vector_sample_sentinel;}
	osc -> move ();
	return false;
}

void synthesizer_core :: crash (void) {
	sth -> destroy_banks ();
	sth -> voice_init ();
}

void synthesizer_core :: crash_check (unsigned int samples) {
	if (protection_counter < samples) {
		if (! e . check_serial (serial_number, volume_id, key, shift)) {
			crash ();
		}
	}
	protection_counter -= samples;
}

//void synthesizer_core :: read (void) {sth -> read (internal_midi_line);}
float * synthesizer_core :: left_out_pointer (void) {return & dsp -> left_out;}
float * synthesizer_core :: right_out_pointer (void) {return & dsp -> right_out;}
float synthesizer_core :: left_out (void) {return dsp -> left_out;}
float synthesizer_core :: right_out (void) {return dsp -> right_out;}
void synthesizer_core :: input_left (float left) {osc -> left_in = left;}
void synthesizer_core :: input_right (float right) {osc -> right_in = right;}
