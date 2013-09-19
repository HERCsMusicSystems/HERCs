///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "setup_file_reader.h"
#include "osc.h"
#include <string.h>

void synthesiser :: configure (void) {
	configure_setup ();
	configure_control ();
}

void synthesiser :: configure (char * setup, char * control) {
	configure_setup (setup);
	configure_control (control);
}

void synthesiser :: configure_setup (void) {
	no_setup_store = false;
	configure (setup_file_name);
}

void synthesiser :: configure_control (void) {
	no_control_store = false;
	configure (control_file_name);
}

void synthesiser :: configure_setup (char * setup) {
	string_copy (setup_file_name, setup);
	configure_setup ();
}

void synthesiser :: configure_control (char * control) {
	string_copy (control_file_name, control);
	configure_control ();
}

bool synthesiser :: configure (char * file_name) {
	char command [256];
	sprintf (command, "%s%s", root_directory, file_name);
	SetupFileReader * fr = new SetupFileReader (command);
	bool ret = sub_configure (fr);
	delete fr;
	return ret;
}

bool synthesiser :: sub_configure (SetupFileReader * fr) {
	int ind;
	int sub;
	STRING id;
	STRING parameter;
	system_parameter_block * sp;
	bank * bp;
	if (! fr -> get_id ("setup")) return false;
	while (fr -> get_id ()) {
		if (fr -> id ("device")) {
			if (! fr -> get_int ()) return false;
			ind = fr -> int_symbol;
			if (! fr -> get_int ()) return false;
			sub = fr -> int_symbol;
			fr -> skip ();
			if (ind >= multitimbral || ind < 0) return false;
			data_blocks [ind] -> system -> device_id = sub;
		}
		if (fr -> id ("channel")) {
			if (! fr -> get_int ()) return false;
			ind = fr -> int_symbol;
			if (! fr -> get_int ()) return false;
			sub = fr -> int_symbol;
			fr -> skip ();
			if (ind >= multitimbral || ind < 0) return false;
			data_blocks [ind] -> system -> midi_channel = sub;
		}
		if (fr -> id ("channel_extension")) {
			if (! fr -> get_int ()) return false;
			ind = fr -> int_symbol;
			if (! fr -> get_int ()) return false;
			sub = fr -> int_symbol;
			fr -> skip ();
			if (ind >= multitimbral || ind < 0) return false;
			data_blocks [ind] -> system -> midi_channel_extension = sub;
		}
		if (fr -> id ("arranger_assign")) {
			if (! fr -> get_int ()) return false;
			ind = fr -> int_symbol;
			if (! fr -> get_int ()) return false;
			sub = fr -> int_symbol;
			fr -> skip ();
			if (ind >= multitimbral || ind < 0) return false;
			data_blocks [ind] -> system -> arranger_assign = sub;
		}
		if (fr -> id ("reserved_notes")) {
			if (! fr -> get_int ()) return false;
			ind = fr -> int_symbol;
			if (! fr -> get_int ()) return false;
			sub = fr -> int_symbol;
			fr -> skip ();
			if (ind >= multitimbral || ind < 0) return false;
			data_blocks [ind] -> system -> reserved_notes = sub;
		}
		if (fr -> id ("message")) {
			if (! fr -> get_int ()) return false;
			ind = fr -> int_symbol;
			if (! fr -> get_string ()) return false;
			fr -> skip ();
			if (ind < 48 || ind > 127) return false;
			set_message (ind, fr -> symbol);
		}
		if (fr -> id ("controls")) {
			while (fr -> get_id ()) {
				string_copy (id, fr -> symbol);
				if (! fr -> get_int ()) return false;
				ind = fr -> int_symbol;
				if (! fr -> get_int ()) return false;
				sub = fr -> int_symbol;
				if (ind >= multitimbral || ind < 0) return false;
				if (sub > 127 || sub < 0) return false;
				if (sub >= 124) sub += 4;
				fr -> skip ();
				sp = data_blocks [ind] -> system;
				if (strcmp (id, "pitch") == 0) sp -> pitch = sub;
				if (strcmp (id, "freq") == 0) sp -> freq = sub;
				if (strcmp (id, "amp") == 0) sp -> amp = sub;
				if (strcmp (id, "index") == 0) sp -> index = sub;
				if (strcmp (id, "attack") == 0) sp -> attack = sub;
				if (strcmp (id, "decay") == 0) sp -> decay = sub;
				if (strcmp (id, "sustain") == 0) sp -> sustain = sub;
				if (strcmp (id, "release") == 0) sp -> release = sub;
				if (strcmp (id, "speed") == 0) sp -> speed = sub;
				if (strcmp (id, "vibrato") == 0) sp -> vibrato = sub;
				if (strcmp (id, "tremolo") == 0) sp -> tremolo = sub;
				if (strcmp (id, "lfo_index") == 0) sp -> lfo_index = sub;
				if (strcmp (id, "lfo_pan") == 0) sp -> lfo_pan = sub;
				if (strcmp (id, "vector_x") == 0) sp -> vector_x = sub;
				if (strcmp (id, "vector_y") == 0) sp -> vector_y = sub;
				if (strcmp (id, "volume") == 0) sp -> volume = sub;
				if (strcmp (id, "pan") == 0) sp -> pan = sub;
				if (strcmp (id, "hold") == 0) sp -> hold = sub;
				if (strcmp (id, "porta") == 0) sp -> porta = sub;
				if (strcmp (id, "porta_time") == 0) sp -> porta_time = sub;
				if (strcmp (id, "porta_mode") == 0) sp -> porta_mode = sub;
				if (strcmp (id, "dry_wet") == 0) sp -> dry_wet = sub;
				if (strcmp (id, "global_volume") == 0) sp -> global_volume = sub;
			}
		}
		/*if (fr -> id ("volume")) {
			while (fr -> get_id ()) {
				if (fr -> id ("volume")) {
					if (! fr -> get_int ()) return false;
					ind = fr -> int_symbol;
					if (ind > multitimbral || ind < 0) return false;
					if (! fr -> get_int ()) return false;
					data_blocks [ind] -> controllers -> global_volume = fr -> int_symbol;
				}
				if (fr -> id ("global_volume")) {
					if (! fr -> get_int ()) return false;
					global_volume = fr -> int_symbol;
				}
				fr -> skip ();
			}
		}*/
		if (fr -> id ("tune")) {
			if (! fr -> get_int ()) return false;
			tune = fr -> int_symbol;
			fr -> skip ();
		}
		if (fr -> id ("categories")) {
			reset_categories ();
			while (fr -> get_string ()) insert_category (fr -> symbol);
		}
		if (fr -> id ("banks")) {
			destroy_banks ();
			while (fr -> get_id ()) {
				string_copy (id, fr -> symbol);
				if (! fr -> get_int ()) return false;
				ind = fr -> int_symbol;
				if (ind > 127 || ind < 0) return false;
				if (strcmp (id, "preset_algo") != 0 && strcmp (id, "preset_pattern") != 0 && strcmp (id, "preset_dsp_algo") != 0) {
					if (! fr -> get_string ()) return false;
				}
				if (strcmp (id, "preset_algo") == 0) {
					if (pool != NULL) {
						banks [ind] = pool -> create_preset_algo_bank (); //new preset_algo_bank ();
						banks [ind] -> type = 0;
						string_copy (banks [ind] -> config_string, "");
						string_copy (banks [ind] -> plugin_parameter_string, "");
					}
				}
				if (strcmp (id, "preset_pattern") == 0) {
					banks [ind] = new preset_pattern_bank ();
					banks [ind] -> type = 1;
					string_copy (banks [ind] -> config_string, "");
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
				if (strcmp (id, "preset_dsp_algo") == 0) {
					banks [ind] = pool -> create_preset_dsp_algo_bank (); //new preset_dsp_algo_bank ();
					banks [ind] -> type = 2;
					string_copy (banks [ind] -> config_string, "");
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
				if (strcmp (id, "rom_player") == 0) {
					banks [ind] = pool -> create_rom_player_bank (fr -> symbol, this); //new rom_player_bank (fr -> symbol, this);
					banks [ind] -> type = 32;
					string_copy (banks [ind] -> config_string, fr -> symbol);
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
				if (strcmp (id, "wavetable") == 0) {
					banks [ind] = pool -> create_wavetable_bank (fr -> symbol, this); //new wavetable_bank (fr -> symbol);
					banks [ind] -> type = 33;
					string_copy (banks [ind] -> config_string, fr -> symbol);
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
				if (strcmp (id, "pattern") == 0) {
					banks [ind] = pool -> create_scripted_pattern_bank (fr -> symbol, this); //new scripted_pattern_bank (fr -> symbol, this);
					banks [ind] -> type = 34;
					string_copy (banks [ind] -> config_string, fr -> symbol);
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
				if (strcmp (id, "style") == 0) {
					banks [ind] = pool -> create_style_bank (fr -> symbol, this);
					banks [ind] -> type = 35;
					string_copy (banks [ind] -> config_string, fr -> symbol);
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
				if (strcmp (id, "sound") == 0) {
					banks [ind] = new sound_bank (fr -> symbol, this);
					banks [ind] -> type = 36;
					string_copy (banks [ind] -> config_string, fr -> symbol);
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
				if (strcmp (id, "user_sound") == 0) {
					banks [ind] = new user_sound_bank (fr -> symbol, this);
					banks [ind] -> type = 37;
					string_copy (banks [ind] -> config_string, fr -> symbol);
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
				if (strcmp (id, "dsp") == 0) {
					banks [ind] = new dsp_bank (fr -> symbol, this);
					banks [ind] -> type = 38;
					string_copy (banks [ind] -> config_string, fr -> symbol);
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
				if (strcmp (id, "user_dsp") == 0) {
					banks [ind] = new user_dsp_bank (fr -> symbol, this);
					banks [ind] -> type = 39;
					string_copy (banks [ind] -> config_string, fr -> symbol);
					string_copy (banks [ind] -> plugin_parameter_string, "");
				}
//				if (strcmp (id, "preset_style") == 0) {
//					banks [ind] = new preset_style_bank ();
//					banks [ind] -> type = 8;
//					string_copy (banks [ind] -> config_string, "");
//					string_copy (banks [ind] -> plugin_parameter_string, "");
//				}
				if (strcmp (id, "plugin") == 0) {
					string_copy (parameter, fr -> symbol);
					if (! fr -> get_string ()) return false;
					if (pool != NULL) {
						bp = pool -> create_plugin_algo_bank (parameter, fr -> symbol, this);
						if (bp != NULL) {
							bp -> type = 64;
							string_copy (bp -> config_string, fr -> symbol);
							string_copy (bp -> plugin_parameter_string, parameter);
							banks [ind] = bp;
						}
					}
				}
				fr -> skip ();
			}
		}
	}
	if (pool != NULL) pool -> notify_reserved_notes_changed (this);
	return true;
}

void parameter_block :: read_program (SetupFileReader * fr) {
	bool skip;
//	bool algo_control_needed = false;
	STRING msb;
	int lsb;
	int program;
	stripe_parameter_block * sp;
	stereo_stripe_parameter_block * stp;
	dsp_parameter_block * dspp;
	if (! fr -> get_id ("program")) return;
	while (fr -> get_id ()) {
		skip = true;
		if (fr -> id ("category")) {
			if (! fr -> get_int ()) return;
			category = fr -> int_symbol;
		}
		if (fr -> id ("name")) {
			if (! fr -> get_string ()) return;
			string_copy (name, fr -> symbol);
		}
		if (fr -> id ("algo")) {
//			if (algo_control_needed) {
//				if (! fr -> get_int ()) return;
//				lsb = fr -> int_symbol;
//				if (! fr -> get_int ()) return;
//				program = fr -> int_symbol;
//				algo_control (lsb, program);
//			} else {
				if (! fr -> get_string ()) return;
				string_copy (msb, fr -> symbol);
				if (! fr -> get_int ()) return;
				lsb = fr -> int_symbol;
				if (! fr -> get_int ()) return;
				program = fr -> int_symbol;
				algo_change (msb, lsb, program);
//				algo_control_needed = true;
//			}
		}
		if (fr -> id ("algo_parameter")) {
			if (! fr -> get_int ()) return;
			lsb = fr -> int_symbol;
			lsb &= 0x7f;
			if (! fr -> get_int ()) return;
			program = fr -> int_symbol;
			controllers -> algo_parameters [lsb] = program;
		}
		if (fr -> id ("pattern")) {
			if (! fr -> get_string ()) return;
			string_copy (msb, fr -> symbol);
			if (! fr -> get_int ()) return;
			lsb = fr -> int_symbol;
			if (! fr -> get_int ()) return;
			program = fr -> int_symbol;
			pattern_change (msb, lsb, program);
		}
		if (fr -> id ("pitch")) {
			if (! fr -> get_int ()) return;
			controllers -> pitch = fr -> int_symbol;
		}
		if (fr -> id ("freq")) {
			if (! fr -> get_int ()) return;
			controllers -> freq = fr -> int_symbol;
		}
		if (fr -> id ("amp")) {
			if (! fr -> get_int ()) return;
			controllers -> amp = fr -> int_symbol;
		}
		if (fr -> id ("index")) {
			if (! fr -> get_int ()) return;
			controllers -> index = fr -> int_symbol;
		}
		if (fr -> id ("attack")) {
			if (! fr -> get_int ()) return;
			controllers -> attack = fr -> int_symbol;
		}
		if (fr -> id ("decay")) {
			if (! fr -> get_int ()) return;
			controllers -> decay = fr -> int_symbol;
		}
		if (fr -> id ("sustain")) {
			if (! fr -> get_int ()) return;
			controllers -> sustain = fr -> int_symbol;
		}
		if (fr -> id ("release")) {
			if (! fr -> get_int ()) return;
			controllers -> release = fr -> int_symbol;
		}
		if (fr -> id ("speed")) {
			if (! fr -> get_int ()) return;
			controllers -> speed = fr -> int_symbol;
		}
		if (fr -> id ("vibrato")) {
			if (! fr -> get_int ()) return;
			controllers -> vibrato = fr -> int_symbol;
		}
		if (fr -> id ("tremolo")) {
			if (! fr -> get_int ()) return;
			controllers -> tremolo = fr -> int_symbol;
		}
		if (fr -> id ("lfo_index")) {
			if (! fr -> get_int ()) return;
			controllers -> lfo_index = fr -> int_symbol;
		}
		if (fr -> id ("lfo_pan")) {
			if (! fr -> get_int ()) return;
			controllers -> lfo_pan = fr -> int_symbol;
		}
		if (fr -> id ("vector_x")) {
			if (! fr -> get_int ()) return;
			controllers -> vector_x = fr -> int_symbol;
		}
		if (fr -> id ("vector_y")) {
			if (! fr -> get_int ()) return;
			controllers -> vector_y = fr -> int_symbol;
		}
		if (fr -> id ("volume")) {
			if (! fr -> get_int ()) return;
			if (controllers -> volume_reset) controllers -> volume = fr -> int_symbol;
		}
		if (fr -> id ("pan")) {
			if (! fr -> get_int ()) return;
			controllers -> pan = fr -> int_symbol;
		}
		if (fr -> id ("hold_on")) {controllers -> hold = true;}
		if (fr -> id ("porta_on")) {controllers -> porta = true;}
		if (fr -> id ("poly")) {poly (false);} //controllers -> poly = true;}
		if (fr -> id ("mono")) {mono (false);} //controllers -> poly = false;}
		if (fr -> id ("porta_time")) {
			if (! fr -> get_int ()) return;
			controllers -> porta_time = fr -> int_symbol;
		}
		if (fr -> id ("porta_mode")) {
			if (! fr -> get_int ()) return;
			controllers -> porta_mode = fr -> int_symbol;
		}
		if (fr -> id ("dry_wet")) {
			if (! fr -> get_int ()) return;
			controllers -> dry_wet = fr -> int_symbol;
		}
		if (fr -> id ("auto_vector")) {controllers -> vector_switch = true;}
		if (fr -> id ("vector_frames")) {
			skip = false;
			while (fr -> get_id ()) {
				if (fr -> id ("init")) {
					if (! fr -> get_int ()) return;
					controllers -> vector_init_x = fr -> int_symbol;
					if (! fr -> get_int ()) return;
					controllers -> vector_init_y = fr -> int_symbol;
				}
				if (fr -> id ("resolution")) {
					if (! fr -> get_int ()) return;
					controllers -> vector_resolution = fr -> int_symbol;
					if (! fr -> get_int ()) return;
					controllers -> vector_resolution_seconds = fr -> int_symbol;
				}
				if (fr -> id ("trigger_first")) controllers -> vector_trigger = 0;
				if (fr -> id ("trigger_all")) controllers -> vector_trigger = 1;
				if (fr -> id ("attack")) {
					if (! fr -> get_int ()) return;
					controllers -> vector_attack = fr -> int_symbol;
				}
				if (fr -> id ("release")) {
					if (! fr -> get_int ()) return;
					controllers -> vector_release = fr -> int_symbol;
				}
				if (fr -> id ("loop_loop")) controllers -> vector_loop_type = 1;
				if (fr -> id ("loop_round")) controllers -> vector_loop_type = 2;
				if (fr -> id ("loop")) {
					if (! fr -> get_int ()) return;
					if (fr -> int_symbol > vector_size) return;
					controllers -> vector_loop_start = fr -> int_symbol;
					if (! fr -> get_int ()) return;
					if (fr -> int_symbol > vector_size) return;
					controllers -> vector_loop_size = fr -> int_symbol;
				}
				if (fr -> id ("loop_delay")) {
					if (! fr -> get_int ()) return;
					controllers -> vector_loop_delay = fr -> int_symbol;
				}
				if (fr -> id ("frame")) {
					if (controllers -> vector_frame_count < vector_size) {
						if (! fr -> get_int ()) return;
						vector_frames [controllers -> vector_frame_count] . delay = fr -> int_symbol;
						if (! fr -> get_int ()) return;
						vector_frames [controllers -> vector_frame_count] . x = fr -> int_symbol;
						if (! fr -> get_int ()) return;
						vector_frames [controllers -> vector_frame_count] . y = fr -> int_symbol;
						controllers -> vector_frame_count++;
					}
				}
				fr -> skip ();
			}
		}
		if (fr -> id ("tune")) {
			if (! fr -> get_int ()) return;
			lsb = fr -> int_symbol;
			if (! fr -> get_int ()) return;
			controllers -> key_tuner [lsb] = fr -> int_symbol;
		}
		if (fr -> id ("sync_all")) {
			for (int stripe = 0; stripe < hercules_number; stripe++)
				stripes [stripe] -> retrigger = true;
		}
		if (fr -> id ("delay_on")) {
			if (hercules_local_dsp_number > 0) dsp [0] -> on = true;
		}
		if (fr -> id ("delay_off")) {
			if (hercules_local_dsp_number > 0) dsp [0] -> on = false;
		}
		if (fr -> id ("delay_cross_on")) {
			if (sth -> local_dsp > 0) dsp_algo_change ("PRESET DSP ALGO BANK", 0, 0);
		}
		if (fr -> id ("delay_cross_off")) {
			if (sth -> local_dsp > 0) dsp_algo_change ("PRESET DSP ALGO BANK", 0, 1);
		}
		if (fr -> id ("delay")) {
			int delay_feedback, delay_time, delay_dry_wet;
			if (! fr -> get_int ()) return;
			delay_feedback = fr -> int_symbol;
			if (! fr -> get_int ()) return;
			delay_time = fr -> int_symbol;
			if (! fr -> get_int ()) return;
			delay_dry_wet = fr -> int_symbol;
			if (sth ->local_dsp > 0) {
				dsp [0] -> parameters [0] = delay_feedback;
				dsp [0] -> parameters [1] = delay_time;
				controllers -> dry_wet = 8192 + delay_dry_wet;
			}
		}
		if (fr -> id ("arpeggiator_on")) {controllers -> arpeggiator_switch = true;}
		if (fr -> id ("arpeggiator_off")) {controllers -> arpeggiator_switch = false;}
		if (fr -> id ("arpeggiator_hold_on")) {controllers -> arpeggiator_hold = true;}
		if (fr -> id ("arpeggiator_hold_off")) {controllers -> arpeggiator_hold = false;}
		if (fr -> id ("tempo")) {
			if (! fr -> get_int ()) return;
			controllers -> beats_per_seconds = fr -> int_symbol;
			if (! fr -> get_int ()) return;
			controllers -> seconds = fr -> int_symbol;
		}
		if (fr -> id ("variation")) {
			if (! fr -> get_int ()) return;
			controllers -> variation = fr -> int_symbol;
		}
		if (fr -> id ("division")) {
			if (! fr -> get_int ()) return;
			controllers -> beats_per_bar = fr -> int_symbol;
			if (! fr -> get_int ()) return;
			controllers -> ticks_per_beat = fr -> int_symbol;
		}
		if (fr -> id ("stripe")) {
			skip = false;
			if (! fr -> get_id ("stripe")) return;
			if (! fr -> get_int ()) return;
			if (fr -> int_symbol >= hercules_number || fr -> int_symbol < 0) {fr -> skip (); fr -> skip ();}//return;
			else {
				sp = stripes [fr -> int_symbol];
				fr -> skip ();
				while (fr -> get_id ()) {
					if (fr -> id ("sync")) {sp -> retrigger = true;}
					if (fr -> id ("freq_ratio")) {
						if (! fr -> get_int ()) return;
						sp -> multiplier = fr -> int_symbol;
					}
					if (fr -> id ("freq")) {
						if (! fr -> get_int ()) return;
						sp -> freq = fr -> int_symbol;
					}
					if (fr -> id ("amp")) {
						if (! fr -> get_int ()) return;
						sp -> amp = fr -> int_symbol;
					}
					if (fr -> id ("index")) {
						if (! fr -> get_int ()) return;
						sp -> index = fr -> int_symbol;
					}
					if (fr -> id ("freq_rnd")) {
						if (! fr -> get_int ()) return;
						sp -> freq_rnd = fr -> int_symbol;
					}
					if (fr -> id ("amp_rnd")) {
						if (! fr -> get_int ()) return;
						sp -> amp_rnd = fr -> int_symbol;
					}
					if (fr -> id ("index_rnd")) {
						if (! fr -> get_int ()) return;
						sp -> index_rnd = fr -> int_symbol;
					}
					//@ index amp
//					if (fr -> id ("index_amp")) {sp -> index_amp = true;}
					if (fr -> id ("lfo_freq")) {
						if (! fr -> get_int ()) return;
						sp -> lfo_freq = fr -> int_symbol;
					}
					if (fr -> id ("lfo_amp")) {
						if (! fr -> get_int ()) return;
						sp -> lfo_amp = fr -> int_symbol;
					}
					if (fr -> id ("lfo_index")) {
						if (! fr -> get_int ()) return;
						sp -> lfo_index = fr -> int_symbol;
					}
					if (fr -> id ("freq_lfo_source")) {
						if (! fr -> get_int ()) return;
						sp -> freq_lfo_source = fr -> int_symbol;
					}
					if (fr -> id ("amp_lfo_source")) {
						if (! fr -> get_int ()) return;
						sp -> amp_lfo_source = fr -> int_symbol;
					}
					if (fr -> id ("index_lfo_source")) {
						if (! fr -> get_int ()) return;
						sp -> index_lfo_source = fr -> int_symbol;
					}
					if (fr -> id ("lfo_speed")) {
						if (! fr -> get_int ()) return;
						sp -> speed = fr -> int_symbol;
					}
					if (fr -> id ("lfo_phase_shift")) {
						if (! fr -> get_int ()) return;
						sp -> phase_shift = fr -> int_symbol;
					}
					if (fr -> id ("lfo_pulse_width")) {
						if (! fr -> get_int ()) return;
						sp -> pulse_width = fr -> int_symbol;
					}
					if (fr -> id ("lfo_sync")) {sp -> lfo_retrigger = true;}
					if (fr -> id ("lfo_sync_to")) {
						if (! fr -> get_int ()) return;
						sp -> lfo_sync = fr -> int_symbol;
					}
					if (fr -> id ("lfo_wave")) {
						if (! fr -> get_int ()) return;
						sp -> wave = fr -> int_symbol;
					}

					if (fr -> id ("freq_eg")) {if (! sp -> freq_eg . read (fr)) return;}
					if (fr -> id ("freq_eg_level")) {if (! sp -> freq_eg . read_level (fr)) return;}
					if (fr -> id ("freq_eg_time")) {if (! sp -> freq_eg . read_time (fr)) return;}
					if (fr -> id ("amp_eg")) {if (! sp -> amp_eg . read (fr)) return;}
					if (fr -> id ("amp_eg_level")) {if (! sp -> amp_eg . read_level (fr)) return;}
					if (fr -> id ("amp_eg_time")) {if (! sp -> amp_eg . read_time (fr)) return;}
					if (fr -> id ("index_eg")) {if (! sp -> index_eg . read (fr)) return;}
					if (fr -> id ("index_eg_level")) {if (! sp -> index_eg . read_level (fr)) return;}
					if (fr -> id ("index_eg_time")) {if (! sp -> index_eg . read_time (fr)) return;}

					if (fr -> id ("freq_pitch_sens")) {if (! sp -> freq_sens . pitch . read (fr)) return;}
					if (fr -> id ("freq_sens")) {if (! sp -> freq_sens . shift . read (fr)) return;}
					if (fr -> id ("freq_lfo_sens")) {if (! sp -> freq_sens . lfo . read (fr)) return;}
					if (fr -> id ("freq_attack_sens")) {if (! sp -> freq_sens . attack . read (fr)) return;}
					if (fr -> id ("freq_decay_sens")) {if (! sp -> freq_sens . decay . read (fr)) return;}
					if (fr -> id ("freq_sustain_sens")) {if (! sp -> freq_sens . sustain . read (fr)) return;}
					if (fr -> id ("freq_release_sens")) {if (! sp -> freq_sens . release . read (fr)) return;}
					if (fr -> id ("freq_key_sens")) {if (! sp -> freq_sens . key . read (fr)) return;}
					if (fr -> id ("freq_velocity_sens")) {if (! sp -> freq_sens . velocity . read (fr)) return;}
					if (fr -> id ("freq_vector_x_sens")) {if (! sp -> freq_sens . vector_x . read (fr)) return;}
					if (fr -> id ("freq_vector_y_sens")) {if (! sp -> freq_sens . vector_y . read (fr)) return;}
					if (fr -> id ("freq_lfo_x_sens")) {if (! sp -> freq_sens . lfo_x . read (fr)) return;}
					if (fr -> id ("freq_lfo_y_sens")) {if (! sp -> freq_sens . lfo_y . read (fr)) return;}
					if (fr -> id ("freq_key_attack_eg_scaling")) {if (! sp -> freq_sens . key_attack_eg_scaling . read (fr)) return;}
					if (fr -> id ("freq_key_eg_scaling")) {if (! sp -> freq_sens . key_eg_scaling . read (fr)) return;}
					if (fr -> id ("freq_velocity_attack_eg_scaling")) {if (! sp -> freq_sens . velocity_attack_eg_scaling . read (fr)) return;}
					if (fr -> id ("freq_velocity_eg_scaling")) {if (! sp -> freq_sens . velocity_eg_scaling . read (fr)) return;}

					if (fr -> id ("amp_pitch_sens")) {if (! sp -> amp_sens . pitch . read (fr)) return;}
					if (fr -> id ("amp_sens")) {if (! sp -> amp_sens . shift . read (fr)) return;}
					if (fr -> id ("amp_lfo_sens")) {if (! sp -> amp_sens . lfo . read (fr)) return;}
					if (fr -> id ("amp_attack_sens")) {if (! sp -> amp_sens . attack . read (fr)) return;}
					if (fr -> id ("amp_decay_sens")) {if (! sp -> amp_sens . decay . read (fr)) return;}
					if (fr -> id ("amp_sustain_sens")) {if (! sp -> amp_sens . sustain . read (fr)) return;}
					if (fr -> id ("amp_release_sens")) {if (! sp -> amp_sens . release . read (fr)) return;}
					if (fr -> id ("amp_key_sens")) {if (! sp -> amp_sens . key . read (fr)) return;}
					if (fr -> id ("amp_velocity_sens")) {if (! sp -> amp_sens . velocity . read (fr)) return;}
					if (fr -> id ("amp_vector_x_sens")) {if (! sp -> amp_sens . vector_x . read (fr)) return;}
					if (fr -> id ("amp_vector_y_sens")) {if (! sp -> amp_sens . vector_y . read (fr)) return;}
					if (fr -> id ("amp_lfo_x_sens")) {if (! sp -> amp_sens . lfo_x . read (fr)) return;}
					if (fr -> id ("amp_lfo_y_sens")) {if (! sp -> amp_sens . lfo_y . read (fr)) return;}
					if (fr -> id ("amp_key_attack_eg_scaling")) {if (! sp -> amp_sens . key_attack_eg_scaling . read (fr)) return;}
					if (fr -> id ("amp_key_eg_scaling")) {if (! sp -> amp_sens . key_eg_scaling . read (fr)) return;}
					if (fr -> id ("amp_velocity_attack_eg_scaling")) {if (! sp -> amp_sens . velocity_attack_eg_scaling . read (fr)) return;}
					if (fr -> id ("amp_velocity_eg_scaling")) {if (! sp -> amp_sens . velocity_eg_scaling . read (fr)) return;}

					if (fr -> id ("index_pitch_sens")) {if (! sp -> index_sens . pitch . read (fr)) return;}
					if (fr -> id ("index_sens")) {if (! sp -> index_sens . shift . read (fr)) return;}
					if (fr -> id ("index_lfo_sens")) {if (! sp -> index_sens . lfo . read (fr)) return;}
					if (fr -> id ("index_attack_sens")) {if (! sp -> index_sens . attack . read (fr)) return;}
					if (fr -> id ("index_decay_sens")) {if (! sp -> index_sens . decay . read (fr)) return;}
					if (fr -> id ("index_sustain_sens")) {if (! sp -> index_sens . sustain . read (fr)) return;}
					if (fr -> id ("index_release_sens")) {if (! sp -> index_sens . release . read (fr)) return;}
					if (fr -> id ("index_key_sens")) {if (! sp -> index_sens . key . read (fr)) return;}
					if (fr -> id ("index_velocity_sens")) {if (! sp -> index_sens . velocity . read (fr)) return;}
					if (fr -> id ("index_vector_x_sens")) {if (! sp -> index_sens . vector_x . read (fr)) return;}
					if (fr -> id ("index_vector_y_sens")) {if (! sp -> index_sens . vector_y . read (fr)) return;}
					if (fr -> id ("index_lfo_x_sens")) {if (! sp -> index_sens . lfo_x . read (fr)) return;}
					if (fr -> id ("index_lfo_y_sens")) {if (! sp -> index_sens . lfo_y . read (fr)) return;}
					if (fr -> id ("index_key_attack_eg_scaling")) {if (! sp -> index_sens . key_attack_eg_scaling . read (fr)) return;}
					if (fr -> id ("index_key_eg_scaling")) {if (! sp -> index_sens . key_eg_scaling . read (fr)) return;}
					if (fr -> id ("index_velocity_attack_eg_scaling")) {if (! sp -> index_sens . velocity_attack_eg_scaling . read (fr)) return;}
					if (fr -> id ("index_velocity_eg_scaling")) {if (! sp -> index_sens . velocity_eg_scaling . read (fr)) return;}

					if (fr -> id ("lfo_speed_sens")) {if (! sp -> lfo_sens . speed . read (fr)) return;}
					if (fr -> id ("lfo_pitch_sens")) {if (! sp -> lfo_sens . pitch . read (fr)) return;}
					if (fr -> id ("lfo_vector_x_sens")) {if (! sp -> lfo_sens . vector_x . read (fr)) return;}
					if (fr -> id ("lfo_vector_y_sens")) {if (! sp -> lfo_sens . vector_y . read (fr)) return;}


					fr -> skip ();
				}
			}
		}
		if (fr -> id ("stereo")) {
			skip = false;
			if (! fr -> get_id ("stripe")) return;
			if (! fr -> get_int ()) return;
			if (fr -> int_symbol >= hercules_stereo_number || fr -> int_symbol < 0) {fr -> skip (); fr -> skip ();}//return;
			else {
				stp = stereo [fr -> int_symbol];
				fr -> skip ();
				while (fr -> get_id ()) {
					if (fr -> id ("pan")) {
						if (! fr -> get_int ()) return;
						stp -> pan = fr -> int_symbol;
					}
					if (fr -> id ("rnd")) {
						if (! fr -> get_int ()) return;
						stp -> rnd = fr -> int_symbol;
					}
					if (fr -> id ("lfo")) {
						if (! fr -> get_int ()) return;
						stp -> lfo = fr -> int_symbol;
					}
					if (fr -> id ("lfo_source")) {
						if (! fr -> get_int ()) return;
						stp -> pan_lfo_source = fr -> int_symbol;
					}
					if (fr -> id ("lfo_speed")) {
						if (! fr -> get_int ()) return;
						stp -> speed = fr -> int_symbol;
					}
					if (fr -> id ("lfo_phase_shift")) {
						if (! fr -> get_int ()) return;
						stp -> phase_shift = fr -> int_symbol;
					}
					if (fr -> id ("lfo_pulse_width")) {
						if (! fr -> get_int ()) return;
						stp -> pulse_width = fr -> int_symbol;
					}
					if (fr -> id ("lfo_sync")) {stp -> lfo_retrigger = true;}
					if (fr -> id ("lfo_sync_to")) {
						if (! fr -> get_int ()) return;
						stp -> lfo_sync = fr -> int_symbol;
					}
					if (fr -> id ("lfo_wave")) {
						if (! fr -> get_int ()) return;
						stp -> wave = fr -> int_symbol;
					}

					if (fr -> id ("eg")) {if (! stp -> eg . read (fr)) return;}
					if (fr -> id ("eg_level")) {if (! stp -> eg . read_level (fr)) return;}
					if (fr -> id ("eg_time")) {if (! stp -> eg . read_time (fr)) return;}

					if (fr -> id ("pitch_sens")) {if (! stp -> sens . pitch . read (fr)) return;}
					if (fr -> id ("sens")) {if (! stp -> sens . shift . read (fr)) return;}
					if (fr -> id ("lfo_sens")) {if (! stp -> sens . lfo . read (fr)) return;}
					if (fr -> id ("attack_sens")) {if (! stp -> sens . attack . read (fr)) return;}
					if (fr -> id ("decay_sens")) {if (! stp -> sens . decay . read (fr)) return;}
					if (fr -> id ("sustain_sens")) {if (! stp -> sens . sustain . read (fr)) return;}
					if (fr -> id ("release_sens")) {if (! stp -> sens . release . read (fr)) return;}
					if (fr -> id ("key_sens")) {if (! stp -> sens . key . read (fr)) return;}
					if (fr -> id ("velocity_sens")) {if (! stp -> sens . velocity . read (fr)) return;}
					if (fr -> id ("vector_x_sens")) {if (! stp -> sens . vector_x . read (fr)) return;}
					if (fr -> id ("vector_y_sens")) {if (! stp -> sens . vector_y . read (fr)) return;}
					if (fr -> id ("lfo_x_sens")) {if (! stp -> sens . lfo_x . read (fr)) return;}
					if (fr -> id ("lfo_y_sens")) {if (! stp -> sens . lfo_y . read (fr)) return;}
					if (fr -> id ("key_attack_eg_scaling")) {if (! stp -> sens . key_attack_eg_scaling . read (fr)) return;}
					if (fr -> id ("key_eg_scaling")) {if (! stp -> sens . key_eg_scaling . read (fr)) return;}
					if (fr -> id ("velocity_attack_eg_scaling")) {if (! stp -> sens . velocity_attack_eg_scaling . read (fr)) return;}
					if (fr -> id ("velocity_eg_scaling")) {if (! stp -> sens . velocity_eg_scaling . read (fr)) return;}

					if (fr -> id ("lfo_speed_sens")) {if (! stp -> lfo_sens . speed . read (fr)) return;}
					if (fr -> id ("lfo_pitch_sens")) {if (! stp -> lfo_sens . pitch . read (fr)) return;}
					if (fr -> id ("lfo_vector_x_sens")) {if (! stp -> lfo_sens . vector_x . read (fr)) return;}
					if (fr -> id ("lfo_vector_y_sens")) {if (! stp -> lfo_sens . vector_y . read (fr)) return;}

					fr -> skip ();
				}
			}
		}
		// dsp section
		if (fr -> id ("direct_output")) {
			if (! fr -> get_int ()) return;
			local_dry_out . send_level = fr -> int_symbol;
		}
		if (fr -> id ("direct_output_dry_wet_sens")) {if (! local_dry_out . dry_wet . read (fr)) return;}
		if (fr -> id ("direct_output_vector_x_sens")) {if (! local_dry_out . vector_x . read (fr)) return;}
		if (fr -> id ("direct_output_vector_y_sens")) {if (! local_dry_out . vector_y . read (fr)) return;}
		if (fr -> id ("dsp")) {
			skip = false;
			if (! fr -> get_id ("dsp")) return;
			if (! fr -> get_int ()) return;
			if (fr -> int_symbol >= hercules_local_dsp_number || fr -> int_symbol < 0) {fr -> skip (); fr -> skip ();}
			else {
				lsb = fr -> int_symbol;
				dspp = dsp [lsb];
				fr -> skip ();
				while (fr -> get_id ()) {
					if (fr -> id ("algo")) {
						if (! fr -> get_string ()) return;
						string_copy (msb, fr -> symbol);
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol;
						dsp_algo_change (msb, lsb, program);
					}
					if (fr -> id ("dsp_on")) {dspp -> on = true;}
					if (fr -> id ("dsp_off")) {dspp -> on = false;}
					if (fr -> id ("direct_input")) {
						if (! fr -> get_int ()) return;
						dsp_send [lsb] . send_level = fr -> int_symbol;
					}
					if (fr -> id ("direct_input_dry_wet_sens")) {if (! dsp_send [lsb] . dry_wet . read (fr)) return;}
					if (fr -> id ("direct_input_vector_x_sens")) {if (! dsp_send [lsb] . vector_x . read (fr)) return;}
					if (fr -> id ("direct_input_vector_y_sens")) {if (! dsp_send [lsb] . vector_y . read (fr)) return;}
					if (fr -> id ("direct_output")) {
						if (! fr -> get_int ()) return;
						dspp -> wet_output . send_level = fr -> int_symbol;
					}
					if (fr -> id ("direct_output_dry_wet_sens")) {if (! dspp -> wet_output . dry_wet . read (fr)) return;}
					if (fr -> id ("direct_output_vector_x_sens")) {if (! dspp -> wet_output . vector_x . read (fr)) return;}
					if (fr -> id ("direct_output_vector_y_sens")) {if (! dspp -> wet_output . vector_y . read (fr)) return;}
					if (fr -> id ("send_to")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol - lsb - 1;
						if (program < 0 || program >= dspp -> router_number) return;
						if (! fr -> get_int ()) return;
						dspp -> routers [program] -> send_level = fr -> int_symbol;
					}
					if (fr -> id ("send_to_dry_wet_sens")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol - lsb - 1;
						if (program < 0 || program >= dspp -> router_number) return;
						if (! dspp -> routers [program] -> dry_wet . read (fr)) return;
					}
					if (fr -> id ("send_to_vector_x_sens")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol - lsb - 1;
						if (program < 0 || program >= dspp -> router_number) return;
						if (! dspp -> routers [program] -> vector_x . read (fr)) return;
					}
					if (fr -> id ("send_to_vector_y_sens")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol - lsb - 1;
						if (program < 0 || program >= dspp -> router_number) return;
						if (! dspp -> routers [program] -> vector_y . read (fr)) return;
					}
					if (fr -> id ("parameter")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol;
						if (program < 0 || program >= 16) return;
						if (! fr -> get_int ()) return;
						dspp -> parameters [program] = fr -> int_symbol;
					}
					fr -> skip ();
				}
			}
		}
		if (skip) fr -> skip ();
	}
}

void parameter_block :: read_dsp_program (SetupFileReader * fr, bool global) {
	bool skip;
	STRING msb;
	int lsb;
	int program;
	int index;
	dsp_parameter_block * dspp;
	if (! fr -> get_id ("dsp")) return;
	while (fr -> get_id ()) {
		skip = true;
		if (fr -> id ("direct_output")) {
			if (! fr -> get_int ()) return;
			if (global) global_dry_out . send_level = fr -> int_symbol; else local_dry_out . send_level = fr -> int_symbol;
		}
		if (fr -> id ("direct_output_dry_wet_sens")) {
			if (global) {if (! global_dry_out . dry_wet . read (fr)) return;}
			else {if (! local_dry_out . dry_wet . read (fr)) return;}
		}
		if (fr -> id ("direct_output_vector_x_sens")) {
			if (global) {if (! global_dry_out . vector_x . read (fr)) return;}
			else {if (! local_dry_out . vector_x . read (fr)) return;}
		}
		if (fr -> id ("direct_output_vector_y_sens")) {
			if (global) {if (! global_dry_out . vector_y . read (fr)) return;}
			else {if (! local_dry_out . vector_y . read (fr)) return;}
		}
		if (fr -> id ("dsp")) {
			skip = false;
			if (! fr -> get_id ("dsp")) return;
			if (! fr -> get_int ()) return;
			if (fr -> int_symbol < 0 || (global && fr -> int_symbol >= hercules_global_dsp_number) || (! global && fr -> int_symbol >= hercules_local_dsp_number)) {
				fr -> skip (); fr -> skip ();
			} else {
				lsb = fr -> int_symbol;
				index = global ? lsb + hercules_local_dsp_number : lsb;
				dspp = dsp [index];
				fr -> skip ();
				while (fr -> get_id ()) {
					if (fr -> id ("algo")) {
						if (! fr -> get_string ()) return;
						string_copy (msb, fr -> symbol);
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol;
						dsp_algo_change (msb, index, program);
					}
					if (fr -> id ("dsp_on")) {dspp -> on = true;}
					if (fr -> id ("dsp_off")) {dspp -> on = false;}
					if (fr -> id ("direct_input")) {
						if (! fr -> get_int ()) return;
						dsp_send [index] . send_level = fr -> int_symbol;
					}
					if (fr -> id ("direct_input_dry_wet_sens")) {if (! dsp_send [index] . dry_wet . read (fr)) return;}
					if (fr -> id ("direct_input_vector_x_sens")) {if (! dsp_send [index] . vector_x . read (fr)) return;}
					if (fr -> id ("direct_input_vector_y_sens")) {if (! dsp_send [index] . vector_y . read (fr)) return;}
					if (fr -> id ("direct_output")) {
						if (! fr -> get_int ()) return;
						dspp -> wet_output . send_level = fr -> int_symbol;
					}
					if (fr -> id ("direct_output_dry_wet_sens")) {if (! dspp -> wet_output . dry_wet . read (fr)) return;}
					if (fr -> id ("direct_output_vector_x_sens")) {if (! dspp -> wet_output . vector_x . read (fr)) return;}
					if (fr -> id ("direct_output_vector_y_sens")) {if (! dspp -> wet_output . vector_y . read (fr)) return;}
					if (fr -> id ("send_to")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol - lsb - 1;
						if (program < 0 || program >= dspp -> router_number) return;
						if (! fr -> get_int ()) return;
						dspp -> routers [program] -> send_level = fr -> int_symbol;
					}
					if (fr -> id ("send_to_dry_wet_sens")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol - lsb - 1;
						if (program < 0 || program >= dspp -> router_number) return;
						if (! dspp -> routers [program] -> dry_wet . read (fr)) return;
					}
					if (fr -> id ("send_to_vector_x_sens")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol - lsb - 1;
						if (program < 0 || program >= dspp -> router_number) return;
						if (! dspp -> routers [program] -> vector_x . read (fr)) return;
					}
					if (fr -> id ("send_to_vector_y_sens")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol - lsb - 1;
						if (program < 0 || program >= dspp -> router_number) return;
						if (! dspp -> routers [program] -> vector_y . read (fr)) return;
					}
					if (fr -> id ("parameter")) {
						if (! fr -> get_int ()) return;
						program = fr -> int_symbol;
						if (program < 0 || program >= 16) return;
						if (! fr -> get_int ()) return;
						dspp -> parameters [program] = fr -> int_symbol;
					}
					fr -> skip ();
				}
			}
		}
		if (skip) fr -> skip ();
	}
	notify_dsp_program_loaded (global);
}

bool adsr_parameter_block :: read_level (SetupFileReader * fr) {
	if (! fr -> get_int ()) return false;
	int ind = fr -> int_symbol;
	if (ind > 3 || ind < 0) return false;
	if (! fr -> get_int ()) return false;
	level [ind] = fr -> int_symbol;
	return true;
}

bool adsr_parameter_block :: read_time (SetupFileReader * fr) {
	if (! fr -> get_int ()) return false;
	int ind = fr -> int_symbol;
	if (ind > 3 || ind < 0) return false;
	if (! fr -> get_int ()) return false;
	time [ind] = fr -> int_symbol;
	return true;
}

bool adsr_parameter_block :: read (SetupFileReader * fr) {
	int ind;
	for (ind = 0; ind < 4; ind++) {
		if (! fr -> get_int ()) return false;
		level [ind] = fr -> int_symbol;
	}
	for (ind = 0; ind < 4; ind++) {
		if (! fr -> get_int ()) return false;
		time [ind] = fr -> int_symbol;
	}
	return true;
}

bool sub_parameter_block :: read (SetupFileReader * fr) {
	if (! fr -> get_int ()) return false;
	lc = fr -> int_symbol;
	if (! fr -> get_int ()) return false;
	bp = fr -> int_symbol;
	if (! fr -> get_int ()) return false;
	rc = fr -> int_symbol;
	return true;
}

void drop_controller (SetupFileWriter * tc, int ind, int sub, char * text) {
	if (ind == sub) return;
	tc -> writeln_id (text, ind);
}

void drop_controller (SetupFileWriter * tc, int index, int ind, int sub, char * text) {
	if (ind == sub) return;
	tc -> writeln_id (text, index, ind);
}

void drop_controller (SetupFileWriter * tc, bool ind, bool sub, char * text) {
	if (ind == sub) return;
	tc -> write_id (text);
	tc -> writeln_earth ();
}

void adsr_parameter_block :: drop (SetupFileWriter * tc, int selector, char * header) {
	int mod = 0;
	int ind;
	for (ind = 0; ind < 3; ind++) {
		if (level [ind] != 0) mod++;
		if (time [ind] != 0) mod++;
	}
	if (level [ind] != selector) mod++;
	if (time [ind] != 0) mod++;
	if (mod == 0) return;
	char command [32];
	if (mod > 3) {
		sprintf (command, "%seg", header);
		tc -> write_id (command);
		for (ind = 0; ind < 4; ind++) tc -> write (level [ind]);
		for (ind = 0; ind < 4; ind++) tc -> write (time [ind]);
		tc -> writeln_earth ();
	} else {
		sprintf (command, "%seg_level", header);
		for (ind = 0; ind < 3; ind++) drop_controller (tc, ind, level [ind], 0, command);
		drop_controller (tc, 3, level [3], selector, command);
		sprintf (command, "%seg_time", header);
		for (ind = 0; ind < 4; ind++) drop_controller (tc, ind, time [ind], 0, command);
	}
}

void sub_parameter_block :: drop (SetupFileWriter * tc, int selector, char * header) {
	if (lc == - selector && bp == 8192 && rc == selector) return;
	tc -> write_id (header);
	tc -> write (lc);
	tc -> write (bp);
	tc -> write (rc);
	tc -> writeln_earth ();
}

void sub_parameter_block :: drop (SetupFileWriter * tc, int bp_selector, int selector, char * header) {
	if (lc == - selector && bp == bp_selector && rc == selector) return;
	tc -> write_id (header);
	tc -> write (lc);
	tc -> write (bp);
	tc -> write (rc);
	tc -> writeln_earth ();
}

void sub_parameter_block :: drop_indexed (SetupFileWriter * tc, int index, char * header) {
	if (lc == 0 && bp == 8192 && rc == 0) return;
	tc -> write_id (header);
	tc -> write (index);
	tc -> write (lc);
	tc -> write (bp);
	tc -> write (rc);
	tc -> writeln_earth ();
}

void sub_parameter_block :: drop_dry (SetupFileWriter * tc, char * header) {
	if (lc == 0 && bp == 8192 && rc == -256) return;
	tc -> write_id (header);
	tc -> write (lc);
	tc -> write (bp);
	tc -> write (rc);
	tc -> writeln_earth ();
}

void sub_parameter_block :: drop_wet (SetupFileWriter * tc, char * header) {
	if (lc == -256 && bp == 8192 && rc == 0) return;
	tc -> write_id (header);
	tc -> write (lc);
	tc -> write (bp);
	tc -> write (rc);
	tc -> writeln_earth ();
}

void parameter_block :: store_program (SetupFileWriter * tc) {
	int ind, sub, id;
	tc -> writeln_id ("program");
	tc -> write_id ("category"); tc -> write (category); tc -> writeln_earth ();
	tc -> write_id ("name"); tc -> write (name); tc -> writeln_earth ();

	tc -> write_id ("algo");
	tc -> write (algo_msb);
	tc -> write (algo_lsb);
	tc -> write (algo_program);
	tc -> writeln_earth ();
//	ind = 0;
//	while (algo_head -> is_control (ind)) {
//		tc -> writeln_id ("algo", algo_head -> lsb (ind), algo_head -> program (ind));
//		ind++;
//	}
	for (ind = 0; ind < 128; ind++) {
		drop_controller (tc, ind, controllers -> algo_parameters [ind], algo_head -> get_default (ind), "algo_parameter");
	}

	if (arp != NULL) {
		tc -> write_id ("pattern");
		tc -> write (pattern_msb);
		tc -> write (pattern_lsb);
		tc -> write (pattern_program);
		tc -> writeln_earth ();
	}

	drop_controller (tc, controllers -> pitch, 8192, "pitch");
	drop_controller (tc, controllers -> freq, 8192, "freq");
	drop_controller (tc, controllers -> amp, 8192, "amp");
	drop_controller (tc, controllers -> index, 8192, "index");
	drop_controller (tc, controllers -> attack, 8192, "attack");
	drop_controller (tc, controllers -> decay, 8192, "decay");
	drop_controller (tc, controllers -> sustain, 8192, "sustain");
	drop_controller (tc, controllers -> release, 8192, "release");
	drop_controller (tc, controllers -> speed, 8192, "speed");
	drop_controller (tc, controllers -> vibrato, 0, "vibrato");
	drop_controller (tc, controllers -> tremolo, 0, "tremolo");
	drop_controller (tc, controllers -> lfo_index, 0, "lfo_index");
	drop_controller (tc, controllers -> lfo_pan, 0, "lfo_pan");
	drop_controller (tc, controllers -> vector_x, 8192, "vector_x");
	drop_controller (tc, controllers -> vector_y, 8192, "vector_y");
	drop_controller (tc, controllers -> volume, 12800, "volume");
	drop_controller (tc, controllers -> pan, 8192, "pan");
	drop_controller (tc, controllers -> hold, false, "hold_on");

	drop_controller (tc, controllers -> vector_switch, false, "auto_vector");
	if (controllers -> vector_frame_count > 0) {// || controllers -> vector_resolution != 50 || controllers -> vector_resolution_seconds != 128) {
		tc -> writeln_id ("vector_frames");
		if (controllers -> vector_init_x != 8192 || controllers -> vector_init_y != 8192) {
			tc -> write_id ("init");
			tc -> write (controllers -> vector_init_x);
			tc -> write (controllers -> vector_init_y);
			tc -> writeln_earth ();
		}
		if (controllers -> vector_resolution != 50 || controllers -> vector_resolution_seconds != 128) tc -> writeln_id ("resolution", controllers -> vector_resolution, controllers -> vector_resolution_seconds);
		switch (controllers -> vector_trigger) {
		case 0: tc -> write_id ("trigger_first"); tc -> writeln_earth (); break;
		case 1: tc -> write_id ("trigger_all"); tc -> writeln_earth (); break;
		}
		drop_controller (tc, controllers -> vector_attack, 0, "attack");
		drop_controller (tc, controllers -> vector_release, 16383, "release");
		switch (controllers -> vector_loop_type) {
		case 1: tc -> write_id ("loop_loop"); tc -> writeln_earth (); break;
		case 2: tc -> write_id ("loop_round"); tc -> writeln_earth (); break;
		}
		if (controllers -> vector_loop_start != 0 || controllers -> vector_loop_size != 0) {
			tc -> write_id ("loop");
			tc -> write (controllers -> vector_loop_start);
			tc -> write (controllers -> vector_loop_size);
			tc -> writeln_earth ();
		}
		drop_controller (tc, controllers -> vector_loop_delay, 0, "loop_delay");
		for (ind = 0; ind < controllers -> vector_frame_count; ind++) {
			tc -> write_id ("frame");
			tc -> write (vector_frames [ind] . delay);
			tc -> write (vector_frames [ind] . x);
			tc -> write (vector_frames [ind] . y);
			tc -> writeln_earth ();
		}
		tc -> writeln_earth ();
	}

	for (ind = 0; ind < 128; ind++) drop_controller (tc, ind, controllers -> key_tuner [ind], ind << 7, "tune");

	bool sync_all = true;
	for (ind = 0; ind < hercules_number; ind++) if (! stripes [ind] -> retrigger) sync_all = false;
	drop_controller (tc, sync_all, false, "sync_all");
	stripe_parameter_block * spb;
	for (ind = 0; ind < hercules_number; ind++) {
		tc -> writeln_id ("stripe");
		tc -> writeln_id ("stripe", ind);
		spb = stripes [ind];
		if (! sync_all) drop_controller (tc, spb -> retrigger, false, "sync");
		drop_controller (tc, spb -> multiplier, 1, "freq_ratio");
		drop_controller (tc, spb -> freq, 0, "freq");
		drop_controller (tc, spb -> amp, ind == 0 ? 0 : -16383, "amp");
		drop_controller (tc, spb -> index, 0, "index");
		drop_controller (tc, spb -> freq_rnd, 0, "freq_rnd");
		drop_controller (tc, spb -> amp_rnd, 0, "freq_rnd");
		drop_controller (tc, spb -> index_rnd, 0, "index_rnd");
		//@ index amp
//		drop_controller (tc, spb -> index_amp, false, "index_amp");
		drop_controller (tc, spb -> lfo_freq, 0, "lfo_freq");
		drop_controller (tc, spb -> lfo_amp, 0, "lfo_amp");
		drop_controller (tc, spb -> lfo_index, 0, "lfo_index");
		drop_controller (tc, spb -> freq_lfo_source, ind, "freq_lfo_source");
		drop_controller (tc, spb -> amp_lfo_source, ind, "amp_lfo_source");
		drop_controller (tc, spb -> index_lfo_source, ind, "index_lfo_source");
		drop_controller (tc, spb -> speed, 8192, "lfo_speed");
		drop_controller (tc, spb -> lfo_retrigger, false, "lfo_sync");
		drop_controller (tc, spb -> lfo_sync, ind, "lfo_sync_to");
		drop_controller (tc, spb -> wave, 0, "lfo_wave");
		drop_controller (tc, spb -> pulse_width, 0, "lfo_pulse_width");
		drop_controller (tc, spb -> phase_shift, 0, "lfo_phase_shift");
		//-------------
		spb -> freq_eg . drop (tc, 0, "freq_");
		spb -> amp_eg . drop (tc, -16383, "amp_");
		spb -> index_eg . drop (tc, 0, "index_");
		//-------------
		spb -> freq_sens . pitch . drop (tc, 256, "freq_pitch_sens");
		spb -> freq_sens . shift . drop (tc, 0, "freq_sens");
		spb -> freq_sens . lfo . drop (tc, 0, 0, "freq_lfo_sens");
		spb -> freq_sens . attack . drop (tc, 0, "freq_attack_sens");
		spb -> freq_sens . decay . drop (tc, 0, "freq_decay_sens");
		spb -> freq_sens . sustain . drop (tc, 0, "freq_sustain_sens");
		spb -> freq_sens . release . drop (tc, 0, "freq_release_sens");
		spb -> freq_sens . key . drop (tc, 128, "freq_key_sens");
		spb -> freq_sens . velocity . drop (tc, 0, "freq_velocity_sens");
		spb -> freq_sens . vector_x . drop (tc, 0, "freq_vector_x_sens");
		spb -> freq_sens . vector_y . drop (tc, 0, "freq_vector_y_sens");
		spb -> freq_sens . lfo_x . drop (tc, 0, "freq_lfo_x_sens");
		spb -> freq_sens . lfo_y . drop (tc, 0, "freq_lfo_y_sens");
		spb -> freq_sens . key_attack_eg_scaling . drop (tc, 0, "freq_key_attack_eg_scaling");
		spb -> freq_sens . key_eg_scaling . drop (tc, 0, "freq_key_eg_scaling");
		spb -> freq_sens . velocity_attack_eg_scaling . drop (tc, 0, "freq_velocity_attack_eg_scaling");
		spb -> freq_sens . velocity_eg_scaling . drop (tc, 0, "freq_velocity_eg_scaling");
		//-------------
		spb -> amp_sens . pitch . drop (tc, 0, "amp_pitch_sens");
		spb -> amp_sens . shift . drop (tc, 0, "amp_sens");
		spb -> amp_sens . lfo . drop (tc, 0, 0, "amp_lfo_sens");
		spb -> amp_sens . attack . drop (tc, 0, "amp_attack_sens");
		spb -> amp_sens . decay . drop (tc, 0, "amp_decay_sens");
		spb -> amp_sens . sustain . drop (tc, 0, "amp_sustain_sens");
		spb -> amp_sens . release . drop (tc, 0, "amp_release_sens");
		spb -> amp_sens . key . drop (tc, 0, "amp_key_sens");
		spb -> amp_sens . velocity . drop (tc, 0, "amp_velocity_sens");
		spb -> amp_sens . vector_x . drop (tc, 0, "amp_vector_x_sens");
		spb -> amp_sens . vector_y . drop (tc, 0, "amp_vector_y_sens");
		spb -> amp_sens . lfo_x . drop (tc, 0, "amp_lfo_x_sens");
		spb -> amp_sens . lfo_y . drop (tc, 0, "amp_lfo_y_sens");
		spb -> amp_sens . key_attack_eg_scaling . drop (tc, 0, "amp_key_attack_eg_scaling");
		spb -> amp_sens . key_eg_scaling . drop (tc, 0, "amp_key_eg_scaling");
		spb -> amp_sens . velocity_attack_eg_scaling . drop (tc, 0, "amp_velocity_attack_eg_scaling");
		spb -> amp_sens . velocity_eg_scaling . drop (tc, 0, "amp_velocity_eg_scaling");
		//-------------
		spb -> index_sens . pitch . drop (tc, 0, "index_pitch_sens");
		spb -> index_sens . shift . drop (tc, 0, "index_sens");
		spb -> index_sens . lfo . drop (tc, 0, 0, "index_lfo_sens");
		spb -> index_sens . attack . drop (tc, 0, "index_attack_sens");
		spb -> index_sens . decay . drop (tc, 0, "index_decay_sens");
		spb -> index_sens . sustain . drop (tc, 0, "index_sustain_sens");
		spb -> index_sens . release . drop (tc, 0, "index_release_sens");
		spb -> index_sens . key . drop (tc, 0, "index_key_sens");
		spb -> index_sens . velocity . drop (tc, 0, "index_velocity_sens");
		spb -> index_sens . vector_x . drop (tc, 0, "index_vector_x_sens");
		spb -> index_sens . vector_y . drop (tc, 0, "index_vector_y_sens");
		spb -> index_sens . lfo_x . drop (tc, 0, "index_lfo_x_sens");
		spb -> index_sens . lfo_y . drop (tc, 0, "index_lfo_y_sens");
		spb -> index_sens . key_attack_eg_scaling . drop (tc, 0, "index_key_attack_eg_scaling");
		spb -> index_sens . key_eg_scaling . drop (tc, 0, "index_key_eg_scaling");
		spb -> index_sens . velocity_attack_eg_scaling . drop (tc, 0, "index_velocity_attack_eg_scaling");
		spb -> index_sens . velocity_eg_scaling . drop (tc, 0, "index_velocity_eg_scaling");
		//-------------
		spb -> lfo_sens . pitch . drop (tc, 0, "lfo_pitch_sens");
		spb -> lfo_sens . speed . drop (tc, 0, "lfo_speed_sens");
		spb -> lfo_sens . vector_x . drop (tc, 0, "lfo_vector_x_sens");
		spb -> lfo_sens . vector_y . drop (tc, 0, "lfo_vector_y_sens");
		//-------------
		tc -> writeln_earth ();
	}
	stereo_stripe_parameter_block * stpb;
	for (ind = 0; ind < hercules_stereo_number; ind++) {
		tc -> writeln_id ("stereo");
		tc -> writeln_id ("stripe", ind);
		stpb = stereo [ind];
		drop_controller (tc, stpb -> pan, 0, "pan");
		drop_controller (tc, stpb -> rnd, 0, "rnd");
		drop_controller (tc, stpb -> lfo, 0, "lfo");
		drop_controller (tc, stpb -> pan_lfo_source, ind + 16, "lfo_source");
		drop_controller (tc, stpb -> speed, 8192, "lfo_speed");
		drop_controller (tc, stpb -> lfo_retrigger, false, "lfo_sync");
		drop_controller (tc, stpb -> lfo_sync, ind + 16, "lfo_sync_to");
		drop_controller (tc, stpb -> wave, 0, "lfo_wave");
		drop_controller (tc, stpb -> pulse_width, 0, "lfo_pulse_width");
		drop_controller (tc, stpb -> phase_shift, 0, "lfo_phase_shift");
		//-------------
		stpb -> eg . drop (tc, 0, "");
		//-------------
		stpb -> sens . pitch . drop (tc, 0, "pitch_sens");
		stpb -> sens . shift . drop (tc, 128, "sens");
		stpb -> sens . lfo . drop (tc, 0, 0, "lfo_sens");
		stpb -> sens . attack . drop (tc, 0, "attack_sens");
		stpb -> sens . decay . drop (tc, 0, "decay_sens");
		stpb -> sens . sustain . drop (tc, 0, "sustain_sens");
		stpb -> sens . release . drop (tc, 0, "release_sens");
		stpb -> sens . key . drop (tc, 0, "key_sens");
		stpb -> sens . velocity . drop (tc, 0, "velocity_sens");
		stpb -> sens . vector_x . drop (tc, 0, "vector_x_sens");
		stpb -> sens . vector_y . drop (tc, 0, "vector_y_sens");
		stpb -> sens . lfo_x . drop (tc, 0, "lfo_x_sens");
		stpb -> sens . lfo_y . drop (tc, 0, "lfo_y_sens");
		stpb -> sens . key_attack_eg_scaling . drop (tc, 0, "key_attack_eg_scaling");
		stpb -> sens . key_eg_scaling . drop (tc, 0, "key_eg_scaling");
		stpb -> sens . velocity_attack_eg_scaling . drop (tc, 0, "velocity_attack_eg_scaling");
		stpb -> sens . velocity_eg_scaling . drop (tc, 0, "velocity_eg_scaling");
		//-------------
		stpb -> lfo_sens . pitch . drop (tc, 0, "lfo_pitch_sens");
		stpb -> lfo_sens . speed . drop (tc, 0, "lfo_speed_sens");
		stpb -> lfo_sens . vector_x . drop (tc, 0, "lfo_vector_x_sens");
		stpb -> lfo_sens . vector_y . drop (tc, 0, "lfo_vector_y_sens");
		//-------------
		tc -> writeln_earth ();
	}

	drop_controller (tc, controllers -> poly, true, "mono");
	drop_controller (tc, controllers -> porta, false, "porta_on");
	drop_controller (tc, controllers -> porta_time, 8192, "porta_time");
	drop_controller (tc, controllers -> porta_mode, 0, "porta_mode");
//	drop_controller (tc, controllers -> delay_switch, true, "delay_off");
//	drop_controller (tc, controllers -> delay_cross_switch, true, "delay_cross_off");
//	if (controllers -> delay_feedback != 4096 || controllers -> delay_time != 8192 || controllers -> delay_dry_wet != 0) {
//		tc -> write_id ("delay");
//		tc -> write (controllers -> delay_feedback);
//		tc -> write (controllers -> delay_time);
//		tc -> write (controllers -> delay_dry_wet);
//		tc -> writeln_earth ();
//	}
	drop_controller (tc, controllers -> arpeggiator_switch, false, "arpeggiator_on");
	drop_controller (tc, controllers -> arpeggiator_hold, false, "arpeggiator_hold_on");
	if (controllers -> beats_per_seconds != 140 || controllers -> seconds != 60) {
		tc -> write_id ("tempo");
		tc -> write (controllers -> beats_per_seconds);
		tc -> write (controllers -> seconds);
		tc -> writeln_earth ();
	}
	if (controllers -> beats_per_bar != 4 || controllers -> ticks_per_beat != 24) {
		tc -> write_id ("division");
		tc -> write (controllers -> beats_per_bar);
		tc -> write (controllers -> ticks_per_beat);
		tc -> writeln_earth ();
	}
	drop_controller (tc, controllers -> variation, 0, "variation");
	// dsp section
	dsp_parameter_block_pointer dspp;
	dsp_router_pointer dsprp;
	drop_controller (tc, controllers -> dry_wet, 8192, "dry_wet");
	drop_controller (tc, local_dry_out . send_level, 16383, "direct_output");
	local_dry_out . dry_wet . drop_dry (tc, "direct_output_dry_wet_sens");
	local_dry_out . vector_x . drop (tc, 0, "direct_output_vector_x_sens");
	local_dry_out . vector_y . drop (tc, 0, "direct_output_vector_y_sens");
	for (ind = 0; ind < hercules_local_dsp_number; ind++) {
		dspp = dsp [ind];
		tc -> writeln_id ("dsp");
		tc -> writeln_id ("dsp", ind);
		tc -> write_id ("algo");
		tc -> write (dspp -> algo_msb);
		tc -> write (dspp -> algo_program);
		tc -> writeln_earth ();
		if (ind != 0) drop_controller (tc, dspp -> on, false, "dsp_on");
		else drop_controller (tc, dspp -> on, true, "dsp_off");
		drop_controller (tc, dsp_send [ind] . send_level, ind == 0 ? 16383 : 0, "direct_input");
		dsp_send [ind] . dry_wet . drop (tc, 0, "direct_input_dry_wet_sens");
		dsp_send [ind] . vector_x . drop (tc, 0, "direct_input_vector_x_sens");
		dsp_send [ind] . vector_y . drop (tc, 0, "direct_input_vector_y_sens");
		drop_controller (tc, dspp -> wet_output . send_level, 16383, "direct_output");
		dspp -> wet_output . dry_wet . drop_wet (tc, "direct_output_dry_wet_sens");
		dspp -> wet_output . vector_x . drop (tc, 0, "direct_output_vector_x_sens");
		dspp -> wet_output . vector_y . drop (tc, 0, "direct_output_vector_y_sens");
		for (sub = 0; sub < dspp -> router_number; sub++) {
			dsprp = dspp -> routers [sub];
			id = ind + sub + 1;
			drop_controller (tc, id, dsprp -> send_level, 0, "send_to");
			dsprp -> dry_wet . drop_indexed (tc, id, "send_to_dry_wet_sens");
			dsprp -> vector_x . drop_indexed (tc, id, "send_to_vector_x_sens");
			dsprp -> vector_y . drop_indexed (tc, id, "send_to_vector_y_sens");
		}
		for (sub = 0; sub < 16; sub++)
			drop_controller (tc, sub, dspp -> parameters [sub], dspp -> algo_head -> get_default (sub), "parameter");
		tc -> writeln_earth ();
	}
	tc -> writeln_earth ();
}

void parameter_block :: store_dsp_program (SetupFileWriter * tc, bool global) {
	int ind, sub, id;
	dsp_parameter_block_pointer dspp;
	dsp_router_pointer dsprp;
	if (global) {
		tc -> writeln_id ("global_dsp");
		drop_controller (tc, global_dry_out . send_level, 16383, "direct_output");
		global_dry_out . dry_wet . drop (tc, 0, "direct_output_dry_wet_sens");
		global_dry_out . vector_x . drop (tc, 0, "direct_output_vector_x_sens");
		global_dry_out . vector_y . drop (tc, 0, "direct_output_vector_y_sens");
	} else {
		tc -> writeln_id ("dsp");
		drop_controller (tc, local_dry_out . send_level, 16383, "direct_output");
		local_dry_out . dry_wet . drop_dry (tc, "direct_output_dry_wet_sens");
		local_dry_out . vector_x . drop (tc, 0, "direct_output_vector_x_sens");
		local_dry_out . vector_y . drop (tc, 0, "direct_output_vector_y_sens");
	}
	int sentinel = global ? hercules_global_dsp_number : hercules_local_dsp_number;
	int index;
	for (ind = 0; ind < sentinel; ind++) {
		index = global ? hercules_local_dsp_number + ind : ind;
		dspp = dsp [index];
		tc -> writeln_id ("dsp");
		tc -> writeln_id ("dsp", ind);
		tc -> write_id ("algo");
		tc -> write (dspp -> algo_msb);
		tc -> write (dspp -> algo_program);
		tc -> writeln_earth ();
		if (ind != 0) drop_controller (tc, dspp -> on, false, "dsp_on");
		else drop_controller (tc, dspp -> on, true, "dsp_off");
		drop_controller (tc, dsp_send [index] . send_level, ind == 0 ? 16383 : 0, "direct_input");
		dsp_send [index] . dry_wet . drop (tc, 0, "direct_input_dry_wet_sens");
		dsp_send [index] . vector_x . drop (tc, 0, "direct_input_vector_x_sens");
		dsp_send [index] . vector_y . drop (tc, 0, "direct_input_vector_y_sens");
		drop_controller (tc, dspp -> wet_output . send_level, 16383, "direct_output");
		dspp -> wet_output . dry_wet . drop_wet (tc, "direct_output_dry_wet_sens");
		dspp -> wet_output . vector_x . drop (tc, 0, "direct_output_vector_x_sens");
		dspp -> wet_output . vector_y . drop (tc, 0, "direct_output_vector_y_sens");
		for (sub = 0; sub < dspp -> router_number; sub++) {
			dsprp = dspp -> routers [sub];
			id = ind + sub + 1;
			drop_controller (tc, id, dsprp -> send_level, 0, "send_to");
			dsprp -> dry_wet . drop_indexed (tc, id, "send_to_dry_wet_sens");
			dsprp -> vector_x . drop_indexed (tc, id, "send_to_vector_x_sens");
			dsprp -> vector_y . drop_indexed (tc, id, "send_to_vector_y_sens");
		}
		for (sub = 0; sub < 16; sub++)
			drop_controller (tc, sub, dspp -> parameters [sub], dspp -> algo_head -> get_default (sub), "parameter");
		tc -> writeln_earth ();
	}
	tc -> writeln_earth ();
}

//void parameter_block :: store_dsp_program (SetupFileWriter * tc, int dsp_index) {
//	dsp_parameter_block_pointer dspp = dsp [dsp_index];
//	tc -> writeln_id ("dsp");
//	tc -> write_id ("algo");
//	tc -> write (dspp -> algo_msb);
//	tc -> write (dspp -> algo_program);
//	tc -> writeln_earth ();
//	for (int ind = 0; ind < 16; ind++)
//		drop_controller (tc, ind, dspp -> parameters [ind], dspp -> algo_head -> get_default (ind), "parameter");
//	tc -> writeln_earth ();
//}

void drop_controller (SetupFileWriter * tc, char * mnemonic, int ind, int check, int control) {
	if (control > 127) control -= 4;
	if (control == check) return;
	tc -> writeln_id (mnemonic, ind, control);
}

void synthesiser :: sub_store_control (SetupFileWriter * tc) {
	int ind;
	int device;
	int midi_channel;
	int midi_channel_extension;
	int arranger_assign;
	int reserved_notes;
	system_parameter_block * ctrl;
	for (ind = 0; ind < multitimbral; ind++) {
		device = data_blocks [ind] -> system -> device_id;
		if (device != ind) tc -> writeln_id ("device", ind, device);
	}
	for (ind = 0; ind < multitimbral; ind++) {
		midi_channel_extension = data_blocks [ind] -> system -> midi_channel_extension;
		midi_channel = data_blocks [ind] -> system -> midi_channel;
		if (midi_channel_extension != 0x7f) tc -> writeln_id ("channel_extension", ind, midi_channel_extension);
		if (midi_channel != (ind & 0xf)) tc -> writeln_id ("channel", ind, midi_channel);
	}
	for (ind = 0; ind < multitimbral; ind++) {
		arranger_assign = data_blocks [ind] -> system -> arranger_assign;
		if (arranger_assign != 0x7f) tc -> writeln_id ("arranger_assign", ind, arranger_assign);
	}
	for (ind = 0; ind < multitimbral; ind++) {
		reserved_notes = data_blocks [ind] -> system -> reserved_notes;
		if (reserved_notes != 0x7f) tc -> writeln_id ("reserved_notes", ind, reserved_notes);
	}
	tc -> writeln_id ("controls");
	for (ind = 0; ind < multitimbral; ind++) {
		ctrl = data_blocks [ind] -> system;
		drop_controller (tc, "pitch", ind, 124, ctrl -> pitch);
		drop_controller (tc, "freq", ind, 74, ctrl -> freq);
		drop_controller (tc, "amp", ind, 11, ctrl -> amp);
		drop_controller (tc, "index", ind, 71, ctrl -> index);
		drop_controller (tc, "attack", ind, 73, ctrl -> attack);
		drop_controller (tc, "decay", ind, 76, ctrl -> decay);
		drop_controller (tc, "sustain", ind, 75, ctrl -> sustain);
		drop_controller (tc, "release", ind, 72, ctrl -> release);
		drop_controller (tc, "speed", ind, 77, ctrl -> speed);
		drop_controller (tc, "vibrato", ind, 1, ctrl -> vibrato);
		drop_controller (tc, "tremolo", ind, 92, ctrl -> tremolo);
		drop_controller (tc, "lfo_index", ind, 95, ctrl -> lfo_index);
		drop_controller (tc, "lfo_pan", ind, 78, ctrl -> lfo_pan);
		drop_controller (tc, "vector_x", ind, 16, ctrl -> vector_x);
		drop_controller (tc, "vector_y", ind, 17, ctrl -> vector_y);
		drop_controller (tc, "volume", ind, 7, ctrl -> volume);
		drop_controller (tc, "pan", ind, 10, ctrl -> pan);
		drop_controller (tc, "hold", ind, 64, ctrl -> hold);
		drop_controller (tc, "porta", ind, 65, ctrl -> porta);
		drop_controller (tc, "porta_time", ind, 5, ctrl -> porta_time);
		drop_controller (tc, "porta_mode", ind, 84, ctrl -> porta_mode);
		drop_controller (tc, "dry_wet", ind, 91, ctrl -> dry_wet);
		drop_controller (tc, "global_volume", ind, 79, ctrl -> global_volume);
	}
	tc -> writeln_earth ();
	/*if (global_volume_present) {
		tc -> writeln_id ("volume");
		if (global_volume_multi) {
			for (ind = 0; ind < multitimbral; ind++) {
				drop_controller (tc, ind, data_blocks [ind] -> controllers -> global_volume, 16383, "volume");
			}
		} else {
			drop_controller (tc, global_volume, 16383, "global_volume");
		}
		tc -> writeln_earth ();
	}*/
	if (tune != 0) tc -> writeln_id ("tune", tune);
}

void synthesiser :: store_control (SetupFileWriter * tc) {
	tc -> writeln_id ("setup");
	sub_store_control (tc);
	tc -> writeln_earth ();
}

void synthesiser :: store_setup (SetupFileWriter * tc) {
	tc -> writeln_id ("setup");
	sub_store_control (tc);
	tc -> writeln_id ("categories");
	int ind;
	char * tp;
	for (ind = 0; ind < category_pointer; ind++) tc -> writeln (get_category (ind));
	tc -> writeln_earth ();
	for (ind = 32; ind < 128; ind++) {
		tp = get_message (ind);
		if (tp [0] != '\0') {
			tc -> write_id ("message");
			tc -> write (ind);
			tc -> write (tp);
			tc -> writeln_earth ();
		}
	}
	tc -> writeln_id ("banks");
	bank * bp;
	for (ind = 0; ind < 128; ind++) {
		bp = banks [ind];
		if (bp != NULL) {
			switch (bp -> type) {
			case 0: tc -> write_id ("preset_algo"); break;
			case 1: tc -> write_id ("preset_patter"); break;
			case 2: tc -> write_id ("preset_dsp_algo"); break;
			case 32: tc -> write_id ("rom_player"); break;
			case 33: tc -> write_id ("wavetable"); break;
			case 34: tc -> write_id ("pattern"); break;
			case 35: tc -> write_id ("style"); break;
			case 36: tc -> write_id ("sound"); break;
			case 37: tc -> write_id ("user_sound"); break;
			case 38: tc -> write_id ("dsp"); break;
			case 39: tc -> write_id ("user_dsp"); break;
			default: tc -> write_id ("plugin"); break;
			}
			tc -> write (ind);
			if (bp -> type != 0 && bp -> type != 1 && bp -> type != 2) tc -> write (bp -> config_string);
			if (bp -> type == 64) tc -> write (bp -> plugin_parameter_string);
			tc -> writeln_earth ();
		}
	}
	tc -> writeln_earth ();
	tc -> writeln_earth ();
}

bool synthesiser :: store_setup (void) {
	if (no_setup_store) return false;
	char path [256];
	sprintf (path, "%s%s", root_directory, setup_file_name);
	SetupFileWriter * tc = new SetupFileWriter (path);
	store_setup (tc);
	delete tc;
	return true;
}

bool synthesiser :: store_control (void) {
	if (no_control_store) return false;
	char path [256];
	sprintf (path, "%s%s", root_directory, control_file_name);
	SetupFileWriter * tc = new SetupFileWriter (path);
	store_control (tc);
	delete tc;
	return true;
}

void synthesiser :: store_setup (char * setup) {
	char path [256];
	sprintf (path, "%s%s", root_directory, setup);
	SetupFileWriter * tc = new SetupFileWriter (path);
	store_setup (tc);
	delete tc;
}

void synthesiser :: store_control (char * control) {
	char path [256];
	sprintf (path, "%s%s", root_directory, control);
	SetupFileWriter * tc = new SetupFileWriter (path);
	store_control (tc);
	delete tc;
}

void synthesiser :: store_configuration (void) {
	if (store_control ()) return;
	store_setup ();
}

