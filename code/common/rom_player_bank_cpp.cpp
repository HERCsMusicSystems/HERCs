///////////////////////////////////////
// Copyright (C) 2004 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc_cpp.h"
#include "string.h"
#include "math.h"
#include "algo_building_blocks.h"

rom_wave * rom_wave :: find (char * name) {
	if (strcmp (wave_name, name) == 0) return this;
	rom_wave * wp = next;
	while (wp != NULL) {
		if (strcmp (wp -> wave_name, name) == 0) return wp;
		wp = wp -> next;
	}
	return NULL;
}

unsigned long int read4 (FILE * fr) {
	unsigned long int ret = fgetc (fr);
	ret += fgetc (fr) << 8;
	ret += fgetc (fr) << 16;
	ret += fgetc (fr) << 24;
	return ret;
}

float read_float (FILE * fr) {
	float ret = 0.0;
	unsigned long int * ip = (unsigned long int *) & ret;
	* ip = read4 (fr);
	return ret;
}

unsigned long int read2 (FILE * fr) {
	unsigned long int ret = fgetc (fr);
	ret += fgetc (fr) << 8;
	return ret;
}

void rom_wave :: calculate_ratio (int bits, int center_freq, int key, int horizontal, int vertical) {
	bit_shift = bits;
	bit_value = 1 << bit_shift;
	bit_mask = bit_value - 1;
	ratio = (float) (key - 8192);
	ratio /= 1536.0f;
	ratio = (float) pow (2.0f, ratio);
	ratio *= 4294967296.0f / (float) bit_value;
	ratio *= (float) center_freq;
	ratio /= (float) horizontal;
	mega_vertical = (float) vertical * (float) bit_value;
}

rom_wave :: rom_wave (char * name, rom_wave * next, int center_freq, int key, int horizontal, int vertical, int head_room) {
	string_copy (wave_name, name);
	data_size = 0;
	data = NULL;
	// ratio
	calculate_ratio (18, center_freq, key, horizontal, vertical);
	tail_present = false;
	this -> next = next;

	SetupFileReader * fr = new SetupFileReader (name);
	if (fr -> file_not_found ()) {
		delete fr;
		char command [256];
		sprintf (command, "File not found [%s]", name);
		message (command);
		return;
	}
	read_scripted_waveform (fr, center_freq, key, vertical, head_room);
	delete fr;
}

#define RETURN {data_size = 0; if (data) delete data; data = NULL; return;}
void rom_wave :: read_scripted_waveform (SetupFileReader * fr, int center_freq, int key, int vertical, int head_room) {
	if (! fr -> get_id ("wave")) RETURN
	int address = 0;
	bool skip;
	int loop_start = 0;
	loop_end = 2147483647;
	float vertical_ratio = 1.0f;
	while (fr -> get_id ()) {
		skip = true;
		if (fr -> id ("horizontal")) {
			if (! fr -> get_int ()) RETURN
			if (fr -> int_symbol < 1) RETURN
			calculate_ratio (18, center_freq, key, center_freq * fr -> int_symbol, vertical);
//			ratio /= (float) fr -> int_symbol;
		}
		if (fr -> id ("vertical")) {
			if (! fr -> get_int ()) RETURN
			vertical_ratio = (float) vertical / (float) fr -> int_symbol;
		}
		if (fr -> id ("length")) {
			if (! fr -> get_int ()) RETURN
			data_size = fr -> int_symbol;
			data = new int [data_size + 32];
			address = 0;
		}
		if (fr -> id ("loop")) {
			if (! fr -> get_int ()) RETURN
			loop_start = fr -> int_symbol;
			if (loop_start < 0 || loop_start >= data_size) RETURN
			if (! fr -> get_int ()) RETURN
			loop_end = fr -> int_symbol;
			loop_size = loop_end - loop_start;
			if (loop_size < 0) RETURN
		}
		if (fr -> id ("tail")) {tail_present = true;}
		if (fr -> id ("wave")) {
			skip = false;
			while (fr -> get_int ()) {
				if (address < data_size) {
					data [address++] = (int) (vertical_ratio * (float) fr -> int_symbol);
				}
			}
		}
		if (skip) fr -> skip ();
	}
}

rom_wave :: rom_wave (char * name, rom_wave * next, int center_freq, int key, int horizontal, int vertical, int head_room, int loop_start, int loop_end, bool tail_present) {
	string_copy (wave_name, name);
	data_size = 0;
	data = NULL;
	calculate_ratio (10, center_freq, key, horizontal, vertical);
//	ratio = (float) (key - 8192);
//	ratio /= 1536.0f;
//	ratio = (float) pow (2.0f, ratio);
//	ratio *= 4194304.0f;
//	ratio *= (float) center_freq;
	this -> tail_present = tail_present;
	this -> next = next;

	int ind;
	int ch;
	FILE * fr = fopen (name, "rb");
	if (fr == NULL) {
		char command [256];
		sprintf (command, "File not found [%s]", name);
		message (command);
		return;
	}
	char id [17];

	for (ind = 0; ind < 4; ind++) id [ind] = fgetc (fr);
	id [4] = '\0';
	for (ind = 0; ind < 4; ind++) fgetc (fr);
	for (ind = 4; ind < 12; ind++) id [ind] = fgetc (fr);
	id [12] = '\0';
	if (strcmp (id, "RIFFWAVEfmt ") != 0) {fclose (fr); return;}
	int format_count = read4 (fr);
	int type = read2 (fr);
	int channels = read2 (fr);
	int sample_rate = read4 (fr);
	int bytes_per_second = read4 (fr);
	int block_align = read2 (fr);
	int bits_per_sample = read2 (fr);
	while (format_count-- > 16) fgetc (fr);

	int file_size = 0;
	while (true) {
		for (ind = 0; ind < 4; ind++) {
			ch = fgetc (fr);
			if (ch == EOF) {fclose (fr); return;}
			id [ind] = (char) ch;
		}
		id [4] = '\0';
		file_size = read4 (fr);
		if (strcmp (id, "data") == 0) break;
		while (file_size-- > 0) {
			if (fgetc (fr) == EOF) {fclose (fr); return;}
		}
	}

	data_size = file_size / block_align;
	calculate_ratio (10, center_freq, key, sample_rate, vertical);
//	ratio /= (float) sample_rate;
	data = new int [data_size + 32];
	int accu, sub;
	int address = 0;
	float ieee = 0.0;
	float vertical_ratio;
	switch (bits_per_sample) {
	case 8:
		vertical_ratio = (float) head_room / 128.0f;
		for (address = 0; address < data_size; address++) {
			accu = 0;
			for (ind = 0; ind < channels; ind++) {
				sub = fgetc (fr) - 128;
				accu += sub;
			}
			data [address] = (int) (vertical_ratio * (float) accu);
		}
		break;
	case 16:
		vertical_ratio = (float) head_room / 32768.0f;
		for (address = 0; address < data_size; address++) {
			accu = 0;
			for (ind = 0; ind < channels; ind++) {
				sub = read2 (fr);
				if (sub >= 32768) sub -= 65536;
				accu += sub;
			}
			data [address] = (int) (vertical_ratio * (float) accu);
		}
		break;
	case 32:
		if (type != 3) {fclose (fr); return;}
		vertical_ratio = (float) head_room;
		for (address = 0; address < data_size; address++) {
			ieee = 0.0f;
			for (ind = 0; ind < channels; ind++) {
				ieee += read_float (fr);
			}
			data [address] = (int) (vertical_ratio * ieee);
		}
		break;
	default: fclose (fr); return; break;
	}
	if (address <= loop_end && ! tail_present) data [address] = data [loop_start];
	else data [address] = data [address - 1];
	fclose (fr);
	this -> loop_end = loop_end;
	this -> loop_size = loop_end - loop_start;
}

rom_wave :: ~ rom_wave (void) {
	if (data != NULL) delete [] data;
	if (next != NULL) delete next;
}

void rom_player_algo :: data_constructor (oscillator * osc) {
	int ind;
	for (ind = 0; ind < waves_to; ind++) {
		osc -> time [ind] = 2147483647;
		osc -> feed [ind] = 0;
		osc -> feed [16 + ind] = 2147483647;
	}
	osc -> feed [32] = 0;
	osc -> feed [33] = 0;
	osc -> feed [40] = osc -> pb -> controllers -> vector_x;
	osc -> feed [41] = osc -> pb -> controllers -> vector_y;
	SMOOTH_STRIPE_INIT (1, 34);
}
void rom_player_algo :: retrigger (oscillator * osc) {
	for (int ind = 0; ind < waves_to; ind++) osc -> time [ind] = 0;
}
void rom_player_algo :: release (oscillator * osc) {
	rom_wave_pointer wp;
	int index;
	for (int ind = 0; ind < waves_to; ind++) {
		if (osc -> time [ind] != 0) {
			index = 0x7f & osc -> feed [ind];
			wp = wave_head [(ind << 7) + osc -> feed [ind]];
			if (wp != NULL) {
				if (wp -> tail_present) osc -> feed [16 + ind] = 2147483647;
			}
		} else osc -> feed [16 + ind] = 2147483647;
	}
}
char * rom_player_algo :: toString (parameter_block * pb) {return "ROM PLAYER";}
char * rom_player_algo :: toString (parameter_block * pb, int ind) {
	if (ind < 0 || ind > 15) return "<wrong index>";
	if (filter_on_2 && ind == 1) return "FILTER";
	return lsb_names [ind];
}

//	feed space:
//	0 .. 15     => index at waveform start
//	16 .. 31    => wave loop end
//	32 .. 33    => FILTER
//	34          => FILTER smoothing
//	40 .. 41    => actual X / Y
int rom_player_algo :: code (oscillator * osc) {
	float vector_amp [5];
	float vector_r, vector_l, vector_t, vector_b;
	SMOOTH_FILTER_EXTRA_DECLARATIONS;
	if (vector_processing) {
		vector_r = 1.0f * (float) osc -> feed [40] / 16384.0f;
		vector_l = 1.0f - vector_r;
		vector_t = 1.0f * (float) osc -> feed [41] / 16384.0f;
		vector_b = 1.0f - vector_t;
		vector_amp [0] = vector_t * vector_l;
		vector_amp [1] = 0.0f;
		vector_amp [2] = vector_t * vector_r;
		vector_amp [3] = vector_b * vector_l;
		vector_amp [4] = vector_b * vector_r;
		at = osc -> pb -> controllers -> vector_x;
		as = osc -> feed [40];
		if (as != at) {
			if (as < at) {
				sub = at - as;
				if (sub < 1024) as++;
				else if (sub < 2048) as += 2;
				else as += 4;
			} else {
				sub = as - at;
				if (sub < 1024) as--;
				else if (sub < 2048) as -= 2;
				else as -= 4;
			}
			osc -> feed [40] = as;
		}
		at = osc -> pb -> controllers -> vector_y;
		as = osc -> feed [41];
		if (as != at) {
			if (as < at) {
				sub = at - as;
				if (sub < 1024) as++;
				else if (sub < 2048) as += 2;
				else as += 4;
			} else {
				sub = as - at;
				if (sub < 1024) as--;
				else if (sub < 2048) as -= 2;
				else as -= 4;
			}
			osc -> feed [41] = as;
		}
	}
	FM_DECLARATIONS;
	MULTIFILTER_DECLARATIONS;
	wave = 0.0f;
	int index;
	unsigned int time, x;
	rom_wave * wp;
	float sample1, sample2, time_fraction1, time_fraction2;
	for (int ind = 0; ind < waves_to; ind++) {
		index = osc -> index [ind] >> 7;
		if (osc -> time [ind] == 0) {
			wp = wave_head [index + (ind << 7)];
			osc -> feed [ind] = index;
			if (wp != NULL) osc -> feed [16 + ind] = wp -> loop_end;
			else osc -> feed [16 + ind] = 2147483647;
		} else {
			index = osc -> feed [ind];
			wp = wave_head [index + (ind << 7)];
		}
		if (wp != NULL && (ind != 1 || ! filter_on_2)) {
			time = osc -> time [ind] >> wp -> bit_shift; //10;
			x = osc -> time [ind] & wp -> bit_mask; //0x3ff;
			time_fraction1 = (float) (wp -> bit_value - x); //(1024 - x);
			time_fraction2 = (float) x;
			if (time >= (unsigned int) osc -> feed [16 + ind]) {
				time -= wp -> loop_size;
				osc -> time [ind] -= wp -> loop_size << wp -> bit_shift; //10;
			}
			if (time < (unsigned int) wp -> data_size) {
				sample1 = (float) wp -> data [time];
				sample2 = (float) wp -> data [time + 1];
				sample1 *= time_fraction1;
				sample2 *= time_fraction2;
				if (vector_processing) wave += (sample1 + sample2) * vector_amp [ind] * (float) osc -> amp [ind] / wp -> mega_vertical;
				else wave += (sample1 + sample2) * (float) osc -> amp [ind] / wp -> mega_vertical;
				x = (int) (((float) osc -> freq [ind]) / wp -> ratio);
				if (x < 1) x = 1;
				osc -> freq [ind] = x;
			} else osc -> freq [ind] = 0;
		}
	}
	if (filter_on_2 && osc -> pb -> stripes [1] -> retrigger) {
		if (osc -> pb -> controllers -> algo_parameters [126] == 0) {
			ROM_WAVE_FILTER (1, 32, 33);
		} else {
			ROM_WAVE_SMOOTH_FILTER (1, 32, 33, 34);
		}
		wave = filter_output;
	}
	return (int) (wave + 0.49f);
}
void rom_player_algo :: waves (int number) {
	filter_on_2 = false;
	default_0 = 0;
	waves_to = number;
	int ind;
	for (ind = 0; ind < 16; ind++) {
		if (ind < number) sprintf (lsb_names [ind], "WAVEFORM %i", ind + 1);
		else sprintf (lsb_names [ind], "inactive");
	}
	reset_waves ();
}
void rom_player_algo :: filtered_waves (int number) {
	int ind;
	default_0 = 0;
	filter_on_2 = true;
	waves_to = number;
	reset_waves ();
	if (number > 0) sprintf (lsb_names [0], "WAVEFORM 1");
	else sprintf (lsb_names [0], "inactive");
	sprintf (lsb_names [1], "FILTER");
	for (ind = 2; ind < 16; ind++) {
		if (ind < number) sprintf (lsb_names [ind], "WAVEFORM %i", ind + 1);
		else sprintf (lsb_names [ind], "inactive");
	}
}
void rom_player_algo :: reset_waves (void) {
	int sentinel = waves_to << 7;
	for (int ind = 0; ind < sentinel; ind++) wave_head [ind] = NULL;
}
void rom_player_algo :: reset_waves (int index) {
	int sentinel = index + 1;
	if (sentinel > waves_to) return;
	sentinel <<= 7;
	for (int ind = index << 7; ind < sentinel; ind++) wave_head [ind] = NULL;
}

char * rom_player_algo :: get_name (int ind) {
	switch (ind) {
	case 0: return "H1 WAVEFORM";
	case 1: return "H2 WAVEFORM";
	case 2: return "H3 WAVEFORM";
	case 3: return "H4 WAVEFORM";
	case 4: return "H5 WAVEFORM";
	case 5: return "H6 WAVEFORM";
	case 6: return "H7 WAVEFORM";
	case 7: return "H8 WAVEFORM";
	case 8: return "H9 WAVEFORM";
	case 9: return "H10 WAVEFORM";
	case 10: return "H11 WAVEFORM";
	case 11: return "H12 WAVEFORM";
	case 12: return "H13 WAVEFORM";
	case 13: return "H14 WAVEFORM";
	case 14: return "H15 WAVEFORM";
	case 15: return "H16 WAVEFORM";
	default: break;
	}
	return algo :: get_name (ind);
}

int rom_player_algo :: get_default (int ind) {
	if (ind == 0) return default_0;
	return 0;
}

void rom_player_algo :: notify_algo_parameter_changed (parameter_block * pb, int ind) {
	if (ind >= waves_to) return;
	if (ind == 1 && filter_on_2) return;
	int program = pb -> controllers -> algo_parameters [ind];
//	program >>= 7; // commenting this out increases the number of waves to 16384
	reset_waves (ind);
	SetupFileReader * fr = new SetupFileReader (bank -> file_name);
	if (fr -> file_not_found ()) {delete fr; return;}
	bank -> read_program (fr, program, & wave_head [ind << 7], lsb_names [ind]);
	delete fr;
}

void rom_player_algo :: notify_all_algo_parameters_changed (parameter_block * pb) {
	for (int ind = 0; ind < 16; ind++) notify_algo_parameter_changed (pb, ind);
}

rom_player_algo :: rom_player_algo (rom_player_bank * bank, rom_wave_pointer * head, int vertical) {
	this -> bank = bank;
	wave_head = head;
	this -> vertical = (float) vertical;
//	this -> mega_vertical = this -> vertical * 1024.0f; // 1048576.0f; // 1024.0f * 1024f;
	filtered_waves (1);
	vector_processing = false;
}

algo_pointer rom_player_bank :: get_algo (int lsb, int program, parameter_block * pb) {
	rom_player_algo_pointer ap = algos [pb -> factory_id];
	switch (lsb) {
	case 0:
		ap -> filtered_waves (2);
		ap -> default_0 = program; // << 7; // commenting this out increases the number of waves to 16384
		return ap;
	case 1:
		if (program > stripes) return NULL;
		ap -> filtered_waves (program);
		return ap;
	case 2:
		if (program > stripes) return NULL;
		ap -> waves (program);
		return ap;
	case 3:
		if (stripes < 5 || program > stripes) return NULL;
		ap -> filtered_waves (program);
		ap -> vector_processing = true;
		return ap;
	default: return NULL;
	}
}

char * rom_player_bank :: toString (void) {return msb_name;}

char * rom_player_bank :: toString (int lsb) {
	switch (lsb) {
	case 0: return "BASE ROM PLAYER ALGO";
	case 1: return "FILTERED MULTIWAVE ALGOS";
	case 2: return "MULTIWAVE ALGOS";
	case 3: return "FILTERED VECTOR 4 WAVE ALGO";
	default: return "EMPTY";
	}
}

int check_last_3_characters (char * name) {
	// txt => 1
	// wav => 2
	// other => 0
	char command [4];
	int length = (int) strlen (name);
	if (length < 3) return 0;
	command [0] = name [length - 3];
	command [1] = name [length - 2];
	command [2] = name [length - 1];
	command [3] = '\0';
	if (strcmp (command, "txt") == 0) return 1;
	if (strcmp (command, "wav") == 0) return 2;
	return 0;
}

rom_wave * rom_player_bank :: import_wave (char * name, int key, int loop_start, int loop_end, bool tail_present) {
	int file_type = check_last_3_characters (name);
	if (wave_head == NULL) {
		if (file_type == 1) wave_head = new rom_wave (name, NULL, center_freq, key, horizontal, vertical, head_room);
		else wave_head = new rom_wave (name, NULL, center_freq, key, horizontal, vertical, head_room, loop_start, loop_end, tail_present);
		return wave_head;
	}
	rom_wave * ret = wave_head -> find (name);
	if (ret != NULL) return ret;
	if (file_type == 1) wave_head = new rom_wave (name, wave_head, center_freq, key, horizontal, vertical, head_room);
	else wave_head = new rom_wave (name, wave_head, center_freq, key, horizontal, vertical, head_room, loop_start, loop_end, tail_present);
	return wave_head;
}

void rom_player_bank :: configure (SetupFileReader * fr, char * directory) {
	string_copy (msb_name, "NOT FOUND");
	if (! fr -> get_id ("msb")) return;
	bool skipper = true;
	int location, from, to, key, loop_start, loop_end;
	bool tail_present;
	STRING wave_location;
	while (fr -> get_id ()) {
		skipper = true;
		if (fr -> id ("name")) {
			if (! fr -> get_string ()) return;
			string_copy (msb_name, fr -> symbol);
		}
		if (fr -> id ("program")) {
			skipper = false;
			location = -1; from = 0; to = 127; key = 8192;
			loop_start = 0; loop_end = 2147483647; tail_present = true;
			while (fr -> get_id ()) {
				if (fr -> id ("location")) {
					if (! fr -> get_int ()) return;
					location = fr -> int_symbol;
				}
				if (fr -> id ("freq")) {
					if (! fr -> get_int ()) return;
					key = fr -> int_symbol;
				}
				if (fr -> id ("loop")) {
					if (! fr -> get_int ()) return;
					loop_start = fr -> int_symbol;
					if (! fr -> get_int ()) return;
					loop_end = fr -> int_symbol;
				}
				if (fr -> id ("no_tail")) tail_present = false;
				if (fr -> id ("wave")) {
					if (! fr -> get_string ()) return;
					sprintf (wave_location, "%s%s", directory, fr -> symbol);
					import_wave (wave_location, key, loop_start, loop_end, tail_present);
					key = 8192;
					loop_start = 0; loop_end = 2147483647; tail_present = true;
				}
				fr -> skip ();
			}
		}
		if (skipper) fr -> skip ();
	}
}

void rom_player_bank :: read_program (SetupFileReader * fr, int program, rom_wave_pointer * rp, char * lsb_name) {
	if (! fr -> get_id ("msb")) return;
	string_copy (lsb_name, "WAVEFORM");
	bool skipper;
	int location, from, to, key;
	int loop_start, loop_end;
	bool tail_present;
	int ind;
	rom_wave_pointer wp;
	STRING wave_location;
	while (fr -> get_id ()) {
		skipper = true;
		if (fr -> id ("program")) {
			skipper = false;
			location = -1; from = 0; to = 127; key = 8192;
			loop_start = 0; loop_end = 2147483647; tail_present = true;
			wp = NULL;
			while (fr -> get_id ()) {
				if (fr -> id ("location")) {
					if (! fr -> get_int ()) return;
					location = fr -> int_symbol;
				}
				if (fr -> id ("default")) {location = program;}
				if (fr -> id ("name")) {
					if (location == program) {
						if (! fr -> get_string ()) return;
						string_copy (lsb_name, fr -> symbol);
					}
				}
				if (fr -> id ("index")) {
					if (location == program) {
						if (! fr -> get_int ()) return;
						from = fr -> int_symbol;
						to = from;
					}
				}
				if (fr -> id ("range")) {
					if (location == program) {
						if (! fr -> get_int ()) return;
						from = fr -> int_symbol;
						if (! fr -> get_int ()) return;
						to = fr -> int_symbol;
						if (from < 0) from = 0;
						if (from > 127) from = 127;
						if (to < 0) to = 0;
						if (to > 127) to = 127;
					}
				}
				if (fr -> id ("freq")) {
					if (location == program) {
						if (! fr -> get_int ()) return;
						key = fr -> int_symbol;
					}
				}
				if (fr -> id ("loop")) {
					if (location == program) {
						if (! fr -> get_int ()) return;
						loop_start = fr -> int_symbol;
						if (! fr -> get_int ()) return;
						loop_end = fr -> int_symbol;
					}
				}
				if (fr -> id ("no_tail")) {
					if (location == program) tail_present = false;
				}
				if (fr -> id ("silence")) {
					if (location == program) {
						wp = NULL;
						for (ind = from; ind <= to; ind++) rp [ind] = wp;
						key = 8192;
						loop_start = 0; loop_end = 2147483647; tail_present = true;
					}
				}
				if (fr -> id ("wave")) {
					if (location == program) {
						if (! fr -> get_string ()) return;
						sprintf (wave_location, "%s%s", directory, fr -> symbol);
						wp = import_wave (wave_location, key, loop_start, loop_end, tail_present);
						for (ind = from; ind <= to; ind++) rp [ind] = wp;
						key = 8192;
						loop_start = 0; loop_end = 2147483647; tail_present = true;
					}
				}
				fr -> skip ();
			}
			if (location == program) return;
		}
		if (skipper) fr -> skip ();
	}
}

rom_player_bank :: rom_player_bank (char * text, synthesiser * sth, int center_freq, int horizontal, int vertical, int head_room) {
	this -> center_freq = center_freq;
	this -> horizontal = horizontal;
	this -> head_room = head_room;
	this -> vertical = vertical;
	this -> stripes = sth -> stripes;
	this -> multitimbral = sth -> multitimbral;
	string_copy (msb_name, "");
	wave_head = NULL;
	wave_table = NULL;
	int table_size = (sth -> multitimbral * sth -> stripes) << 7;
	sprintf (directory, "%s%s", sth -> root_directory, text);
	sprintf (file_name, "%smsb.txt", directory);
	wave_table = new rom_wave_pointer [table_size + 32];
	SetupFileReader * fr = new SetupFileReader (file_name);
	configure (fr, directory);
	delete fr;

	int ind;
	for (ind = 0; ind < table_size; ind++) wave_table [ind] = NULL;

	algos = new rom_player_algo_pointer [sth -> multitimbral];
	for (ind = 0; ind < multitimbral; ind++) {
		algos [ind] = new rom_player_algo (this, & wave_table [(ind * sth -> stripes) << 7], head_room);
	}
}

rom_player_bank :: ~ rom_player_bank (void) {
	delete [] wave_table;
	if (wave_head != NULL) delete wave_head;
	for (int ind = 0; ind < multitimbral; ind++) {
		delete algos [ind];
	}
	delete [] algos;
}










