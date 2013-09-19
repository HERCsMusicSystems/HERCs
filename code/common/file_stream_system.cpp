///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "setup_file_reader.h"
#include "mnemonic.h"

bool copy_one (SetupFileReader * fr, midi_stream * line, int id) {
	line -> insert (id);
	if (! fr -> get_int ()) return false;
	emit (line, fr -> int_symbol);
	return true;
}

bool copy_short (SetupFileReader * fr, midi_stream * line, int id) {
	line -> insert (id);
	if (! fr -> get_int ()) return false;
	line -> insert (fr -> int_symbol & 0x7f);
	return true;
}

bool copy_two_long (SetupFileReader * fr, midi_stream * line, int id) {
	line -> insert (id);
	if (! fr -> get_int ()) return false;
	emit (line, fr -> int_symbol);
	if (! fr -> get_int ()) return false;
	emit (line, fr -> int_symbol);
	return true;
}

bool copy_two (SetupFileReader * fr, midi_stream * line, int id) {
	line -> insert (id);
	if (! fr -> get_int ()) return false;
	line -> insert (fr -> int_symbol & 0x7f);
	if (! fr -> get_int ()) return false;
	emit (line, fr -> int_symbol);
	return true;
}

bool copy_three (SetupFileReader * fr, midi_stream * line, int id) {
	line -> insert (id);
	for (int ind = 0; ind < 3; ind++) {
		if (! fr -> get_int ()) return false;
		emit (line, fr -> int_symbol);
	}
	return true;
}

bool copy_indexed_sens (SetupFileReader * fr, midi_stream * line, int id) {
	line -> insert (id);
	if (! fr -> get_int ()) return false;
	line -> insert (fr -> int_symbol & 0x7f);
	for (int ind = 0; ind < 3; ind++) {
		if (! fr -> get_int ()) return false;
		emit (line, fr -> int_symbol);
	}
	return true;
}

bool copy_eight (SetupFileReader * fr, midi_stream * line, int id) {
	line -> insert (id);
	for (int ind = 0; ind < 8; ind++) {
		if (! fr -> get_int ()) return false;
		emit (line, fr -> int_symbol);
	}
	return true;
}

void synthesiser :: transfer_program (SetupFileReader * fr, midi_stream * line) {
	bool algo_control = false;
	line -> insert (PROGRAM);
	bool skip;
	if (! fr -> get_id ("program")) return;
	while (fr -> get_id ()) {
		skip = true;
		if (fr -> id ("category")) {
			if (! fr -> get_int ()) return;
			line -> insert (CATEGORY);
			line -> insert (fr -> int_symbol);
		}
		if (fr -> id ("name")) {
			if (! fr -> get_string ()) return;
			line -> insert (NAME);
			emit (line, fr -> symbol);
		}
		if (fr -> id ("algo")) {
			line -> insert (ALGO);
			if (! algo_control) {
				if (! fr -> get_string ()) return;
				emit (line, fr -> symbol);
				algo_control = true;
			}
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
		}
		if (fr -> id ("algo_parameter")) {
			copy_two (fr, line, ALGO_PARAMETER);
		}
		if (fr -> id ("pattern")) {
			line -> insert (PATTERN);
			if (! fr -> get_string ()) return;
			emit (line, fr -> symbol);
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
		}
		if (fr -> id ("pitch")) {
			if (! copy_one (fr, line, PITCH)) return;
		}
		if (fr -> id ("freq")) {
			if (! copy_one (fr, line, FREQ)) return;
		}
		if (fr -> id ("amp")) {
			if (! copy_one (fr, line, AMP)) return;
		}
		if (fr -> id ("index")) {
			if (! copy_one (fr, line, INDEX)) return;
		}
		if (fr -> id ("attack")) {
			if (! copy_one (fr, line, ATTACK)) return;
		}
		if (fr -> id ("decay")) {
			if (! copy_one (fr, line, DECAY)) return;
		}
		if (fr -> id ("sustain")) {
			if (! copy_one (fr, line, SUSTAIN)) return;
		}
		if (fr -> id ("release")) {
			if (! copy_one (fr, line, RELEASE)) return;
		}
		if (fr -> id ("speed")) {
			if (! copy_one (fr, line, SPEED)) return;
		}
		if (fr -> id ("vibrato")) {
			if (! copy_one (fr, line, VIBRATO)) return;
		}
		if (fr -> id ("tremolo")) {
			if (! copy_one (fr, line, TREMOLO)) return;
		}
		if (fr -> id ("lfo_index")) {
			if (! copy_one (fr, line, LFO_INDEX)) return;
		}
		if (fr -> id ("lfo_pan")) {
			if (! copy_one (fr, line, LFO_PAN)) return;
		}
		if (fr -> id ("vector_x")) {
			if (! copy_one (fr, line, VECTOR_X)) return;
		}
		if (fr -> id ("vector_y")) {
			if (! copy_one (fr, line, VECTOR_Y)) return;
		}
		if (fr -> id ("volume")) {
			if (! copy_one (fr, line, VOLUME)) return;
		}
		if (fr -> id ("pan")) {
			if (! copy_one (fr, line, PAN)) return;
		}
		if (fr -> id ("hold_on")) {line -> insert (HOLD_ON);}
		if (fr -> id ("porta_on")) {line -> insert (PORTA_ON);}
		if (fr -> id ("poly")) {line -> insert (POLY);}
		if (fr -> id ("mono")) {line -> insert (MONO);}
		if (fr -> id ("porta_time")) {
			if (! copy_one (fr, line, PORTA_TIME)) return;
		}
		if (fr -> id ("porta_mode")) {
			if (! copy_short (fr, line, PORTA_MODE)) return;
		}
		if (fr -> id ("dry_wet")) {
			if (! copy_one (fr, line, DRY_WET)) return;
		}
		if (fr -> id ("tune")) {
			if (! copy_two (fr, line, TUNE)) return;
		}
		if (fr -> id ("sync_all")) {line -> insert (SYNC_ALL);}
//		if (fr -> id ("delay_on")) {line -> insert (DELAY_ON);}
//		if (fr -> id ("delay_off")) {line -> insert (DELAY_OFF);}
//		if (fr -> id ("delay_cross_on")) {line -> insert (DELAY_CROSS_ON);}
//		if (fr -> id ("delay_cross_off")) {line -> insert (DELAY_CROSS_OFF);}
//		if (fr -> id ("delay")) {
//			if (! copy_three (fr, line, DELAY)) return;
//		}
		if (fr -> id ("arpeggiator_on")) {line -> insert (ARPEGGIATOR_ON);}
		if (fr -> id ("arpeggiator_hold_on")) {line -> insert (ARPEGGIATOR_HOLD);}
		if (fr -> id ("tempo")) {if (! copy_two_long (fr, line, TEMPO)) return;}
		if (fr -> id ("division")) {if (! copy_two_long (fr, line, DIVISION)) return;}
		if (fr -> id ("variation")) {if (! copy_one (fr, line, VARIATION)) return;}
		if (fr -> id ("stripe")) {
			line -> insert (STRIPE);
			skip = false;
			if (! fr -> get_id ("stripe")) return;
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			fr -> skip ();
			while (fr -> get_id ()) {
				if (fr -> id ("sync")) {line -> insert (SYNC);}
				if (fr -> id ("freq_ratio")) {
					if (! copy_short (fr, line, FREQ_RATIO)) return;
				}
				if (fr -> id ("freq")) {
					if (! copy_one (fr, line, FREQ)) return;
				}
				if (fr -> id ("amp")) {
					if (! copy_one (fr, line, AMP)) return;
				}
				if (fr -> id ("index")) {
					if (! copy_one (fr, line, INDEX)) return;
				}
				//@ index amp
//				if (fr -> id ("index_amp")) {line -> insert (INDEX_AMP);}
				if (fr -> id ("lfo_freq")) {
					if (! copy_one (fr, line, LFO_FREQ)) return;
				}
				if (fr -> id ("lfo_amp")) {
					if (! copy_one (fr, line, LFO_AMP)) return;
				}
				if (fr -> id ("lfo_index")) {
					if (! copy_one (fr, line, LFO_INDEX)) return;
				}
				if (fr -> id ("lfo_speed")) {
					if (! copy_one (fr, line, SPEED)) return;
				}
				if (fr -> id ("freq_lfo_source")) {
					if (! copy_short (fr, line, FREQ_LFO_SOURCE)) return;
				}
				if (fr -> id ("amp_lfo_source")) {
					if (! copy_short (fr, line, AMP_LFO_SOURCE)) return;
				}
				if (fr -> id ("index_lfo_source")) {
					if (! copy_short (fr, line, INDEX_LFO_SOURCE)) return;
				}
				if (fr -> id ("lfo_phase_shift")) {
					if (! copy_one (fr, line, LFO_PHASE_SHIFT)) return;
				}
				if (fr -> id ("lfo_pulse_width")) {
					if (! copy_one (fr, line, LFO_PULSE_WIDTH)) return;
				}
				if (fr -> id ("lfo_sync")) {line -> insert (LFO_SYNC);}
				if (fr -> id ("lfo_sync_to")) {
					if (! copy_short (fr, line, LFO_SYNC_TO)) return;
				}
				if (fr -> id ("lfo_wave")) {
					if (! copy_short (fr, line, LFO_WAVE)) return;
				}

				if (fr -> id ("freq_eg")) {if (! copy_eight (fr, line, FREQ_EG)) return;}
				if (fr -> id ("freq_eg_level")) {if (! copy_two (fr, line, FREQ_EG_LEVEL)) return;}
				if (fr -> id ("freq_eg_time")) {if (! copy_two (fr, line, FREQ_EG_TIME)) return;}
				if (fr -> id ("amp_eg")) {if (! copy_eight (fr, line, AMP_EG)) return;}
				if (fr -> id ("amp_eg_level")) {if (! copy_two (fr, line, AMP_EG_LEVEL)) return;}
				if (fr -> id ("amp_eg_time")) {if (! copy_two (fr, line, AMP_EG_TIME)) return;}
				if (fr -> id ("index_eg")) {if (! copy_eight (fr, line, INDEX_EG)) return;}
				if (fr -> id ("index_eg_level")) {if (! copy_two (fr, line, INDEX_EG_LEVEL)) return;}
				if (fr -> id ("index_eg_time")) {if (! copy_two (fr, line, INDEX_EG_TIME)) return;}

				if (fr -> id ("freq_pitch_sens")) {if (! copy_three (fr, line, FREQ_PITCH_SENS)) return;}
				if (fr -> id ("freq_sens")) {if (! copy_three (fr, line, FREQ_SENS)) return;}
				if (fr -> id ("freq_lfo_sens")) {if (! copy_three (fr, line, FREQ_LFO_SENS)) return;}
				if (fr -> id ("freq_attack_sens")) {if (! copy_three (fr, line, FREQ_ATTACK_SENS)) return;}
				if (fr -> id ("freq_decay_sens")) {if (! copy_three (fr, line, FREQ_DECAY_SENS)) return;}
				if (fr -> id ("freq_sustain_sens")) {if (! copy_three (fr, line, FREQ_SUSTAIN_SENS)) return;}
				if (fr -> id ("freq_release_sens")) {if (! copy_three (fr, line, FREQ_RELEASE_SENS)) return;}
				if (fr -> id ("freq_key_sens")) {if (! copy_three (fr, line, FREQ_KEY_SENS)) return;}
				if (fr -> id ("freq_velocity_sens")) {if (! copy_three (fr, line, FREQ_VELOCITY_SENS)) return;}
				if (fr -> id ("freq_vector_x_sens")) {if (! copy_three (fr, line, FREQ_VECTOR_X_SENS)) return;}
				if (fr -> id ("freq_vector_y_sens")) {if (! copy_three (fr, line, FREQ_VECTOR_Y_SENS)) return;}
				if (fr -> id ("freq_lfo_x_sens")) {if (! copy_three (fr, line, FREQ_LFO_X_SENS)) return;}
				if (fr -> id ("freq_lfo_y_sens")) {if (! copy_three (fr, line, FREQ_LFO_Y_SENS)) return;}
				if (fr -> id ("freq_key_eg_scaling")) {if (! copy_three (fr, line, FREQ_KEY_EG_SCALING)) return;}
				if (fr -> id ("freq_velocity_eg_scaling")) {if (! copy_three (fr, line, FREQ_VELOCITY_EG_SCALING)) return;}

				if (fr -> id ("amp_pitch_sens")) {if (! copy_three (fr, line, AMP_PITCH_SENS)) return;}
				if (fr -> id ("amp_sens")) {if (! copy_three (fr, line, AMP_SENS)) return;}
				if (fr -> id ("amp_lfo_sens")) {if (! copy_three (fr, line, AMP_LFO_SENS)) return;}
				if (fr -> id ("amp_attack_sens")) {if (! copy_three (fr, line, AMP_ATTACK_SENS)) return;}
				if (fr -> id ("amp_decay_sens")) {if (! copy_three (fr, line, AMP_DECAY_SENS)) return;}
				if (fr -> id ("amp_sustain_sens")) {if (! copy_three (fr, line, AMP_SUSTAIN_SENS)) return;}
				if (fr -> id ("amp_release_sens")) {if (! copy_three (fr, line, AMP_RELEASE_SENS)) return;}
				if (fr -> id ("amp_key_sens")) {if (! copy_three (fr, line, AMP_KEY_SENS)) return;}
				if (fr -> id ("amp_velocity_sens")) {if (! copy_three (fr, line, AMP_VELOCITY_SENS)) return;}
				if (fr -> id ("amp_vector_x_sens")) {if (! copy_three (fr, line, AMP_VECTOR_X_SENS)) return;}
				if (fr -> id ("amp_vector_y_sens")) {if (! copy_three (fr, line, AMP_VECTOR_Y_SENS)) return;}
				if (fr -> id ("amp_lfo_x_sens")) {if (! copy_three (fr, line, AMP_LFO_X_SENS)) return;}
				if (fr -> id ("amp_lfo_y_sens")) {if (! copy_three (fr, line, AMP_LFO_Y_SENS)) return;}
				if (fr -> id ("amp_key_eg_scaling")) {if (! copy_three (fr, line, AMP_KEY_EG_SCALING)) return;}
				if (fr -> id ("amp_velocity_eg_scaling")) {if (! copy_three (fr, line, AMP_VELOCITY_EG_SCALING)) return;}

				if (fr -> id ("index_pitch_sens")) {if (! copy_three (fr, line, INDEX_PITCH_SENS)) return;}
				if (fr -> id ("index_sens")) {if (! copy_three (fr, line, INDEX_SENS)) return;}
				if (fr -> id ("index_lfo_sens")) {if (! copy_three (fr, line, INDEX_LFO_SENS)) return;}
				if (fr -> id ("index_attack_sens")) {if (! copy_three (fr, line, INDEX_ATTACK_SENS)) return;}
				if (fr -> id ("index_decay_sens")) {if (! copy_three (fr, line, INDEX_DECAY_SENS)) return;}
				if (fr -> id ("index_sustain_sens")) {if (! copy_three (fr, line, INDEX_SUSTAIN_SENS)) return;}
				if (fr -> id ("index_release_sens")) {if (! copy_three (fr, line, INDEX_RELEASE_SENS)) return;}
				if (fr -> id ("index_key_sens")) {if (! copy_three (fr, line, INDEX_KEY_SENS)) return;}
				if (fr -> id ("index_velocity_sens")) {if (! copy_three (fr, line, INDEX_VELOCITY_SENS)) return;}
				if (fr -> id ("index_vector_x_sens")) {if (! copy_three (fr, line, INDEX_VECTOR_X_SENS)) return;}
				if (fr -> id ("index_vector_y_sens")) {if (! copy_three (fr, line, INDEX_VECTOR_Y_SENS)) return;}
				if (fr -> id ("index_lfo_x_sens")) {if (! copy_three (fr, line, INDEX_LFO_X_SENS)) return;}
				if (fr -> id ("index_lfo_y_sens")) {if (! copy_three (fr, line, INDEX_LFO_Y_SENS)) return;}
				if (fr -> id ("index_key_eg_scaling")) {if (! copy_three (fr, line, INDEX_KEY_EG_SCALING)) return;}
				if (fr -> id ("index_velocity_eg_scaling")) {if (! copy_three (fr, line, INDEX_VELOCITY_EG_SCALING)) return;}

				if (fr -> id ("lfo_speed_sens")) {if (! copy_three (fr, line, LFO_SPEED_SENS)) return;}
				if (fr -> id ("lfo_pitch_sens")) {if (! copy_three (fr, line, LFO_PITCH_SENS)) return;}
				if (fr -> id ("lfo_vector_x_sens")) {if (! copy_three (fr, line, LFO_VECTOR_X_SENS)) return;}
				if (fr -> id ("lfo_vector_y_sens")) {if (! copy_three (fr, line, LFO_VECTOR_Y_SENS)) return;}

				fr -> skip ();
			}
			line -> insert (EARTH);
		}
		if (fr -> id ("stereo")) {
			line -> insert (STEREO);
			skip = false;
			if (! fr -> get_id ("stripe")) return;
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			fr -> skip ();
			while (fr -> get_id ()) {
				if (fr -> id ("pan")) {
					if (! copy_one (fr, line, PAN)) return;
				}
				if (fr -> id ("rnd")) {
					if (! copy_one (fr, line, RND)) return;
				}
				if (fr -> id ("lfo")) {
					if (! copy_one (fr, line, LFO)) return;
				}
				if (fr -> id ("lfo_source")) {
					if (! copy_short (fr, line, LFO_SOURCE)) return;
				}
				if (fr -> id ("lfo_speed")) {
					if (! copy_one (fr, line, LFO_SPEED)) return;
				}
				if (fr -> id ("lfo_phase_shift")) {
					if (! copy_one (fr, line, LFO_PHASE_SHIFT)) return;
				}
				if (fr -> id ("lfo_pulse_width")) {
					if (! copy_one (fr, line, LFO_PULSE_WIDTH)) return;
				}
				if (fr -> id ("lfo_sync")) {line -> insert (LFO_SYNC);}
				if (fr -> id ("lfo_sync_to")) {
					if (! copy_short (fr, line, LFO_SYNC_TO)) return;
				}
				if (fr -> id ("lfo_wave")) {
					if (! copy_short (fr, line, LFO_WAVE)) return;
				}

				if (fr -> id ("eg")) {if (! copy_eight (fr, line, EG)) return;}
				if (fr -> id ("eg_level")) {if (! copy_two (fr, line, EG_LEVEL)) return;}
				if (fr -> id ("eg_time")) {if (! copy_two (fr, line, EG_TIME)) return;}

				if (fr -> id ("pitch_sens")) {if (! copy_three (fr, line, PITCH_SENS)) return;}
				if (fr -> id ("sens")) {if (! copy_three (fr, line, SENS)) return;}
				if (fr -> id ("lfo_sens")) {if (! copy_three (fr, line, LFO_SENS)) return;}
				if (fr -> id ("attack_sens")) {if (! copy_three (fr, line, ATTACK_SENS)) return;}
				if (fr -> id ("decay_sens")) {if (! copy_three (fr, line, DECAY_SENS)) return;}
				if (fr -> id ("sustain_sens")) {if (! copy_three (fr, line, SUSTAIN_SENS)) return;}
				if (fr -> id ("release_sens")) {if (! copy_three (fr, line, RELEASE_SENS)) return;}
				if (fr -> id ("key_sens")) {if (! copy_three (fr, line, KEY_SENS)) return;}
				if (fr -> id ("velocity_sens")) {if (! copy_three (fr, line, VELOCITY_SENS)) return;}
				if (fr -> id ("vector_x_sens")) {if (! copy_three (fr, line, VECTOR_X_SENS)) return;}
				if (fr -> id ("vector_y_sens")) {if (! copy_three (fr, line, VECTOR_Y_SENS)) return;}
				if (fr -> id ("lfo_x_sens")) {if (! copy_three (fr, line, LFO_X_SENS)) return;}
				if (fr -> id ("lfo_y_sens")) {if (! copy_three (fr, line, LFO_Y_SENS)) return;}
				if (fr -> id ("key_eg_scaling")) {if (! copy_three (fr, line, KEY_EG_SCALING)) return;}
				if (fr -> id ("velocity_eg_scaling")) {if (! copy_three (fr, line, VELOCITY_EG_SCALING)) return;}

				if (fr -> id ("lfo_speed_sens")) {if (! copy_three (fr, line, LFO_SPEED_SENS)) return;}
				if (fr -> id ("lfo_pitch_sens")) {if (! copy_three (fr, line, LFO_PITCH_SENS)) return;}
				if (fr -> id ("lfo_vector_x_sens")) {if (! copy_three (fr, line, LFO_VECTOR_X_SENS)) return;}
				if (fr -> id ("lfo_vector_y_sens")) {if (! copy_three (fr, line, LFO_VECTOR_Y_SENS)) return;}

				fr -> skip ();
			}
			line -> insert (EARTH);
		}
		if (fr -> id ("direct_output")) {
			if (! copy_one (fr, line, DIRECT_OUTPUT)) return;
		}
		if (fr -> id ("direct_output_dry_wet_sens")) {
			if (! copy_three (fr, line, DIRECT_OUTPUT_DRY_WET_SENS)) return;
		}
		if (fr -> id ("direct_output_vector_x_sens")) {
			if (! copy_three (fr, line, DIRECT_OUTPUT_VECTOR_X_SENS)) return;
		}
		if (fr -> id ("direct_output_vector_y_sens")) {
			if (! copy_three (fr, line, DIRECT_OUTPUT_VECTOR_Y_SENS)) return;
		}
		if (fr -> id ("dsp")) {
			line -> insert (DSP);
			skip = false;
			if (! fr -> get_id ("dsp")) return;
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			fr -> skip ();
			while (fr -> get_id ()) {
				if (fr -> id ("algo")) {
					line -> insert (ALGO);
					if (! fr -> get_string ()) return;
					emit (line, fr -> symbol);
					if (fr -> get_int ()) return;
					line -> insert (fr -> int_symbol);
				}
				if (fr -> id ("dsp_on")) {line -> insert (DSP_ON);}
				if (fr -> id ("dsp_off")) {line -> insert (DSP_OFF);}
				if (fr -> id ("direct_input")) {
					if (! copy_one (fr, line, DIRECT_INPUT)) return;
				}
				if (fr -> id ("direct_input_dry_wet_sens")) {
					if (! copy_three (fr, line, DIRECT_INPUT_DRY_WET_SENS)) return;
				}
				if (fr -> id ("direct_input_vector_x_sens")) {
					if (! copy_three (fr, line, DIRECT_INPUT_VECTOR_X_SENS)) return;
				}
				if (fr -> id ("direct_input_vector_y_sens")) {
					if (! copy_three (fr, line, DIRECT_INPUT_VECTOR_Y_SENS)) return;
				}
				if (fr -> id ("direct_output")) {
					if (! copy_one (fr, line, DIRECT_OUTPUT)) return;
				}
				if (fr -> id ("direct_output_dry_wet_sens")) {
					if (! copy_three (fr, line, DIRECT_OUTPUT_DRY_WET_SENS)) return;
				}
				if (fr -> id ("direct_output_vector_x_sens")) {
					if (! copy_three (fr, line, DIRECT_OUTPUT_VECTOR_X_SENS)) return;
				}
				if (fr -> id ("direct_output_vector_y_sens")) {
					if (! copy_three (fr, line, DIRECT_OUTPUT_VECTOR_Y_SENS)) return;
				}
				if (fr -> id ("send_to")) {
					if (! copy_two (fr, line, SEND_TO)) return;
				}
				if (fr -> id ("send_to_dry_wet_sens")) {
					if (! copy_indexed_sens (fr, line, SEND_TO_DRY_WET_SENS)) return;
				}
				if (fr -> id ("send_to_vector_x_sens")) {
					if (! copy_indexed_sens (fr, line, SEND_TO_VECTOR_X_SENS)) return;
				}
				if (fr -> id ("send_to_vector_y_sens")) {
					if (! copy_indexed_sens (fr, line, SEND_TO_VECTOR_Y_SENS)) return;
				}
				if (fr -> id ("parameter")) {
					if (! copy_two (fr, line, PARAMETER)) return;
				}
				fr -> skip ();
			}
			line -> insert (EARTH);
		}
		if (skip) fr -> skip ();
	}
	line -> insert (EARTH);
}

void synthesiser :: transfer_setup_file (SetupFileReader * fr, midi_stream * line) {
	int ind;
	line -> insert (SETUP);
	while (fr -> get_id ()) {
		if (fr -> id ("device")) {
			line -> insert (DEVICE);
			if (! fr -> get_int ()) return;
			emit (line, fr -> int_symbol);
			if (! fr -> get_int ()) return;
			emit (line, fr -> int_symbol);
			fr -> skip ();
		}
		if (fr -> id ("channel")) {
			line -> insert (CHANNEL);
			if (! fr -> get_int ()) return;
			emit (line, fr -> int_symbol);
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			fr -> skip ();
		}
		if (fr -> id ("channel_extension")) {
			line -> insert (CHANNEL_EXTENSION);
			if (! fr -> get_int ()) return;
			emit (line, fr -> int_symbol);
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			fr -> skip ();
		}
		if (fr -> id ("arranger_assign")) {
			line -> insert (ARRANGER_ASSIGN);
			if (! fr -> get_int ()) return;
			emit (line, fr -> int_symbol);
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			fr -> skip ();
		}
		if (fr -> id ("reserved_notes")) {
			line -> insert (RESERVED_NOTES);
			if (! fr -> get_int ()) return;
			emit (line, fr -> int_symbol);
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			fr -> skip ();
		}
		if (fr -> id ("message")) {
			line -> insert (MESSAGE);
			if (! fr -> get_int ()) return;
			line -> insert (fr -> int_symbol);
			if (! fr -> get_string ()) return;
			emit (line, fr -> symbol);
		}
		if (fr -> id ("controls")) {
			line -> insert (CONTROLS);
			while (fr -> get_id ()) {
				if (fr -> id ("pitch")) line -> insert (PITCH);
				if (fr -> id ("freq")) line -> insert (FREQ);
				if (fr -> id ("amp")) line -> insert (AMP);
				if (fr -> id ("index")) line -> insert (INDEX);
				if (fr -> id ("attack")) line -> insert (ATTACK);
				if (fr -> id ("decay")) line -> insert (DECAY);
				if (fr -> id ("sustain")) line -> insert (SUSTAIN);
				if (fr -> id ("release")) line -> insert (RELEASE);
				if (fr -> id ("speed")) line -> insert (SPEED);
				if (fr -> id ("vibrato")) line -> insert (VIBRATO);
				if (fr -> id ("tremolo")) line -> insert (TREMOLO);
				if (fr -> id ("lfo_index")) line -> insert (LFO_INDEX);
				if (fr -> id ("lfo_pan")) line -> insert (LFO_PAN);
				if (fr -> id ("vector_x")) line -> insert (VECTOR_X);
				if (fr -> id ("vector_y")) line -> insert (VECTOR_Y);
				if (fr -> id ("volume")) line -> insert (VOLUME);
				if (fr -> id ("pan")) line -> insert (PAN);
				if (fr -> id ("hold")) line -> insert (HOLD);
				if (fr -> id ("porta")) line -> insert (PORTA);
				if (fr -> id ("porta_time")) line -> insert (PORTA_TIME);
				if (fr -> id ("porta_mode")) line -> insert (PORTA_MODE);
				if (fr -> id ("dry_wet")) line -> insert (DRY_WET);
				if (fr -> id ("global_volume")) line -> insert (GLOBAL_VOLUME);
				if (! fr -> get_int ()) return;
				emit (line, fr -> int_symbol);
				if (! fr -> get_int ()) return;
				line -> insert (fr -> int_symbol);
				fr -> skip ();
			}
			line -> insert (EARTH);
		}
		/*if (fr -> id ("volume")) {
			line -> insert (VOLUME);
			while (fr -> get_id ()) {
				if (fr -> id ("volume")) {
					line -> insert (VOLUME);
					if (! fr -> get_int ()) return;
					line -> insert (fr -> int_symbol);
					if (! fr -> get_int ()) return;
					emit (line, fr -> int_symbol);
				}
				if (fr -> id ("global_volume")) {
					line -> insert (GLOBAL_VOLUME);
					if (! fr -> get_int ()) return;
					emit (line, fr -> int_symbol);
				}
				fr -> skip ();
			}
			line -> insert (EARTH);
		}*/
		if (fr -> id ("tune")) {
			line -> insert (TUNE);
			if (! fr -> get_int ()) return;
			emit (line, fr -> int_symbol);
			fr -> skip ();
		}
		if (fr -> id ("categories")) {
			line -> insert (CATEGORIES);
			while (fr -> get_string ()) emit (line, fr -> symbol);
			line -> insert (EARTH);
		}
		if (fr -> id ("banks")) {
			line -> insert (BANKS);
			while (fr -> get_id ()) {
				if (fr -> id ("preset_algo")) ind = PRESET_ALGO;
				if (fr -> id ("preset_pattern")) ind = PRESET_PATTERN;
				if (fr -> id ("preset_dsp_algo")) ind = PRESET_DSP_ALGO;
				if (fr -> id ("rom_player")) ind = ROM_PLAYER;
				if (fr -> id ("wavetable")) ind = WAVETABLE;
				if (fr -> id ("pattern")) ind = PATTERN;
				if (fr -> id ("style")) ind = STYLE;
				if (fr -> id ("sound")) ind = SOUND;
				if (fr -> id ("user_sound")) ind = USER_SOUND;
				if (fr -> id ("dsp")) ind = DSP;
				if (fr -> id ("user_dsp")) ind = USER_DSP;
				if (fr -> id ("plugin")) ind = PLUGIN;
				line -> insert (ind);
				if (! fr -> get_int ()) return;
				line -> insert (fr -> int_symbol);
				if (ind != PRESET_ALGO && ind != PRESET_PATTERN && ind != PRESET_DSP_ALGO) {
					if (! fr -> get_string ()) return;
					emit (line, fr -> symbol);
				}
				if (ind == PLUGIN) {
					if (! fr -> get_string ()) return;
					emit (line, fr -> symbol);
				}
			}
			line -> insert (EARTH);
		}
	}
	line -> insert (EARTH);
}

void synthesiser :: transfer_program_map (SetupFileReader * fr, midi_stream * line) {
	line -> insert (MSB);
	while (fr -> get_id ()) {
		if (fr -> id ("name")) {
			line -> insert (NAME);
			if (! fr -> get_string ()) return;
			emit (line, fr -> symbol);
			fr -> skip ();
		}
		if (fr -> id ("lsb")) {
			line -> insert (LSB);
			while (fr -> get_id ()) {
				if (fr -> id ("location")) {
					line -> insert (LOCATION);
					if (! fr -> get_int ()) return;
					line -> insert (fr -> int_symbol);
					fr -> skip ();
				}
				if (fr -> id ("default")) {
					line -> insert (DEFAULT);
					fr -> skip ();
				}
				if (fr -> id ("name")) {
					line -> insert (NAME);
					if (! fr -> get_string ()) return;
					emit (line, fr -> symbol);
					fr -> skip ();
				}
				if (fr -> id ("program")) {
					line -> insert (PROGRAM);
					while (fr -> get_id ()) {
						if (fr -> id ("location")) {
							line -> insert (LOCATION);
							if (! fr -> get_int ()) return;
							line -> insert (fr -> int_symbol);
						}
						if (fr -> id ("default")) line -> insert (DEFAULT);
						if (fr -> id ("file")) {
							line -> insert (FILE);
							if (! fr -> get_string ()) return;
							emit (line, fr -> symbol);
						}
						fr -> skip ();
					}
					line -> insert (EARTH);
				}
			}
			line -> insert (EARTH);
		}
	}
	line -> insert (EARTH);
}

void synthesiser :: transfer_key_file (SetupFileReader * fr, midi_stream * line) {
	line -> insert (KEY);
	if (! fr -> get_string ()) return;
	emit (line, fr -> symbol);
}

void copy_eight (midi_stream * line, SetupFileWriter * tc, char * id) {
	tc -> write_id (id);
	int ind;
	for (ind = 0; ind < 4; ind++) tc -> write (line -> get_negative ());
	for (ind = 0; ind < 4; ind++) tc -> write (line -> get_int ());
	tc -> writeln_earth ();
}

void copy_eight_very_negative (midi_stream * line, SetupFileWriter * tc, char * id) {
	tc -> write_id (id);
	int ind;
	for (ind = 0; ind < 4; ind++) tc -> write (line -> get_very_negative ());
	for (ind = 0; ind < 4; ind++) tc -> write (line -> get_int ());
	tc -> writeln_earth ();
}

void copy_three (midi_stream * line, SetupFileWriter * tc, char * id) {
	tc -> write_id (id);
	tc -> write (line -> get_negative ());
	tc -> write (line -> get_int ());
	tc -> write (line -> get_negative ());
	tc -> writeln_earth ();
}

void copy_indexed_sens (midi_stream * line, SetupFileWriter * tc, char * id) {
	tc -> write_id (id);
	tc -> write (line -> get ());
	tc -> write (line -> get_negative ());
	tc -> write (line -> get_int ());
	tc -> write (line -> get_negative ());
	tc -> writeln_earth ();
}

void copy_two_long (midi_stream * line, SetupFileWriter * tc, char * id) {
	tc -> write_id (id);
	tc -> write (line -> get_int ());
	tc -> write (line -> get_int ());
	tc -> writeln_earth ();
}

void copy_two_positive (midi_stream * line, SetupFileWriter * tc, char * id) {
	tc -> write_id (id);
	tc -> write (line -> get ());
	tc -> write (line -> get_int ());
	tc -> writeln_earth ();
}

void copy_two_negative (midi_stream * line, SetupFileWriter * tc, char * id) {
	tc -> write_id (id);
	tc -> write (line -> get ());
	tc -> write (line -> get_negative ());
	tc -> writeln_earth ();
}

void copy_two_very_negative (midi_stream * line, SetupFileWriter * tc, char * id) {
	tc -> write_id (id);
	tc -> write (line -> get ());
	tc -> write (line -> get_very_negative ());
	tc -> writeln_earth ();
}

void synthesiser :: transfer_program (midi_stream * line, SetupFileWriter * tc) {
	bool algo_control = false;
	tc -> writeln_id ("program");
	int command = line -> get ();
	char msb [256];
	while (command != EARTH) {
		switch (command) {
		case CATEGORY: tc -> writeln_id ("category", line -> get ()); break;
		case NAME:
			line -> get (msb);
			tc -> write_id ("name");
			tc -> write (msb);
			tc -> writeln_earth ();
			break;
		case ALGO:
			tc -> write_id ("algo");
			if (! algo_control) {
				line -> get (msb);
				tc -> write (msb);
				algo_control = true;
			}
			tc -> write (line -> get ());
			tc -> write (line -> get ());
			tc -> writeln_earth ();
			break;
		case ALGO_PARAMETER: copy_two_positive (line, tc, "algo_parameter"); break;
		case PATTERN:
			tc -> write_id ("pattern");
			line -> get (msb);
			tc -> write (msb);
			tc -> write (line -> get ());
			tc -> write (line -> get ());
			tc -> writeln_earth ();
			break;
		case PITCH: tc -> writeln_id ("pitch", line -> get_int ()); break;
		case FREQ: tc -> writeln_id ("freq", line -> get_int ()); break;
		case AMP: tc -> writeln_id ("amp", line -> get_int ()); break;
		case INDEX: tc -> writeln_id ("index", line -> get_int ()); break;
		case ATTACK: tc -> writeln_id ("attack", line -> get_int ()); break;
		case DECAY: tc -> writeln_id ("decay", line -> get_int ()); break;
		case SUSTAIN: tc -> writeln_id ("sustain", line -> get_int ()); break;
		case RELEASE: tc -> writeln_id ("release", line -> get_int ()); break;
		case SPEED: tc -> writeln_id ("speed", line -> get_int ()); break;
		case VIBRATO: tc -> writeln_id ("vibrato", line -> get_int ()); break;
		case TREMOLO: tc -> writeln_id ("tremolo", line -> get_int ()); break;
		case LFO_INDEX: tc -> writeln_id ("lfo_index", line -> get_int ()); break;
		case LFO_PAN: tc -> writeln_id ("lfo_pan", line -> get_int ()); break;
		case VECTOR_X: tc -> writeln_id ("vector_x", line -> get_int ()); break;
		case VECTOR_Y: tc -> writeln_id ("vector_y", line -> get_int ()); break;
		case VOLUME: tc -> writeln_id ("volume", line -> get_int ()); break;
		case PAN: tc -> writeln_id ("pan", line -> get_int ()); break;
		case HOLD_ON: tc -> write_id ("hold_on"); tc -> writeln_earth (); break;
		case PORTA_ON: tc -> write_id ("porta_on"); tc -> writeln_earth (); break;
		case POLY: tc -> write_id ("poly"); tc -> writeln_earth (); break;
		case MONO: tc -> write_id ("mono"); tc -> writeln_earth (); break;
		case PORTA_TIME: tc -> writeln_id ("porta_time", line -> get_int ()); break;
		case PORTA_MODE: tc -> writeln_id ("porta_mode", line -> get ()); break;
		case DRY_WET: tc -> writeln_id ("dry_wet", line -> get_int ()); break;
		case TUNE: copy_two_positive (line, tc, "tune"); break;
		case SYNC_ALL: tc -> write_id ("sync_all"); tc -> writeln_earth (); break;
//		case DELAY_ON: tc -> write_id ("delay_on"); tc -> writeln_earth (); break;
//		case DELAY_OFF: tc -> write_id ("delay_off"); tc -> writeln_earth (); break;
//		case DELAY_CROSS_ON: tc -> write_id ("delay_cross_on"); tc -> writeln_earth (); break;
//		case DELAY_CROSS_OFF: tc -> write_id ("delay_cross_off"); tc -> writeln_earth (); break;
//		case DELAY:
//			tc -> write_id ("delay");
//			tc -> write (line -> get_int ());
//			tc -> write (line -> get_int ());
//			tc -> write (line -> get_negative ());
//			tc -> writeln_earth ();
//			break;
		case ARPEGGIATOR_ON: tc -> write_id ("arpeggiator_on"); tc -> writeln_earth (); break;
		case ARPEGGIATOR_HOLD: tc -> write_id ("arpeggiator_hold_on"); tc -> writeln_earth (); break;
		case TEMPO: copy_two_long (line, tc, "tempo"); break;
		case DIVISION: copy_two_long (line, tc, "division"); break;
		case VARIATION: tc -> writeln_id ("variation", line -> get_int ()); break;
		case STRIPE:
			tc -> writeln_id ("stripe");
			command = line -> get ();
			tc -> writeln_id ("stripe", command);
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case SYNC: tc -> write_id ("sync"); tc -> writeln_earth (); break;
				case FREQ_RATIO: tc -> writeln_id ("freq_ratio", line -> get_short_negative ()); break;
				case FREQ: tc -> writeln_id ("freq", line -> get_negative ()); break;
				case AMP: tc -> writeln_id ("amp", line -> get_very_negative ()); break;
				case INDEX: tc -> writeln_id ("index", line -> get_int ()); break;
				//@ index amp
//				case INDEX_AMP: tc -> write_id ("index_amp"); tc -> writeln_earth (); break;
				case LFO_FREQ: tc -> writeln_id ("lfo_freq", line -> get_int ()); break;
				case LFO_AMP: tc -> writeln_id ("lfo_amp", line -> get_int ()); break;
				case LFO_INDEX: tc -> writeln_id ("lfo_index", line -> get_int ()); break;
				case FREQ_LFO_SOURCE: tc -> writeln_id ("freq_lfo_source", line -> get ()); break;
				case AMP_LFO_SOURCE: tc -> writeln_id ("amp_lfo_source", line -> get ()); break;
				case INDEX_LFO_SOURCE: tc -> writeln_id ("index_lfo_source", line -> get ()); break;
				case LFO_SPEED: tc -> writeln_id ("lfo_speed", line -> get_int ()); break;
				case LFO_PHASE_SHIFT: tc -> writeln_id ("lfo_phase_shift", line -> get_int ()); break;
				case LFO_PULSE_WIDTH: tc -> writeln_id ("lfo_pulse_width", line -> get_negative ()); break;
				case LFO_SYNC: tc -> write_id ("lfo_sync"); tc -> writeln_earth (); break;
				case LFO_SYNC_TO: tc -> writeln_id ("lfo_sync_to", line -> get ()); break;
				case LFO_WAVE: tc -> writeln_id ("lfo_wave", line -> get ()); break;

				case FREQ_EG: copy_eight (line, tc, "freq_eg"); break;
				case FREQ_EG_LEVEL: copy_two_negative (line, tc, "freq_eg_level"); break;
				case FREQ_EG_TIME: copy_two_positive (line, tc, "freq_eg_time"); break;
				case AMP_EG: copy_eight_very_negative (line, tc, "freq_eg"); break;
				case AMP_EG_LEVEL: copy_two_very_negative (line, tc, "freq_eg_level"); break;
				case AMP_EG_TIME: copy_two_positive (line, tc, "freq_eg_time"); break;
				case INDEX_EG: copy_eight (line, tc, "freq_eg"); break;
				case INDEX_EG_LEVEL: copy_two_negative (line, tc, "freq_eg_level"); break;
				case INDEX_EG_TIME: copy_two_positive (line, tc, "freq_eg_time"); break;

				case FREQ_PITCH_SENS: copy_three (line, tc, "freq_pitch_sens"); break;
				case FREQ_SENS: copy_three (line, tc, "freq_sens"); break;
				case FREQ_LFO_SENS: copy_three (line, tc, "freq_lfo_sens"); break;
				case FREQ_ATTACK_SENS: copy_three (line, tc, "freq_attack_sens"); break;
				case FREQ_DECAY_SENS: copy_three (line, tc, "freq_decay_sens"); break;
				case FREQ_SUSTAIN_SENS: copy_three (line, tc, "freq_sustain_sens"); break;
				case FREQ_RELEASE_SENS: copy_three (line, tc, "freq_release_sens"); break;
				case FREQ_KEY_SENS: copy_three (line, tc, "freq_key_sens"); break;
				case FREQ_VELOCITY_SENS: copy_three (line, tc, "freq_velocity_sens"); break;
				case FREQ_VECTOR_X_SENS: copy_three (line, tc, "freq_vector_x_sens"); break;
				case FREQ_VECTOR_Y_SENS: copy_three (line, tc, "freq_vector_y_sens"); break;
				case FREQ_LFO_X_SENS: copy_three (line, tc, "freq_lfo_x_sens"); break;
				case FREQ_LFO_Y_SENS: copy_three (line, tc, "freq_lfo_y_sens"); break;
				case FREQ_KEY_EG_SCALING: copy_three (line, tc, "freq_key_eg_scaling"); break;
				case FREQ_VELOCITY_EG_SCALING: copy_three (line, tc, "freq_velocity_eg_scaling"); break;

				case AMP_PITCH_SENS: copy_three (line, tc, "amp_pitch_sens"); break;
				case AMP_SENS: copy_three (line, tc, "amp_sens"); break;
				case AMP_LFO_SENS: copy_three (line, tc, "amp_lfo_sens"); break;
				case AMP_ATTACK_SENS: copy_three (line, tc, "amp_attack_sens"); break;
				case AMP_DECAY_SENS: copy_three (line, tc, "amp_decay_sens"); break;
				case AMP_SUSTAIN_SENS: copy_three (line, tc, "amp_sustain_sens"); break;
				case AMP_RELEASE_SENS: copy_three (line, tc, "amp_release_sens"); break;
				case AMP_KEY_SENS: copy_three (line, tc, "amp_key_sens"); break;
				case AMP_VELOCITY_SENS: copy_three (line, tc, "amp_velocity_sens"); break;
				case AMP_VECTOR_X_SENS: copy_three (line, tc, "amp_vector_x_sens"); break;
				case AMP_VECTOR_Y_SENS: copy_three (line, tc, "amp_vector_y_sens"); break;
				case AMP_LFO_X_SENS: copy_three (line, tc, "amp_lfo_x_sens"); break;
				case AMP_LFO_Y_SENS: copy_three (line, tc, "amp_lfo_y_sens"); break;
				case AMP_KEY_EG_SCALING: copy_three (line, tc, "amp_key_eg_scaling"); break;
				case AMP_VELOCITY_EG_SCALING: copy_three (line, tc, "amp_velocity_eg_scaling"); break;

				case INDEX_PITCH_SENS: copy_three (line, tc, "index_pitch_sens"); break;
				case INDEX_SENS: copy_three (line, tc, "index_sens"); break;
				case INDEX_LFO_SENS: copy_three (line, tc, "index_lfo_sens"); break;
				case INDEX_ATTACK_SENS: copy_three (line, tc, "index_attack_sens"); break;
				case INDEX_DECAY_SENS: copy_three (line, tc, "index_decay_sens"); break;
				case INDEX_SUSTAIN_SENS: copy_three (line, tc, "index_sustain_sens"); break;
				case INDEX_RELEASE_SENS: copy_three (line, tc, "index_release_sens"); break;
				case INDEX_KEY_SENS: copy_three (line, tc, "index_key_sens"); break;
				case INDEX_VELOCITY_SENS: copy_three (line, tc, "index_velocity_sens"); break;
				case INDEX_VECTOR_X_SENS: copy_three (line, tc, "index_vector_x_sens"); break;
				case INDEX_VECTOR_Y_SENS: copy_three (line, tc, "index_vector_y_sens"); break;
				case INDEX_LFO_X_SENS: copy_three (line, tc, "index_lfo_x_sens"); break;
				case INDEX_LFO_Y_SENS: copy_three (line, tc, "index_lfo_y_sens"); break;
				case INDEX_KEY_EG_SCALING: copy_three (line, tc, "index_key_eg_scaling"); break;
				case INDEX_VELOCITY_EG_SCALING: copy_three (line, tc, "index_velocity_eg_scaling"); break;

				case LFO_SPEED_SENS: copy_three (line, tc, "lfo_speed_sens"); break;
				case LFO_PITCH_SENS: copy_three (line, tc, "lfo_pitch_sens"); break;
				case LFO_VECTOR_X_SENS: copy_three (line, tc, "lfo_vector_x_sens"); break;
				case LFO_VECTOR_Y_SENS: copy_three (line, tc, "lfo_vector_y_sens"); break;

				default: return;
				}
				command = line -> get ();
			}
			tc -> writeln_earth ();
			break;
		case STEREO:
			tc -> writeln_id ("stereo");
			command = line -> get ();
			tc -> writeln_id ("stripe", command);
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case PAN: tc -> writeln_id ("pan", line -> get_negative ()); break;
				case RND: tc -> writeln_id ("rnd", line -> get_int ()); break;
				case LFO: tc -> writeln_id ("lfo", line -> get_int ()); break;
				case LFO_SOURCE: tc -> writeln_id ("lfo_source", line -> get ()); break;
				case LFO_SPEED: tc -> writeln_id ("lfo_speed", line -> get_int ()); break;
				case LFO_PHASE_SHIFT: tc -> writeln_id ("lfo_phase_shift", line -> get_int ()); break;
				case LFO_PULSE_WIDTH: tc -> writeln_id ("lfo_pulse_width", line -> get_negative ()); break;
				case LFO_SYNC: tc -> write_id ("lfo_sync"); tc -> writeln_earth (); break;
				case LFO_SYNC_TO: tc -> writeln_id ("lfo_sync_to", line -> get ()); break;
				case LFO_WAVE: tc -> writeln_id ("lfo_wave", line -> get ()); break;

				case EG: copy_eight (line, tc, "eg"); break;
				case EG_LEVEL: copy_two_negative (line, tc, "eg_level"); break;
				case EG_TIME: copy_two_positive (line, tc, "eg_time"); break;

				case PITCH_SENS: copy_three (line, tc, "pitch_sens"); break;
				case SENS: copy_three (line, tc, "sens"); break;
				case LFO_SENS: copy_three (line, tc, "lfo_sens"); break;
				case ATTACK_SENS: copy_three (line, tc, "attack_sens"); break;
				case DECAY_SENS: copy_three (line, tc, "decay_sens"); break;
				case SUSTAIN_SENS: copy_three (line, tc, "sustain_sens"); break;
				case RELEASE_SENS: copy_three (line, tc, "release_sens"); break;
				case KEY_SENS: copy_three (line, tc, "key_sens"); break;
				case VELOCITY_SENS: copy_three (line, tc, "velocity_sens"); break;
				case VECTOR_X_SENS: copy_three (line, tc, "vector_x_sens"); break;
				case VECTOR_Y_SENS: copy_three (line, tc, "vector_y_sens"); break;
				case LFO_X_SENS: copy_three (line, tc, "lfo_x_sens"); break;
				case LFO_Y_SENS: copy_three (line, tc, "lfo_y_sens"); break;
				case KEY_EG_SCALING: copy_three (line, tc, "key_eg_scaling"); break;
				case VELOCITY_EG_SCALING: copy_three (line, tc, "velocity_eg_scaling"); break;

				case LFO_SPEED_SENS: copy_three (line, tc, "lfo_speed_sens"); break;
				case LFO_PITCH_SENS: copy_three (line, tc, "lfo_pitch_sens"); break;
				case LFO_VECTOR_X_SENS: copy_three (line, tc, "lfo_vector_x_sens"); break;
				case LFO_VECTOR_Y_SENS: copy_three (line, tc, "lfo_vector_y_sens"); break;

				default: return;
				}
				command = line -> get ();
			}
			tc -> writeln_earth ();
			break;
		// dsp section
		case DIRECT_OUTPUT: tc -> writeln_id ("direct_output", line -> get_int ()); break;
		case DIRECT_OUTPUT_DRY_WET_SENS: copy_three (line, tc, "direct_output_dry_wet_sens"); break;
		case DIRECT_OUTPUT_VECTOR_X_SENS: copy_three (line, tc, "direct_output_vector_x_sens"); break;
		case DIRECT_OUTPUT_VECTOR_Y_SENS: copy_three (line, tc, "direct_output_vecotr_y_sens"); break;
		case DSP:
			tc -> writeln_id ("dsp");
			command = line -> get ();
			tc -> writeln_id ("dsp", command);
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case ALGO:
					tc -> writeln_id ("algo");
					line -> get (msb);
					tc -> write (msb);
					tc -> write (line -> get());
					tc -> writeln_earth ();
					break;
				case DSP_ON: tc -> write_id ("dsp_on"); tc -> writeln_earth (); break;
				case DSP_OFF: tc -> write_id ("dsp_off"); tc -> writeln_earth (); break;
				case DIRECT_INPUT: tc -> writeln_id ("direct_input", line -> get_int ()); break;
				case DIRECT_INPUT_DRY_WET_SENS: copy_three (line, tc, "direct_input_dry_wet_sens"); break;
				case DIRECT_INPUT_VECTOR_X_SENS: copy_three (line, tc, "direct_input_vector_x_sens"); break;
				case DIRECT_INPUT_VECTOR_Y_SENS: copy_three (line, tc, "direct_input_vector_y_sens"); break;
				case DIRECT_OUTPUT: tc -> writeln_id ("direct_output", line -> get_int ()); break;
				case DIRECT_OUTPUT_DRY_WET_SENS: copy_three (line, tc, "direct_output_dry_wet_sens"); break;
				case DIRECT_OUTPUT_VECTOR_X_SENS: copy_three (line, tc, "direct_output_vector_x_sens"); break;
				case DIRECT_OUTPUT_VECTOR_Y_SENS: copy_three (line, tc, "direct_output_vector_y_sens"); break;
				case SEND_TO: copy_two_positive (line, tc, "send_to"); break;
				case SEND_TO_DRY_WET_SENS: copy_indexed_sens (line, tc, "send_to_dry_wet_sens"); break;
				case SEND_TO_VECTOR_X_SENS: copy_indexed_sens (line, tc, "send_to_vector_x_sens"); break;
				case SEND_TO_VECTOR_Y_SENS: copy_indexed_sens (line, tc, "send_to_vector_y_sens"); break;
				case PARAMETER: copy_two_positive (line, tc, "parameter"); break;
				default: return;
				}
				command = line -> get ();
			}
			tc -> writeln_earth ();
			break;
		default: return;
		}
		command = line -> get ();
	}
}

void synthesiser :: transfer_setup_file (midi_stream * line, SetupFileWriter * tc) {
	tc -> writeln_id ("setup");
	int command = line -> get ();
	//STRING text;
	char text [1024];
	while (command != EARTH) {
		switch (command) {
		case DEVICE:
			tc -> write_id ("device");
			tc -> write (line -> get_int ());
			tc -> write (line -> get_int ());
			tc -> writeln_earth ();
			break;
		case CHANNEL:
			tc -> write_id ("channel");
			tc -> write (line -> get_int ());
			tc -> write (line -> get ());
			tc -> writeln_earth ();
			break;
		case CHANNEL_EXTENSION:
			tc -> write_id ("channel_extension");
			tc -> write (line -> get_int ());
			tc -> write (line -> get ());
			tc -> writeln_earth ();
			break;
		case ARRANGER_ASSIGN:
			tc -> write_id ("arranger_assign");
			tc -> write (line -> get_int ());
			tc -> write (line -> get ());
			tc -> writeln_earth ();
			break;
		case RESERVED_NOTES:
			tc -> write_id ("reserved_notes");
			tc -> write (line -> get_int ());
			tc -> write (line -> get ());
			tc -> writeln_earth ();
			break;
		case MESSAGE:
			tc -> write_id ("message");
			tc -> write (line -> get ());
			line -> get (text);
			tc -> write (text);
			tc -> writeln_earth ();
		case CONTROLS:
			tc -> writeln_id ("controls");
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case PITCH: tc -> write_id ("pitch"); break;
				case FREQ: tc -> write_id ("freq"); break;
				case AMP: tc -> write_id ("amp"); break;
				case INDEX: tc -> write_id ("index"); break;
				case ATTACK: tc -> write_id ("attack"); break;
				case DECAY: tc -> write_id ("decay"); break;
				case SUSTAIN: tc -> write_id ("sustain"); break;
				case RELEASE: tc -> write_id ("release"); break;
				case SPEED: tc -> write_id ("speed"); break;
				case VIBRATO: tc -> write_id ("vibrato"); break;
				case TREMOLO: tc -> write_id ("tremolo"); break;
				case LFO_INDEX: tc -> write_id ("lfo_index"); break;
				case LFO_PAN: tc -> write_id ("lfo_pan"); break;
				case VECTOR_X: tc -> write_id ("vector_x"); break;
				case VECTOR_Y: tc -> write_id ("vector_y"); break;
				case VOLUME: tc -> write_id ("volume"); break;
				case PAN: tc -> write_id ("pan"); break;
				case HOLD: tc -> write_id ("hold"); break;
				case PORTA: tc -> write_id ("porta"); break;
				case PORTA_TIME: tc -> write_id ("porta_time"); break;
				case PORTA_MODE: tc -> write_id ("porta_mode"); break;
				case DRY_WET: tc -> write_id ("dry_wet"); break;
				case GLOBAL_VOLUME: tc -> write_id ("global_volume"); break;
				}
				tc -> write (line -> get ());
				tc -> write (line -> get_int ());
				command = line -> get ();
			}
			tc -> writeln_earth ();
			break;
		/*case VOLUME:
			tc -> writeln_id ("volume");
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case VOLUME:
					tc -> write_id ("volume");
					tc -> write (line -> get ());
					tc -> write (line -> get_int ());
					tc -> writeln_earth ();
					break;
				case GLOBAL_VOLUME:
					tc -> write_id ("global_volume");
					tc -> write (line -> get_int ());
					tc -> writeln_earth ();
					break;
				default: break;
				}
				command = line -> get ();
			}
			tc -> writeln_earth ();
			break;*/
		case TUNE: tc -> writeln_id ("tune", line -> get_int ()); break;
		case CATEGORIES:
			tc -> writeln_id ("categories");
			line -> get (text);
			while (text [0] != EARTH) {
				tc -> writeln (text);
				line -> get (text);
			}
			tc -> writeln_earth ();
			break;
		case BANKS:
			tc -> writeln_id ("banks");
			command = line -> get ();
			while (command != EARTH) {
				command = line -> get ();
				switch (command) {
				case PRESET_ALGO: tc -> write_id ("preset_algo"); break;
				case PRESET_PATTERN: tc -> write_id ("preset_pattern"); break;
				case PRESET_DSP_ALGO: tc -> write_id ("preset_dsp_algo"); break;
				case ROM_PLAYER: tc -> write_id ("rom_player"); break;
				case WAVETABLE: tc -> write_id ("wavetable"); break;
				case PATTERN: tc -> write_id ("pattern"); break;
				case STYLE: tc -> write_id ("style"); break;
				case SOUND: tc -> write_id ("sound"); break;
				case USER_SOUND: tc -> write_id ("user_sound"); break;
				case DSP: tc -> write_id ("dsp"); break;
				case USER_DSP: tc -> write_id ("user_dsp"); break;
				case PLUGIN: tc -> write_id ("plugin"); break;
				default: return;
				}
				tc -> write (line -> get ());
				if (command != PRESET_ALGO && command != PRESET_PATTERN && command != PRESET_DSP_ALGO) {
					line -> get (text);
					tc -> write (text);
				}
				if (command == PLUGIN) {
					line -> get (text);
					tc -> write (text);
				}
				tc -> writeln_earth ();
				command = line -> get ();
			}
			tc -> writeln_earth ();
			break;
		default: return;
		}
		command = line -> get ();
	}
	tc -> writeln_earth ();
}

void synthesiser :: transfer_program_map (midi_stream * line, SetupFileWriter * tc) {
	tc -> writeln_id ("msb");
	int command = line -> get ();
	STRING text;
	while (command != EARTH) {
		switch (command) {
		case NAME:
			line -> get (text);
			tc -> writeln_id ("name");
			tc -> write (text);
			tc -> writeln_earth ();
			break;
		case LSB:
			tc -> writeln_id ("lsb");
			command = line -> get ();
			while (command != EARTH) {
				switch (command) {
				case LOCATION: tc -> writeln_id ("location", line -> get ()); break;
				case DEFAULT: tc -> write_id ("default"); tc -> writeln_earth (); break;
				case NAME:
					tc -> write_id ("name");
					line -> get (text);
					tc -> write (text);
					tc -> writeln_earth ();
					break;
				case PROGRAM:
					tc -> writeln_id ("program");
					command = line -> get ();
					while (command != EARTH) {
						switch (command) {
						case LOCATION: tc -> writeln_id ("location", line -> get ()); break;
						case DEFAULT: tc -> write_id ("default"); tc -> writeln_earth (); break;
						case FILE:
							tc -> write_id ("file");
							line -> get (text);
							tc -> write (text);
							tc -> writeln_earth ();
							break;
						default: return;
						}
						command = line -> get ();
					}
					tc -> writeln_earth ();
					break;
				default: return;
				}
				command = line -> get ();
			}
			tc -> writeln_earth ();
			break;
		}
		command = line -> get ();
	}
	tc -> writeln_earth ();
}

void synthesiser :: transfer_key_file (midi_stream * line, SetupFileWriter * tc) {
	tc -> write_id ("key");
	STRING command;
	line -> get (command);
	tc -> write (command);
	tc -> write_earth ();
}

void synthesiser :: transfer (SetupFileReader * fr, midi_stream * line) {
	if (! fr -> get_id ()) return;
	if (fr -> id ("program")) {transfer_program (fr, line); return;}
	if (fr -> id ("setup")) {transfer_setup_file (fr, line); return;}
	if (fr -> id ("msb")) {transfer_program_map (fr, line); return;}
	if (fr -> id ("key")) {transfer_key_file (fr, line); return;}
}

void synthesiser :: transfer (midi_stream * line, SetupFileWriter * tc) {
	int command = line -> get ();
	switch (command) {
	case PROGRAM: transfer_program (line, tc); break;
	case SETUP: transfer_setup_file (line, tc); break;
	case MSB: transfer_program_map (line, tc); break;
	case KEY: transfer_key_file (line, tc); break;
	}
}

