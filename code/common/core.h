///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _CORE_SYNTHESIZER
#define _CORE_SYNTHESIZER

#include "data.h"
#include "transport.h"
#include "osc_cpp.h"
#include "encoder.h"

class orthogonal_core;
class synthesizer_core;

class orthogonal_core {
private:
	int horizontal;
	int security_crash_after;
	unsigned int protection_counter;
	encoder e;

	midi_stream * internal_midi_line;

//	PrologMidiCommand * prolog_console;
//	prolog_midi_reader * prolog_reader;
//	int prolog_ctrl;
//	PrologResolutionPool * pool;

	int prolog_sample_counter, prolog_sample_sentinel;
	int arp_sample_counter, arp_sample_sentinel;
	int vector_sample_counter, vector_sample_sentinel;

	synthesizer * sth;
	arpeggiator_pool * arps;
	vector_pool * vects;
	oscillator_pool_cpp * osc;
	float_stereo_dsp_line_pool * dsp;

public:
	PrologRoot * root;
	delayed_buffered_midi_stream * external_midi_in;
	midi_stream * external_midi_out;
	midi_stream * conn_midi_in;
	midi_stream * conn_midi_out;
	midi_stream * conn_midi_feed;

	float * left_out_pointer (void);
	float * right_out_pointer (void);
	float left_out (void);
	float right_out (void);
	// multi threading equivalent
	float left_out (int sample);
	float right_out (int sample);
	void build_synthesizer (config * cfg, PrologResourceLoader * resource_loader = NULL, PrologServiceClassLoader * service_loader = NULL);
	void destroy_synthesizer (void);
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
};


class synthesizer_core {
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

	synthesizer * sth;
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
	void build_synthesizer (config * cfg);
	void destroy_synthesizer (void);
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
