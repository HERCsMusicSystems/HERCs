///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc.h"
#include "math.h"

int noise :: move (void) {
	v *= a;
	v += c;
	v &= m;
	out = (v - shift) >> right_shift;
	return out;
}

int noise :: move (int min, int max) {
	v *= a;
	v += c;
	v &= m;
	out = v;
	out *= max - min;
	out /= range;
	out += min;
	return out;
}

void noise :: resolution (int bits) {
	range = 1 << bits;
	m = range - 1;
	bits--;
	shift = 1 << bits;
	right_shift = 0;
}

void noise :: resolution (int bits, int lsb_bits) {
	right_shift = lsb_bits;
	bits += lsb_bits;
	range = 1 << bits;
	m = range - 1;
	bits--;
	shift = 1 << bits;
}

void noise :: control (int out) {
	this -> out = out;
	v = out + shift;
}

void noise :: control (int a, int c) {
	this -> a = a;
	this -> c = c;
}

noise :: noise (void) {
	resolution (14, 10);
	control (0);
	control (0x5599d1, 1);
}

char * algo :: toString (parameter_block * pb) {return "SILENCE";}
char * algo :: toString (parameter_block * pb, int ind) {return "silence";}
//void algo :: control (int lsb, int program) {}
//bool algo :: is_control (int ind) {return false;}
//int algo :: lsb (int ind) {return 0;}
//int algo :: program (int ind) {return 0;}
char * algo :: get_name (int ind) {return "NOT USED";}
int algo :: get_default (int ind) {return 0;}
//void algo :: notify_algo_parameter_changed (parameter_block * pb, int ind) {}
//void algo :: notify_all_algo_parameters_changed (parameter_block * pb) {}

algo silence;

char * dsp_algo :: toString (parameter_block * pb) {return "SILENCE";}
char * dsp_algo :: toString (parameter_block * pb, int ind) {return "NOT USED";}
int dsp_algo :: get_default (int ind) {return 0;}

dsp_algo silence_dsp_algo;

oscillator_pool :: oscillator_pool (config * cfg, synthesiser * sth) {
	constructor (cfg -> oscillators, cfg -> multitimbral);
	sth -> connect_oscillator_pool (this);
}

void oscillator_pool :: constructor (int polyphony, int multitimbral) {
	this -> polyphony = polyphony;

	part_connections = new int [polyphony];
	reserved_notes_from = new int [multitimbral];
	reserved_notes_to = new int [multitimbral];
	locked = new int [polyphony];
	reserved = new bool [polyphony];
	hold = new bool [polyphony];
	keys = new int [polyphony];
	unlock_counter = 1;

	int ind;
	for (ind = 0; ind < polyphony; ind++) {
		part_connections [ind] = 0;
		locked [ind] = unlock_counter++;
		reserved [ind] = false;
		hold [ind] = false;
		keys [ind] = -1;
	}

	free_oscillators_from = 0;
	for (ind = 0; ind < multitimbral; ind++) {
		reserved_notes_from [ind] = 0;
		reserved_notes_to [ind] = polyphony;
	}
}

oscillator_pool :: ~ oscillator_pool (void) {
	delete [] part_connections;
	delete [] reserved_notes_from;
	delete [] reserved_notes_to;
	delete [] locked;
	delete [] reserved;
	delete [] hold;
	delete [] keys;
}

void oscillator_pool :: notify_reserved_notes_changed (synthesiser * sth) {
	int multitimbral = sth -> multitimbral;
	free_oscillators_from = 0;
	int reserved_notes;
	for (int ind = 0; ind < multitimbral; ind++) {
		reserved_notes = sth -> data_blocks [ind] -> system -> reserved_notes;
		if (reserved_notes == 0x7f) {
			reserved_notes_from [ind] = 0;
			reserved_notes_to [ind] = polyphony;
		} else {
			reserved_notes_from [ind] = free_oscillators_from;
			free_oscillators_from += reserved_notes;
			if (free_oscillators_from > polyphony) free_oscillators_from = polyphony;
			reserved_notes_to [ind] = free_oscillators_from;
		}
	}
}

#define IS_FREE if (locked [ind] > 0 && locked [ind] < found) {found = locked [ind]; root = ind;}
#define IS_NON_RESERVED if (! reserved [ind] && locked [ind] > found) {found = locked [ind]; root = ind;}
#define IS_LOCKED if (locked [ind] > found) {found = locked [ind]; root = ind;}
#define RETURN_ROTATOR {part_connections [root] = part; keys [root] = key; locked [root] = - (unlock_counter++); return root;}
int oscillator_pool :: lock_oscillator (int part, int key) {
	int ind;
	int found = unlock_counter;
	int from = reserved_notes_from [part];
	int to = reserved_notes_to [part];
	int root = 0;
	// searching part for free oscillators
	for (ind = from; ind < to; ind++) IS_FREE
	if (found < unlock_counter) RETURN_ROTATOR
	// searching unassigned for free oscillators
	for (ind = free_oscillators_from; ind < polyphony; ind++) IS_FREE
	if (found < unlock_counter) RETURN_ROTATOR
	// searching (part and unassigned) for non-reserved and the longest playing oscillator
	found = - unlock_counter;
	for (ind = from; ind < to; ind++) IS_NON_RESERVED
	for (ind = free_oscillators_from; ind < polyphony; ind++) IS_NON_RESERVED
	if (found > - unlock_counter) RETURN_ROTATOR
	// searching (part and unassigned) for the longest playing (reserved) oscillator
	for (ind = from; ind < to; ind++) IS_LOCKED
	for (ind = free_oscillators_from; ind < polyphony; ind++) IS_LOCKED
	if (found > - unlock_counter) RETURN_ROTATOR
	// there are no part nor unassigned oscillators
	// searching for free oscillators
	found = unlock_counter;
	for (ind = 0; ind < polyphony; ind++) IS_FREE
	if (found < unlock_counter) RETURN_ROTATOR
	// searching for non-reserved and the longes playing oscillator
	found = - unlock_counter;
	for (ind = 0; ind < polyphony; ind++) IS_NON_RESERVED
	if (found > - unlock_counter) RETURN_ROTATOR
	// searching for the longes playing (reserved) oscillator
	for (ind = 0; ind < polyphony; ind++) IS_LOCKED
	RETURN_ROTATOR
}

void oscillator_pool :: unlock_oscillator (int oscillator) {
	if (reserved [oscillator]) return;
	if (locked [oscillator] > 0) return;
	locked [oscillator] = unlock_counter++;
}

int oscillator_pool :: reserve_oscillator (int part) {
	int ind = lock_oscillator (part, 0);
	reserved [ind] = true;
	return ind;
}

int oscillator_pool :: get_oscillator (int part, int key) {
	for (int ind = 0; ind < polyphony; ind++) {
		if (locked [ind] <= 0 && part_connections [ind] == part && keys [ind] == key && ! hold [ind])
			return ind;
	}
	return -1;
}

void oscillator_pool :: return_oscillator (int ind) {
	if (ind < 0) return;
	if (ind >= polyphony) return;
	reserved [ind] = false;
	locked [ind] = unlock_counter++;
}

bool oscillator_pool :: oscillator_holding (int oscillator) {return hold [oscillator];}
void oscillator_pool :: hold_oscillator (int oscillator) {hold [oscillator] = true;}
void oscillator_pool :: unhold_oscillator (int oscillator) {hold [oscillator] = false;}

algo_pointer oscillator_pool :: get_default_algo (void) {return & silence;}
dsp_algo_pointer oscillator_pool :: get_default_dsp_algo (void) {return & silence_dsp_algo;}
preset_algo_bank * oscillator_pool :: create_preset_algo_bank (void) {return NULL;}
preset_dsp_algo_bank * oscillator_pool :: create_preset_dsp_algo_bank (void) {return NULL;}
algo_bank * oscillator_pool :: create_rom_player_bank (char * text, synthesiser * sth) {return NULL;}
algo_bank * oscillator_pool :: create_wavetable_bank (char * text, synthesiser * sth) {return NULL;}
bank * oscillator_pool :: create_plugin_algo_bank (char * plugin, char * parameter, synthesiser * sth) {return NULL;}
pattern_bank * oscillator_pool :: create_scripted_pattern_bank (char * text, synthesiser * sth) {return NULL;}
bank * oscillator_pool :: create_style_bank (char * text, synthesiser * sth) {return NULL;}

void oscillator_pool :: notify_retrigger_change (parameter_block * pb) {}
void oscillator_pool :: notify_ratio_change (parameter_block * pb) {}
void oscillator_pool :: notify_freq_shifts_update (parameter_block * pb) {}
void oscillator_pool :: notify_amp_shifts_update (parameter_block * pb) {}
void oscillator_pool :: notify_index_shifts_update (parameter_block * pb) {}
//@ index amp
//void oscillator_pool :: notify_index_amp_change (parameter_block * pb, int ind) {}
void oscillator_pool :: notify_rnd_update (parameter_block * pb) {}
void oscillator_pool :: notify_pan_shifts_update (parameter_block * pb) {}
void oscillator_pool :: notify_all_shifts_update (parameter_block * pb) {}
void oscillator_pool :: notify_freq_lfo_source_update (parameter_block * pb) {}
void oscillator_pool :: notify_amp_lfo_source_update (parameter_block * pb) {}
void oscillator_pool :: notify_index_lfo_source_update (parameter_block * pb) {}
void oscillator_pool :: notify_pan_lfo_source_update (parameter_block * pb) {}
void oscillator_pool :: notify_all_lfo_source_update (parameter_block * pb) {}
void oscillator_pool :: notify_lfo_freq_shifts_update (parameter_block * pb) {}
void oscillator_pool :: notify_lfo_amp_shifts_update (parameter_block * pb) {}
void oscillator_pool :: notify_lfo_index_shifts_update (parameter_block * pb) {}
void oscillator_pool :: notify_lfo_pan_shifts_update (parameter_block * pb) {}
void oscillator_pool :: notify_lfo_all_shifts_update (parameter_block * pb) {}
void oscillator_pool :: notify_lfo_speeds_update (parameter_block * pb) {}
void oscillator_pool :: notify_lfo_retrigger_required (parameter_block * pb) {}
void oscillator_pool :: notify_lfo_wave_change (parameter_block * pb) {}
void oscillator_pool :: notify_lfo_pulse_width_change (parameter_block * pb) {}
void oscillator_pool :: notify_sensitivity_change (parameter_block * pb, int nrpn_msb, int xxx, int nnnn) {}
void oscillator_pool :: notify_algo_change (parameter_block * pb) {}
void oscillator_pool :: notify_dsp_algo_change (parameter_block * pb, int ind) {}
void oscillator_pool :: notify_attack_change (parameter_block * pb) {}
void oscillator_pool :: notify_decay_change (parameter_block * pb) {}
void oscillator_pool :: notify_sustain_change (parameter_block * pb) {}
void oscillator_pool :: notify_release_change (parameter_block * pb) {}
void oscillator_pool :: notify_adsr_level_update (parameter_block * pb, int nrpn_msb, int xxx, int nnnn) {}
void oscillator_pool :: notify_adsr_time_update (parameter_block * pb, int nrpn_msb, int xxx, int nnnn) {}
void oscillator_pool :: notify_porta_update (parameter_block * pb) {}
void oscillator_pool :: notify_tune (int tune) {}
void oscillator_pool :: notify_tune (parameter_block * pb) {notify_tune (pb -> sth -> tune);}
void oscillator_pool :: notify_algo_parameter_changed (parameter_block * pb, int ind) {}
void oscillator_pool :: notify_dsp_change (parameter_block * pb, int nrpn_lsb) {}
void oscillator_pool :: notify_dry_change (parameter_block * pb) {}
void oscillator_pool :: notify_dry_wet_change_all (parameter_block * pb) {}
void oscillator_pool :: notify_dry_wet_change (parameter_block * pb, int ind) {}
void oscillator_pool :: notify_dsp_ground (parameter_block * pb) {}
void oscillator_pool :: notify_volume_change (parameter_block * pb) {}
void oscillator_pool :: notify_global_volume_change (parameter_block * pb) {}
void oscillator_pool :: notify_oscillator_assign (parameter_block * pb, int oscillator) {}
void oscillator_pool :: lock_retrigger (int oscillator) {}
void oscillator_pool :: key_on (int oscillator, int key) {}
void oscillator_pool :: key_on (int oscillator, int key, int velocity) {}
void oscillator_pool :: key_on (parameter_block * pb, int oscillator, int key, int velocity) {}
void oscillator_pool :: notify_key_off (int oscillator) {}
void oscillator_pool :: notify_ground (int oscillator) {}
void oscillator_pool :: notify_program_loaded (parameter_block * pb) {}
void oscillator_pool :: notify_dsp_program_loaded (parameter_block * pb) {}
void oscillator_pool :: notify_global_dsp_program_loaded (synthesiser * sth) {}
void oscillator_pool :: notify_multi_global_dsp_program_loaded (parameter_block * pb) {}
void oscillator_pool :: move (void) {}
void oscillator_pool :: multi_move_parts (int samples) {}
void oscillator_pool :: multi_move_oscillators (int samples) {}
void oscillator_pool :: multi_move_dsp (int samples) {}

void oscillator_pool :: notify_key_on (int oscillator, int key) {
	keys [oscillator] = key;
	key_on (oscillator, key);
}

void oscillator_pool :: notify_key_on (int oscillator, int key, int velocity) {
	keys [oscillator] = key;
	key_on (oscillator, key, velocity);
}

void oscillator_pool :: notify_key_on (parameter_block * pb, int oscillator, int key, int velocity) {
	keys [oscillator] = key;
	key_on (pb, oscillator, key, velocity);
}

void dsp_line_pool :: add (int factory_id, int value, int * pan) {}
void dsp_line_pool :: move (void) {}
void dsp_line_pool :: multi_add (int factory_id, int value, int * pan, int sample) {}
void dsp_line_pool :: multi_move (int samples) {}
void dsp_line_pool :: notify_algo_change (parameter_block * pb, int ind) {}
void dsp_line_pool :: notify_volume_change (parameter_block * pb) {}
void dsp_line_pool :: notify_global_volume_change (parameter_block * pb) {}
void dsp_line_pool :: notify_dsp_change (parameter_block * pb, int nrpn_lsb) {}
void dsp_line_pool :: notify_dry_change (parameter_block * pb) {}
void dsp_line_pool :: notify_dry_wet_change_all (parameter_block * pb) {}
void dsp_line_pool :: notify_dry_wet_change (parameter_block * pb, int ind) {}
void dsp_line_pool :: notify_program_loaded (parameter_block * pb) {}
void dsp_line_pool :: notify_global_dsp_program_loaded (synthesiser * sth) {}
void dsp_line_pool :: notify_multi_global_dsp_program_loaded (parameter_block * pb) {}
void dsp_line_pool :: notify_dsp_ground (int factory_id) {}
extern dsp_algo silence_dsp_algo;
dsp_algo_pointer dsp_line_pool :: get_default_dsp_algo (void) {return & silence_dsp_algo;}

