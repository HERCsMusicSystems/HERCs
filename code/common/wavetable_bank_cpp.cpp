///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc_cpp.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "algo_building_blocks.h"

wave_cell * wave_cell :: find (char * name) {
	if (strcmp (wave_name, name) == 0) return this;
	wave_cell * wc = next;
	while (wc != NULL) {
		if (strcmp (wc -> wave_name, name) == 0) return wc;
		wc = wc -> next;
	}
	return NULL;
}

void wave_cell :: read_wave (SetupFileReader * fr) {
	int ind;
	double sine_spectrum [128];
	double cosine_spectrum [128];
	for (ind = 0; ind < 128; ind++) {sine_spectrum [ind] = 0.0; cosine_spectrum [ind] = 0.0;}
	if (fr -> file_not_found ()) return;
	if (! fr -> get_id ("spectrum")) return;
	double normalization = 1.0 / 100.0;
	while (fr -> get_id ()) {
		ind = 0;
		if (fr -> id ("normalization")) {
			if (! fr -> get_int ()) return;
			normalization = 1.0 / (double) fr -> int_symbol;
			fr -> skip ();
		}
		if (fr -> id ("sin")) {
			while (fr -> get_int () && ind < 128) sine_spectrum [ind++] = normalization * (double) fr -> int_symbol;
			while (ind < 128) sine_spectrum [ind++] = 0.0;
		}
		if (fr -> id ("cos")) {
			while (fr -> get_int () && ind < 128) cosine_spectrum [ind++] = normalization * (double) fr -> int_symbol;
			while (ind < 128) cosine_spectrum [ind++] = 0.0;
		}
	}
	int sub;
	double accu;
	ind = 0;
	double omega = M_PI / 128.0;
	while (ind < 256) {
		accu = 0.0;
		for (sub = 0; sub < 128; sub++) {
			accu += sin (((double) (ind * (sub + 1))) * omega) * sine_spectrum [sub];
			accu += cos (((double) (ind * (sub + 1))) * omega) * cosine_spectrum [sub];
			data [ind + (sub << 8)] = (float) accu;
		}
		ind++;
	}
}

wave_cell :: wave_cell (char * name, wave_cell * next, int vertical) {
	this -> vertical = vertical;
	string_copy (wave_name, name);
	this -> next = next;
	SetupFileReader * fr = new SetupFileReader (name);
	read_wave (fr);
	delete fr;
}

void wavetable_location :: ground (void) {
	inactive = true;
	from_cell = NULL;
	to_cell = NULL;
	from_index = 0;
	index_length = 0;
}

void wavetable_location :: set (int from, int to, wave_cell * fc, wave_cell * tc) {
	inactive = false;
	from_cell = fc;
	to_cell = tc;
	from_index = from << 7;
	index_length = (to - from) << 7;
	if (from_cell == NULL) inactive = true;
	if (to_cell == NULL) inactive = true;
}

float wavetable_location :: get (unsigned int time, unsigned int freq, float amp, int index) {
	if (inactive) return 0.0f;
	int shift;
	if (freq == 0) shift = 127 << 8;
	else {
		freq = 0x80000000 / freq;
		shift = freq - 1;
		if (shift < 0) shift = 0;
		if (shift > 127) shift = 127;
		shift <<= 8;
	}
	unsigned int mask = 0xffffff;
	int roll = 24;
	unsigned int time_limit = 254;
	float wave_fraction2 = (float) (time & mask) / (float) (mask + 1);
	float wave_fraction1 = 1.0f - wave_fraction2;
	time >>= roll;
	float v1 = wave_fraction2 * (time > time_limit ? from_cell -> data [shift] : from_cell -> data [shift + time + 1]);
	v1 += wave_fraction1 * from_cell -> data [shift + time];
	float v2 = wave_fraction2 * (time > time_limit ? to_cell -> data [shift] : to_cell -> data [shift + time + 1]);
	v2 += wave_fraction1 * to_cell -> data [shift + time];
	float fraction2 = (float) (index - from_index) / (float) index_length;
	float fraction1 = 1.0f - fraction2;
	return amp * (v1 * fraction1 + v2 * fraction2);
}

char * wavetable_algo :: toString (parameter_block * pb) {return "WAVETABLE";}
char * wavetable_algo :: toString (parameter_block * pb, int ind) {
	if (ind < 0 || ind > 15) return "<wrong index>";
	if (filter_on_2 && ind == 1) return "FILTER";
	return lsb_names [ind];
}

void wavetable_algo :: data_constructor (oscillator * osc) {
	FILTER_INIT (1, 32);
	FILTER_INIT (33, 34);
	FILTER_INIT (36, 37);
	FILTER_INIT (38, 39);
}

// feed space
// 40 .. 41    => actual X / Y
int wavetable_algo :: code (oscillator * osc) {
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
	SECOND_STAGE_FILTER_DECLARATIONS;
	wave = 0.0f;
	wavetable_location * wp;
	int location = 0;
	int index;
	for (int ind = 0; ind < waves_to; ind++) {
		if (ind != 1 || ! filter_on_2) {
			index = osc -> index [ind] & 0x3fff;
			wp = & wavetable_head [location + (index >> 7)];
			wave += wp -> get (osc -> time [ind], osc -> freq [ind], vector_processing ? vector_amp [ind] * (float) osc -> amp [ind] : (float) osc -> amp [ind], index);
		}
		location += 128;
	}
	if (filter_on_2 && osc -> pb -> stripes [1] -> retrigger) {
		float pre_filter_signal = wave;;
		int filter_type = osc -> pb -> controllers -> algo_parameters [124];
		if (osc -> pb -> controllers -> algo_parameters [126] == 0) {
			ROM_WAVE_FILTER (1, 32, 33);
		} else {
			ROM_WAVE_SMOOTH_FILTER (1, 32, 33, 34);
		}
		if (filter_type > 0) {
			SECOND_STAGE_FILTER (1, 36, 37);
			if (filter_type > 1) {
				THIRD_STAGE_FILTER (1, 38, 39);
			}
		}
		wave = filter_output;
		if (osc -> pb -> controllers -> algo_parameters [123] != 0) wave = pre_filter_signal - wave;
	}
	return (int) (wave + 0.49f);
}

void wavetable_algo :: waves (int number) {
	filter_on_2 = false;
	default_0 = 0;
	waves_to = number;
	int ind;
	for (ind = 0; ind < 16; ind++) {
		if (ind < number) sprintf (lsb_names [ind], "WAVE CELL %i", ind + 1);
		else sprintf (lsb_names [ind], "inactive");
	}
	reset_waves ();
}

void wavetable_algo :: filtered_waves (int number) {
	int ind;
	default_0 = 0;
	filter_on_2 = true;
	waves_to = number;
	reset_waves ();
	if (number > 0) sprintf (lsb_names [0], "WAVE CELL 1");
	else sprintf (lsb_names [0], "inactive");
	sprintf (lsb_names [1], "FILTER");
	for (ind = 2; ind < 16; ind++) {
		if (ind < number) sprintf (lsb_names [ind], "WAVE CELL %i", ind + 1);
		else sprintf (lsb_names [ind], "inactive");
	}
}
void wavetable_algo :: reset_waves (void) {
	int sentinel = waves_to << 7;
	for (int ind = 0; ind < sentinel; ind++) wavetable_head [ind] . ground ();
}
void wavetable_algo :: reset_waves (int index) {
	int sentinel = index + 1;
	if (sentinel > waves_to) return;
	sentinel <<= 7;
	for (int ind = index << 7; ind < sentinel; ind++) wavetable_head [ind] . ground ();
}


char * wavetable_algo :: get_name (int ind) {
	switch (ind) {
	case 0: return "H1 WAVETABLE";
	case 1: return "H2 WAVETABLE";
	case 2: return "H3 WAVETABLE";
	case 3: return "H4 WAVETABLE";
	case 4: return "H5 WAVETABLE";
	case 5: return "H6 WAVETABLE";
	case 6: return "H7 WAVETABLE";
	case 7: return "H8 WAVETABLE";
	case 8: return "H9 WAVETABLE";
	case 9: return "H10 WAVETABLE";
	case 10: return "H11 WAVETABLE";
	case 11: return "H12 WAVETABLE";
	case 12: return "H13 WAVETABLE";
	case 13: return "H14 WAVETABLE";
	case 14: return "H15 WAVETABLE";
	case 15: return "H16 WAVETABLE";
	case 123: return "FILTER TYPE";
	case 124: return "SLOPE";
	default: break;
	}
	return algo :: get_name (ind);
}

int wavetable_algo :: get_default (int ind) {
	if (ind == 0) return default_0;
	return 0;
}

void wavetable_algo :: notify_algo_parameter_changed (parameter_block * pb, int ind) {
	if (ind >= waves_to) return;
	if (ind == 1 && filter_on_2) return;
	int program = pb -> controllers -> algo_parameters [ind];
//	program >>= 7; // commenting this out increases the number of waves to 16384
	reset_waves (ind);
	SetupFileReader * fr = new SetupFileReader (bank -> file_name);
	if (fr -> file_not_found ()) {delete fr; return;}
	bank -> read_program (fr, program, & wavetable_head [ind << 7], lsb_names [ind]);
	delete fr;
}

void wavetable_algo :: notify_all_algo_parameters_changed (parameter_block * pb) {
	for (int ind = 0; ind < 16; ind++) notify_algo_parameter_changed (pb, ind);
}

wavetable_algo :: wavetable_algo (wavetable_bank_cpp * bank, wavetable_location * head, int vertical) {
	this -> bank = bank;
	wavetable_head = head;
	this -> vertical = (float) vertical;
	filtered_waves (1);
	vector_processing = false;
}

algo_pointer wavetable_bank_cpp :: get_algo (int lsb, int program, parameter_block * pb) {
	wavetable_algo_pointer ap = algos [pb -> factory_id];
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

char * wavetable_bank_cpp :: toString (void) {return msb_name;}
char * wavetable_bank_cpp :: toString (int lsb) {
	switch (lsb) {
	case 0: return "BASE WAVETABLE ALGO";
	case 1: return "FILTERED MULTIWAVETABLE ALGOS";
	case 2: return "MULTIWAVETABLE ALGOS";
	case 3: return "FILTERED VECTOR 4 WAVETABLE ALGO";
	default: return "EMPTY";
	}
}

wave_cell * wavetable_bank_cpp :: import_wave (char * name) {
	if (cell_head == NULL) {
		cell_head = new wave_cell (name, NULL, vertical);
		return cell_head;
	}
	wave_cell * ret = cell_head -> find (name);
	if (ret != NULL) return ret;
	cell_head = new wave_cell (name, cell_head, vertical);
	return cell_head;
}

void wavetable_bank_cpp :: configure (SetupFileReader * fr, char * directory) {
	string_copy (msb_name, "NOT FOUND");
	if (! fr -> get_id ("msb")) return;
	bool skipper = true;
	STRING wave_location;
	while (fr -> get_id ()) {
		skipper = true;
		if (fr -> id ("name")) {
			if (! fr -> get_string ()) return;
			string_copy (msb_name, fr -> symbol);
		}
		if (fr -> id ("program")) {
			skipper = false;
			while (fr -> get_id ()) {
				if (fr -> id ("waves")) {
					if (! fr -> get_string ()) return;
					sprintf (wave_location, "%s%s", directory, fr -> symbol);
					import_wave (wave_location);
					if (! fr -> get_string ()) return;
					sprintf (wave_location, "%s%s", directory, fr -> symbol);
					import_wave (wave_location);
				}
				fr -> skip ();
			}
		}
		if (skipper) fr -> skip ();
	}
}

void wavetable_bank_cpp :: read_program (SetupFileReader * fr, int program, wavetable_location * wl, char * lsb_name) {
	if (! fr -> get_id ("msb")) return;
	string_copy (lsb_name, "WAVE CELL");
	bool skipper;
	int ind;
	int location, from, to;
	wave_cell * wave_from, * wave_to;
	STRING wave_location;
	while (fr -> get_id ()) {
		skipper = true;
		if (fr -> id ("program")) {
			skipper = false;
			location = -1; from = 0; to = 128;
			wave_from = NULL; wave_to = NULL;
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
						from = fr -> int_symbol >> 7;
						to = from;
					}
				}
				if (fr -> id ("range")) {
					if (location == program) {
						if (! fr -> get_int ()) return;
						from = fr -> int_symbol >> 7;
						if (! fr -> get_int ()) return;
						to = fr -> int_symbol >> 7;
					}
				}
				if (fr -> id ("silence")) {
					if (location == program) {
						if (from < 0) from = 0;
						if (from > 127) from = 127;
						if (to < from) to = from;
						if (to > 128) to = 128;
						wave_from = NULL; wave_to = NULL;
						for (ind = from; ind < to; ind++) wl [ind] . set (from, to, wave_from, wave_to);
					}
				}
				if (fr -> id ("waves")) {
					if (location == program) {
						if (from < 0) from = 0;
						if (from > 127) from = 127;
						if (to < from) to = from;
						if (to > 128) to = 128;
						if (! fr -> get_string ()) return;
						sprintf (wave_location, "%s%s", directory, fr -> symbol);
						wave_from = import_wave (wave_location);
						if (! fr -> get_string ()) return;
						sprintf (wave_location, "%s%s", directory, fr -> symbol);
						wave_to = import_wave (wave_location);
						for (ind = from; ind < to; ind++) wl [ind] . set (from, to, wave_from, wave_to);
					}
				}
				fr -> skip ();
			}
			if (location == program) return;
		}
		if (skipper) fr -> skip ();
	}
}

wavetable_bank_cpp :: wavetable_bank_cpp (STRING text, synthesiser * sth, int vertical) {
	this -> vertical = vertical;
	this -> stripes = sth -> stripes;
	this -> multitimbral = sth -> multitimbral;
	cell_head = NULL;
	wavetables = NULL;
	int wavetable_size = (sth -> multitimbral * sth -> stripes) << 7;
	wavetables = new wavetable_location [wavetable_size];
	string_copy (msb_name, "");
	sprintf (directory, "%s%s", sth -> root_directory, text);
	sprintf (file_name, "%smsb.txt", directory);
	SetupFileReader * fr = new SetupFileReader (file_name);
	configure (fr, directory);
	delete fr;

	int ind;
	algos = new wavetable_algo_pointer [sth -> multitimbral];
	for (ind = 0; ind < multitimbral; ind++) {
		algos [ind] = new wavetable_algo (this, & wavetables [(ind * sth -> stripes) << 7], vertical);
	}
}

wavetable_bank_cpp :: ~ wavetable_bank_cpp (void) {
	for (int ind = 0; ind < multitimbral; ind++) {
		delete algos [ind];
	}
	delete [] algos;
}
