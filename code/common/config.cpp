///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include <string.h>
#include "config.h"

config :: config (void) {
	strcpy (config_location, "config.txt");
	SetupFileReader * fr = new SetupFileReader (config_location);
	read_config (fr);
	delete fr;
}

config :: config (char * file_name) {
	strcpy (config_location, file_name);
	SetupFileReader * fr = new SetupFileReader (config_location);
	read_config (fr);
	delete fr;
}

bool config :: config_not_found (void) {return strlen (config_location) == 0;}

void config :: read_config (SetupFileReader * fr) {
	bool skipper;
	multitimbral = CONFIG_MULTITIMBRAL;
	stripes = CONFIG_STRIPES;
	stereo = CONFIG_STEREO;
	local_dsp = CONFIG_LOCAL_DSP;
	global_dsp = CONFIG_GLOBAL_DSP;
	oscillators = CONFIG_OSCILLATORS;
	processors = CONFIG_PROCESSORS;
	key_stream = CONFIG_KEY_STREAM;
	arranger_keys = CONFIG_ARRANGER_KEYS;
	arranger_horizontal = CONFIG_ARRANGER_HORIZONTAL;
	vector_size = CONFIG_VECTOR_SIZE;
	vector_horizontal = CONFIG_VECTOR_HORIZONTAL;
	center_freq = CONFIG_CENTER_FREQ;
	horizontal = CONFIG_HORIZONTAL;
	vertical = CONFIG_VERTICAL;
	head_room = CONFIG_HEAD_ROOM;
	feed_space = CONFIG_FEED_SPACE;
	dsp_line_space = CONFIG_DSP_LINE_SPACE;
	dsp_variable_space = CONFIG_DSP_VARIABLE_SPACE;
	style_program_size = CONFIG_STYLE_PROGRAM_SIZE;
	prolog_horizontal = CONFIG_PROLOG_HORIZONTAL;
	prolog_console_horizontal = CONFIG_PROLOG_CONSOLE_HORIZONTAL;
	strcpy (prolog_library_load, CONFIG_PROLOG_LIBRARY_LOAD);
	midi_in = CONFIG_MIDI_IN;
	midi_out = CONFIG_MIDI_OUT;
	console_midi_in = CONFIG_CONSOLE_MIDI_IN;
	console_midi_out = CONFIG_CONSOLE_MIDI_OUT;
	strcpy (root_directory, CONFIG_ROOT_DIRECTORY);
	strcpy (prolog_root_directory, CONFIG_PROLOG_ROOT_DIRECTORY);
	strcpy (serial_number, "");
	strcpy (key, "");
	sampling_freq = CONFIG_SAMPLING_FREQ;
	linear_pan = CONFIG_LINEAR_PAN;
	volume_reset = CONFIG_VOLUME_RESET;
	global_volume_present = CONFIG_GLOBAL_VOLUME_PRESENT;
	global_volume_multi = CONFIG_GLOBAL_VOLUME_MULTI;
	global_dsp_multi = CONFIG_GLOBAL_DSP_MULTI;
	vector_auto_return = CONFIG_VECTOR_AUTO_RETURN;
	default_algo_msb = CONFIG_DEFAULT_ALGO_MSB;
	default_algo_lsb = CONFIG_DEFAULT_ALGO_LSB;
	default_preset_sound_msb = CONFIG_DEFAULT_PRESET_SOUND_MSB;
	default_preset_sound_lsb = CONFIG_DEFAULT_PRESET_SOUND_LSB;
	default_user_sound_msb = CONFIG_DEFAULT_USER_SOUND_MSB;
	default_user_sound_lsb = CONFIG_DEFAULT_USER_SOUND_LSB;
	default_pattern_msb = CONFIG_DEFAULT_PATTERN_MSB;
	default_pattern_lsb = CONFIG_DEFAULT_PATTERN_LSB;
	default_style_msb = CONFIG_DEFAULT_STYLE_MSB;
	default_style_lsb = CONFIG_DEFAULT_STYLE_LSB;
	default_dsp_algo_msb = CONFIG_DEFAULT_DSP_ALGO_MSB;
	default_dsp_algo_lsb = CONFIG_DEFAULT_DSP_ALGO_LSB;
	default_preset_dsp_msb = CONFIG_DEFAULT_PRESET_DSP_MSB;
	default_preset_dsp_lsb = CONFIG_DEFAULT_PRESET_DSP_LSB;
	default_user_dsp_msb = CONFIG_DEFAULT_USER_DSP_MSB;
	default_user_dsp_lsb = CONFIG_DEFAULT_USER_DSP_LSB;
	latency_block = CONFIG_LATENCY_BLOCK;
	oscilloscope_horizontal = CONFIG_OSCILLOSCOPE_HORIZONTAL;
	oscilloscope_resolution_shift = CONFIG_OSCILLOSCOPE_RESOLUTION_SHIFT;
	oscilloscope_horizontal_shift = CONFIG_OSCILLOSCOPE_HORIZONTAL_SHIFT;
	dco_blocking_freq = CONFIG_DCO_BLOCKING_FREQ;
	layout_small = CONFIG_LAYOUT_SMALL;
	modulation_wheel_id = CONFIG_MODULATION_WHEEL_ID;
	external_store_protect_level = CONFIG_EXTERNAL_STORE_PROTECT_LEVEL;
	vst_controllers_redraw = CONFIG_VST_CONTROLLERS_REDRAW;
	vst_record_midi = CONFIG_VST_RECORD_MIDI;
	vst_automation = CONFIG_VST_AUTOMATION;
	if (! fr -> get_id ("configuration")) {
		strcpy (config_location, "");
		return;
	}
	while (fr -> get_id ()) {
		skipper = true;
		if (fr -> id ("multitimbral")) {
			if (! fr -> get_int ()) return;
			multitimbral = fr -> int_symbol;
		}
		if (fr -> id ("stripes")) {
			if (! fr -> get_int ()) return;
			stripes = fr -> int_symbol;
		}
		if (fr -> id ("stereo")) {
			if (! fr -> get_int ()) return;
			stereo = fr -> int_symbol;
		}
		if (fr -> id ("dsp")) {
			if (! fr -> get_int ()) return;
			local_dsp = fr -> int_symbol;
			if (fr -> get_int ()) global_dsp = fr -> int_symbol;
			else {
				if (fr -> symbol_control == 2) skipper = false;
				else return;
			}
		}
		if (fr -> id ("oscillators")) {
			if (! fr -> get_int ()) return;
			oscillators = fr -> int_symbol;
		}
		if (fr -> id ("processors")) {
			if (! fr -> get_int ()) return;
			processors = fr -> int_symbol;
		}
		if (fr -> id ("key_stream")) {
			if (! fr -> get_int ()) return;
			key_stream = fr -> int_symbol;
		}
		if (fr -> id ("arranger_keys")) {
			if (! fr -> get_int ()) return;
			arranger_keys = fr -> int_symbol;
		}
		if (fr -> id ("arranger_horizontal")) {
			if (! fr -> get_int ()) return;
			arranger_horizontal = fr -> int_symbol;
		}
		if (fr -> id ("vector_size")) {
			if (! fr -> get_int ()) return;
			vector_size = fr -> int_symbol;
		}
		if (fr -> id ("vector_horizontal")) {
			if (! fr -> get_int ()) return;
			vector_horizontal = fr -> int_symbol;
		}
		if (fr -> id ("center_freq")) {
			if (! fr -> get_int ()) return;
			center_freq = fr -> int_symbol;
		}
		if (fr -> id ("horizontal")) {
			if (! fr -> get_int ()) return;
			horizontal = fr -> int_symbol;
		}
		if (fr -> id ("vertical")) {
			if (! fr -> get_int ()) return;
			vertical = fr -> int_symbol;
		}
		if (fr -> id ("head_room")) {
			if (! fr -> get_int ()) return;
			head_room = fr -> int_symbol;
		}
		if (fr -> id ("feed_space")) {
			if (! fr -> get_int ()) return;
			feed_space = fr -> int_symbol;
		}
		if (fr -> id ("dsp_line_space")) {
			if (! fr -> get_int ()) return;
			dsp_line_space = fr -> int_symbol;
		}
		if (fr -> id ("dsp_variable_space")) {
			if (! fr -> get_int ()) return;
			dsp_variable_space = fr -> int_symbol;
		}
		if (fr -> id ("style_program_size")) {
			if (! fr -> get_int ()) return;
			style_program_size = fr -> int_symbol;
		}
		if (fr -> id ("prolog_horizontal")) {
			if (! fr -> get_int ()) return;
			prolog_horizontal = fr -> int_symbol;
		}
		if (fr -> id ("prolog_console_horizontal")) {
			if (! fr -> get_int ()) return;
			prolog_console_horizontal = fr -> int_symbol;
		}
		if (fr -> id ("prolog_library_load")) {
			if (! fr -> get_string ()) return;
			strcpy (prolog_library_load, fr -> symbol);
		}
		if (fr -> id ("midi_in")) {
			if (! fr -> get_int ()) return;
			midi_in = fr -> int_symbol;
		}
		if (fr -> id ("midi_out")) {
			if (! fr -> get_int ()) return;
			midi_out = fr -> int_symbol;
		}
		if (fr -> id ("console_midi_in")) {
			if (! fr -> get_int ()) return;
			console_midi_in = fr -> int_symbol;
		}
		if (fr -> id ("console_midi_out")) {
			if (! fr -> get_int ()) return;
			console_midi_out = fr -> int_symbol;
		}
		if (fr -> id ("root_directory")) {
			if (! fr -> get_string ()) return;
			strcpy (root_directory, fr -> symbol);
		}
		if (fr -> id ("prolog_root_directory")) {
			if (! fr -> get_string ()) return;
			strcpy (prolog_root_directory, fr -> symbol);
		}
		if (fr -> id ("serial_number")) {
			if (! fr -> get_string ()) return;
			strcpy (serial_number, fr -> symbol);
		}
		if (fr -> id ("key")) {
			if (! fr -> get_string ()) return;
			strcpy (key, fr -> symbol);
		}
		if (fr -> id ("sampling_freq")) {
			if (! fr -> get_int ()) return;
			sampling_freq = fr -> int_symbol;
		}
		if (fr -> id ("linear_pan")) {linear_pan = true;}
		if (fr -> id ("power_pan")) {linear_pan = false;}
		if (fr -> id ("volume_reset")) {volume_reset = true;}
		if (fr -> id ("no_volume_reset")) {volume_reset = false;}
		if (fr -> id ("global_volume_present")) {global_volume_present = true;}
		if (fr -> id ("no_global_volume")) {global_volume_present = false;}
		if (fr -> id ("global_volume_multi")) {global_volume_multi = true;}
		if (fr -> id ("global_dsp_multi")) {global_dsp_multi = true;}
		if (fr -> id ("vector_auto_return")) {vector_auto_return = true;}
		if (fr -> id ("default_algo_bank")) {
			if (! fr -> get_int ()) return;
			default_algo_msb = fr -> int_symbol;
			if (fr -> get_int ()) default_algo_lsb = fr -> int_symbol;
			else {
				if (fr -> symbol_control == 2) skipper = false;
				else return;
			}
		}
		if (fr -> id ("default_preset_sound_bank")) {
			if (! fr -> get_int ()) return;
			default_preset_sound_msb = fr -> int_symbol;
			if (fr -> get_int ()) default_preset_sound_lsb = fr -> int_symbol;
			else {
				if (fr -> symbol_control == 2) skipper = false;
				else return;
			}
		}
		if (fr -> id ("default_user_sound_bank")) {
			if (! fr -> get_int ()) return;
			default_user_sound_msb = fr -> int_symbol;
			if (fr -> get_int ()) default_user_sound_lsb = fr -> int_symbol;
			else {
				if (fr -> symbol_control == 2) skipper = false;
				else return;
			}
		}
		if (fr -> id ("default_pattern_bank")) {
			if (! fr -> get_int ()) return;
			default_pattern_msb = fr -> int_symbol;
			if (fr -> get_int ()) default_pattern_lsb = fr -> int_symbol;
			else {
				if (fr -> symbol_control == 2) skipper = false;
				else return;
			}
		}
		if (fr -> id ("default_style_bank")) {
			if (! fr -> get_int ()) return;
			default_style_msb = fr -> int_symbol;
			if (fr -> get_int ()) default_style_lsb = fr -> int_symbol;
			else {
				if (fr -> symbol_control == 2) skipper = false;
				else return;
			}
		}
		if (fr -> id ("default_dsp_algo_bank")) {
			if (! fr -> get_int ()) return;
			default_dsp_algo_msb = fr -> int_symbol;
			if (fr -> get_int ()) default_dsp_algo_lsb = fr -> int_symbol;
			else {
				if (fr -> symbol_control == 2) skipper = false;
				else return;
			}
		}
		if (fr -> id ("default_preset_dsp_bank")) {
			if (! fr -> get_int ()) return;
			default_preset_dsp_msb = fr -> int_symbol;
			if (fr -> get_int ()) default_preset_dsp_lsb = fr -> int_symbol;
			else {
				if (fr -> symbol_control == 2) skipper = false;
				else return;
			}
		}
		if (fr -> id ("default_user_dsp_bank")) {
			if (! fr -> get_int ()) return;
			default_user_dsp_msb = fr -> int_symbol;
			if (fr -> get_int ()) default_user_dsp_lsb = fr -> int_symbol;
			else {
				if (fr -> symbol_control == 2) skipper = false;
				else return;
			}
		}
		if (fr -> id ("latency_block")) {
			if (! fr -> get_int ()) return;
			latency_block = fr -> int_symbol;
		}
		if (fr -> id ("oscilloscope_horizontal")) {
			if (! fr -> get_int ()) return;
			oscilloscope_horizontal = fr -> int_symbol;
		}
		if (fr -> id ("oscilloscope_resolution_shift")) {
			if (! fr -> get_int ()) return;
			oscilloscope_resolution_shift = fr -> int_symbol;
		}
		if (fr -> id ("oscilloscope_horizontal_shift")) {
			if (! fr -> get_int ()) return;
			oscilloscope_horizontal_shift = fr -> int_symbol;
		}
		if (fr -> id ("dco_blocking_freq")) {
			if (! fr -> get_int ()) return;
			dco_blocking_freq = fr -> int_symbol;
		}
		if (fr -> id ("layout_small")) {layout_small = true;}
		if (fr -> id ("modulation_wheel_id")) {
			if (! fr -> get_int ()) return;
			modulation_wheel_id = fr -> int_symbol;
		}
		if (fr -> id ("external_store_protected")) {external_store_protect_level = 0;}
		if (fr -> id ("external_store_automatic")) {external_store_protect_level = 1;}
		if (fr -> id ("external_store_allowed")) {external_store_protect_level = 2;}
		if (fr -> id ("vst_controllers_redraw")) {vst_controllers_redraw = true;}
		if (fr -> id ("vst_record_midi")) {vst_record_midi = true;}
		if (fr -> id ("vst_no_automation")) {vst_automation = false;}
		if (skipper) fr -> skip ();
	}
	correct ();
	fr -> close ();
}

void config :: store (void) {
	correct ();
	SetupFileWriter * fw = new SetupFileWriter (config_location);
	store_config (fw);
	delete fw;
}

void config :: store_config (SetupFileWriter * fw) {
	fw -> writeln_id ("configuration");
	if (multitimbral != CONFIG_MULTITIMBRAL) fw -> writeln_id ("multitimbral", multitimbral);
	if (stripes != CONFIG_STRIPES) fw -> writeln_id ("stripes", stripes);
	if (stereo != CONFIG_STEREO) fw -> writeln_id ("stereo", stereo);
	if (local_dsp != CONFIG_LOCAL_DSP || global_dsp != CONFIG_GLOBAL_DSP) {
		fw -> write_id ("dsp");
		fw -> write (local_dsp);
		if (global_dsp != CONFIG_GLOBAL_DSP) fw -> write (global_dsp);
		fw -> writeln_earth ();
	}
	if (oscillators != CONFIG_OSCILLATORS) fw -> writeln_id ("oscillators", oscillators);
	if (processors != CONFIG_PROCESSORS) fw -> writeln_id ("processors", processors);
	if (key_stream != CONFIG_KEY_STREAM) fw -> writeln_id ("key_stream", key_stream);
	if (arranger_keys != CONFIG_ARRANGER_KEYS) fw -> writeln_id ("arranger_keys", arranger_keys);
	if (arranger_horizontal != CONFIG_ARRANGER_HORIZONTAL) fw -> writeln_id ("arranger_horizontal", arranger_horizontal);
	if (vector_size != CONFIG_VECTOR_SIZE) {fw -> writeln_id ("vector_size", vector_size);}
	if (vector_horizontal != CONFIG_VECTOR_HORIZONTAL) {fw -> writeln_id ("vector_horizontal", vector_horizontal);}
	if (center_freq != CONFIG_CENTER_FREQ) fw -> writeln_id ("center_freq", center_freq);
	if (horizontal != CONFIG_HORIZONTAL) fw -> writeln_id ("horizontal", horizontal);
	if (vertical != CONFIG_VERTICAL) fw -> writeln_id ("vertical", vertical);
	if (head_room != CONFIG_HEAD_ROOM) fw -> writeln_id ("head_room", head_room);
	if (feed_space != CONFIG_FEED_SPACE) fw -> writeln_id ("feed_space", feed_space);
	if (dsp_line_space != CONFIG_DSP_LINE_SPACE) fw -> writeln_id ("dsp_line_space", dsp_line_space);
	if (dsp_variable_space != CONFIG_DSP_VARIABLE_SPACE) fw -> writeln_id ("dsp_variable_space", dsp_variable_space);
	if (style_program_size != CONFIG_STYLE_PROGRAM_SIZE) fw -> writeln_id ("style_program_size", style_program_size);
	if (prolog_horizontal != CONFIG_PROLOG_HORIZONTAL) fw -> writeln_id ("prolog_horizontal", prolog_horizontal);
	if (prolog_console_horizontal != CONFIG_PROLOG_CONSOLE_HORIZONTAL) fw -> writeln_id ("prolog_console_horizontal", prolog_console_horizontal);
	if (strcmp (prolog_library_load, CONFIG_PROLOG_LIBRARY_LOAD) != 0) {
		fw -> write_id ("prolog_library_load");
		fw -> write (prolog_library_load);
		fw -> writeln_earth ();
	}
	if (midi_in != CONFIG_MIDI_IN) fw -> writeln_id ("midi_in", midi_in);
	if (midi_out != CONFIG_MIDI_OUT) fw -> writeln_id ("midi_out", midi_out);
	if (console_midi_in != CONFIG_CONSOLE_MIDI_IN) fw -> writeln_id ("console_midi_in", midi_in);
	if (console_midi_out != CONFIG_CONSOLE_MIDI_OUT) fw -> writeln_id ("console_midi_out", midi_out);
	if (strlen (root_directory) > 0) {
		fw -> write_id ("root_directory");
		fw -> write (root_directory);
		fw -> writeln_earth ();
	}
	if (strcmp (prolog_root_directory, CONFIG_PROLOG_ROOT_DIRECTORY) != 0) {
		fw -> write_id ("prolog_root_directory");
		fw -> write (prolog_root_directory);
		fw -> writeln_earth ();
	}
	if (strlen (serial_number) > 0) {
		fw -> write_id ("serial_number");
		fw -> write (serial_number);
		fw -> writeln_earth ();
	}
	if (strlen (key) > 0) {
		fw -> write_id ("key");
		fw -> write (key);
		fw -> writeln_earth ();
	}
	if (sampling_freq != CONFIG_SAMPLING_FREQ) fw -> writeln_id ("sampling_freq", sampling_freq);
	if (linear_pan != CONFIG_LINEAR_PAN) {fw -> write_id ("power_pan"); fw -> writeln_earth ();}
	if (volume_reset != CONFIG_VOLUME_RESET) {fw -> write_id ("no_volume_reset"); fw -> writeln_earth ();}
	if (global_volume_present != CONFIG_GLOBAL_VOLUME_PRESENT) {fw -> write_id ("no_global_volume"); fw -> writeln_earth ();}
	if (global_volume_multi != CONFIG_GLOBAL_VOLUME_MULTI) {fw -> write_id ("global_volume_multi"); fw -> writeln_earth ();}
	if (global_dsp_multi != CONFIG_GLOBAL_DSP_MULTI) {fw -> write_id ("global_dsp_multi"); fw -> writeln_earth ();}
	if (vector_auto_return != CONFIG_VECTOR_AUTO_RETURN) {fw -> write_id ("vector_auto_return"); fw -> writeln_earth ();}
	if (default_algo_msb != CONFIG_DEFAULT_ALGO_MSB || default_algo_lsb != CONFIG_DEFAULT_ALGO_LSB) {
		fw -> write_id ("default_algo_bank");
		fw -> write (default_algo_msb);
		if (default_algo_lsb != CONFIG_DEFAULT_ALGO_LSB) fw -> write (default_algo_lsb);
		fw -> writeln_earth ();
	}
	if (default_preset_sound_msb != CONFIG_DEFAULT_PRESET_SOUND_MSB || default_preset_sound_lsb != CONFIG_DEFAULT_PRESET_SOUND_LSB) {
		fw -> write_id ("default_preset_sound_bank");
		fw -> write (default_preset_sound_msb);

		if (default_preset_sound_lsb != CONFIG_DEFAULT_PRESET_SOUND_LSB) fw -> write (default_preset_sound_lsb);
		fw -> writeln_earth ();
	}
	if (default_user_sound_msb != CONFIG_DEFAULT_USER_SOUND_MSB || default_user_sound_lsb != CONFIG_DEFAULT_USER_SOUND_LSB) {
		fw -> write_id ("default_user_sound_bank");
		fw -> write (default_user_sound_msb);
		if (default_user_sound_lsb != CONFIG_DEFAULT_USER_SOUND_LSB) fw -> write (default_user_sound_lsb);
		fw -> writeln_earth ();
	}
	if (default_pattern_msb != CONFIG_DEFAULT_PATTERN_MSB || default_pattern_lsb != CONFIG_DEFAULT_PATTERN_LSB) {
		fw -> write_id ("default_pattern_bank");
		fw -> write (default_pattern_msb);
		if (default_pattern_lsb != CONFIG_DEFAULT_PATTERN_LSB) fw -> write (default_pattern_lsb);
		fw -> writeln_earth ();
	}
	if (default_style_msb != CONFIG_DEFAULT_STYLE_MSB || default_style_lsb != CONFIG_DEFAULT_STYLE_LSB) {
		fw -> write_id ("default_style_bank");
		fw -> write (default_style_msb);
		if (default_style_lsb != CONFIG_DEFAULT_STYLE_LSB) fw -> write (default_style_lsb);
		fw -> writeln_earth ();
	}
	if (default_dsp_algo_msb != CONFIG_DEFAULT_DSP_ALGO_MSB || default_dsp_algo_lsb != CONFIG_DEFAULT_DSP_ALGO_LSB) {
		fw -> write_id ("default_dsp_algo_bank");
		fw -> write (default_dsp_algo_msb);
		if (default_dsp_algo_lsb != CONFIG_DEFAULT_DSP_ALGO_LSB) fw -> write (default_dsp_algo_lsb);
		fw -> writeln_earth ();
	}
	if (default_preset_dsp_msb != CONFIG_DEFAULT_PRESET_DSP_MSB || default_preset_dsp_lsb != CONFIG_DEFAULT_PRESET_DSP_LSB) {
		fw -> write_id ("default_preset_dsp_bank");
		fw -> write (default_preset_dsp_msb);
		if (default_preset_dsp_lsb != CONFIG_DEFAULT_PRESET_DSP_LSB) fw -> write (default_preset_dsp_lsb);
		fw -> writeln_earth ();
	}
	if (default_user_dsp_msb != CONFIG_DEFAULT_USER_DSP_MSB || default_user_dsp_lsb != CONFIG_DEFAULT_USER_DSP_LSB) {
		fw -> write_id ("default_user_dsp_bank");
		fw -> write (default_user_dsp_msb);
		if (default_user_dsp_lsb != CONFIG_DEFAULT_USER_DSP_LSB) fw -> write (default_user_dsp_lsb);
		fw -> writeln_earth ();
	}
	if (latency_block != CONFIG_LATENCY_BLOCK) {fw -> writeln_id ("latency_block", latency_block);}
	if (oscilloscope_horizontal != CONFIG_OSCILLOSCOPE_HORIZONTAL) {fw -> writeln_id ("oscilloscope_horizontal", oscilloscope_horizontal);}
	if (oscilloscope_resolution_shift != CONFIG_OSCILLOSCOPE_RESOLUTION_SHIFT) {fw -> writeln_id ("oscilloscope_resolution_shift", oscilloscope_resolution_shift);}
	if (oscilloscope_horizontal_shift != CONFIG_OSCILLOSCOPE_HORIZONTAL_SHIFT) {fw -> writeln_id ("oscilloscope_horizontal_shift", oscilloscope_horizontal_shift);}
	if (dco_blocking_freq != CONFIG_DCO_BLOCKING_FREQ) {fw -> writeln_id ("dco_blocking_freq", dco_blocking_freq);}
	if (layout_small != CONFIG_LAYOUT_SMALL) {fw -> write_id ("layout_small"); fw -> writeln_earth ();}
	if (modulation_wheel_id != CONFIG_MODULATION_WHEEL_ID) {fw -> writeln_id ("modulation_wheel_id", modulation_wheel_id);}
	if (external_store_protect_level == 0) {fw -> write_id ("external_store_protected"); fw -> writeln_earth ();}
	if (external_store_protect_level == 2) {fw -> write_id ("external_store_allowed"); fw -> writeln_earth ();}
	if (vst_controllers_redraw != CONFIG_VST_CONTROLLERS_REDRAW) {fw -> write_id ("vst_controllers_redraw"); fw -> writeln_earth ();}
	if (vst_record_midi != CONFIG_VST_RECORD_MIDI) {fw -> write_id ("vst_record_midi"); fw -> writeln_earth ();}
	if (vst_automation != CONFIG_VST_AUTOMATION) {fw -> write_id ("vst_no_automation"); fw -> writeln_earth ();}
	fw -> write_earth ();
}

void config :: correct (void) {
	while (local_dsp > 8) local_dsp--;
	while (local_dsp + global_dsp > 8) global_dsp--;
	if (! global_dsp_multi) global_volume_multi = false;
	if (dsp_line_space < (horizontal << 1)) dsp_line_space = horizontal << 1;
}

bool config :: correct_horizontal (void) {
	if (horizontal < 1) horizontal = 1;
	if (sampling_freq < 1) sampling_freq = 1;
	int oversampling = horizontal / sampling_freq;
	if (oversampling < 1) oversampling = 1;
	if (oversampling * sampling_freq == horizontal) return false;
	horizontal = oversampling * sampling_freq;
	return true;
}
