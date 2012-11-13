///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "data.h"
#include "osc.h"
#include "transport.h"

void string_copy (char * to, char * from) {
	int ind = STRING_SIZE - 1;
	while (* from != '\0') {
		if (ind-- == 0) {* to = '\0'; return;}
		* to++ = * from++;
	}
	* to = '\0';
}

void sub_parameter_block :: voice_init (int selector) {
	bp = 8192;
	lc = -selector;
	rc = selector;
}

void sub_parameter_block :: voice_init (void) {
	bp = 0;
	lc = 0;
	rc = 0;
}

void sub_parameter_block :: dry_init (void) {
	bp = 8192;
	lc = 0;
	rc = -256;
}

void sub_parameter_block :: wet_init (void) {
	bp = 8192;
	lc = -256;
	rc = 0;
}

int sub_parameter_block :: get (int ind) {
	ind -= bp;
	if (ind > 0) return (ind * rc) >> 7;
	return (- ind * lc) >> 7;
}

int sub_parameter_block :: get_negative (int ind) {
	int sub = 0;
	int rcx = (rc * (16383 - bp)) >> 7;
	int lcx = (lc * bp) >> 7;
	if (rcx > sub) sub = rcx;
	if (lcx > sub) sub = lcx;
	sub = 0; // this line deactivates negative effect
	ind -= bp;
	if (ind > 0) return ((ind * rc) >> 7) - sub;
	return ((- ind * lc) >> 7) - sub;
}

int sub_parameter_block :: get_negative (void) {
	int sub = 0;
	int rcx = (rc * (16383 - bp)) >> 7;
	int lcx = (lc * bp) >> 7;
	if (rcx > sub) sub = rcx;
	if (lcx > sub) sub = lcx;
	sub = 0; // this line deactivates negative effect
	return sub;
}

void adsr_parameter_block :: voice_init (int selector) {
	for (int ind = 0; ind < 4; ind++) {
		level [ind] = 0;
		time [ind] = 0;
	}
	level [3] = selector;
}

void sensitivity_parameter_block :: voice_init (int selector) {
	pitch . voice_init (selector * 2);// / 32);
	shift . voice_init (0);
	lfo . voice_init ();
	attack . voice_init (0);
	decay . voice_init (0);
	sustain . voice_init (0);
	release . voice_init (0);
	key . voice_init (selector);
	velocity . voice_init (0);
	vector_x . voice_init (0);
	vector_y . voice_init (0);
	lfo_x . voice_init (0);
	lfo_y . voice_init (0);
	key_eg_scaling . voice_init (0);
	velocity_eg_scaling . voice_init (0);
}

void lfo_sensitivity_parameter_block :: voice_init (void) {
	speed . voice_init (0);
	pitch . voice_init (0);
	vector_x . voice_init (0);
	vector_y . voice_init (0);
}

void stripe_parameter_block :: voice_init (int ind, int selector) {
//	string_copy (algo_name, "sine");
	retrigger = false;
	multiplier = 1;
	freq = 0; //8192;
	amp = selector;
	index = 0;
	//@ index amp
//	index_amp = false;
	freq_eg . voice_init (0);
	amp_eg . voice_init (-16383);
	index_eg . voice_init (0);
	freq_rnd = 0;
	amp_rnd = 0;
	index_rnd = 0;
	lfo_retrigger = false;
	lfo_freq = 0;
	lfo_amp = 0;
	lfo_index = 0;
	speed = 8192;
	pulse_width = 0;
	wave = 0;
	lfo_sync = ind;
	freq_lfo_source = ind;
	amp_lfo_source = ind;
	index_lfo_source = ind;
	phase_shift = 0;
	freq_sens . voice_init (128);
	amp_sens . voice_init (0);
	index_sens . voice_init (0);
	lfo_sens . voice_init ();
}

system_parameter_block :: system_parameter_block (void) {

	device_id = 0x7f;
	midi_channel = 0x7f;
	midi_channel_extension = 0x7f;
	arranger_assign = 0x7f;
	reserved_notes = 0x7f;

	bank_msb = 0;
	bank_lsb = 0;
	program = 0;

	nrpn_msb = 127;
	nrpn_lsb = 127;
	data_lsb = 0;
	rpn_msb = 127;
	rpn_lsb = 127;

	pitch = 128;
	freq = 74;
	amp = 11;
	index = 71;
	attack = 73;
	decay = 76;
	sustain = 75;
	release = 72;
	speed = 77;
	vibrato = 1;
	tremolo = 92;
	lfo_index = 95;
	lfo_pan = 78;
	vector_x = 16;
	vector_y = 17;
	volume = 7;
	pan = 10;
	hold = 64;
	porta = 65;
	porta_time = 5;
	porta_mode = 84;

	dry_wet = 91;
	global_volume = 79;
}

void stereo_stripe_parameter_block :: voice_init (int ind) {
	pan = 0;
	lfo = 0;
	eg . voice_init (0);
	rnd = 0;
	lfo_retrigger = false;
	lfo_sync = 16 + ind;
	pan_lfo_source = 16 + ind;
	speed = 8192;
	phase_shift = 0;
	pulse_width = 0;
	wave = 0;
	sens . voice_init (0);
	sens . shift . lc = -128;
	sens . shift . rc = 128;
	lfo_sens . voice_init ();
}

void controllers_parameter_block :: reset_all_controllers (void) {
	pitch = 8192;
	freq = 8192;
	amp = 8192;
	index = 8192;
	attack = 8192;
	decay = 8192;
	sustain = 8192;
	release = 8192;
	speed = 8192;
	vibrato = 0;
	tremolo = 0;
	lfo_index = 0;
	lfo_pan = 0;
	vector_x = 8192;
	vector_y = 8192;
	if (volume_reset) volume = 12800;
	pan = 8192;
	hold = false;
	dry_wet = 8192;
}

void controllers_parameter_block :: voice_init (void) {
	reset_all_controllers ();
//	poly = true;		because parameter block calls poly (); and handles it in constructor
	porta = false;
	porta_time = 8192;
	porta_mode = 0;
	global_volume = 16383;
//	global_volume = 16383;		moved into the constructor because of setup file

	arpeggiator_switch = false;
	arpeggiator_hold = false;
	beats_per_seconds = 140;
	seconds = 60;
	beats_per_bar = 4;
	ticks_per_beat = 24;
	variation = 0;

	vector_switch = false;
	vector_init_x = 8192;
	vector_init_y = 8192;
	vector_frame_count = 0;
	vector_frame_selector = 0;
	vector_resolution = 50;
	vector_resolution_seconds = 128;
	vector_trigger = 0;
	vector_attack = 0;
	vector_release = 16383;
	vector_loop_type = 0;
	vector_loop_start = 0;
	vector_loop_size = 0;
	vector_loop_delay = 0;

	for (int ind = 0; ind < 128; ind++) key_tuner [ind] = ind << 7;
	for (int ind = 0; ind < 128; ind++) algo_parameters [ind] = 0;
}

void dsp_router :: voice_init (int level) {
	send_level = level;
	dry_wet . voice_init (0);
	vector_x . voice_init (0);
	vector_y . voice_init (0);
}

void dsp_router :: dry_init (void) {
	send_level = 16383;
	dry_wet . dry_init ();
	vector_x . voice_init (0);
	vector_y . voice_init (0);
}

void dsp_router :: wet_init (void) {
	send_level = 16383;
	dry_wet . wet_init ();
	vector_x . voice_init (0);
	vector_y . voice_init (0);
}

int dsp_router :: get (int ctrl_dry_wet, int ctrl_vector_x, int ctrl_vector_y) {
	return send_level + dry_wet . get (ctrl_dry_wet) + vector_x . get (ctrl_vector_x) + vector_y . get (ctrl_vector_y);
}

void dsp_parameter_block :: voice_init (void) {
	string_copy (algo_msb, "PRESET DSP ALGO BANK");
	algo_program = 0;
	algo_head = sth -> get_default_dsp_algo ();
	on = false;
	wet_output . wet_init ();
//	wet_output = 16383;
//	dry_wet_out_sens . wet_init ();
//	vector_x_out_sens . voice_init (0);
//	vector_y_out_sens . voice_init (0);
	int sub;
	for (sub = 0; sub < 16; sub++) parameters [sub] = algo_head -> get_default (sub);
	for (sub = 0; sub < router_number; sub++) routers [sub] -> voice_init (0);
}

dsp_parameter_block :: dsp_parameter_block (synthesizer * sth, int ind, int dsp_number) {
	this -> sth = sth;
	algo_head = sth -> get_default_dsp_algo ();
	router_number = dsp_number - ind - 1;
	if (router_number > 0) routers = new dsp_router_pointer [router_number];
	else routers = NULL;
	for (int sub = 0; sub < router_number; sub++) routers [sub] = new dsp_router ();
	algo_notification_not_required = false;
}

dsp_parameter_block :: ~ dsp_parameter_block (void) {
	if (routers == NULL) return;
	for (int ind = 0; ind < router_number; ind++) delete routers [ind];
	delete [] routers;
//	if (router_number > 0) delete [] routers;
}

void parameter_block :: voice_init (void) {
	int ind = 0;
	string_copy (name, "RESET");
	category = 0;
	string_copy (algo_msb, "PRESET ALGO BANK");
	algo_lsb = 0;
	algo_program = 0;
	algo_head = sth -> get_default_algo ();
	string_copy (pattern_msb, "PRESET PATTERN BANK");
	pattern_lsb = 0;
	pattern_program = 0;
	pattern_head = sth -> get_default_pattern ();
	controllers -> voice_init ();
	for (ind = 0; ind < vector_size; ind++) {
		vector_frames [ind] . delay = 1;
		vector_frames [ind] . x = 8192;
		vector_frames [ind] . y = 8192;
	}
	poly (false);				// will return at the first time because of constructor
	stripes [0] -> voice_init (0, 0);
	for (ind = 1; ind < hercules_number; ind++)
		stripes [ind] -> voice_init (ind, -16383);
	for (ind = 0; ind < hercules_stereo_number; ind++)
		stereo [ind] -> voice_init (ind);
	local_dsp_voice_init ();
}
//	for (ind = 0; ind < hercules_local_dsp_number; ind++) {
//		dsp [ind] -> voice_init (ind);
//	}
//	if (hercules_local_dsp_number > 0) dsp [0] -> on = true;
//	local_dry_out = 16383;
//	dry_wet_local_out_sens . dry_init ();
//	vector_x_local_out_sens . voice_init (0);
//	vector_y_local_out_sens . voice_init (0);
//	for (ind = 0; ind < hercules_local_dsp_number; ind++) {
//		dsp_send [ind] = (ind == 0 ? 16383 : 0);
//		dry_wet_dsp_send_sens [ind] . voice_init (0);
//		vector_x_dsp_send_sens [ind] . voice_init (0);
//		vector_y_dsp_send_sens [ind] . voice_init (0);
//	}
//}

void parameter_block :: dsp_voice_init (bool global) {
	if (global) local_dsp_voice_init ();
	else global_dsp_voice_init ();
}

void parameter_block :: local_dsp_voice_init (void) {
	local_dry_out . dry_init ();
	for (int ind = 0; ind < hercules_local_dsp_number; ind++) {
		dsp [ind] -> voice_init ();
		dsp_send [ind] . voice_init (ind == 0 ? 16383 : 0);
	}
	if (hercules_local_dsp_number > 0) dsp [0] -> on = true;
}

void parameter_block :: global_dsp_voice_init (void) {
	reset_global_dsp_parameters ();
	for (int ind = hercules_local_dsp_number; ind < hercules_all_dsp_number; ind++)
		dsp [ind] -> voice_init ();
}

void parameter_block :: reset_global_dsp_parameters (void) {
	global_dry_out . voice_init (16383);
	for (int ind = hercules_local_dsp_number; ind < hercules_all_dsp_number; ind++) {
		dsp_send [ind] . voice_init (ind == hercules_local_dsp_number ? 16383 : 0);
	}
}

parameter_block :: parameter_block (synthesizer * sth, int stripes, int stereo, int vector_size, bool vector_auto_return, int queue, int factory_id, int local_dsp, int global_dsp) {
	this -> sth = sth;
	this -> queue = new key_queue (queue);
	int ind = 0;
	if (stripes < 1) stripes = 1;
	if (stripes > 16) stripes = 16;
	hercules_number = stripes;
	this -> stripes = new stripe_parameter_block_pointer [stripes];
	for (ind = 0; ind < stripes; ind++)
		this -> stripes [ind] = new stripe_parameter_block ();
	if (stereo < 1) stereo = 1;
	if (stereo > 16) stereo = 16;
	hercules_stereo_number = stereo;
	this -> stereo = new stereo_stripe_parameter_block_pointer [stereo];
	for (ind = 0; ind < stereo; ind++)
		this -> stereo [ind] = new stereo_stripe_parameter_block ();
	this -> vector_size = vector_size;
	this -> vector_auto_return = vector_auto_return;
	vector_frames = new vector_frame [vector_size];
	algo_head = sth -> get_default_algo ();
	arp = NULL;
	vect = NULL;
	pattern_head = sth -> get_default_pattern ();
	controllers = new controllers_parameter_block ();
	controllers -> poly = true;		// to make sure that the first call to poly (); will return;
	controllers -> volume_reset = sth -> volume_reset;
	controllers -> volume = 12800;
//	controllers -> global_volume = 16383;
	system = new system_parameter_block ();
	system -> device_id = factory_id;
	system -> midi_channel = factory_id & 0xf; // 0x7f;
	system -> midi_channel_extension = 0x7f;
	system -> arranger_assign = 0x7f;
	this -> factory_id = factory_id;
	locked_oscillator = 0;
	last_key = -1;
	algo_notification_not_required = false;
	pattern_notification_not_required = false;
	hercules_local_dsp_number = local_dsp;
	hercules_global_dsp_number = global_dsp;
	hercules_all_dsp_number = local_dsp + global_dsp;
	dsp_send = new dsp_router [hercules_all_dsp_number];
//	dsp_send = new int [hercules_all_dsp_number];
//	dry_wet_dsp_send_sens = new sub_parameter_block [hercules_all_dsp_number];
//	vector_x_dsp_send_sens = new sub_parameter_block [hercules_all_dsp_number];
//	vector_y_dsp_send_sens = new sub_parameter_block [hercules_all_dsp_number];
	dsp = new dsp_parameter_block_pointer [hercules_all_dsp_number];
	for (ind = 0; ind < local_dsp; ind++) {
		dsp [ind] = new dsp_parameter_block (sth, ind, local_dsp);
	}
	for (ind = 0; ind < global_dsp; ind++) {
		dsp [ind + local_dsp] = (sth -> dsp == NULL ? new dsp_parameter_block (sth, ind, global_dsp) : sth -> dsp [ind]);
	}
	if (sth -> dsp != NULL) reset_global_dsp_parameters ();
	else global_dsp_voice_init ();
//	global_dry_out = 16383;
//	dry_wet_global_out_sens . voice_init (0); //dry_init ();
//	vector_x_global_out_sens . voice_init (0);
//	vector_y_global_out_sens . voice_init (0);
//	for (ind = hercules_local_dsp_number; ind < hercules_all_dsp_number; ind++) {
//		dsp_send [ind] = (ind == hercules_local_dsp_number ? 16383 : 0);
//		dry_wet_dsp_send_sens [ind] . voice_init (0);
//		vector_x_dsp_send_sens [ind] . voice_init (0);
//		vector_y_dsp_send_sens [ind] . voice_init (0);
//	}
}

parameter_block :: ~ parameter_block (void) {
	int ind;
	for (ind = 0; ind < hercules_number; ind++) delete stripes [ind];
	delete [] stripes;
	for (ind = 0; ind < hercules_stereo_number; ind++) delete stereo [ind];
	delete [] stereo;
	delete [] vector_frames;
	for (ind = 0; ind < hercules_local_dsp_number; ind++) delete dsp [ind];
	if (sth -> dsp == NULL)
		for (ind = 0; ind < hercules_global_dsp_number; ind++)
			delete dsp [ind + hercules_local_dsp_number];
	delete [] dsp;
//	delete algo_head;
	delete controllers;
	delete system;
	delete queue;
	delete [] dsp_send;
//	delete [] dry_wet_dsp_send_sens;
//	delete [] vector_x_dsp_send_sens;
//	delete [] vector_y_dsp_send_sens;
}

void parameter_block :: notify_retrigger_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_retrigger_change (this);
}

void parameter_block :: notify_ratio_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_ratio_change (this);
}

void parameter_block :: notify_freq_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_freq_shifts_update (this);
}

void parameter_block :: notify_amp_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_amp_shifts_update (this);
}

void parameter_block :: notify_index_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_index_shifts_update (this);
}

//@ index amp
//void parameter_block :: notify_index_amp_change (int ind) {
//	oscillator_pool * pool = sth -> pool;
//	if (pool == NULL) return;
//	pool -> notify_index_amp_change (this, ind);
//}

void parameter_block :: notify_rnd_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_rnd_update (this);
}

void parameter_block :: notify_pan_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_pan_shifts_update (this);
}

void parameter_block :: notify_all_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_all_shifts_update (this);
}

void parameter_block :: notify_freq_lfo_source_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_freq_lfo_source_update (this);
}

void parameter_block :: notify_amp_lfo_source_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_amp_lfo_source_update (this);
}

void parameter_block :: notify_index_lfo_source_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_index_lfo_source_update (this);
}

void parameter_block :: notify_pan_lfo_source_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_pan_lfo_source_update (this);
}

void parameter_block :: notify_all_lfo_source_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_all_lfo_source_update (this);
}

void parameter_block :: notify_lfo_freq_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_lfo_freq_shifts_update (this);
}

void parameter_block :: notify_lfo_amp_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_lfo_amp_shifts_update (this);
}

void parameter_block :: notify_lfo_index_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_lfo_index_shifts_update (this);
}

void parameter_block :: notify_lfo_pan_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_lfo_pan_shifts_update (this);
}

void parameter_block :: notify_lfo_all_shifts_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_lfo_all_shifts_update (this);
}

void parameter_block :: notify_lfo_speeds_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_lfo_speeds_update (this);
}

void parameter_block :: notify_lfo_retrigger_required (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_lfo_retrigger_required (this);
}

void parameter_block :: notify_lfo_wave_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_lfo_wave_change (this);
}

void parameter_block :: notify_lfo_pulse_width_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_lfo_pulse_width_change (this);
}

void parameter_block :: notify_sensitivity_change (int nrpn_msb, int xxx, int nnnn) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_sensitivity_change (this, nrpn_msb, xxx, nnnn);
}

void parameter_block :: notify_algo_change (void) {
	if (algo_notification_not_required) {
		algo_notification_not_required = false;
		return;
	}
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_algo_change (this);
}

void parameter_block :: notify_dsp_algo_change (int ind) {
	if (ind >= hercules_all_dsp_number) return;
	if (dsp [ind] -> algo_notification_not_required) {
		dsp [ind] -> algo_notification_not_required = false;
		return;
	}
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_dsp_algo_change (this, ind);
}

void parameter_block :: notify_pattern_change (void) {
	if (pattern_notification_not_required) {
		pattern_notification_not_required = false;
		return;
	}
	if (arp == NULL) return;
	int ind = pattern_head -> get_division ();
	if (ind > 0) controllers -> beats_per_bar = ind;
	ind = pattern_head -> get_subdivision ();
	if (ind > 0) controllers -> ticks_per_beat = ind;
	ind = pattern_head -> get_tempo ();
	if (ind >= 0) {controllers -> seconds = 1; controllers -> beats_per_seconds = ind;}
	arp -> notify_pattern_change ();
}

void parameter_block :: notify_attack_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_attack_change (this);
}

void parameter_block :: notify_decay_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_decay_change (this);
}

void parameter_block :: notify_sustain_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_sustain_change (this);
}

void parameter_block :: notify_release_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_release_change (this);
}

void parameter_block :: notify_adsr_level_update (int nrpn_msb, int xxx, int nnnn) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_adsr_level_update (this, nrpn_msb, xxx, nnnn);
}

void parameter_block :: notify_adsr_time_update (int nrpn_msb, int xxx, int nnnn) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_adsr_time_update (this, nrpn_msb, xxx, nnnn);
}

void parameter_block :: notify_porta_update (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_porta_update (this);
}

void parameter_block :: notify_tune (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_tune (this);
}

void parameter_block :: notify_reserved_notes_changed (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_reserved_notes_changed (sth);
}

void parameter_block :: notify_algo_parameter_changed (int ind) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_algo_parameter_changed (this, ind);
}

void parameter_block :: notify_volume_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_volume_change (this);
}

void parameter_block :: notify_global_volume_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_global_volume_change (this);
}

void parameter_block :: notify_dsp_change (int nrpn_lsb) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_dsp_change (this, nrpn_lsb);
}

void parameter_block :: notify_dry_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_dry_change (this);
}

void parameter_block :: notify_dry_wet_change (void) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_dry_wet_change_all (this);
}

void parameter_block :: notify_dry_wet_change (int ind) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_dry_wet_change (this, ind);
}

void parameter_block :: notify_tempo_change (void) {
	if (arp == NULL) return;
	arp -> notify_tempo (controllers -> beats_per_seconds, controllers -> seconds);
}

void parameter_block :: notify_division_change (void) {
	if (arp == NULL) return;
	arp -> notify_division (controllers -> beats_per_bar);
}

void parameter_block :: notify_sub_division_change (void) {
	if (arp == NULL) return;
	arp -> notify_sub_division (controllers -> ticks_per_beat);
}

void parameter_block :: notify_metrum_change (void) {
	if (arp == NULL) return;
	arp -> notify_division (controllers -> beats_per_bar, controllers -> ticks_per_beat);
}

void parameter_block :: notify_arpeggiator_switch (void) {
	if (arp == NULL) return;
	arp -> notify_switch (controllers -> arpeggiator_switch);
}

void parameter_block :: notify_arpeggiator_hold (void) {
	if (arp == NULL) return;
	arp -> notify_hold (controllers -> arpeggiator_hold);
}

void parameter_block :: notify_variation_change (void) {
	if (arp == NULL) return;
	arp -> notify_variation (controllers -> variation);
}

void parameter_block :: notify_arpeggiator_loaded (void) {
	if (arp == NULL) return;
	arp -> notify_tempo (controllers -> beats_per_seconds, controllers -> seconds);
	arp -> notify_division (controllers -> beats_per_bar, controllers -> ticks_per_beat);
	arp -> notify_switch (controllers -> arpeggiator_switch);
	arp -> notify_hold (controllers -> arpeggiator_hold);
	arp -> notify_variation (controllers -> variation);
	arp -> notify_key_off ();
}

void parameter_block :: notify_program_loaded (void) {
	relemat_all ();
	if (arp != NULL) arp -> notify_program_loaded ();
	oscillator_pool * pool = sth -> pool;
	if (pool != NULL) pool -> notify_program_loaded (this);
	loading_ground ();
}

void parameter_block :: notify_dsp_program_loaded (bool global) {
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	if (global) pool -> notify_multi_global_dsp_program_loaded (this);
	else pool -> notify_dsp_program_loaded (this);
}

void parameter_block :: program_change (void) {
	bank * bp = sth -> banks [system -> bank_msb];
	if (bp == NULL) return;
	bp -> program_change (system -> bank_lsb, system -> program, this);
}

void parameter_block :: bank_change (char * msb) {
	bank * bptr;
	bank * * banks = sth -> banks;
	for (int ind = 0; ind < 128; ind++) {
		bptr = banks [ind];
		if (bptr != NULL) {
			if (strcmp (bptr -> toString (), msb) == 0) {
				system -> bank_msb = ind;
				return;
			}
		}
	}
	STRING command;
	sprintf (command, "named bank not installed [%s]", msb);
	sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 0x7, 0x7f, 0x7f, command);
}

void parameter_block :: algo_change (char * msb, int lsb, int program) {
	bank * bptr;
	bank * * banks = sth -> banks;
	for (int ind = 0; ind < 128; ind++) {
		bptr = banks [ind];
		if (bptr != NULL) {
			if (strcmp (bptr -> toString (), msb) == 0) {
				algo_notification_not_required = true;
				bptr -> program_change (lsb, program, this);
				return;
			}
		}
	}
	STRING command;
	sprintf (command, "algo bank not installed [%s]", msb);
	sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 0x7, lsb, program, command);
}

void parameter_block :: eg_copy (int channel, int source, int destination) {
	int source_type = source >> 4; int source_index = source & 0xf;
	int destination_type = destination >> 4; int destination_index = destination & 0xf;
	adsr_parameter_block * source_eg = NULL;
	adsr_parameter_block * destination_eg = NULL;
	sensitivity_parameter_block * source_sens_eg = NULL;
	sensitivity_parameter_block * destination_sens_eg = NULL;
	switch (source_type) {
	case 0: if (source_index < hercules_number) {source_eg = & stripes [source_index] -> index_eg; source_sens_eg = & stripes [source_index] -> index_sens;} break;
	case 1: if (source_index < hercules_number) {source_eg = & stripes [source_index] -> freq_eg; source_sens_eg = & stripes [source_index] -> freq_sens;} break;
	case 2: if (source_index < hercules_number) {source_eg = & stripes [source_index] -> amp_eg; source_sens_eg = & stripes [source_index] -> amp_sens;} break;
	case 3: if (source_index < hercules_stereo_number) {source_eg = & stereo [source_index] -> eg; source_sens_eg = & stereo [source_index] -> sens;} break;
	default: break;
	}
	switch (destination_type) {
	case 0: if (destination_index < hercules_number) {destination_eg = & stripes [destination_index] -> index_eg; destination_sens_eg = & stripes [destination_index] -> index_sens;} break;
	case 1: if (destination_index < hercules_number) {destination_eg = & stripes [destination_index] -> freq_eg; destination_sens_eg = & stripes [destination_index] -> freq_sens;} break;
	case 2: if (destination_index < hercules_number) {destination_eg = & stripes [destination_index] -> amp_eg; destination_sens_eg = & stripes [destination_index] -> amp_sens;} break;
	case 3: if (destination_index < hercules_stereo_number) {destination_eg = & stereo [destination_index] -> eg; destination_sens_eg = & stereo [destination_index] -> sens;} break;
	default: break;
	}
	if (source_eg == NULL) {sth -> send_error (channel, 0x1, source, destination, "eg copy source out of range"); return;}
	if (destination_eg == NULL) {sth -> send_error (channel, 0x1, source, destination, "eg copy destination out of range"); return;}
	for (int ind = 0; ind < 4; ind++) {
		destination_eg -> level [ind] = source_eg -> level [ind];
		destination_eg -> time [ind] = source_eg -> time [ind];
	}
	if (source_sens_eg == NULL || destination_sens_eg == NULL) return;
	destination_sens_eg -> attack . bp = source_sens_eg -> attack . bp;
	destination_sens_eg -> attack . lc = source_sens_eg -> attack . lc;
	destination_sens_eg -> attack . rc = source_sens_eg -> attack . rc;
	destination_sens_eg -> decay . bp = source_sens_eg -> decay . bp;
	destination_sens_eg -> decay . lc = source_sens_eg -> decay . lc;
	destination_sens_eg -> decay . rc = source_sens_eg -> decay . rc;
	destination_sens_eg -> sustain . bp = source_sens_eg -> sustain . bp;
	destination_sens_eg -> sustain . lc = source_sens_eg -> sustain . lc;
	destination_sens_eg -> sustain . rc = source_sens_eg -> sustain . rc;
	destination_sens_eg -> release . bp = source_sens_eg -> release . bp;
	destination_sens_eg -> release . lc = source_sens_eg -> release . lc;
	destination_sens_eg -> release . rc = source_sens_eg -> release . rc;
	destination_sens_eg -> key_eg_scaling . bp = source_sens_eg -> key_eg_scaling . bp;
	destination_sens_eg -> key_eg_scaling . lc = source_sens_eg -> key_eg_scaling . lc;
	destination_sens_eg -> key_eg_scaling . rc = source_sens_eg -> key_eg_scaling . rc;
	destination_sens_eg -> velocity_eg_scaling . bp = source_sens_eg -> velocity_eg_scaling . bp;
	destination_sens_eg -> velocity_eg_scaling . lc = source_sens_eg -> velocity_eg_scaling . lc;
	destination_sens_eg -> velocity_eg_scaling . rc = source_sens_eg -> velocity_eg_scaling . rc;
}

void parameter_block :: pattern_change (char * msb, int lsb, int program) {
	bank * bptr;
	bank * * banks = sth -> banks;
	for (int ind = 0; ind < 128; ind++) {
		bptr = banks [ind];
		if (bptr != NULL) {
			if (strcmp (bptr -> toString (), msb) == 0) {
				pattern_notification_not_required = true;
				bptr -> program_change (lsb, program, this);
				return;
			}
		}
	}
	STRING command;
	sprintf (command, "pattern bank not installed [%s]", msb);
	sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 0x7, lsb, program, command);
}

void parameter_block :: dsp_algo_change (char * msb, int lsb, int program) {
	if (lsb >= hercules_all_dsp_number) return;
	bank * bptr;
	bank * * banks = sth -> banks;
	for (int ind = 0; ind < 128; ind++) {
		bptr = banks [ind];
		if (bptr != NULL) {
			if (strcmp (bptr -> toString (), msb) == 0) {
				dsp [lsb] -> algo_notification_not_required = true;
				bptr -> program_change (lsb, program, this);
				return;
			}
		}
	}
	STRING command;
	sprintf (command, "dsp bank not installed [%s]", msb);
	sth -> send_error (system -> midi_channel_extension, system -> midi_channel * 0xf, 0x7, lsb, program, command);
}

int key_queue :: key_on (int key) {
	if (top >= size) {
		for (top = 0; top < (size - 1); top++) {
			queue [top] = queue [top + 1];
		}
	}
	queue [top++] = key;
	return top;
}

int key_queue :: key_off (int key) {
	if (top < 1) return -1;					// queue empty [return -1]
	int ind = top - 1;
	while (ind >= 0 && queue [ind] != key) ind--;
	if (ind < 0) return queue [top - 1];	// not found [return last]
	top--;									// move queue
	while (ind < top) {
		queue [ind] = queue [ind + 1];
		ind++;
	}
	if (top < 1) return -1;					// queue empty [return -1]
	return queue [top - 1];					// queue not empty [return last]
}

void key_queue :: key_off (void) {
	top = 0;
}

key_queue :: key_queue (int size) {
	queue = new int [size];
	this -> size = size;
	top = 0;
}

key_queue :: ~ key_queue (void) {delete [] queue;}

void synthesizer :: constructor (int multitimbral, int stripes, int stereo, int vector_size, bool vector_auto_return, int queue, char * root_directory, bool volume_reset, bool global_dsp_multi, int local_dsp, int global_dsp, bool global_volume_multi, bool global_volume_present) {
	transport_seconds = 0;
	tune = 0;
	pool = NULL;
	arps = NULL;
	root = NULL;
	string_copy (this -> root_directory, root_directory);
	string_copy (setup_file_name, "setup.txt");
	string_copy (control_file_name, "control.txt");
	no_setup_store = true;
	no_control_store = true;
	midi_out = NULL;
	reset_categories ();
	reset_messages ();
	this -> multitimbral = multitimbral;
	this -> stripes = stripes;
	this -> stereo = stereo;
	this -> vector_size = vector_size;
	this -> vector_auto_return = vector_auto_return;
	this -> local_dsp = local_dsp;
	this -> global_dsp = global_dsp;
	this -> all_dsp = local_dsp + global_dsp;
	this -> volume_reset = volume_reset;
//	this -> global_volume = 16383;
	this -> global_volume_multi = global_volume_multi;
	this -> global_volume_present = global_volume_present;
	int ind;
	if (global_dsp_multi) dsp = NULL;
	else {
		dsp = new dsp_parameter_block_pointer [global_dsp];
		for (ind = 0; ind < global_dsp; ind++)
			dsp [ind] = new dsp_parameter_block (this, ind, global_dsp);
	}
	data_blocks = new parameter_block_pointer [multitimbral];
	for (ind = 0; ind < multitimbral; ind++) {
		data_blocks [ind] = new parameter_block (this, stripes, stereo, vector_size, vector_auto_return, queue, ind, local_dsp, global_dsp);
	}
	reset_banks ();
}

//synthesizer :: synthesizer (int multitimbral, int stripes, int stereo, int queue, char * root_directory, bool volume_reset) {
//	constructor (multitimbral, stripes, stereo, queue, root_directory, volume_reset);
//}

synthesizer :: synthesizer (config * cfg) {
	constructor (cfg -> multitimbral, cfg -> stripes, cfg -> stereo, cfg -> vector_size, cfg -> vector_auto_return, cfg->key_stream, cfg -> root_directory, cfg -> volume_reset, cfg -> global_dsp_multi, cfg -> local_dsp, cfg -> global_dsp, cfg -> global_volume_multi, cfg -> global_volume_present);
}

synthesizer :: ~ synthesizer (void) {
	int ind;
	for (ind = 0; ind < multitimbral; ind++) {
		delete data_blocks [ind];
	}
	delete [] data_blocks;
	if (dsp != NULL) {
		for (ind = 0; ind < global_dsp; ind++)
			delete dsp [ind];
		delete [] dsp;
	}
	destroy_banks ();
}

void synthesizer :: voice_init (void) {
	notify_tune ();
	global_volume = 16383;
//	global_volume = 16383;		moved to the constructor because of the setup file
	int ind;
	if (dsp != NULL) {
		for (ind = 0; ind < global_dsp; ind++)
			dsp [ind] -> voice_init ();
		notify_global_dsp_program_loaded ();
	}
	if (data_blocks == NULL) return;
	parameter_block * pb;
	for (ind = 0; ind < multitimbral; ind++) {
		pb = data_blocks [ind];
		pb -> voice_init ();
//		pb -> relemat_all ();
		pb -> notify_program_loaded ();
	}
}

void synthesizer :: notify_tune (void) {
	if (pool == NULL) return;
	pool -> notify_tune (tune);
}

void synthesizer :: notify_global_dsp_program_loaded (void) {
	if (pool == NULL) return;
	pool -> notify_global_dsp_program_loaded (this);
}

void synthesizer :: reset_categories (void) {category_pointer = 0;}

void synthesizer :: insert_category (char * name) {
	if (category_pointer >= 128) return;
	string_copy (categories [category_pointer++], name);
}

char * synthesizer :: get_category (int ind) {
	if (category_pointer == 0) return "--: <not installed>";
	if (ind >= category_pointer) ind = 0;
	return categories [ind];
}

void synthesizer :: reset_messages (void) {for (int ind = 0; ind < 96; ind++) string_copy (messages [ind], "");}

void synthesizer :: set_message (int ind, char * messaget) {
	ind -= 0x20;
	if (ind < 0) return;
	if (ind > 95) return;
	string_copy (messages [ind], messaget);
}

char * synthesizer :: get_message (int ind) {
	ind -= 0x20;
	if (ind < 0 || ind > 95) return "";
	return messages [ind];
}

void synthesizer :: reset_banks (void) {for (int ind = 0; ind < 128; ind++) banks [ind] = NULL;}
void synthesizer :: destroy_banks (void) {
	for (int ind = 0; ind < 128; ind++) {
		if (banks [ind] != NULL) {
			delete banks [ind];
			banks [ind] = NULL;
		}
	}
}

algo_pointer synthesizer :: get_default_algo (void) {
	if (pool == NULL) return get_silence_algo ();
	return pool -> get_default_algo ();			// by default the pool will return silence
}

algo_pointer synthesizer :: get_silence_algo (void) {return & silence;}

pattern_pointer synthesizer :: get_default_pattern (void) {
	if (arps == NULL) return get_silence_pattern ();
	return arps -> get_default_pattern ();
}

pattern_pointer synthesizer :: get_silence_pattern (void) {return & silence_pattern;}

dsp_algo_pointer synthesizer :: get_default_dsp_algo (void) {
	if (pool == NULL) return get_silence_dsp_algo ();
	return pool -> get_default_dsp_algo ();
}

dsp_algo_pointer synthesizer :: get_silence_dsp_algo (void) {return & silence_dsp_algo;}

void synthesizer :: connect_midi_out (midi_stream * line) {midi_out = line;}
void synthesizer :: disconnect_midi_out (void) {midi_out = NULL;}
void synthesizer :: connect_oscillator_pool (oscillator_pool * pool) {this -> pool = pool;}
void synthesizer :: disconnect_oscillator_pool (void) {pool = NULL;}
void synthesizer :: connect_arpeggiator_pool (arpeggiator_pool * arps) {
	for (int ind = 0; ind < multitimbral; ind++) data_blocks [ind] -> arp = arps -> get_arpeggiator (ind);
	this -> arps = arps;
}
void synthesizer :: disconnect_arpeggiator_pool (void) {
	arps = NULL;
	for (int ind = 0; ind < multitimbral; ind++) data_blocks [ind] -> arp = NULL;
}

void synthesizer :: connect_vector_pool (vector_pool * vects) {
	for (int ind = 0; ind < multitimbral; ind++) data_blocks [ind] -> vect = vects -> get_vector (ind);
	this -> vects = vects;
}
void synthesizer :: disconnect_vector_pool (void) {
	vects = NULL;
	for (int ind = 0; ind < multitimbral; ind++) data_blocks [ind] -> vect = NULL;
}
