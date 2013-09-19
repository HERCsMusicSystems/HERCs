///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _OSCILLATORS
#define _OSCILLATORS

#include "data.h"

class dsp_line_pool;

typedef int multi_line [2048];

class noise {
private:
	int a, c, m, v;
	int shift, right_shift;
	int range;
public:
	int out;
	multi_line outs;
	int move (void);
	int move (int min, int max);                       // inclusive min / exclusive max
	void resolution (int bits);
	void resolution (int bits, int lsb_bits);
	void control (int out);
	void control (int a, int c);
	noise (void);
};

class dsp_line_pool {
public:
	// single threaded
	virtual void add (int factory_id, int value, int * pan);
	virtual void move (void);
	// multi threaded
	virtual void multi_add (int factory_id, int value, int * pan, int sample);
	virtual void multi_move (int samples);
	// end threaded
	virtual void notify_algo_change (parameter_block * pb, int ind);
	virtual void notify_volume_change (parameter_block * pb);
	virtual void notify_global_volume_change (parameter_block * pb);
	virtual void notify_dsp_change (parameter_block * pb, int nrpn_lsb);
	virtual void notify_dry_change (parameter_block * pb);
	virtual void notify_dry_wet_change_all (parameter_block * pb);
	virtual void notify_dry_wet_change (parameter_block * pb, int ind);
	virtual void notify_program_loaded (parameter_block * pb);
	virtual void notify_global_dsp_program_loaded (synthesiser * sth);
	virtual void notify_multi_global_dsp_program_loaded (parameter_block * pb);
	virtual void notify_dsp_ground (int factory_id);
	virtual dsp_algo_pointer get_default_dsp_algo (void);
};

class oscillator_pool {
public:
	int polyphony;
	int unlock_counter;
public:
	int * part_connections;
	int * keys;
private:
	int * reserved_notes_from;
	int * reserved_notes_to;
	int free_oscillators_from;
	int * locked;
	bool * reserved;
	bool * hold;

private:
	void constructor (int polyphony, int multitimbral);
public:
	int lock_oscillator (int part, int key);
	void unlock_oscillator (int oscillator);
	int get_oscillator (int part, int key);
	int reserve_oscillator (int part);
	void return_oscillator (int ind);
	bool oscillator_holding (int oscillator);
	void hold_oscillator (int oscillator);
	void unhold_oscillator (int oscillator);
	virtual algo_pointer get_default_algo (void);
	virtual dsp_algo_pointer get_default_dsp_algo (void);
	virtual preset_algo_bank * create_preset_algo_bank (void);
	virtual preset_dsp_algo_bank * create_preset_dsp_algo_bank (void);
	virtual algo_bank * create_rom_player_bank (char * text, synthesiser * sth);
	virtual algo_bank * create_wavetable_bank (char * text, synthesiser * sth);
	virtual bank * create_plugin_algo_bank (char * plugin, char * parameter, synthesiser * sth);
	virtual pattern_bank * create_scripted_pattern_bank (char * text, synthesiser * sth);
	virtual bank * create_style_bank (char * text, synthesiser * sth);
public:
	virtual void notify_retrigger_change (parameter_block * pb);
	virtual void notify_ratio_change (parameter_block * pb);
	virtual void notify_freq_shifts_update (parameter_block * pb);
	virtual void notify_amp_shifts_update (parameter_block * pb);
	virtual void notify_index_shifts_update (parameter_block * pb);
	//@ index amp
//	virtual void notify_index_amp_change (parameter_block * pb, int ind);
	virtual void notify_rnd_update (parameter_block * pb);
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
	virtual void notify_attack_change (parameter_block * pb);
	virtual void notify_decay_change (parameter_block * pb);
	virtual void notify_sustain_change (parameter_block * pb);
	virtual void notify_release_change (parameter_block * pb);
	virtual void notify_adsr_level_update (parameter_block * pb, int nrpn_msb, int xxx, int nnnn);
	virtual void notify_adsr_time_update (parameter_block * pb, int nrpn_msb, int xxx, int nnnn);
	virtual void notify_porta_update (parameter_block * pb);
	virtual void notify_tune (int tune);
	virtual void notify_tune (parameter_block * pb);
	virtual void notify_reserved_notes_changed (synthesiser * sth);
	virtual void notify_algo_parameter_changed (parameter_block * pb, int ind);
	virtual void notify_oscillator_assign (parameter_block * pb, int oscillator);
	virtual void notify_volume_change (parameter_block * pb);
	virtual void notify_global_volume_change (parameter_block * pb);
	virtual void notify_dsp_change (parameter_block * pb, int nrpn_lsb);
	virtual void notify_dry_change (parameter_block * pb);
	virtual void notify_dry_wet_change_all (parameter_block * pb);
	virtual void notify_dry_wet_change (parameter_block * pb, int ind);
	virtual void notify_dsp_ground (parameter_block * pb);
	virtual void notify_program_loaded (parameter_block * pb);
	virtual void notify_dsp_program_loaded (parameter_block * pb);
	virtual void notify_global_dsp_program_loaded (synthesiser * sth);
	virtual void notify_multi_global_dsp_program_loaded (parameter_block * sth);
	virtual void lock_retrigger (int oscillator);
protected:
	virtual void key_on (int oscillator, int key);
	virtual void key_on (int oscillator, int key, int velocity);
	virtual void key_on (parameter_block * pb, int oscillator, int key, int velocity);
public:
	void notify_key_on (int oscillator, int key); // legato (mono)
	void notify_key_on (int oscillator, int key, int velocity); // key on (mono)
	void notify_key_on (parameter_block * pb, int oscillator, int key, int velocity); // ground & key on (poly)
	virtual void notify_key_off (int oscillator);
	virtual void notify_ground (int oscillator);
	// single threaded
	virtual void move (void);
	// multi threaded
	virtual void multi_move_parts (int samples);
	virtual void multi_move_oscillators (int samples);
	virtual void multi_move_dsp (int samples);
	// end threaded
public:
	oscillator_pool (config * cfg, synthesiser * sth);
	~ oscillator_pool (void);
};

#endif

