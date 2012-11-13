///////////////////////////////////////
// Copyright (C) 2002 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "setup_file_reader.h"
#include <string.h>

SetupFileReader :: SetupFileReader (char * file_name) {
	setup_file = NULL;
	setup_file = fopen (file_name, "rb");
	act_char = 0;
}

SetupFileReader :: ~ SetupFileReader (void) {
	close ();
}

void SetupFileReader :: close (void) {
	if (setup_file == NULL) return;
	fclose (setup_file);
	setup_file = NULL;
}

bool SetupFileReader :: file_not_found (void) {
	if (setup_file == NULL) return true;
	return false;
}

int SetupFileReader :: get_char (void) {
	act_char = fgetc (setup_file);
	return act_char;
}

void SetupFileReader :: get_symbol (void) {
	bool negative = false;
	char * symbol_pointer;
	double fraction;
	if (setup_file == NULL) {
		symbol_control = 0;
		return;
	}
	while (act_char < 33) {
		if (act_char == EOF) {
			symbol_control = 0;
			close ();
			return;
		}
		get_char ();
	}
	if (act_char == '[') {
		symbol_control = 1;
		get_char ();
		return;
	}
	if (act_char == ']') {
		symbol_control = 2;
		get_char ();
		return;
	}
	if ((act_char >= 'a' && act_char <= 'z') || act_char == '_' || (act_char >= 'A' && act_char <= 'Z')) {
		symbol_pointer = symbol;
		while ((act_char >= 'a' && act_char <= 'z') || act_char == '_' || (act_char >= 'A' && act_char <= 'Z') || (act_char >= '0' && act_char <= '9')) {
			* (symbol_pointer++) = act_char;
			get_char ();
		}
		symbol_control = 3;
		* symbol_pointer = '\0';
		return;
	}
	if (act_char == '-') {
		negative = true;
		get_char ();
	}
	if (act_char >= '0' && act_char <= '9') {
		int_symbol = act_char - '0';
		get_char ();
		while (act_char >= '0' && act_char <= '9') {
			int_symbol *= 10;
			int_symbol += act_char - '0';
			get_char ();
		}
		if (act_char == '.') {
			get_char ();
			float_symbol = int_symbol;
			symbol_control = 6;
			fraction = 1.0 / 10.0;
			int_symbol = 0;
			while (act_char >= '0' && act_char <= '9') {
				float_symbol += ((double) (act_char - '0')) * fraction;
				fraction /= 10.0;
				get_char ();
			}
			if (negative) float_symbol = - float_symbol;
			return;
		}
		symbol_control = 5;
		if (negative) int_symbol = 0 - int_symbol;
		return;
	}
	if (negative) {
		symbol_control = 7;
		return;
	}
	if (act_char == '"') {
		get_char ();
		symbol_pointer = symbol;
		while (act_char > 0 && act_char != '"') {
			if (act_char == '\\')
				get_char ();
			* (symbol_pointer++) = act_char;
			get_char ();
		}
		* symbol_pointer = '\0';
		if (act_char <= 0) {
			symbol_control = 0;
			return;
		}
		symbol_control = 4;
		get_char ();
		return;
	}
	get_char ();
	symbol_control = 7;
}

void SetupFileReader :: skip (void) {
	int ind = 1;
	while (ind > 0) {
		get_symbol ();
		switch (symbol_control) {
		case 0: ind = 0; break;
		case 1: ind++; break;
		case 2: ind--; break;
		default: break;
		}
	}
}

bool SetupFileReader :: id (char * name) {
	if (symbol_control != 1) return false;
	return strcmp (symbol, name) == 0;
}

bool SetupFileReader :: get_string (void) {
	get_symbol ();
	return symbol_control == 4;
}

bool SetupFileReader :: get_int (void) {
	get_symbol ();
	return symbol_control == 5;
}

bool SetupFileReader :: get_float (void) {
	get_symbol ();
	return symbol_control == 6;
}

bool SetupFileReader :: get_id (void) {
	get_symbol ();
	if (symbol_control != 3) return false;
	get_symbol ();
	return symbol_control == 1;
}

bool SetupFileReader :: get_id (char * name) {
	get_symbol ();
	if (symbol_control != 3 || strcmp (symbol, name) != 0) return false;
	get_symbol ();
	return symbol_control == 1;
}


SetupFileWriter :: SetupFileWriter (char * file_name) {
	setup_file = NULL;
	setup_file = fopen (file_name, "w");
	nest_level = 0;
	space = false;
	nest = false;
}

SetupFileWriter :: ~ SetupFileWriter (void) {
	close ();
}

void SetupFileWriter :: close (void) {
	if (setup_file == NULL) return;
	fclose (setup_file);
	setup_file = NULL;
}

void SetupFileWriter :: drop_nest (void) {
	if (nest) for (int ind = 0; ind < nest_level; ind++) fprintf (setup_file, "	");
}

void SetupFileWriter :: drop_space (void) {
	if (space) fprintf (setup_file, " ");
}

void SetupFileWriter :: drop_nest_space (void) {
	if (nest) for (int ind = 0; ind < nest_level; ind++) fprintf (setup_file, "	");
	if (space) fprintf (setup_file, " ");
}

bool SetupFileWriter :: put_char (int ch) {
	if (! setup_file) return false;
	putc (ch, setup_file);
	return true;
}

bool SetupFileWriter :: write_id (char * id) {
	if (! setup_file) return false;;
	drop_nest_space ();
	fprintf (setup_file, "%s [", id);
	space = false;
	nest = false;
	nest_level++;
	return true;
}

bool SetupFileWriter :: writeln_id (char * id) {
	if (! setup_file) return false;
	drop_nest_space ();
	fprintf (setup_file, "%s [\n", id);
	space = false;
	nest = true;
	nest_level++;
	return true;
}

bool SetupFileWriter :: writeln_id (char * id, int ind) {
	if (! setup_file) return false;
	drop_nest_space ();
	fprintf (setup_file, "%s [%i]\n", id, ind);
	space = false;
	nest = true;
	return true;
}

bool SetupFileWriter :: writeln_id (char * id, int ind, int sub) {
	if (! setup_file) return false;
	drop_nest_space ();
	fprintf (setup_file, "%s [%i %i]\n", id, ind, sub);
	space = false;
	nest = true;
	return true;
}

bool SetupFileWriter :: write_earth (void) {
	if (! setup_file) return false;
	nest_level--;
	drop_nest ();
	fprintf (setup_file, "]");
	space = true;
	nest = false;
	return true;
}

bool SetupFileWriter :: writeln_earth (void) {
	if (! setup_file) return false;
	nest_level--;
	drop_nest ();
	fprintf (setup_file, "]\n");
	space = false;
	nest = true;
	return true;
}

bool SetupFileWriter :: write (int ind) {
	if (! setup_file) return false;
	drop_nest_space ();
	fprintf (setup_file, "%i", ind);
	space = true;
	nest = false;
	return true;
}

bool SetupFileWriter :: writeln (int ind) {
	if (! setup_file) return false;
	drop_nest_space ();
	fprintf (setup_file, "%i\n", ind);
	space = false;
	nest = true;
	return true;
}

bool SetupFileWriter :: write (char * text) {
	if (! setup_file) return false;
	drop_nest_space ();
	fprintf (setup_file, "\"%s\"", text);
	space = true;
	nest = false;
	return true;
}

bool SetupFileWriter :: writeln (char * text) {
	if (! setup_file) return false;
	drop_nest_space ();
	fprintf (setup_file, "\"%s\"\n", text);
	space = false;
	nest = true;
	return true;
}
