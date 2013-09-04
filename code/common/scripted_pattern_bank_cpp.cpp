///////////////////////////////////////
// Copyright (C) 2005 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "scripted_pattern_bank_cpp.h"
#include "string.h"

char * scripted_pattern_cpp :: styleName (parameter_block * pb) {return style_pointer;}
char * scripted_pattern_cpp :: toString (parameter_block * pb) {return name_pointer;}
void scripted_pattern_cpp :: tick_code (arranger * arp) {
	if (arp -> variation == 0) arp -> wt ();
	else code (arp);
}
void scripted_pattern_cpp :: beat_code (arranger * arp) {tick_code (arp);}
void scripted_pattern_cpp :: bar_code (arranger * arp) {tick_code (arp);}
void scripted_pattern_cpp :: variation_code (arranger * arp) {
	if (arp -> variation != 0) return;
	arp -> reset ();
	arp -> update_variation ();
	arp -> status = 0;
	go_to (arp, arp -> variation, 0);
	arp -> schedule_tick_code ();
//	code (arp);
}
void scripted_pattern_cpp :: transcription_code (arranger * arp) {}
void scripted_pattern_cpp :: go_to (arranger * arp, int variation, int location) {
	instruction * i = ip;
	int ind = 0;
	while (i -> mnemonic != END) {
		if (i -> mnemonic == ENTER && i -> arg1 == variation && i -> arg2 == location) {
			arp -> counter = ind;
			arp -> status = variation;
			return;
		}
		i++;
		ind++;
	}
}
int scripted_pattern_cpp :: get_tempo (void) {return 0;}
void scripted_pattern_cpp :: code (arranger * arp) {
	instruction * i;
	while (true) {
		if (arp -> variation == 0) return;
		i = & ip [arp -> counter];
		switch (i -> mnemonic) {
		case END: arp -> counter = 0; break;
		case WT: arp -> wt (i -> arg1); arp -> counter++; return;
		case WB: arp -> wait_beat (i -> arg1); arp -> counter++; return;
		case WBR: arp -> wait_bar (i -> arg1); arp -> counter++; return;
		case WH: arp -> wt (note_length (arp)); arp -> counter++; return;
		case UV:
			arp -> update_variation ();
			arp -> counter++;
			if (arp -> status != arp -> variation) go_to (arp, arp -> variation, 0);
			break;
		case UT: arp -> update_transcriptions (); arp -> counter++; break;
		case UPDATE:
			arp -> update ();
			arp -> counter++;
			if (arp -> status != arp -> variation) go_to (arp, arp -> variation, 0);
			break;
		case ENTER: arp -> counter++; break;
		case CHANGE:
			arp -> counter++;
			go_to (arp, i -> arg1, i -> arg2);
			arp -> variation = i -> arg1;
			break;
		case COND:
			if (arp -> top_key_index () >= i -> arg1) break;
			arp -> counter++;
			go_to (arp, i -> arg2, i -> arg3);
			arp -> variation = i -> arg2;
			break;
		case AR: arp -> arrange (i -> arg1, i -> arg2, i -> arg3); arp -> counter++; break;
		case REL: arp -> relative (i -> arg1, i -> arg2); arp -> counter++; break;
		case KEY: arp -> key_on (i -> arg1, i -> arg2); arp -> counter++; break;
		case HTV: arp -> hit_top (i -> arg1, i -> arg2, i -> arg3); arp -> counter++; break;
		case HT: arp -> hit_top (i -> arg1, i -> arg2); arp -> counter++; break;
		case HTBV: arp -> hit_bottom (i -> arg1, i -> arg2, i -> arg3); arp -> counter++; break;
		case HTB: arp -> hit_bottom (i -> arg1, i -> arg2); arp -> counter++; break;
		case AOFF: arp -> key_off (); arp -> counter++; break;
		default: arp -> counter++; break;
		}
	}
}

//bool scripted_pattern_cpp :: auto_keyoff (void) {return false;}

scripted_pattern_cpp :: scripted_pattern_cpp (char * style_pointer, char * name_pointer, instruction * ip) {
	this -> style_pointer = style_pointer;
	this -> name_pointer = name_pointer;
	this -> ip = ip;
}

void drop_instructions (PrologRoot * root, instruction * i) {
	bool cont = true;
	char command [256];
	do {
		switch (i -> mnemonic) {
		case END: sprintf (command, "end []\n"); cont = false; break;
		case WT: sprintf (command, "wt [%i]\n", i -> arg1); break;
		case WB: sprintf (command, "wb [%i]\n", i -> arg1); break;
		case WBR: sprintf (command, "wbr [%i]\n", i -> arg1); break;
		case WH: sprintf (command, "wh []\n"); break;
		case UV: sprintf (command, "uv []\n"); break;
		case UT: sprintf (command, "ut []\n"); break;
		case UPDATE: sprintf (command, "update []\n"); break;
		case ENTER: sprintf (command, "enter [%i %i]\n", i -> arg1, i -> arg2); break;
		case CHANGE: sprintf (command, "change [%i %i]\n", i -> arg1, i -> arg2); break;
		case COND: sprintf (command, "cond [%i %i %i]\n", i -> arg1, i -> arg2, i -> arg3); break;
		case AR: sprintf (command, "ar [%i %i %i]\n", i -> arg1, i -> arg2, i -> arg3); break;
		case REL: sprintf (command, "rel [%i %i]\n", i -> arg1, i -> arg2); break;
		case KEY: sprintf (command, "key [%i %i]\n", i -> arg1, i -> arg2); break;
		case HTV: sprintf (command, "htv [%i %i %i]\n", i -> arg1, i -> arg2, i -> arg3); break;
		case HT: sprintf (command, "ht [%i %i]\n", i -> arg1, i -> arg2); break;
		case HTBV: sprintf (command, "htv [%i %i %i]\n", i -> arg1, i -> arg2, i -> arg3); break;
		case HTB: sprintf (command, "ht [%i %i]\n", i -> arg1, i -> arg2); break;
		case AOFF: sprintf (command, "aoff []\n"); break;
		}
		root -> print (command);
		i++;
	} while (cont);
}

bool read_instruction_args (SetupFileReader * fr, instruction * i, int mnemonic, int count) {
	i -> mnemonic = mnemonic;
	if (count > 0) {
		if (! fr -> get_int ()) return false;
		i -> arg1 = fr -> int_symbol;
	}
	if (count > 1) {
		if (! fr -> get_int ()) return false;
		i -> arg2 = fr -> int_symbol;
	}
	if (count > 2) {
		if (! fr -> get_int ()) return false;
		i -> arg3 = fr -> int_symbol;
	}
	fr -> skip ();
	return true;
}

void script_tables :: read_scripted_pattern (SetupFileReader * fr, int factory_id) {//, int track) {
//	if (track == 0x7f) return;
	int location = 0;
	instruction * i = instruction_tables [factory_id];
	if (! fr -> get_id ("pattern")) return;
//	if (! fr -> get_id ("name")) return;
//	if (! fr -> get_string ()) return;
//	string_copy (style_names [factory_id], fr -> symbol);
//	fr -> skip ();
//	while (track-- > 0) {
//		if (! fr -> get_id ("track")) return;
//		fr -> skip ();
//	}
//	if (! fr -> get_id ("track")) return;
	while (fr -> get_id ()) {
		if (fr -> id ("name")) {
			if (! fr -> get_string ()) return;
			string_copy (pattern_names [factory_id], fr -> symbol);
			fr -> skip ();
			location--;
			i--;
		}
		if (fr -> id ("end")) {if (! read_instruction_args (fr, i, END_)) return;}
		if (fr -> id ("wt")) {if (! read_instruction_args (fr, i, WT_)) return;}
		if (fr -> id ("wb")) {if (! read_instruction_args (fr, i, WB_)) return;}
		if (fr -> id ("wbr")) {if (! read_instruction_args (fr, i, WBR_)) return;}
		if (fr -> id ("wh")) {if (! read_instruction_args (fr, i, WH_)) return;}
		if (fr -> id ("uv")) {if (! read_instruction_args (fr, i, UV_)) return;}
		if (fr -> id ("ut")) {if (! read_instruction_args (fr, i, UT_)) return;}
		if (fr -> id ("update")) {if (! read_instruction_args (fr, i, UPDATE_)) return;}
		if (fr -> id ("enter")) {if (! read_instruction_args (fr, i, ENTER_)) return;}
		if (fr -> id ("change")) {if (! read_instruction_args (fr, i, CHANGE_)) return;}
		if (fr -> id ("cond")) {if (! read_instruction_args (fr, i, COND_)) return;}
		if (fr -> id ("ar")) {if (! read_instruction_args (fr, i, AR_)) return;}
		if (fr -> id ("rel")) {if (! read_instruction_args (fr, i, REL_)) return;}
		if (fr -> id ("key")) {if (! read_instruction_args (fr, i, KEY_)) return;}
		if (fr -> id ("htv")) {if (! read_instruction_args (fr, i, HTV_)) return;}
		if (fr -> id ("ht")) {if (! read_instruction_args (fr, i, HT_)) return;}
		if (fr -> id ("htbv")) {if (! read_instruction_args (fr, i, HTBV_)) return;}
		if (fr -> id ("htb")) {if (! read_instruction_args (fr, i, HTB_)) return;}
		if (fr -> id ("aoff")) {if (! read_instruction_args (fr, i, AOFF_)) return;}
		location++;
		i++;
		if (location == style_program_size - 1) {
			i -> mnemonic = END;
			return;
		}
	}
	i -> mnemonic = END;
}
void script_tables :: reset_instructions (int factory_id) {
	instruction * i = instruction_tables [factory_id];
	i -> mnemonic = END;
}
script_tables :: script_tables (int multitimbral, int style_program_size) {
	this -> style_program_size = style_program_size;
	this -> multitimbral = multitimbral;
	algos = new scripted_pattern_cpp_pointer [multitimbral];
	style_names = new STRING [multitimbral];
	pattern_names = new STRING [multitimbral];
	instruction_tables = new instruction_pointer [multitimbral];
	for (int ind = 0; ind < multitimbral; ind++) {
		sprintf (style_names [ind], "");
		sprintf (pattern_names [ind], "");
		instruction_tables [ind] = new instruction [style_program_size];
		algos [ind] = new scripted_pattern_cpp (style_names [ind], pattern_names [ind], instruction_tables [ind]);
	}
}
script_tables :: ~ script_tables (void) {
	for (int ind = 0; ind < multitimbral; ind++) {
		delete [] instruction_tables [ind];
	}
	delete [] instruction_tables;
	delete [] pattern_names;
	delete [] style_names;
	delete [] algos;
}

char * scripted_pattern_bank :: toString (void) {return msb_name;}
char * scripted_pattern_bank :: toString (int lsb) {
	if (lsb < 0 || lsb > 127) return default_lsb_name;
	if (strcmp (lsb_name [lsb], "") == 0) return default_lsb_name;
	return lsb_name [lsb];
}
void scripted_pattern_bank :: extract_banks (SetupFileReader * fr) {
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
			string_copy (name, "SUB PATTERN BANK");
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
pattern_pointer scripted_pattern_bank :: get_pattern (int lsb, int program, parameter_block * pb) {
	SetupFileReader * fr = new SetupFileReader (msb_file_name);
	pattern * pp = extract_program (fr, lsb, program, pb);
	delete fr;
	return pp;
}
pattern * scripted_pattern_bank :: extract_program (SetupFileReader * fr, int lsb, int program, parameter_block * pb) {
	int location;
	int program_location;
	STRING file_name;
	pattern * pp = NULL;
	if (! fr -> get_id ("msb")) return NULL;
	while (fr -> get_id ()) {
		if (fr -> id ("lsb")) {
			location = -1;
			while (fr -> get_id ()) {
				if (fr -> id ("location")) {
					if (! fr -> get_int ()) return NULL;
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
								if (! fr -> get_int ()) return NULL;
								program_location = fr -> int_symbol;
							}
							if (fr -> id ("default")) {program_location = program;}
							if (fr -> id ("file")) {
								if (! fr -> get_string ()) return NULL;
								sprintf (file_name, "%s%s", directory, fr -> symbol);
							}
							fr -> skip ();
						}
						if (program_location == program) {
							fr -> close ();
							scripts -> reset_instructions (pb -> factory_id);
							SetupFileReader * sr = new SetupFileReader (file_name);
							scripts -> read_scripted_pattern (sr, pb -> factory_id);//, 0);//pb -> system -> arranger_assign);
							delete sr;
//							drop_instructions (sth -> root, instruction_tables [pb -> factory_id]);
							return scripts -> algos [pb -> factory_id];
						}
					} else fr -> skip ();
				}
			}
		} else fr -> skip ();
	}
	fr -> close ();
	return NULL;
}
scripted_pattern_bank :: scripted_pattern_bank (STRING text, synthesizer * sth, int style_program_size) {
	scripts = new script_tables (sth -> multitimbral, style_program_size);
	sprintf (directory, "%s%s", sth -> root_directory, text);
	sprintf (msb_file_name, "%s%s", directory, "pattern_msb.txt");
	string_copy (msb_name, "PATTERN BANK");
	for (int ind = 0; ind < 128; ind++) string_copy (lsb_name [ind], "");
	string_copy (default_lsb_name, "<not installed>");
	SetupFileReader * fr = new SetupFileReader (msb_file_name);
	extract_banks (fr);
	delete fr;
}
scripted_pattern_bank :: ~ scripted_pattern_bank (void) {delete scripts;}

////////////////
// STYLE BANK //
////////////////

void style_bank :: program_change (int lsb, int program, parameter_block * pb) {
	SetupFileReader * fr = new SetupFileReader (msb_file_name);
	extract_program (fr, lsb, program, pb);
	delete fr;
}
void style_bank :: extract_program (SetupFileReader * fr, int lsb, int program, parameter_block * pb) {
	int location;
	int program_location;
	int track;
	int x, y;
	PrologRoot * root;
	STRING file_name;
	SetupFileReader * sr;
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
//							if (fr -> id ("file")) {
//								if (! fr -> get_string ()) return;
//								sprintf (file_name, "%s%s", directory, fr -> symbol);
//							}
							if (fr -> id ("name") && program_location == program && pb -> system -> arranger_assign < 0x7f) {
								if (! fr -> get_string ()) return;
								sprintf (scripts -> style_names [pb -> factory_id], "%s", fr -> symbol);
							}
							if (fr -> id ("sound")) {
								if (! fr -> get_int ()) return;
								track = fr -> int_symbol;
								if (program_location == program && track == pb -> system -> arranger_assign) {
									if (! fr -> get_string ()) return;
									sprintf (file_name, "%s%s", directory, fr -> symbol);
									sr = new SetupFileReader (file_name);
									pb -> voice_init ();
									pb -> read_program (sr);
									pb -> notify_program_loaded ();
									delete sr;
								}
							}
							if (fr -> id ("tempo") && program_location == program && pb -> system -> arranger_assign < 0x7f) {
								if (! fr -> get_int ()) return;
								x = fr -> int_symbol;
								if (! fr -> get_int ()) return;
								y = fr -> int_symbol;
								root = pb -> sth -> root;
								// to do
//								if (root != NULL) root -> transportTempo (x, y);
							}
							if (fr -> id ("division") && program_location == program && pb -> system -> arranger_assign < 0x7f) {
								if (! fr -> get_int ()) return;
								x = fr -> int_symbol;
								if (! fr -> get_int ()) return;
								y = fr -> int_symbol;
								root = pb -> sth -> root;
								// to do
//								if (root != NULL) root -> transportDivision (x, y);
							}
							if (fr -> id ("pattern")) {
								if (! fr -> get_int ()) return;
								track = fr -> int_symbol;
								if (program_location == program && track == pb -> system -> arranger_assign) {
									if (! fr -> get_string ()) return;
									sprintf (file_name, "%s%s", directory, fr -> symbol);
									scripts -> reset_instructions (pb -> factory_id);
									sr = new SetupFileReader (file_name);
									scripts -> read_scripted_pattern (sr, pb -> factory_id);//, pb -> system -> arranger_assign);
									delete sr;
									pb -> pattern_head = scripts -> algos [pb -> factory_id];
									pb -> notify_pattern_change ();
								}
							}
							fr -> skip ();
						}
					} else fr -> skip ();
				}
			}
		} else fr -> skip ();
	}
	fr -> close ();
}
char * style_bank :: toString (void) {return msb_name;}
char * style_bank :: toString (int lsb) {
	if (lsb < 0 || lsb > 127) return default_lsb_name;
	if (strcmp (lsb_name [lsb], "") == 0) return default_lsb_name;
	return lsb_name [lsb];
}
void style_bank :: extract_banks (SetupFileReader * fr) {
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
			string_copy (name, "SUB STYLE BANK");
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
	fr -> skip ();
}

style_bank :: style_bank (STRING text, synthesizer * sth, int style_program_size) {
	scripts = new script_tables (sth -> multitimbral, style_program_size);
	sprintf (directory, "%s%s", sth -> root_directory, text);
	sprintf (msb_file_name, "%s%s", directory, "style_msb.txt");
	string_copy (msb_name, "STYLE BANK");
	for (int ind = 0; ind < 128; ind++) string_copy (lsb_name [ind], "");
	string_copy (default_lsb_name, "<not installed>");
	SetupFileReader * fr = new SetupFileReader (msb_file_name);
	extract_banks (fr);
	delete fr;
}
style_bank :: ~ style_bank (void) {delete scripts;}

