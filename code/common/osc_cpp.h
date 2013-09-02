///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _OSCILLATORS_CPP
#define _OSCILLATORS_CPP

#include "data.h"
#include "osc.h"

class noise;
class dsp_line;
class dsp_algo_cpp;
class float_stereo_dsp_line_pool;
class oscillator;
class oscillator_pool_cpp;
class algo_cpp;
class dsp_algo_cpp;

class preset_algo_bank_cpp : public preset_algo_bank {
public:
	virtual algo_pointer get_algo (int lsb, int program, parameter_block * pb);
	virtual char * toString (int lsb);
	preset_algo_bank_cpp (void);
	~ preset_algo_bank_cpp (void);
};

class preset_dsp_algo_bank_cpp : public preset_dsp_algo_bank {
public:
	virtual dsp_algo_pointer get_dsp_algo (int program, parameter_block * pb);
};



typedef int * int_pointer;

class dsp_line;
typedef dsp_line * dsp_line_pointer;

class dsp_line {
public:
	synthesizer * sth;
	int horizontal;
	int vertical;
	int head_room;
	int parameters [16];
	float * variables;
	int * indices;
	float * inputs;
	float * outputs;
	float * line;
	int length;
	dsp_line_pointer * routers;
	int router_number;
	float * send_to;
	void create_routers (int ind);
	void remove_routers (void);
	void ground (void);
};

class dsp_algo_cpp : public dsp_algo {
public:
	virtual void code (dsp_line * dsp);
	virtual void parameter_change (dsp_line * dsp, int ind);
	virtual void all_parameters_changed (dsp_line * dsp);
};

typedef dsp_algo_cpp * dsp_algo_cpp_pointer;

class default_dsp_algo_cpp : public dsp_algo_cpp {
public:
	virtual char * toString (parameter_block * pb);
	virtual char * toString (parameter_block * pb, int ind);
	virtual int get_default (int ind);
	virtual void code (dsp_line * dsp);
	virtual void parameter_change (dsp_line * dsp, int ind);
	virtual void all_parameters_changed (dsp_line * dsp);
};

class float_stereo_dsp_line_pool : public dsp_line_pool {
public:
	float left_out;
	float right_out;
	float left_outs [2048];
	float right_outs [2048];
private:
	float global_left_out;
	float global_right_out;
	float head_room;
	int multitimbral;
	int local_dsp;
	int global_dsp;
	bool global_dsp_multi;
	int all_dsp;
	int all_dsp_multi;
	int total_dsp_number;
	int total_input_output;
	int total_local_dsp;
	int multi_step;
	int global_multi_step;
	int global_multi_single_step;
	float dco_blocking_fraction;
	float * dsp_inputs;
	float * dsp_outputs;
	float * dsp_input_ratios;
	float * dsp_output_ratios;
	float * volumes;
	float * global_volumes;
	float * dry_ratios;
	dsp_line * lines;
	dsp_algo_cpp_pointer * algos;
	void * entry_semaphore;
private:
	void notify_volume_change_only (parameter_block * pb);
public:
	virtual void add (int factory_id, int value, int * pan);
	virtual void move (void);
	virtual void multi_add (int factory_id, int value, int * pan, int sample);
	virtual void multi_move (int samples);
	virtual void notify_algo_change (parameter_block * pb, int ind);
	virtual void notify_volume_change (parameter_block * pb);
	virtual void notify_global_volume_change (parameter_block * pb);
	virtual void notify_dsp_change (parameter_block * pb, int nrpn_lsb);
	virtual void notify_dry_change (parameter_block * pb);
	virtual void notify_dry_wet_change_all (parameter_block * pb);
	virtual void notify_dry_wet_change (parameter_block * pb, int ind);
	virtual void notify_program_loaded (parameter_block * pb);
	virtual void notify_global_dsp_program_loaded (synthesizer * sth);
	virtual void notify_multi_global_dsp_program_loaded (parameter_block * pb);
	virtual void notify_dsp_ground (int factor_id);
	virtual dsp_algo_pointer get_default_dsp_algo (void);
private:
	void configure (synthesizer * sth, int local_dsp, int global_dsp, bool global_dsp_multi, int multitimbral, int horizontal, int vertical, int head_room, int dco_blocking_freq, int data_size, int line_size);
	void notify_global_algo_change (synthesizer * sth, int ind);
	void notify_global_dsp_change (synthesizer * sth, int ind, int parameter);
public:
	float_stereo_dsp_line_pool (config * cfg, synthesizer * sth);
	~ float_stereo_dsp_line_pool (void);
};

class adsr_control_block {
public:
	int size;
	int index;
	int * next_index;
	int * x;
	int * d0;
	int * d1;
	int * d2;
	int * y0;
	int * y1;
	int * y2;
	int * y_target;                                  // possible troublemaker
	int retrigger_index;
	int retrigger_x;
	int retrigger_y;
	int retrigger_d;
	int retrigger_d1;
	int retrigger_d2;
	int wave; // 0 = saw/sqr, 1 = sine, 2 = rnd, 3 = s/h
	int target;
	void constructor (int size);
	void adsr_constructor (void);
	void lfo_constructor (void);
	void key_glider_constructor (void);
	void destructor (void);
};

class oscillator {
public:
	synthesizer * sth;
	parameter_block * pb;
	oscillator_pool_cpp * pool;
	int stripes;
	int stereo;
	int vertical;
	unsigned int * time;
	unsigned int * freq;
	int * abstract_freq;
	int * ratio;
	int * amp;
	int * index;
	int * pan;
	int * feed;
	void * extra;
	float left_in;
	float right_in;
	int noise_in;
	multi_line outputs;
	multi_line multi_pan;
	void abstract_freq_to_real (int ind, int abstract_freq);
	~ oscillator (void) {}
};

class algo_cpp : public algo {
public:
	virtual int code (oscillator * osc);
	virtual void data_constructor (oscillator * osc);
	virtual void data_destructor (oscillator * osc);
	virtual void retrigger (oscillator * osc);
	virtual void release (oscillator * osc);
	virtual void ground (oscillator * osc);
	virtual void notify_algo_parameter_changed (parameter_block * pb, int ind);
	virtual void notify_all_algo_parameters_changed (parameter_block * pb);
};

typedef algo_cpp * algo_cpp_pointer;

#include "rom_player_bank_cpp.h"
#include "scripted_pattern_bank_cpp.h"
#include "wavetable_bank_cpp.h"

class default_algo_cpp : public algo_cpp {
public:
	virtual char * toString (parameter_block * pb);
	virtual char * toString (parameter_block * pb, int ind);
//	virtual int code (oscillator * osc);
};

class oscillator_pool_cpp_runner;
typedef oscillator_pool_cpp_runner * core_pointer;

class oscillator_pool_cpp : public oscillator_pool {
private:
	int style_program_size;
public:
	int horizontal, center_freq, vertical, head_room;
	float left_in;
	float right_in;
	float left_ins [2048];
	float right_ins [2048];
private:
	int stripes, stripes2, stripes3;
	int stereo;
	int stripes_stereo;
	int velocity_size;
	int multitimbral;
	int total_stripes;
	int total_stripes3;
	int total_stripes9;
	int slope_shift;
	int stripe_shift;
	int stripe_shift4;
	int total_stereo;
	int adsr_size;
	int lfo_size;
	int key_glider_size;
	int total_adsr_size;
	int lfo_gate_gap, lfo_gate_gap2;
	int core_number;
	void constructor (synthesizer * sth, int horizontal, int center_freq, int vertical, int head_room, int feed_space, dsp_line_pool * dsp, int style_program_size);
public:
	void initialize_math_support (void);
private:
	int * adsr_x, * pan_x, * lfo_x, * key_glide_x;
	int * adsr_d, * pan_d, * lfo_d, * key_glide_d;
	int * adsr_delta, * pan_delta, * lfo_delta, * key_glide_delta;
	int * adsr_y, * pan_y, * lfo_y, * key_glide_y;
	int * lfo_out;
	multi_line * lfo_outs;
	unsigned int * time;
	unsigned int * delta_time;
	int * freq, * amp, * index, * pan;
	int * feed;
	int * freq_shift, * amp_shift, * index_shift, * pan_shift;
	int * lfo_freq_shift, * lfo_amp_shift, * lfo_index_shift, * lfo_pan_shift;
	int * ratio;
	int * velocities;
//	int * lfo_rnds; made redundant by calculating velocity
	int * original_velocities;
	bool * retriggers_required;
	int * freq_lfo_source, * amp_lfo_source, * index_lfo_source, * pan_lfo_source;
	//@ index amp
//	bool * index_amps;
//	bool index_amp_required;

	int * slopes;
	adsr_control_block * ctrl, * pan_ctrl, * lfo_ctrl, * key_glide_ctrl;
	oscillator * osc;
	algo_cpp_pointer * algo;
	int * lfo_connections;
	noise rnd;
	dsp_line_pool * dsp;
private:
	int * amp_target;
	int * index_target;
	int * pan_target;
private:
	void calculate_velocities (int oscillator, int velocity);
	void retrigger_times (int index);
	void release_times (int index);
	void ground_times (int index);
	void retrigger_key_glider (int ind, bool skipper);
	void lfo_retrigger (int ind);
	void adsr_switch (int ind, int stage);
	void adsr_retrigger (int ind, int stage);
	void adsr_update_head (int ind, int adsr_ind, int type, int stripe);
	void adsr_update_tail (int ind, int adst_ind, int type, int stripe);
	void all_adsr_update_head (int ind);
	void all_adsr_update_tail (int ind);
	void adsr_move (int ind, int sample);
	void load_sensitivity (int oscillator);
	void load_freq_shifts (int oscillator);
	void load_amp_shifts (int oscillator);
	void load_index_shifts (int oscillator);
	void load_pan_shifts (int oscillator);
	void load_all_shifts (int oscillator, bool unfiltered);
	void load_freq_lfo_source (int oscillator);
	void load_amp_lfo_source (int oscillator);
	void load_index_lfo_source (int oscillator);
	void load_pan_lfo_source (int oscillator);
	void load_all_lfo_source (int oscillator);
	void load_ratios (int oscillator);
	void load_lfo_freq_shifts (int oscillator);
	void load_lfo_amp_shifts (int oscillator);
	void load_lfo_index_shifts (int oscillator);
	void load_lfo_pan_shifts (int oscillator);
	void load_lfo_all_shifts (int oscillator);
	//@ index amp
//	void check_index_amps (void);
	void algo_change (int ind);
public:
	virtual algo_pointer get_default_algo (void);
	virtual dsp_algo_pointer get_default_dsp_algo (void);
	virtual preset_algo_bank * create_preset_algo_bank (void);
	virtual preset_dsp_algo_bank * create_preset_dsp_algo_bank (void);
	virtual algo_bank * create_rom_player_bank (char * text, synthesizer * sth);
	virtual algo_bank * create_wavetable_bank (char * text, synthesizer * sth);
	virtual bank * create_plugin_algo_bank (char * plugin, char * parameter, synthesizer * sth);
	virtual pattern_bank * create_scripted_pattern_bank (char * text, synthesizer * sth);
	virtual bank * create_style_bank (char * text, synthesizer * sth);
public:
	virtual void notify_ratio_change (parameter_block * pb);
	virtual void notify_freq_shifts_update (parameter_block * pb);
	virtual void notify_amp_shifts_update (parameter_block * pb);
	virtual void notify_index_shifts_update (parameter_block * pb);
	//@ index amp
//	virtual void notify_index_amp_change (parameter_block * pb, int ind);
	virtual void notify_pan_shifts_update (parameter_block * pb);
	virtual void notify_all_shifts_update (parameter_block * pb);
	virtual void notify_freq_lfo_source_update (parameter_block * pb);
	virtual void notify_amp_lfo_source_update (parameter_block * pb);
	virtual void notify_index_lfo_source_update (parameter_block * pb);
	virtual void notify_pan_lfo_source_update (parameter_block * pb);
	virtual void notify_all_lfo_source_update (parameter_block * pb);
	virtual void notify_lfo_freq_shifts_update (parameter_block * pb);
	virtual void notify_lfo_amp_shifts_update (parameter_block * pb);
	virtual void notify_lfo_index_shifts_update (parameter_block * pb);
	virtual void notify_lfo_pan_shifts_update (parameter_block * pb);
	virtual void notify_lfo_all_shifts_update (parameter_block * pb);
	virtual void notify_lfo_speeds_update (parameter_block * pb);
	virtual void notify_lfo_retrigger_required (parameter_block * pb);
	virtual void notify_lfo_wave_change (parameter_block * pb);
	virtual void notify_lfo_pulse_width_change (parameter_block * pb);
	virtual void notify_sensitivity_change (parameter_block * pb, int nrpn_msb, int xxx, int nnnn);
	virtual void notify_algo_change (parameter_block * pb);
	virtual void notify_dsp_algo_change (parameter_block * pb, int ind);
	virtual void notify_tune (int tune);
	virtual void notify_algo_parameter_changed (parameter_block * pb, int ind);
	virtual void notify_oscillator_assign (parameter_block * pb, int oscillator);
	virtual void notify_volume_change (parameter_block * pb);
	virtual void notify_global_volume_change (parameter_block * pb);
	virtual void notify_dsp_change (parameter_block * pb, int nrpn_lsb);
	virtual void notify_dsp_ground (parameter_block * pb);
	virtual void notify_dry_change (parameter_block * pb);
	virtual void notify_dry_wet_change_all (parameter_block * pb);
	virtual void notify_dry_wet_change (parameter_block * pb, int ind);
	virtual void notify_program_loaded (parameter_block * pb);
	virtual void notify_dsp_program_loaded (parameter_block * pb);
	virtual void notify_global_dsp_program_loaded (synthesizer * sth);
	virtual void notify_multi_global_dsp_program_loaded (parameter_block * pb);
	virtual void lock_retrigger (int oscillator);
protected:
	virtual void key_on (int oscillator, int key); //  legato (mono)
	virtual void key_on (int oscillator, int key, int velocity); // key on (mono)
	virtual void key_on (parameter_block * pb, int oscillator, int key, int velocity); // ground & key on (poly)
public:
	virtual void notify_key_off (int oscillator);
	virtual void notify_ground (int oscillator);
	virtual void move (void);
	virtual void multi_move_parts (int samples);
	virtual void multi_move_oscillators (int samples);
	virtual void multi_move_dsp (int samples);
	void move_oscillator (int counter, int samples);
	void move_oscillators (int from, int to, int samples); // from inclusive to exclusive
	void transfer_to_dsp (int sample);
	oscillator_pool_cpp (config * cfg, synthesizer * sth, dsp_line_pool * dsp);
	~ oscillator_pool_cpp (void);
};

class oscillator_pool_cpp_runner {
public:
	oscillator_pool_cpp * osc_cpp;
	int from, to;
	bool active, finished;
	void * enter_semaphore;
	void * exit_semaphore;
	int buffer_size;
	void move ();
	oscillator_pool_cpp_runner (oscillator_pool_cpp * osc_cpp, int from, int to);
	~ oscillator_pool_cpp_runner (void);
};

#endif
