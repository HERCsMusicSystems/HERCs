///////////////////////////////////////
// Copyright (C) 2002 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////


#ifndef _SYMBOL_READER_
#define _SYMBOL_READER_

#include "stdio.h"

#include "hercs_export_definitions.h"

class HERCs_SDK_EXPORT SetupFileReader;
class HERCs_SDK_EXPORT SetupFileWriter;


#define SYMBOL_SIZE 1000

class SetupFileReader {
public:
	int symbol_control;
		// 0: EOF
		// 1: [
		// 2: ]
		// 3: identifier
		// 4: string
		// 5: integer
		// 6: float
		// 7: unknown
	int int_symbol;
	double float_symbol;
	char symbol [SYMBOL_SIZE];
	FILE * setup_file;
	int act_char;

	SetupFileReader (char * file_name);
	~ SetupFileReader (void);

	void close (void);
	bool file_not_found (void);
	int get_char (void);
	void get_symbol (void);
	void skip (void);
	bool id (char * name);
	bool get_string (void);
	bool get_int (void);
	bool get_float (void);
	bool get_id (void);
	bool get_id (char * name);
};

class SetupFileWriter {
private:
	void drop_nest (void);
	void drop_space (void);
	void drop_nest_space (void);

public:

	FILE * setup_file;
	int nest_level;
	bool space;
	bool nest;

	SetupFileWriter (char * file_name);
	~ SetupFileWriter (void);

	bool put_char (int ch);
	bool write_id (char * id);
	bool write_earth (void);
	bool write (int ind);
	bool write (char * text);
	bool writeln_id (char * id);
	bool writeln_id (char * id, int ind);
	bool writeln_id (char * id, int ind, int sub);
	bool writeln_earth (void);
	bool writeln (int ind);
	bool writeln (char * text);
	void close (void);
};

HERCs_SDK_EXPORT extern void message (char * head);
HERCs_SDK_EXPORT extern void message (char * head, int ind);
HERCs_SDK_EXPORT extern void message (char * head, int ind, int sub);
HERCs_SDK_EXPORT extern void message (char * head, int ind, int sub, int id);
HERCs_SDK_EXPORT extern void get_registry_key (char * root, char * key, char * value);
HERCs_SDK_EXPORT extern unsigned long int get_volume_id (char * path);

#endif