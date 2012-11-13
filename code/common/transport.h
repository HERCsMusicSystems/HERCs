///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _TRANSPORT
#define _TRANSPORT

#include "data.h"

class HERCs_SDK_EXPORT transport;
class HERCs_SDK_EXPORT arpeggiator_cpp;
class HERCs_SDK_EXPORT arpeggiator_pool_cpp;
class HERCs_SDK_EXPORT vector_cpp;
class HERCs_SDK_EXPORT vector_pool_cpp;

class transport {
private:
	int d, d0, d1, d2;
	int sub_tick, sub_beat;
	int horizontal;
	int wait_type; // 0 : off, 1 : tick, 2 : beat, 3 : bar
	bool inactive;
	int wait_sentinel;
	void tempo (void);
public:
	int ticks_per_beat, beats_per_bar;
	int beats_per_seconds, seconds;
	int tick;
	int beat;
	int bar;

	void reset (void);
	void tempo (int beats_per_minute);
	void tempo (int beats_per_seconds, int seconds);
	void sub_division (int ticks_per_beat);
	void division (int beats_per_bar);
	void division (int beats_per_bar, int ticks_per_beat);
	void metrum (int top, int bottom);

	void wt (void);
	void wt (int ind);
	void wait_beat (void);
	void wait_beat (int ind);
	void wait_bar (void);
	void wait_bar (int ind);
	void start (void);
	void pause (void);
	void stop (void);
	void schedule_tick_code (void);

	void signal (void);
	void move (void);
	virtual void tick_code (void);
	virtual void beat_code (void);
	virtual void bar_code (void);
	transport (int horizontal);
};

class arranger : public transport{
private:
//	int arp_wait_beats;
//	int key_off_ticks;
//	int arp_wait_bars;
//	int arp_wait_type;                 // 0 = beat, 1 = bar
//	bool no_continuous_play;
//	bool auto_keyoff;
//	bool transcriptions_buffered;
//	bool variation_buffered;
	int private_variation;
	int private_base_note;
	int private_transcriptions [64];
	bool transcriptions_ready;
	void reset_transcriptions (void);
public:
	int status;
	int sub_status;
	int octave;
	int counter;
	int variation;
	int base_note;
	int transcriptions [64];
	void set_variation (int variation);
	void set_transcription (int key);
	void set_transcription (int index, int key);
	void update_variation (void);
	void update_transcriptions (void);
	void update (void);
public:
	virtual void tick_code (void);
	virtual void beat_code (void);
	virtual void bar_code (void);
public:
	parameter_block * pb;
	pattern * head;
	void key_on (int key, int velocity);
	void key_off (int key);
	void key_off (void);
	virtual int top_key_index (void);
	virtual int get_hit_key (int counter);
	virtual int get_velocity (int counter);
	bool hit (int counter, int octave, int velocity);
	bool hit (int coutner, int octave);
	void hit_top (int counter, int octave, int velocity);
	void hit_top (int counter, int octave);
	void hit_bottom (int counter, int octave, int velocity);
	void hit_bottom (int counter, int octave);
	void arrange (int key, int octave, int velocity);
	void relative (int key, int velocity);
public:
	virtual void notify_key_on (int key, int velocity);
	virtual void notify_key_off (int key);
	virtual void notify_key_off (void);
	virtual void notify_tempo (int beats_per_seconds, int seconds);
	virtual void notify_division (int beats_per_bar);
	virtual void notify_division (int beats_per_bar, int ticks_per_beat);
	virtual void notify_sub_division (int ticks_per_beat);
	virtual void notify_signal (void);
	virtual void notify_switch (bool on);
	virtual void notify_switch_loaded (bool on);
	virtual void notify_hold (bool hold);
	virtual void notify_variation (int variation);
	virtual void notify_pattern_change (void);
	virtual void notify_program_loaded (void);
	arranger (parameter_block * pb, int horizontal);
};

typedef arranger * arranger_pointer;

class arpeggiator_pool {
public:
	virtual arranger_pointer get_arpeggiator (int ind);
	virtual pattern_pointer get_default_pattern (void);
	virtual void move (void);
	virtual ~ arpeggiator_pool (void);
};

class arpeggio_pattern : public pattern {
public:
	void wait_beat (arranger * arp);
	void wait_beat (arranger * arp, int ind);
	virtual void tick_code (arranger * arp);
	virtual void beat_code (arranger * arp);
	virtual void bar_code (arranger * arp);
	virtual void code (arranger * arp);
};

class arpeggiator_cpp : public arranger {
private:
	int * arp_keys;
	int * arp_velocities;
	bool * arp_hold;
	int arp_max_keys;
	int arp_key_top;
	bool hold;
public:
	virtual int top_key_index (void);
	virtual int get_hit_key (int counter);
	virtual int get_velocity (int counter);
public:
	virtual void notify_key_on (int key, int velocity);
	virtual void notify_key_off (int key);
	virtual void notify_key_off (void);
	virtual void notify_switch (bool on);
	virtual void notify_switch_loaded (bool on);
	virtual void notify_hold (bool hold);
	virtual void notify_variation (int variation);
	arpeggiator_cpp (parameter_block * pb, int horizontal, int keys);
	~ arpeggiator_cpp (void);
};

typedef arpeggiator_cpp * arpeggiator_cpp_pointer;

class arpeggiator_pool_cpp : public arpeggiator_pool {
private:
	synthesizer * sth;
	arpeggiator_cpp_pointer * arps;
	int multitimbral;
public:
	virtual arranger_pointer get_arpeggiator (int ind);
	virtual void move (void);
	arpeggiator_pool_cpp (config * cfg, synthesizer * sth);
	~ arpeggiator_pool_cpp (void);
};

////////////
// VECTOR //
////////////

class vector : public transport {
public:
	virtual void notify_key_on (void);
	virtual void notify_key_off (void);
	virtual void notify_all_keys_off (void);
	virtual void notify_control_vector_x (int x);
	virtual void notify_control_vector_y (int y);
public:
	vector (int horizontal);
};

typedef vector * vector_pointer;

class vector_pool {
public:
	virtual vector_pointer get_vector (int ind);
	virtual void move (void);
	virtual ~ vector_pool (void);
};

class vector_cpp : public vector {
private:
	int key_counter;
	int frame_counter;
	bool recording;
	bool frame_x_recorded;
	bool frame_y_recorded;
	bool attacking;
	bool releasing;
	bool looping;
	bool rounding;
	int loop_target;
	int slope_time;
	int slope_time_counter;
	int start_x, start_y;
	parameter_block * pb;
	controllers_parameter_block * cb;
private:
	void start_playback (void);
	void attack_phase_finished (void);
	void start_loop (void);
	void loop_phase_finished (void);
	void start_recording (void);
public:
	virtual void notify_key_on (void);
	virtual void notify_key_off (void);
	virtual void notify_all_keys_off (void);
	virtual void notify_control_vector_x (int x);
	virtual void notify_control_vector_y (int y);
	virtual void tick_code (void);
public:
	vector_cpp (parameter_block * pb, int horizontal);
};

typedef vector_cpp * vector_cpp_pointer;

class vector_pool_cpp : public vector_pool {
public:
	synthesizer * sth;
	vector_cpp_pointer * vects;
	int multitimbral;
public:
	virtual vector_pointer get_vector (int ind);
	virtual void move (void);
	vector_pool_cpp (config * cfg, synthesizer * sth);
	~ vector_pool_cpp (void);
};

#endif
