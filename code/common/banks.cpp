///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "string.h"

void bank :: program_change (int lsb, int program, parameter_block * pb) {}
char * bank :: toString (void) {return "BANK MSB";}
char * bank :: toString (int lsb) {return "BANK LSB";}
bank :: ~ bank (void) {}

void algo_bank :: program_change (int lsb, int program, parameter_block * pb) {
	string_copy (pb -> algo_msb, toString ());
	pb -> algo_lsb = lsb;
	pb -> algo_program = program;
	algo_pointer ap = get_algo (lsb, program, pb);
	if (ap == NULL) ap = pb -> sth -> get_default_algo ();
	pb -> algo_head = ap;
	controllers_parameter_block * cb = pb -> controllers;
	for (int ind = 0; ind < 128; ind++) cb -> algo_parameters [ind] = ap -> get_default (ind);
	pb -> notify_algo_change ();
}
algo_pointer algo_bank :: get_algo (int lsb, int program, parameter_block * pb) {return pb -> sth -> get_default_algo ();}

void pattern_bank :: program_change (int lsb, int program, parameter_block * pb) {
	string_copy (pb -> pattern_msb, toString ());
	pb -> pattern_lsb = lsb;
	pb -> pattern_program = program;
	pattern_pointer pp = get_pattern (lsb, program, pb);
	if (pp == NULL) pp = pb -> sth -> get_default_pattern ();
	pb -> pattern_head = pp;
	pb -> notify_pattern_change ();
}
pattern_pointer pattern_bank :: get_pattern (int lsb, int program, parameter_block * pb) {return pb -> sth -> get_default_pattern ();}

char * dsp_algo_bank :: toString (void) {return "DSP ALGO BANK";}
char * dsp_algo_bank :: toString (int lsb) {
	switch (lsb) {
	case 0: return "DSP 1";
	case 1: return "DSP 2";
	case 2: return "DSP 3";
	case 3: return "DSP 4";
	case 4: return "DSP 5";
	case 5: return "DSP 6";
	case 6: return "DSP 7";
	case 7: return "DSP 8";
	default: return "EMPTY";
	}
}
void dsp_algo_bank :: program_change (int lsb, int program, parameter_block * pb) {
	if (lsb >= pb -> hercules_all_dsp_number) return;
	dsp_parameter_block * dspp = pb -> dsp [lsb];
	string_copy (dspp -> algo_msb, toString ());
	dspp -> algo_program = program;
	dsp_algo_pointer ap = get_dsp_algo (program, pb);
	if (ap == NULL) ap = pb -> sth -> get_default_dsp_algo ();
	dspp -> algo_head = ap;
	for (int ind = 0; ind < 16; ind++) dspp -> parameters [ind] = ap -> get_default (ind);
	pb -> notify_dsp_algo_change (lsb);
}

dsp_algo_pointer dsp_algo_bank :: get_dsp_algo (int program, parameter_block * pb) {return pb -> sth -> get_default_dsp_algo ();}

char * preset_algo_bank :: toString (void) {return "PRESET ALGO BANK";}
char * preset_dsp_algo_bank :: toString (void) {return "PRESET DSP ALGO BANK";}

void sound_bank :: program_change (int lsb, int program, parameter_block * pb) {
	SetupFileReader * fr = new SetupFileReader (msb_file_name);
	extract_program (fr, lsb, program, pb);
	delete fr;
}
void sound_bank :: extract_program (SetupFileReader * fr, int lsb, int program, parameter_block * pb) {
	int location;
	int program_location;
	STRING file_name;
	if (! fr -> get_id ("msb")) return;
	while (fr -> get_id ()) {
		if (fr -> id ("lsb")) {
			location = -1;
			while (fr -> get_id ()) {
				if (fr -> id ("location")) {
					if (! fr -> get_int ()) return;
					location = fr -> int_symbol;
					fr -> skip ();
				}
				if (fr -> id ("default")) {location = lsb; fr -> skip ();}
				if (fr -> id ("name")) {fr -> skip ();}
				if (fr -> id ("program")) {
					if (location == lsb) {
						program_location = -1;
						string_copy (file_name, "");
						while (fr -> get_id ()) {
							if (fr -> id ("location")) {
								if (! fr -> get_int ()) return;
								program_location = fr -> int_symbol;
							}
							if (fr -> id ("default")) {program_location = program;}
							if (fr -> id ("file")) {
								if (! fr -> get_string ()) return;
								sprintf (file_name, "%s%s", directory, fr -> symbol);
							}
							fr -> skip ();
						}
						if (program_location == program) {
							fr -> close ();
							SetupFileReader * sr = new SetupFileReader (file_name);
							pb -> voice_init ();
							pb -> read_program (sr);
							pb -> notify_program_loaded ();
							delete sr;
							return;
						}
					} else fr -> skip ();
				}
			}
		} else fr -> skip ();
	}
	fr -> close ();
}
char * sound_bank :: toString (void) {return msb_name;}
char * sound_bank :: toString (int lsb) {
	if (lsb < 0 || lsb > 127) return default_lsb_name;
	if (strcmp (lsb_name [lsb], "") == 0) return default_lsb_name;
	return lsb_name [lsb];
}
void sound_bank :: extract_banks (SetupFileReader * fr) {
	bool is_default;
	int location;
	STRING name;
	if (! fr -> get_id ("msb")) return;
	while (fr -> get_id ()) {
		if (fr -> id ("name")) {
			if (! fr -> get_string ()) return;
			string_copy (msb_name, fr -> symbol);
			fr -> skip ();
		}
		if (fr -> id ("lsb")) {
			is_default = false;
			location = 0;
			string_copy (name, "SUB SOUND BANK");
			while (fr -> get_id ()) {
				if (fr -> id ("location")) {
					is_default = false;
					if (! fr -> get_int ()) return;
					location = fr -> int_symbol;
				}
				if (fr -> id ("default")) {is_default = true;}
				if (fr -> id ("name")) {
					if (! fr -> get_string ()) return;
					string_copy (name, fr -> symbol);
				}
				fr -> skip ();
			}
			if (location > 127 || location < 0) return;
			if (is_default) string_copy (default_lsb_name, name);
			else string_copy (lsb_name [location], name);
		}
	}
	fr -> close ();
}
sound_bank :: sound_bank (STRING text, synthesiser * sth) {
	sprintf (directory, "%s%s", sth -> root_directory, text);
	sprintf (msb_file_name, "%s%s", directory, "msb.txt");
	string_copy (msb_name, "SOUND BANK");
	for (int ind = 0; ind < 128; ind++) string_copy (lsb_name [ind], "");
	string_copy (default_lsb_name, "<not installed>");
	SetupFileReader * fr = new SetupFileReader (msb_file_name);
	extract_banks (fr);
	delete fr;
}

void user_sound_bank :: program_change (int lsb, int program, parameter_block * pb) {
	char command [256];
	sprintf (command, "%s%02X.txt", directory, program);
	if (lsb != 127) {
		pb -> voice_init ();
		SetupFileReader * fr = new SetupFileReader (command);
		pb -> read_program (fr);
		pb -> notify_program_loaded ();
		delete fr;
	} else {
		SetupFileWriter * tc = new SetupFileWriter (command);
		pb -> store_program (tc);
		delete tc;
	}
}

char * user_sound_bank :: toString (void) {
	return "USER SOUND";
}
char * user_sound_bank :: toString (int lsb) {
	if (lsb == 127) return "STORE";
	return "READ";
}
user_sound_bank :: user_sound_bank (STRING text, synthesiser * sth) {
	sprintf (directory, "%s%s", sth -> root_directory, text);
}

void dsp_bank :: program_change (int lsb, int program, parameter_block * pb) {
	SetupFileReader * fr = new SetupFileReader (msb_file_name);
	extract_program (fr, lsb, program, pb);
	delete fr;
}
void dsp_bank :: extract_program (SetupFileReader * fr, int lsb, int program, parameter_block * pb) {
	int location;
	int program_location;
	STRING file_name;
	if (! fr -> get_id ("msb")) return;
	while (fr -> get_id ()) {
		if (fr -> id ("lsb")) {
			location = -1;
			while (fr -> get_id ()) {
				if (fr -> id ("location")) {
					if (! fr -> get_int ()) return;
					location = fr -> int_symbol;
					fr -> skip ();
				}
				if (fr -> id ("default")) {location = lsb; fr -> skip ();}
				if (fr -> id ("name")) {fr -> skip ();}
				if (fr -> id ("program")) {
					if (location == lsb) {
						program_location = -1;
						string_copy (file_name, "");
						while (fr -> get_id ()) {
							if (fr -> id ("location")) {
								if (! fr -> get_int ()) return;
								program_location = fr -> int_symbol;
							}
							if (fr -> id ("default")) {program_location = program;}
							if (fr -> id ("file")) {
								if (! fr -> get_string ()) return;
								sprintf (file_name, "%s%s", directory, fr -> symbol);
							}
							fr -> skip ();
						}
						if (program_location == program) {
							fr -> close ();
							SetupFileReader * sr = new SetupFileReader (file_name);
							pb -> dsp_voice_init (global);
							pb -> read_dsp_program (sr, global);
							pb -> notify_dsp_program_loaded (global);
							delete sr;
							return;
						}
					} else fr -> skip ();
				}
			}
			fr -> skip ();
		} else fr -> skip ();
	}
	fr -> close ();
}
char * dsp_bank :: toString (void) {return msb_name;}
char * dsp_bank :: toString (int lsb) {
	if (lsb < 0 || lsb > 127) return default_lsb_name;
	if (strcmp (lsb_name [lsb], "") == 0) return default_lsb_name;
	return lsb_name [lsb];
}
void dsp_bank :: extract_banks (SetupFileReader * fr) {
	bool is_default;
	int location;
	STRING name;
	if (! fr -> get_id ("msb")) return;
	while (fr -> get_id ()) {
		if (fr -> id ("name")) {
			if (! fr -> get_string ()) return;
			string_copy (msb_name, fr -> symbol);
			fr -> skip ();
		}
		if (fr -> id ("global_dsp")) {
			global = true;
			fr -> skip ();
		}
		if (fr -> id ("lsb")) {
			is_default = false;
			location = 0;
			string_copy (name, "SUB SOUND BANK");
			while (fr -> get_id ()) {
				if (fr -> id ("location")) {
					is_default = false;
					if (! fr -> get_int ()) return;
					location = fr -> int_symbol;
				}
				if (fr -> id ("default")) {is_default = true;}
				if (fr -> id ("name")) {
					if (! fr -> get_string ()) return;
					string_copy (name, fr -> symbol);
				}
				fr -> skip ();
			}
			if (location > 127 || location < 0) return;
			if (is_default) string_copy (default_lsb_name, name);
			else string_copy (lsb_name [location], name);
		}
	}
	fr -> close ();
}
dsp_bank :: dsp_bank (STRING text, synthesiser * sth) {
	global = false;
	sprintf (directory, "%s%s", sth -> root_directory, text);
	sprintf (msb_file_name, "%s%s", directory, "dsp_msb.txt");
	string_copy (msb_name, "DSP BANK");
	for (int ind = 0; ind < 128; ind++) string_copy (lsb_name [ind], "");
	string_copy (default_lsb_name, "<not installed>");
	SetupFileReader * fr = new SetupFileReader (msb_file_name);
	extract_banks (fr);
	delete fr;
}

void user_dsp_bank :: program_change (int lsb, int program, parameter_block * pb) {
	char command [256];
	sprintf (command, "%s%02X.txt", directory, program);
	if (lsb != 127) {
		pb -> dsp_voice_init (global);
		SetupFileReader * fr = new SetupFileReader (command);
		pb -> read_dsp_program (fr, global);
		pb -> notify_dsp_program_loaded (global);
		delete fr;
	} else {
		SetupFileWriter * tc = new SetupFileWriter (command);
		pb -> store_dsp_program (tc, global);
		delete tc;
	}
}
char * user_dsp_bank :: toString (void) {
	if (global) return "USER GLOBAL DSP";
	return "USER DSP";
}
char * user_dsp_bank :: toString (int lsb) {
	if (lsb == 127) return "STORE";
	return "READ";
}
void user_dsp_bank :: extract_dsp_type (SetupFileReader * fr) {
	global = false;
	if (! fr -> get_id ()) return;
	if (fr -> id ("global_dsp")) global = true;
}
user_dsp_bank :: user_dsp_bank (STRING text, synthesiser * sth) {
	sprintf (directory, "%s%s", sth -> root_directory, text);
	STRING file_name;
	sprintf (file_name, "%s%s", directory, "user_dsp_msb.txt");
	SetupFileReader * fr = new SetupFileReader (file_name);
	extract_dsp_type (fr);
	delete fr;
}
