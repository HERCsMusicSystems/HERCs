///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "mnemonic.h"

void emit (midi_stream * line, int data) {
	int lsb = data & 0x7f;
	int msb = data >> 7;
	msb &= 0x7f;
	line -> insert (lsb);
	line -> insert (msb);
}

void emit (midi_stream * line, int id, int data) {
	int lsb = data & 0x7f;
	int msb = data >> 7;
	msb &= 0x7f;
	line -> insert (id);
	line -> insert (lsb);
	line -> insert (msb);
}

void emit (midi_stream * line, char * text) {
	line -> insert (text);
	line -> insert (EARTH);
}

void adsr_parameter_block :: drop (midi_stream * line, int selector, int id) {
	int mod = 0;
	int ind;
	for (ind = 0; ind < 3; ind++) {
		if (level [ind] != 0) mod++;
		if (time [ind] != 0) mod++;
	}
	if (level [ind] != selector) mod++;
	if (time [ind] != 0) mod++;
	if (mod == 0) return;
	if (mod > 3) {
		line -> insert (id);
		for (ind = 0; ind < 4; ind++) emit (line, level [ind]);
		for (ind = 0; ind < 4; ind++) emit (line, time [ind]);
	} else {
		id++;
		for (ind = 0; ind < 3; ind++) drop_controller (line, ind, level [ind], 0, id);
		drop_controller (line, 3, level [3], selector, id);
		id++;
		for (ind = 0; ind < 4; ind++) drop_controller (line, ind, time [ind], 0, id);
	}
}

void sub_parameter_block :: drop (midi_stream * line, int selector, int id) {
	if (lc == - selector && bp == 8192 && rc == selector) return;
	line -> insert (id);
	emit (line, lc);
	emit (line, bp);
	emit (line, rc);
}

void sub_parameter_block :: drop (midi_stream * line, int bp_selector, int selector, int id) {
	if (lc == - selector && bp == bp_selector && rc == selector) return;
	line -> insert (id);
	emit (line, lc);
	emit (line, bp);
	emit (line, rc);
}

void sub_parameter_block :: drop_indexed (midi_stream * line, int index, int id) {
	if (lc == 0 && bp == 8192 && rc == 0) return;
	line -> insert (id);
	line -> insert (index);
	emit (line, lc);
	emit (line, bp);
	emit (line, rc);
}

void sub_parameter_block :: drop_dry (midi_stream * line, int id) {
	if (lc == 0 && bp == 8192 && rc == -256) return;
	line -> insert (id);
	emit (line, lc);
	emit (line, bp);
	emit (line, rc);
}

void sub_parameter_block :: drop_wet (midi_stream * line, int id) {
	if (lc == -256 && bp == 8192 && rc == 0) return;
	line -> insert (id);
	emit (line, lc);
	emit (line, bp);
	emit (line, rc);
}

void drop_controller (midi_stream * line, int ind, int sub, int id) {
	if (ind == sub) return;
	emit (line, id, ind);
}

void drop_short_controller (midi_stream * line, int ind, int sub, int id) {
	if (ind == sub) return;
	line -> insert (id);
	line -> insert (ind);
}

void drop_controller (midi_stream * line, int index, int ind, int sub, int id) {
	if (ind == sub) return;
	line -> insert (id);
	line -> insert (index);
	emit (line, ind);
}

void drop_controller (midi_stream * line, bool ind, bool sub, int id) {
	if (ind == sub) return;
	line -> insert (id);
}

void parameter_block :: send_algo_bulk_dump (int channel) {
	midi_stream * line = sth -> midi_out;
	if (line == NULL) return;
	line -> open_system_exclusive ();
	line -> insert (0x50 | channel);
	line -> insert (0x50);
	emit (line, algo_msb);
	line -> insert (algo_lsb);
	line -> insert (algo_program);
	int parameter;
	for (int ind = 0; ind < 128; ind++) {
		parameter = controllers -> algo_parameters [ind];
		if (parameter != algo_head -> get_default (ind)) {
			line -> insert (ind);
			emit (line, parameter);
		}
	}
//	line -> insert_checksum ();
	line -> close_system_exclusive ();
}

void parameter_block :: send_pattern_bulk_dump (int channel) {
	midi_stream * line = sth -> midi_out;
	if (line == NULL) return;
	line -> open_system_exclusive ();
	line -> insert (0x50 | channel);
	line -> insert (0x51);
	emit (line, pattern_msb);
	line -> insert (pattern_lsb);
	line -> insert (pattern_program);
//	line -> insert_checksum ();
	line -> close_system_exclusive ();
}

void parameter_block :: send_dsp_algo_bulk_dump (int channel) {
	for (int ind = 0; ind < hercules_all_dsp_number; ind++)
		send_dsp_algo_bulk_dump (channel, ind);
}

void parameter_block :: send_dsp_algo_bulk_dump (int channel, int ind) {
	midi_stream * line = sth -> midi_out;
	if (line == NULL || ind < 0 || ind >= hercules_all_dsp_number) return;
	line -> open_system_exclusive ();
	line -> insert (0x50 | channel);
	line -> insert (0x58 | ind);
	emit (line, dsp [ind] -> algo_msb);
	line -> insert (dsp [ind] -> algo_program);
//	line -> insert_checksum ();
	line -> close_system_exclusive ();
}

void parameter_block :: send_vector_bulk_dump (int channel) {
	midi_stream * line = sth -> midi_out;
	if (line == NULL) return;
	line -> open_system_exclusive ();
	line -> insert (0x50 | channel);
	line -> insert (0x11);
	int sentinel = controllers -> vector_frame_count;
	for (int ind = 0; ind < sentinel; ind++) {
		emit (line, vector_frames [ind] . delay);
		emit (line, vector_frames [ind] . x);
		emit (line, vector_frames [ind] . y);
	}
	line -> close_system_exclusive ();
}

void parameter_block :: store_program (midi_stream * line) {
	int ind, sub, id;
	line -> insert (PROGRAM);
	line -> insert (CATEGORY); line -> insert (category);
	line -> insert (NAME); emit (line, name);

	line -> insert (ALGO);
	emit (line, algo_msb);
	line -> insert (algo_lsb);
	line -> insert (algo_program);
	for (ind = 0; ind < 128; ind++) {
		drop_controller (line, ind, controllers -> algo_parameters [ind], algo_head -> get_default (ind), ALGO_PARAMETER);
	}

	if (arp != NULL) {
		line -> insert (PATTERN);
		emit (line, pattern_msb);
		line -> insert (pattern_lsb);
		line -> insert (pattern_program);
	}

	drop_controller (line, controllers -> pitch, 8192, PITCH);
	drop_controller (line, controllers -> freq, 8192, FREQ);
	drop_controller (line, controllers -> amp, 8192, AMP);
	drop_controller (line, controllers -> index, 8192, INDEX);
	drop_controller (line, controllers -> attack, 8192, ATTACK);
	drop_controller (line, controllers -> decay, 8192, DECAY);
	drop_controller (line, controllers -> sustain, 8192, SUSTAIN);
	drop_controller (line, controllers -> release, 8192, RELEASE);
	drop_controller (line, controllers -> speed, 8192, SPEED);
	drop_controller (line, controllers -> vibrato, 0, VIBRATO);
	drop_controller (line, controllers -> tremolo, 0, TREMOLO);
	drop_controller (line, controllers -> lfo_index, 0, LFO_INDEX);
	drop_controller (line, controllers -> lfo_pan, 0, LFO_PAN);
	drop_controller (line, controllers -> vector_x, 8192, VECTOR_X);
	drop_controller (line, controllers -> vector_y, 8192, VECTOR_Y);
	drop_controller (line, controllers -> volume, 12800, VOLUME);
	drop_controller (line, controllers -> pan, 8192, PAN);
	drop_controller (line, controllers -> hold, false, HOLD_ON);

	for (ind = 0; ind < 128; ind++) drop_controller (line, ind, controllers -> key_tuner [ind], ind << 7, TUNE);

	bool sync_all = true;
	for (ind = 0; ind < hercules_number; ind++) if (! stripes [ind] -> retrigger) sync_all = false;
	drop_controller (line, sync_all, false, SYNC_ALL);
	stripe_parameter_block * spb;
	for (ind = 0; ind < hercules_number; ind++) {
		line -> insert (STRIPE);
		line -> insert (ind);
//		tc -> writeln_id ("stripe");
//		tc -> writeln_id ("stripe", ind);
		spb = stripes [ind];
		if (! sync_all) drop_controller (line, spb -> retrigger, false, SYNC);
		drop_short_controller (line, spb -> multiplier, 1, FREQ_RATIO);
		drop_controller (line, spb -> freq, 0, FREQ);
		drop_controller (line, spb -> amp, ind == 0 ? 0 : -16383, AMP);
		drop_controller (line, spb -> index, 0, INDEX);
		drop_controller (line, spb -> freq_rnd, 0, FREQ_RND);
		drop_controller (line, spb -> amp_rnd, 0, AMP_RND);
		drop_controller (line, spb -> index_rnd, 0, INDEX_RND);
		//@ index amp
//		drop_controller (line, spb -> index_amp, false, INDEX_AMP);
		drop_controller (line, spb -> lfo_freq, 0, LFO_FREQ);
		drop_controller (line, spb -> lfo_amp, 0, LFO_AMP);
		drop_controller (line, spb -> lfo_index, 0, LFO_INDEX);
		drop_controller (line, spb -> speed, 8192, LFO_SPEED);
		drop_controller (line, spb -> lfo_retrigger, false, LFO_SYNC);
		drop_short_controller (line, spb -> lfo_sync, ind, LFO_SYNC_TO);
		drop_short_controller (line, spb -> wave, 0, LFO_WAVE);
		drop_controller (line, spb -> pulse_width, 0, LFO_PULSE_WIDTH);
		drop_controller (line, spb -> phase_shift, 0, LFO_PHASE_SHIFT);
		//-------------
		spb -> freq_eg . drop (line, 0, FREQ_EG);
		spb -> amp_eg . drop (line, -16383, AMP_EG);
		spb -> index_eg . drop (line, 0, INDEX_EG);
		//-------------
		spb -> freq_sens . pitch . drop (line, 256, FREQ_PITCH_SENS);
		spb -> freq_sens . shift . drop (line, 0, FREQ_SENS);
		spb -> freq_sens . lfo . drop (line, 0, 0, FREQ_LFO_SENS);
		spb -> freq_sens . attack . drop (line, 0, FREQ_ATTACK_SENS);
		spb -> freq_sens . decay . drop (line, 0, FREQ_DECAY_SENS);
		spb -> freq_sens . sustain . drop (line, 0, FREQ_SUSTAIN_SENS);
		spb -> freq_sens . release . drop (line, 0, FREQ_RELEASE_SENS);
		spb -> freq_sens . key . drop (line, 128, FREQ_KEY_SENS);
		spb -> freq_sens . velocity . drop (line, 0, FREQ_VELOCITY_SENS);
		spb -> freq_sens . vector_x . drop (line, 0, FREQ_VECTOR_X_SENS);
		spb -> freq_sens . vector_y . drop (line, 0, FREQ_VECTOR_Y_SENS);
		spb -> freq_sens . lfo_x . drop (line, 0, FREQ_LFO_X_SENS);
		spb -> freq_sens . lfo_y . drop (line, 0, FREQ_LFO_Y_SENS);
		spb -> freq_sens . key_attack_eg_scaling . drop (line, 0, FREQ_KEY_ATTACK_EG_SCALING);
		spb -> freq_sens . key_eg_scaling . drop (line, 0, FREQ_KEY_EG_SCALING);
		spb -> freq_sens . velocity_attack_eg_scaling . drop (line, 0, FREQ_VELOCITY_ATTACK_EG_SCALING);
		spb -> freq_sens . velocity_eg_scaling . drop (line, 0, FREQ_VELOCITY_EG_SCALING);
		//-------------
		spb -> amp_sens . pitch . drop (line, 0, AMP_PITCH_SENS);
		spb -> amp_sens . shift . drop (line, 0, AMP_SENS);
		spb -> amp_sens . lfo . drop (line, 0, 0, AMP_LFO_SENS);
		spb -> amp_sens . attack . drop (line, 0, AMP_ATTACK_SENS);
		spb -> amp_sens . decay . drop (line, 0, AMP_DECAY_SENS);
		spb -> amp_sens . sustain . drop (line, 0, AMP_SUSTAIN_SENS);
		spb -> amp_sens . release . drop (line, 0, AMP_RELEASE_SENS);
		spb -> amp_sens . key . drop (line, 0, AMP_KEY_SENS);
		spb -> amp_sens . velocity . drop (line, 0, AMP_VELOCITY_SENS);
		spb -> amp_sens . vector_x . drop (line, 0, AMP_VECTOR_X_SENS);
		spb -> amp_sens . vector_y . drop (line, 0, AMP_VECTOR_Y_SENS);
		spb -> amp_sens . lfo_x . drop (line, 0, AMP_LFO_X_SENS);
		spb -> amp_sens . lfo_y . drop (line, 0, AMP_LFO_Y_SENS);
		spb -> amp_sens . key_attack_eg_scaling . drop (line, 0, AMP_KEY_ATTACK_EG_SCALING);
		spb -> amp_sens . key_eg_scaling . drop (line, 0, AMP_KEY_EG_SCALING);
		spb -> amp_sens . velocity_attack_eg_scaling . drop (line, 0, AMP_VELOCITY_ATTACK_EG_SCALING);
		spb -> amp_sens . velocity_eg_scaling . drop (line, 0, AMP_VELOCITY_EG_SCALING);
		//-------------
		spb -> index_sens . pitch . drop (line, 0, INDEX_PITCH_SENS);
		spb -> index_sens . shift . drop (line, 0, INDEX_SENS);
		spb -> index_sens . lfo . drop (line, 0, 0, INDEX_LFO_SENS);
		spb -> index_sens . attack . drop (line, 0, INDEX_ATTACK_SENS);
		spb -> index_sens . decay . drop (line, 0, INDEX_DECAY_SENS);
		spb -> index_sens . sustain . drop (line, 0, INDEX_SUSTAIN_SENS);
		spb -> index_sens . release . drop (line, 0, INDEX_RELEASE_SENS);
		spb -> index_sens . key . drop (line, 0, INDEX_KEY_SENS);
		spb -> index_sens . velocity . drop (line, 0, INDEX_VELOCITY_SENS);
		spb -> index_sens . vector_x . drop (line, 0, INDEX_VECTOR_X_SENS);
		spb -> index_sens . vector_y . drop (line, 0, INDEX_VECTOR_Y_SENS);
		spb -> index_sens . lfo_x . drop (line, 0, INDEX_LFO_X_SENS);
		spb -> index_sens . lfo_y . drop (line, 0, INDEX_LFO_Y_SENS);
		spb -> index_sens . key_attack_eg_scaling . drop (line, 0, INDEX_KEY_ATTACK_EG_SCALING);
		spb -> index_sens . key_eg_scaling . drop (line, 0, INDEX_KEY_EG_SCALING);
		spb -> index_sens . velocity_attack_eg_scaling . drop (line, 0, INDEX_VELOCITY_ATTACK_EG_SCALING);
		spb -> index_sens . velocity_eg_scaling . drop (line, 0, INDEX_VELOCITY_EG_SCALING);
		//-------------
		spb -> lfo_sens . pitch . drop (line, 0, LFO_PITCH_SENS);
		spb -> lfo_sens . speed . drop (line, 0, LFO_SPEED_SENS);
		spb -> lfo_sens . vector_x . drop (line, 0, LFO_VECTOR_X_SENS);
		spb -> lfo_sens . vector_y . drop (line, 0, LFO_VECTOR_Y_SENS);
		//-------------
		line -> insert (EARTH);
	}
	stereo_stripe_parameter_block * stpb;
	for (ind = 0; ind < hercules_stereo_number; ind++) {
		line -> insert (STEREO);
		line -> insert (ind);
//		tc -> writeln_id ("stereo");
//		tc -> writeln_id ("stripe", ind);
		stpb = stereo [ind];
		drop_controller (line, stpb -> pan, 0, PAN);
		drop_controller (line, stpb -> rnd, 0, RND);
		drop_controller (line, stpb -> lfo, 0, LFO);
		drop_controller (line, stpb -> speed, 8192, LFO_SPEED);
		drop_controller (line, stpb -> lfo_retrigger, false, LFO_SYNC);
		drop_short_controller (line, stpb -> lfo_sync, ind + 16, LFO_SYNC_TO);
		drop_short_controller (line, stpb -> wave, 0, LFO_WAVE);
		drop_controller (line, stpb -> pulse_width, 0, LFO_PULSE_WIDTH);
		drop_controller (line, stpb -> phase_shift, 0, LFO_PHASE_SHIFT);
		//-------------
		stpb -> eg . drop (line, 0, EG);
		//-------------
		stpb -> sens . pitch . drop (line, 0, PITCH_SENS);
		stpb -> sens . shift . drop (line, 128, SENS);
		stpb -> sens . lfo . drop (line, 0, 0, LFO_SENS);
		stpb -> sens . attack . drop (line, 0, ATTACK_SENS);
		stpb -> sens . decay . drop (line, 0, DECAY_SENS);
		stpb -> sens . sustain . drop (line, 0, SUSTAIN_SENS);
		stpb -> sens . release . drop (line, 0, RELEASE_SENS);
		stpb -> sens . key . drop (line, 0, KEY_SENS);
		stpb -> sens . velocity . drop (line, 0, VELOCITY_SENS);
		stpb -> sens . vector_x . drop (line, 0, VECTOR_X_SENS);
		stpb -> sens . vector_y . drop (line, 0, VECTOR_Y_SENS);
		stpb -> sens . lfo_x . drop (line, 0, LFO_X_SENS);
		stpb -> sens . lfo_y . drop (line, 0, LFO_Y_SENS);
		stpb -> sens . key_attack_eg_scaling . drop (line, 0, KEY_ATTACK_EG_SCALING);
		stpb -> sens . key_eg_scaling . drop (line, 0, KEY_EG_SCALING);
		stpb -> sens . velocity_attack_eg_scaling . drop (line, 0, VELOCITY_ATTACK_EG_SCALING);
		stpb -> sens . velocity_eg_scaling . drop (line, 0, VELOCITY_EG_SCALING);
		//-------------
		stpb -> lfo_sens . pitch . drop (line, 0, LFO_PITCH_SENS);
		stpb -> lfo_sens . speed . drop (line, 0, LFO_SPEED_SENS);
		stpb -> lfo_sens . vector_x . drop (line, 0, LFO_VECTOR_X_SENS);
		stpb -> lfo_sens . vector_y . drop (line, 0, LFO_VECTOR_Y_SENS);
		//-------------
		line -> insert (EARTH);
	}

	drop_controller (line, controllers -> poly, true, MONO);
	drop_controller (line, controllers -> porta, false, PORTA_ON);
	drop_controller (line, controllers -> porta_time, 8192, PORTA_TIME);
	drop_controller (line, controllers -> porta_mode, 0, PORTA_MODE);
//	drop_controller (line, controllers -> delay_switch, true, DELAY_OFF);
//	drop_controller (line, controllers -> delay_cross_switch, true, DELAY_CROSS_OFF);
//	if (controllers -> delay_feedback != 4096 || controllers -> delay_time != 8192 || controllers -> delay_dry_wet != 0) {
//		line -> insert (DELAY);
//		emit (line, controllers -> delay_feedback);
//		emit (line, controllers -> delay_time);
//		emit (line, controllers -> delay_dry_wet);
//	}
	drop_controller (line, controllers -> arpeggiator_switch, false, ARPEGGIATOR_ON);
	drop_controller (line, controllers -> arpeggiator_hold, false, ARPEGGIATOR_HOLD);
	if (controllers -> beats_per_seconds != 140 || controllers -> seconds != 60) {
		line -> insert (TEMPO);
		emit (line, controllers -> beats_per_seconds);
		emit (line, controllers -> seconds);
	}
	if (controllers -> beats_per_bar != 4 || controllers -> ticks_per_beat != 24) {
		line -> insert (DIVISION);
		emit (line, controllers -> beats_per_bar);
		emit (line, controllers -> ticks_per_beat);
	}
	drop_controller (line, controllers -> variation, 0, VARIATION);
	// dsp section
	dsp_parameter_block_pointer dspp;
	dsp_router_pointer dsprp;
	drop_controller (line, controllers -> dry_wet, 8192, DRY_WET);
	drop_controller (line, local_dry_out . send_level, 16383, DIRECT_OUTPUT);
	local_dry_out . dry_wet . drop_dry (line, DIRECT_OUTPUT_DRY_WET_SENS);
	local_dry_out . vector_x . drop (line, 0, DIRECT_OUTPUT_VECTOR_X_SENS);
	local_dry_out . vector_y . drop (line, 0, DIRECT_OUTPUT_VECTOR_Y_SENS);
	for (ind = 0; ind < hercules_local_dsp_number; ind++) {
		dspp = dsp [ind];
		line -> insert (DSP);
		line -> insert (ind);
		line -> insert (ALGO);
		emit (line, dspp -> algo_msb);
		line -> insert (dspp -> algo_program);
		if (ind != 0) drop_controller (line, dspp -> on, false, DSP_ON);
		else drop_controller (line, dspp -> on, true, DSP_OFF);
		drop_controller (line, dsp_send [ind] . send_level, ind == 0 ? 16383 : 0, DIRECT_INPUT);
		dsp_send [ind] . dry_wet . drop (line, 0, DIRECT_INPUT_DRY_WET_SENS);
		dsp_send [ind] . vector_x . drop (line, 0, DIRECT_INPUT_VECTOR_X_SENS);
		dsp_send [ind] . vector_y . drop (line, 0, DIRECT_INPUT_VECTOR_Y_SENS);
		drop_controller (line, dspp -> wet_output . send_level, 16383, DIRECT_OUTPUT);
		dspp -> wet_output . dry_wet . drop_wet (line, DIRECT_OUTPUT_DRY_WET_SENS);
		dspp -> wet_output . vector_x . drop (line, 0, DIRECT_OUTPUT_VECTOR_X_SENS);
		dspp -> wet_output . vector_y . drop (line, 0, DIRECT_OUTPUT_VECTOR_Y_SENS);
		for (sub = 0; sub < dspp -> router_number; sub++) {
			dsprp = dspp -> routers [sub];
			id = ind + sub + 1;
			drop_controller (line, id, dsprp -> send_level, 0, SEND_TO);
			dsprp -> dry_wet . drop_indexed (line, id, SEND_TO_DRY_WET_SENS);
			dsprp -> vector_x . drop_indexed (line, id, SEND_TO_VECTOR_X_SENS);
			dsprp -> vector_y . drop_indexed (line, id, SEND_TO_VECTOR_Y_SENS);
		}
		for (sub = 0; sub < 16; sub++)
			drop_controller (line, sub, dspp -> parameters [sub], dspp -> algo_head -> get_default (sub), PARAMETER);
		line -> insert (EARTH);
	}
	line -> insert (EARTH);
}

void parameter_block :: store_dsp_program (midi_stream * line, bool global) {
	int ind, sub, id;
	dsp_parameter_block_pointer dspp;
	dsp_router_pointer dsprp;
	if (global) {
		drop_controller (line, global_dry_out . send_level, 16383, DIRECT_OUTPUT);
		global_dry_out . dry_wet . drop_dry (line, DIRECT_OUTPUT_DRY_WET_SENS);
		global_dry_out . vector_x . drop (line, 0, DIRECT_OUTPUT_VECTOR_X_SENS);
		global_dry_out . vector_y . drop (line, 0, DIRECT_OUTPUT_VECTOR_Y_SENS);
	} else {
		drop_controller (line, local_dry_out . send_level, 16383, DIRECT_OUTPUT);
		local_dry_out . dry_wet . drop_dry (line, DIRECT_OUTPUT_DRY_WET_SENS);
		local_dry_out . vector_x . drop (line, 0, DIRECT_OUTPUT_VECTOR_X_SENS);
		local_dry_out . vector_y . drop (line, 0, DIRECT_OUTPUT_VECTOR_Y_SENS);
	}
	int sentinel = global ? hercules_global_dsp_number : hercules_local_dsp_number;
	int index;
	for (ind = 0; ind < sentinel; ind++) {
		index = global ? hercules_local_dsp_number + ind : ind;
		dspp = dsp [index];
		line -> insert (DSP);
		line -> insert (ind);
		line -> insert (ALGO);
		emit (line, dspp -> algo_msb);
		line -> insert (dspp -> algo_program);
		if (ind != 0) drop_controller (line, dspp -> on, false, DSP_ON);
		else drop_controller (line, dspp -> on, true, DSP_OFF);
		drop_controller (line, dsp_send [index] . send_level, ind == 0 ? 16383 : 0, DIRECT_INPUT);
		dsp_send [index] . dry_wet . drop (line, 0, DIRECT_INPUT_DRY_WET_SENS);
		dsp_send [index] . vector_x . drop (line, 0, DIRECT_INPUT_VECTOR_X_SENS);
		dsp_send [index] . vector_y . drop (line, 0, DIRECT_INPUT_VECTOR_Y_SENS);
		drop_controller (line, dspp -> wet_output . send_level, 16383, DIRECT_OUTPUT);
		dspp -> wet_output . dry_wet . drop_wet (line, DIRECT_OUTPUT_DRY_WET_SENS);
		dspp -> wet_output . vector_x . drop (line, 0, DIRECT_OUTPUT_VECTOR_X_SENS);
		dspp -> wet_output . vector_y . drop (line, 0, DIRECT_OUTPUT_VECTOR_Y_SENS);
		for (sub = 0; sub < dspp -> router_number; sub++) {
			dsprp = dspp -> routers [sub];
			id = ind + sub + 1;
			drop_controller (line, id, dsprp -> send_level, 0, SEND_TO);
			dsprp -> dry_wet . drop_indexed (line, id, SEND_TO_DRY_WET_SENS);
			dsprp -> vector_x . drop_indexed (line, id, SEND_TO_VECTOR_X_SENS);
			dsprp -> vector_y . drop_indexed (line, id, SEND_TO_VECTOR_Y_SENS);
		}
		for (sub = 0; sub < 16; sub++)
			drop_controller (line, sub, dspp -> parameters [sub], dspp -> algo_head -> get_default (sub), PARAMETER);
		line -> insert (EARTH);
	}
	line -> insert (EARTH);
}

void parameter_block :: read_algo_bulk_dump (midi_stream * line) {
	STRING msb;
	line -> get (msb);
	int lsb = line -> get ();
	int program = line -> get ();
	if (lsb > 0x7f || program > 0x7f) return;
	algo_change (msb, lsb, program);
	lsb = line -> get ();
	while (lsb <= 0x7f) {
		program = line -> get_int ();
//		algo_control (lsb, program);
		controllers -> algo_parameters [lsb] = program;
		lsb = line -> get ();
	}
	notify_algo_change ();
}

void parameter_block :: read_pattern_bulk_dump (midi_stream * line) {
	STRING msb;
	line -> get (msb);
	int lsb = line -> get ();
	int program = line -> get ();
	if (lsb > 0x7f || program > 0x7f) return;
	pattern_change (msb, lsb, program);
	notify_pattern_change ();
}

void parameter_block :: read_dsp_algo_bulk_dump (midi_stream * line, int ind) {
	STRING msb;
	line -> get (msb);
	int program = line -> get ();
	if (ind > 0x7 || program > 0x7f) return;
	dsp_algo_change (msb, ind, program);
	notify_dsp_algo_change (ind);
}

void parameter_block :: read_vector_bulk_dump (midi_stream * line) {
	int delay = line -> get ();
	int ind = 0;
	while (delay < 0x7f) {
		delay += line -> get () << 7;
		vector_frames [ind] . delay = delay;
		vector_frames [ind] . x = line -> get_int ();
		vector_frames [ind] . y = line -> get_int ();
		ind++;
	}
	controllers -> vector_frame_selector = 0;
	controllers -> vector_frame_count = ind;
}

void parameter_block :: read_program (midi_stream * line) {
//	bool algo_control_needed = false;
	int command = line -> get ();
	STRING msb;
	int lsb;
	int program;
	stripe_parameter_block * sp;
	stereo_stripe_parameter_block * stp;
	dsp_parameter_block * dspp;
	while (command != EARTH) {
		switch (command) {
		case CATEGORY: category = line -> get (); break;
		case NAME: line -> get (name); break;
		case ALGO:
//			if (algo_control_needed) {
//				lsb = line -> get ();
//				program = line -> get ();
//				algo_control (lsb, program);
//				break;
//			}
			line -> get (msb);
			lsb = line -> get ();
			program = line -> get ();
			algo_change (msb, lsb, program);
//			algo_control_needed = true;
			break;
		case ALGO_PARAMETER:
			lsb = line -> get ();
			controllers -> algo_parameters [lsb] = line -> get_int ();
			break;
		case PATTERN:
			line -> get (msb);
			lsb = line -> get ();
			program = line -> get ();
			pattern_change (msb, lsb, program);
			break;
		case PITCH: controllers -> pitch = line -> get_int (); break;
		case FREQ: controllers -> freq = line -> get_int (); break;
		case AMP: controllers -> amp = line -> get_int (); break;
		case INDEX: controllers -> index = line -> get_int (); break;
		case ATTACK: controllers -> attack = line -> get_int (); break;
		case DECAY: controllers -> decay = line -> get_int (); break;
		case SUSTAIN: controllers -> sustain = line -> get_int (); break;
		case RELEASE: controllers -> release = line -> get_int (); break;
		case SPEED: controllers -> speed = line -> get_int (); break;
		case VIBRATO: controllers -> vibrato = line -> get_int (); break;
		case TREMOLO: controllers -> tremolo = line -> get_int (); break;
		case LFO_INDEX: controllers -> lfo_index = line -> get_int (); break;
		case LFO_PAN: controllers -> lfo_pan = line -> get_int (); break;
		case VECTOR_X: controllers -> vector_x = line -> get_int (); break;
		case VECTOR_Y: controllers -> vector_y = line -> get_int (); break;
		case VOLUME:
			if (controllers -> volume_reset) controllers -> volume = line -> get_int ();
			else line -> get_int ();
			break;
		case PAN: controllers -> pan = line -> get_int (); break;
		case HOLD_ON: controllers -> hold = true; break;
		case PORTA_ON: controllers -> porta = true; break;
		case POLY: poly (false); break; //controllers -> poly = true; break;
		case MONO: mono (false); break; //controllers -> poly = false; break;
		case PORTA_TIME: controllers -> porta_time = line -> get_int (); break;
		case PORTA_MODE: controllers -> porta_mode = line -> get (); break;
		case DRY_WET: controllers -> dry_wet = line -> get_int (); break;
		case TUNE: lsb = line -> get (); controllers -> key_tuner [lsb] = line -> get_int (); break;
		case SYNC_ALL:
			for (int stripe = 0; stripe < hercules_number; stripe++)
				stripes [stripe] -> retrigger = true;
			break;
		case ARPEGGIATOR_ON: controllers -> arpeggiator_switch = true; break;
		case ARPEGGIATOR_HOLD: controllers -> arpeggiator_hold = true; break;
		case TEMPO:
			controllers -> beats_per_seconds = line -> get_int ();
			controllers -> seconds = line -> get_int ();
			break;
		case DIVISION:
			controllers -> beats_per_bar = line -> get_int ();
			controllers -> ticks_per_beat = line -> get_int ();
			break;
		case VARIATION: controllers -> variation = line -> get_int ();
		case STRIPE:
			command = line -> get ();
			if (command < 0 || command >= hercules_number) return;
			sp = stripes [command];
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case SYNC: sp -> retrigger = true; break;
				case FREQ_RATIO: sp -> multiplier = line -> get_short_negative (); break;
				case FREQ: sp -> freq = line -> get_negative (); break;
				case AMP: sp -> amp = line -> get_very_negative (); break;
				case INDEX: sp -> index = line -> get_int (); break;
				case FREQ_RND: sp -> freq_rnd = line -> get_int (); break;
				case AMP_RND: sp -> amp_rnd = line -> get_int (); break;
				case INDEX_RND: sp -> index_rnd = line -> get_int (); break;
				//@ index amp
//				case INDEX_AMP: sp -> index_amp = true; break;
				case LFO_FREQ: sp -> lfo_freq = line -> get_int (); break;
				case LFO_AMP: sp -> lfo_amp = line -> get_int (); break;
				case LFO_INDEX: sp -> lfo_index = line -> get_int (); break;
				case LFO_SPEED: sp -> speed = line -> get_int (); break;
				case LFO_PHASE_SHIFT: sp -> phase_shift = line -> get_int (); break;
				case LFO_PULSE_WIDTH: sp -> pulse_width = line -> get_negative (); break;
				case LFO_SYNC: sp -> lfo_retrigger = true; break;
				case LFO_SYNC_TO: sp -> lfo_sync = line -> get (); break;
				case LFO_WAVE: sp -> wave = line -> get (); break;

				case FREQ_EG: sp -> freq_eg . read (line); break;
				case FREQ_EG_LEVEL: sp -> freq_eg . read_level (line); break;
				case FREQ_EG_TIME: sp -> freq_eg . read_time (line); break;
				case AMP_EG: sp -> amp_eg . read_amp (line); break;
				case AMP_EG_LEVEL: sp -> amp_eg . read_amp_level (line); break;
				case AMP_EG_TIME: sp -> amp_eg . read_time (line); break;
				case INDEX_EG: sp -> index_eg . read (line); break;
				case INDEX_EG_LEVEL: sp -> index_eg . read_level (line); break;
				case INDEX_EG_TIME: sp -> index_eg . read_time (line); break;

				case FREQ_PITCH_SENS: sp -> freq_sens . pitch . read (line); break;
				case FREQ_SENS: sp -> freq_sens . shift . read (line); break;
				case FREQ_LFO_SENS: sp -> freq_sens . lfo . read (line); break;
				case FREQ_ATTACK_SENS: sp -> freq_sens . attack . read (line); break;
				case FREQ_DECAY_SENS: sp -> freq_sens . decay . read (line); break;
				case FREQ_SUSTAIN_SENS: sp -> freq_sens . sustain . read (line); break;
				case FREQ_RELEASE_SENS: sp -> freq_sens . release . read (line); break;
				case FREQ_KEY_SENS: sp -> freq_sens . key . read (line); break;
				case FREQ_VELOCITY_SENS: sp -> freq_sens . velocity . read (line); break;
				case FREQ_VECTOR_X_SENS: sp -> freq_sens . vector_x . read (line); break;
				case FREQ_VECTOR_Y_SENS: sp -> freq_sens . vector_y . read (line); break;
				case FREQ_LFO_X_SENS: sp -> freq_sens . lfo_x . read (line); break;
				case FREQ_LFO_Y_SENS: sp -> freq_sens . lfo_y . read (line); break;
				case FREQ_KEY_ATTACK_EG_SCALING: sp -> freq_sens . key_attack_eg_scaling . read (line); break;
				case FREQ_KEY_EG_SCALING: sp -> freq_sens . key_eg_scaling . read (line); break;
				case FREQ_VELOCITY_ATTACK_EG_SCALING: sp -> freq_sens . velocity_attack_eg_scaling . read (line); break;
				case FREQ_VELOCITY_EG_SCALING: sp -> freq_sens . velocity_eg_scaling . read (line); break;

				case AMP_PITCH_SENS: sp -> amp_sens . pitch . read (line); break;
				case AMP_SENS: sp -> amp_sens . shift . read (line); break;
				case AMP_LFO_SENS: sp -> amp_sens . lfo . read (line); break;
				case AMP_ATTACK_SENS: sp -> amp_sens . attack . read (line); break;
				case AMP_DECAY_SENS: sp -> amp_sens . decay . read (line); break;
				case AMP_SUSTAIN_SENS: sp -> amp_sens . sustain . read (line); break;
				case AMP_RELEASE_SENS: sp -> amp_sens . release . read (line); break;
				case AMP_KEY_SENS: sp -> amp_sens . key . read (line); break;
				case AMP_VELOCITY_SENS: sp -> amp_sens . velocity . read (line); break;
				case AMP_VECTOR_X_SENS: sp -> amp_sens . vector_x . read (line); break;
				case AMP_VECTOR_Y_SENS: sp -> amp_sens . vector_y . read (line); break;
				case AMP_LFO_X_SENS: sp -> amp_sens . lfo_x . read (line); break;
				case AMP_LFO_Y_SENS: sp -> amp_sens . lfo_y . read (line); break;
				case AMP_KEY_ATTACK_EG_SCALING: sp -> amp_sens . key_attack_eg_scaling . read (line); break;
				case AMP_KEY_EG_SCALING: sp -> amp_sens . key_eg_scaling . read (line); break;
				case AMP_VELOCITY_ATTACK_EG_SCALING: sp -> amp_sens . velocity_attack_eg_scaling . read (line); break;
				case AMP_VELOCITY_EG_SCALING: sp -> amp_sens . velocity_eg_scaling . read (line); break;

				case INDEX_PITCH_SENS: sp -> index_sens . pitch . read (line); break;
				case INDEX_SENS: sp -> index_sens . shift . read (line); break;
				case INDEX_LFO_SENS: sp -> index_sens . lfo . read (line); break;
				case INDEX_ATTACK_SENS: sp -> index_sens . attack . read (line); break;
				case INDEX_DECAY_SENS: sp -> index_sens . decay . read (line); break;
				case INDEX_SUSTAIN_SENS: sp -> index_sens . sustain . read (line); break;
				case INDEX_RELEASE_SENS: sp -> index_sens . release . read (line); break;
				case INDEX_KEY_SENS: sp -> index_sens . key . read (line); break;
				case INDEX_VELOCITY_SENS: sp -> index_sens . velocity . read (line); break;
				case INDEX_VECTOR_X_SENS: sp -> index_sens . vector_x . read (line); break;
				case INDEX_VECTOR_Y_SENS: sp -> index_sens . vector_y . read (line); break;
				case INDEX_LFO_X_SENS: sp -> index_sens . lfo_x . read (line); break;
				case INDEX_LFO_Y_SENS: sp -> index_sens . lfo_y . read (line); break;
				case INDEX_KEY_ATTACK_EG_SCALING: sp -> index_sens . key_attack_eg_scaling . read (line); break;
				case INDEX_KEY_EG_SCALING: sp -> index_sens . key_eg_scaling . read (line); break;
				case INDEX_VELOCITY_ATTACK_EG_SCALING: sp -> index_sens . velocity_attack_eg_scaling . read (line); break;
				case INDEX_VELOCITY_EG_SCALING: sp -> index_sens . velocity_eg_scaling . read (line); break;

				case LFO_SPEED_SENS: sp -> lfo_sens . speed . read (line); break;
				case LFO_PITCH_SENS: sp -> lfo_sens . pitch . read (line); break;
				case LFO_VECTOR_X_SENS: sp -> lfo_sens . vector_x . read (line); break;
				case LFO_VECTOR_Y_SENS: sp -> lfo_sens . vector_y . read (line); break;

				default: return;
				}
				command = line -> get ();
			}
			break;
		case STEREO:
			command = line -> get ();
			if (command < 0 || command >= hercules_stereo_number) return;
			stp = stereo [command];
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case PAN: stp -> pan = line -> get_negative (); break;
				case RND: stp -> rnd = line -> get_int (); break;
				case LFO: stp -> lfo = line -> get_int (); break;
				case LFO_SPEED: stp -> speed = line -> get_int (); break;
				case LFO_PHASE_SHIFT: stp -> phase_shift = line -> get_int (); break;
				case LFO_PULSE_WIDTH: stp -> pulse_width = line -> get_negative (); break;
				case LFO_SYNC: stp -> lfo_retrigger = true; break;
				case LFO_SYNC_TO: stp -> lfo_sync = line -> get (); break;
				case LFO_WAVE: stp -> wave = line -> get (); break;

				case EG: stp -> eg . read (line); break;
				case EG_LEVEL: stp -> eg . read_level (line); break;
				case EG_TIME: stp -> eg . read_time (line); break;

				case PITCH_SENS: stp -> sens . pitch . read (line); break;
				case SENS: stp -> sens . shift . read (line); break;
				case LFO_SENS: stp -> sens . lfo . read (line); break;
				case ATTACK_SENS: stp -> sens . attack . read (line); break;
				case DECAY_SENS: stp -> sens . decay . read (line); break;
				case SUSTAIN_SENS: stp -> sens . sustain . read (line); break;
				case RELEASE_SENS: stp -> sens . release . read (line); break;
				case KEY_SENS: stp -> sens . key . read (line); break;
				case VELOCITY_SENS: stp -> sens . velocity . read (line); break;
				case VECTOR_X_SENS: stp -> sens . vector_x . read (line); break;
				case VECTOR_Y_SENS: stp -> sens . vector_y . read (line); break;
				case LFO_X_SENS: stp -> sens . lfo_x . read (line); break;
				case LFO_Y_SENS: stp -> sens . lfo_y . read (line); break;
				case KEY_ATTACK_EG_SCALING: stp -> sens . key_attack_eg_scaling . read (line); break;
				case KEY_EG_SCALING: stp -> sens . key_eg_scaling . read (line); break;
				case VELOCITY_ATTACK_EG_SCALING: stp -> sens . velocity_attack_eg_scaling . read (line); break;
				case VELOCITY_EG_SCALING: stp -> sens . velocity_eg_scaling . read (line); break;

				case LFO_SPEED_SENS: stp -> lfo_sens . speed . read (line); break;
				case LFO_PITCH_SENS: stp -> lfo_sens . pitch . read (line); break;
				case LFO_VECTOR_X_SENS: stp -> lfo_sens . vector_x . read (line); break;
				case LFO_VECTOR_Y_SENS: stp -> lfo_sens . vector_y . read (line); break;

				default: return;
				}
				command = line -> get ();
			}
			break;
		case DIRECT_OUTPUT: local_dry_out . send_level = line -> get_int (); break;
		case DIRECT_OUTPUT_DRY_WET_SENS: local_dry_out . dry_wet . read (line); break;
		case DIRECT_OUTPUT_VECTOR_X_SENS: local_dry_out . vector_x . read (line); break;
		case DIRECT_OUTPUT_VECTOR_Y_SENS: local_dry_out . vector_y . read (line); break;
		case DSP:
			lsb = line -> get ();
			if (lsb < 0 || lsb >= hercules_local_dsp_number) return;
			dspp = dsp [lsb];
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case ALGO:
					line -> get (msb);
					program = line -> get ();
					dsp_algo_change (msb, lsb, program);
					break;
				case DSP_ON: dspp -> on = true; break;
				case DSP_OFF: dspp -> on = false; break;
				case DIRECT_INPUT: dsp_send [lsb] . send_level = line -> get_int (); break;
				case DIRECT_INPUT_DRY_WET_SENS: dsp_send [lsb] . dry_wet . read (line); break;
				case DIRECT_INPUT_VECTOR_X_SENS: dsp_send [lsb] . vector_x . read (line); break;
				case DIRECT_INPUT_VECTOR_Y_SENS: dsp_send [lsb] . vector_y . read (line); break;
				case DIRECT_OUTPUT: dspp -> wet_output . send_level = line -> get_int (); break;
				case DIRECT_OUTPUT_DRY_WET_SENS: dspp -> wet_output . dry_wet . read (line); break;
				case DIRECT_OUTPUT_VECTOR_X_SENS: dspp -> wet_output . vector_x . read (line); break;
				case DIRECT_OUTPUT_VECTOR_Y_SENS: dspp -> wet_output . vector_y . read (line); break;
				case SEND_TO:
					program = line -> get () - lsb - 1;
					if (program < 0 || program >= dspp -> router_number) return;
					dspp -> routers [program] -> send_level = line -> get_int ();
					break;
				case SEND_TO_DRY_WET_SENS:
					program = line -> get () - lsb - 1;
					if (program < 0 || program >= dspp -> router_number) return;
					dspp -> routers [program] -> dry_wet . read (line);
					break;
				case SEND_TO_VECTOR_X_SENS:
					program = line -> get () - lsb - 1;
					if (program < 0 || program >= dspp -> router_number) return;
					dspp -> routers [program] -> vector_x . read (line);
					break;
				case SEND_TO_VECTOR_Y_SENS:
					program = line -> get () - lsb - 1;
					if (program < 0 || program >= dspp -> router_number) return;
					dspp -> routers [program] -> vector_y . read (line);
					break;
				case PARAMETER:
					program = line -> get ();
					if (program < 0 || program >= 16) return;
					dspp -> parameters [program] = line -> get_int ();
					break;
				default: return;
				}
				command = line -> get ();
			}
			break;
		default: return;
		}
		command = line -> get ();
	}
}

void parameter_block :: read_dsp_program (midi_stream * line, bool global) {
	int command = line -> get ();
	STRING msb;
	int lsb;
	int program;
	int index;
	dsp_parameter_block * dspp;
	while (command != EARTH) {
		switch (command) {
		case DIRECT_OUTPUT:
			if (global) global_dry_out . send_level = line -> get_int ();
			else local_dry_out . send_level = line -> get_int ();
			break;
		case DIRECT_OUTPUT_DRY_WET_SENS:
			if (global) global_dry_out . dry_wet . read (line);
			else local_dry_out . dry_wet . read (line);
			break;
		case DIRECT_OUTPUT_VECTOR_X_SENS:
			if (global) global_dry_out . vector_x . read (line);
			else local_dry_out . vector_x . read (line);
			break;
		case DIRECT_OUTPUT_VECTOR_Y_SENS:
			if (global) global_dry_out . vector_y . read (line);
			else local_dry_out . vector_y . read (line);
			break;
		case DSP:
			lsb = line -> get ();
			if (lsb < 0 || (global && lsb >= hercules_global_dsp_number) || (! global && lsb >= hercules_local_dsp_number)) return;
			index = global ? lsb + hercules_local_dsp_number : lsb;
			dspp = dsp [index];
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case ALGO:
					line -> get (msb);
					program = line -> get ();
					dsp_algo_change (msb, index, program);
					break;
				case DSP_ON: dspp -> on = true; break;
				case DSP_OFF: dspp -> on = false; break;
				case DIRECT_INPUT: dsp_send [index] . send_level = line -> get_int (); break;
				case DIRECT_INPUT_DRY_WET_SENS: dsp_send [index] . dry_wet . read (line); break;
				case DIRECT_INPUT_VECTOR_X_SENS: dsp_send [index] . vector_x . read (line); break;
				case DIRECT_INPUT_VECTOR_Y_SENS: dsp_send [index] . vector_y . read (line); break;
				case DIRECT_OUTPUT: dspp -> wet_output . send_level = line -> get_int (); break;
				case DIRECT_OUTPUT_DRY_WET_SENS: dspp -> wet_output . dry_wet . read (line); break;
				case DIRECT_OUTPUT_VECTOR_X_SENS: dspp -> wet_output . vector_x . read (line); break;
				case DIRECT_OUTPUT_VECTOR_Y_SENS: dspp -> wet_output . vector_y . read (line); break;
				case SEND_TO:
					program = line -> get () - lsb - 1;
					if (program < 0 || program >= dspp -> router_number) return;
					dspp -> routers [program] -> send_level = line -> get_int ();
					break;
				case SEND_TO_DRY_WET_SENS:
					program = line -> get () - lsb - 1;
					if (program < 0 || program >= dspp -> router_number) return;
					dspp -> routers [program] -> dry_wet . read (line);
					break;
				case SEND_TO_VECTOR_X_SENS:
					program = line -> get () - lsb - 1;
					if (program < 0 || program >= dspp -> router_number) return;
					dspp -> routers [program] -> vector_x . read (line);
					break;
				case SEND_TO_VECTOR_Y_SENS:
					program = line -> get () - lsb - 1;
					if (program < 0 || program >= dspp -> router_number) return;
					dspp -> routers [program] -> vector_y . read (line);
					break;
				case PARAMETER:
					program = line -> get ();
					if (program < 0 || program >= 16) return;
					dspp -> parameters [program] = line -> get_int ();
					break;
				default: return;
				}
				command = line -> get ();
			}
			break;
		default: return;
		}
		command = line -> get ();
	}
	notify_dsp_program_loaded (global);
}

void sub_parameter_block :: read (midi_stream * line) {
	lc = line -> get_negative ();
	bp = line -> get_int ();
	rc = line -> get_negative ();
}

void adsr_parameter_block :: read_level (midi_stream * line) {
	int ind = line -> get ();
	if (ind < 0 || ind > 3) return;
	level [ind] = line -> get_negative ();
}

void adsr_parameter_block :: read_time (midi_stream * line) {
	int ind = line -> get ();
	if (ind < 0 || ind > 3) return;
	time [ind] = line -> get_int ();
}

void adsr_parameter_block :: read (midi_stream * line) {
	int ind;
	for (ind = 0; ind < 4; ind++) level [ind] = line -> get_negative ();
	for (ind = 0; ind < 4; ind++) time [ind] = line -> get_int ();
}

void adsr_parameter_block :: read_amp_level (midi_stream * line) {
	int ind = line -> get ();
	if (ind < 0 || ind > 3) return;
	level [ind] = line -> get_very_negative ();
}

void adsr_parameter_block :: read_amp (midi_stream * line) {
	int ind;
	for (ind = 0; ind < 4; ind++) level [ind] = line -> get_very_negative ();
	for (ind = 0; ind < 4; ind++) time [ind] = line -> get_int ();
}

void drop_setup_controller (midi_stream * line, int mnemonic, int ind, int check, int control) {
	if (control > 127) control -= 4;
	if (control == check) return;
	line -> insert (mnemonic);
	emit (line, ind);
	line -> insert (control);
}

void synthesiser :: sub_store_control (midi_stream * line) {
	int ind;
	int device;
	int midi_channel;
	int midi_channel_extension;
	int arranger_assign;
	int reserved_notes;
	system_parameter_block * ctrl;
	for (ind = 0; ind < multitimbral; ind++) {
		device = data_blocks [ind] -> system -> device_id;
		if (device != ind) {
			line -> insert (DEVICE);
			emit (line, ind);
			emit (line, device);
		}
	}
	for (ind = 0; ind < multitimbral; ind++) {
		midi_channel_extension = data_blocks [ind] -> system -> midi_channel_extension;
		midi_channel = data_blocks [ind] -> system -> midi_channel;
		if (midi_channel_extension != 0x7f) {
			line -> insert (CHANNEL_EXTENSION);
			emit (line, ind);
			line -> insert (midi_channel_extension);
		}
		if (midi_channel != (ind & 0xf)) {
			line -> insert (CHANNEL);
			emit (line, ind);
			line -> insert (midi_channel);
		}
	}
	for (ind = 0; ind < multitimbral; ind++) {
		arranger_assign = data_blocks [ind] -> system -> arranger_assign;
		if (arranger_assign != 0x7f) {
			line -> insert (ARRANGER_ASSIGN);
			emit (line, ind);
			line -> insert (arranger_assign);
		}
	}
	for (ind = 0; ind < multitimbral; ind++) {
		reserved_notes = data_blocks [ind] -> system -> reserved_notes;
		if (reserved_notes != 0x7f) {
			line -> insert (RESERVED_NOTES);
			emit (line, ind);
			line -> insert (reserved_notes);
		}
	}
	line -> insert (CONTROLS);
	for (ind = 0; ind < multitimbral; ind++) {
		ctrl = data_blocks [ind] -> system;
		drop_setup_controller (line, PITCH, ind, 124, ctrl -> pitch);
		drop_setup_controller (line, FREQ, ind, 74, ctrl -> freq);
		drop_setup_controller (line, AMP, ind, 11, ctrl -> amp);
		drop_setup_controller (line, INDEX, ind, 71, ctrl -> index);
		drop_setup_controller (line, ATTACK, ind, 73, ctrl -> attack);
		drop_setup_controller (line, DECAY, ind, 76, ctrl -> decay);
		drop_setup_controller (line, SUSTAIN, ind, 75, ctrl -> sustain);
		drop_setup_controller (line, RELEASE, ind, 72, ctrl -> release);
		drop_setup_controller (line, SPEED, ind, 77, ctrl -> speed);
		drop_setup_controller (line, VIBRATO, ind, 1, ctrl -> vibrato);
		drop_setup_controller (line, TREMOLO, ind, 1, ctrl -> tremolo);
		drop_setup_controller (line, LFO_INDEX, ind, 1, ctrl -> lfo_index);
		drop_setup_controller (line, LFO_PAN, ind, 1, ctrl -> lfo_pan);
		drop_setup_controller (line, VECTOR_X, ind, 16, ctrl -> vector_x);
		drop_setup_controller (line, VECTOR_Y, ind, 17, ctrl -> vector_y);
		drop_setup_controller (line, VOLUME, ind, 7, ctrl -> volume);
		drop_setup_controller (line, PAN, ind, 10, ctrl -> pan);
		drop_setup_controller (line, HOLD, ind, 64, ctrl -> hold);
		drop_setup_controller (line, PORTA, ind, 65, ctrl -> porta);
		drop_setup_controller (line, PORTA_TIME, ind, 5, ctrl -> porta_time);
		drop_setup_controller (line, PORTA_MODE, ind, 84, ctrl -> porta_mode);
		drop_setup_controller (line, DRY_WET, ind, 91, ctrl -> dry_wet);
		drop_setup_controller (line, GLOBAL_VOLUME, ind, 11, ctrl -> dry_wet);
	}
	line -> insert (EARTH);
	/*if (global_volume_present) {
		line -> insert (VOLUME);
		if (global_volume_multi) {
			for (ind = 0; ind < multitimbral; ind++) {
				drop_controller (line, ind, data_blocks [ind] -> controllers -> global_volume, 16383, VOLUME);
			}
		} else {
			drop_controller (line, global_volume, 16383, GLOBAL_VOLUME);
		}
		line -> insert (EARTH);
	}*/
	if (tune != 0) emit (line, TUNE, tune);
	line -> insert (EARTH);
}

bool synthesiser :: store_control (midi_stream * line) {
	if (no_control_store) return false;
	emit (line, control_file_name);
	line -> insert (SETUP);
	sub_store_control (line);
	line -> insert (EARTH);
	return true;
}

bool synthesiser :: store_setup (midi_stream * line) {
	if (no_setup_store) return false;
	emit (line, setup_file_name);
	line -> insert (SETUP);
	sub_store_control (line);
	line -> insert (CATEGORIES);
	int ind;
	for (ind = 0; ind < category_pointer; ind++) emit (line, get_category (ind));
	line -> insert (EARTH);
	char * tp;
	for (ind = 32; ind < 128; ind++) {
		tp = get_message (ind);
		if (tp [0] != '\0') {
			line -> insert (MESSAGE);
			line -> insert (ind);
			emit (line, tp);
		}
	}
	line -> insert (BANKS);
	bank * bp;
	for (ind = 0; ind < 128; ind++) {
		bp = banks [ind];
		if (bp != NULL) {
			switch (bp -> type) {
			case 0: line -> insert (PRESET_ALGO); break;
			case 1: line -> insert (PRESET_PATTERN); break;
			case 2: line -> insert (PRESET_DSP_ALGO); break;
			case 32: line -> insert (ROM_PLAYER); break;
			case 33: line -> insert (WAVETABLE); break;
			case 34: line -> insert (PATTERN); break;
			case 35: line -> insert (STYLE); break;
			case 36: line -> insert (SOUND); break;
			case 37: line -> insert (USER_SOUND); break;
			case 38: line -> insert (DSP); break;
			case 39: line -> insert (USER_DSP); break;
			default: line -> insert (PLUGIN); break;
			}
			line -> insert (ind);
			if (bp -> type != 0 && bp -> type != 1 && bp -> type != 2) emit (line, bp -> config_string);
			if (bp -> type == 64) emit (line, bp -> plugin_parameter_string);
		}
	}
	line -> insert (EARTH);
	line -> insert (EARTH);
	return true;
}

void synthesiser :: store_configuration (midi_stream * line) {
	if (store_control (line)) return;
	store_setup (line);
}

