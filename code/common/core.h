///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _CORE_SYNTHESISER
#define _CORE_SYNTHESISER

#include "data.h"
#include "transport.h"
#include "osc_cpp.h"
#include "encoder.h"
#include "prolog_midi_command.h"

class orthogonal_core;
class synthesiser_core;

extern char * hercs_resource;
class HERCsServiceClass : public PrologServiceClass {
private:
	orthogonal_core * core;
public:
	virtual PrologNativeCode * getNativeCode (char * name);
	HERCsServiceClass (orthogonal_core * core);
};

class orthogonal_core {
private:
	int horizontal;
	int security_crash_after;
	unsigned int protection_counter;
	encoder e;

//	PrologMidiCommand * prolog_console;
//	prolog_midi_reader * prolog_reader;
//	int prolog_ctrl;
//	PrologResolutionPool * pool;

	int prolog_sample_counter, prolog_sample_sentinel;
	int arp_sample_counter, arp_sample_sentinel;
	int vector_sample_counter, vector_sample_sentinel;

	synthesiser * sth;
	arpeggiator_pool * arps;
	vector_pool * vects;
	oscillator_pool_cpp * osc;
	float_stereo_dsp_line_pool * dsp;

public:
	PrologRoot * root;
	PrologMidiCommand * prolog_console;
	bool resolution_finished;
	buffered_midi_stream conn_midi_source;
	buffered_midi_stream conn_midi_feed;

	midi_stream * lines;
	midi_stream * line;

	void insertMidiSource (midi_stream * source);
	void removeMidiSource (midi_stream * source);
	void insertMidiDestination (midi_stream * destination);
	void removeMidiDestination (midi_stream * destination);

	float * left_out_pointer (void);
	float * right_out_pointer (void);
	float left_out (void);
	float right_out (void);
	// multi threading equivalent
	float left_out (int sample);
	float right_out (int sample);
	void build_synthesiser (config * cfg, PrologResourceLoader * resource_loader = NULL, PrologServiceClassLoader * service_loader = NULL);
	void destroy_synthesiser (void);
	bool move (void); // true if finished
	void conn_move (void);
	void conn_move (midi_reader * conn);
	bool multi_move (int samples); // true if finished
	void crash (void);
	void crash_check (unsigned int samples);
	void set_security_crash_after (int ind);
	int get_security_crash_after (void);
	void set_volume_id (unsigned long int volume_id);
	void set_serial_shift (int shift);
	void input_left (float left);
	void input_right (float right);
	// multi threading equivalent
	void input_left (float left, int sample);
	void input_right (float right, int sample);
	orthogonal_core (void);
};


class synthesiser_core {
private:
	int horizontal;
	int security_crash_after;
	unsigned int protection_counter;
	encoder e;

	STRING serial_number;
	STRING key;
	unsigned long int volume_id;
	int shift;

	int arp_sample_counter, arp_sample_sentinel;
	int vector_sample_counter, vector_sample_sentinel;

	synthesiser * sth;
	arpeggiator_pool * arps;
	vector_pool * vects;
	oscillator_pool_cpp * osc;
	float_stereo_dsp_line_pool * dsp;

public:
	delayed_buffered_midi_stream * external_midi_in;
	midi_stream * external_midi_out;
	midi_stream * conn_midi_in;
	midi_stream * conn_midi_out;
	midi_stream * conn_midi_feed;

	float * left_out_pointer (void);
	float * right_out_pointer (void);
	float left_out (void);
	float right_out (void);
	void build_synthesiser (config * cfg);
	void destroy_synthesiser (void);
	bool move (void); // true if finished
	void conn_move (void);
	void conn_move (midi_reader * conn);
	bool multi_move (int samples); // ture if finished
	void crash (void);
	void crash_check (unsigned int samples);
	void set_security_crash_after (int ind);
	int get_security_crash_after (void);
	void set_volume_id (unsigned long int volume_id);
	void set_serial_shift (int shift);
	void input_left (float left);
	void input_right (float right);
};

#endif
