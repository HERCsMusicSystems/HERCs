///////////////////////////////////////
// Copyright (C) 2005 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _SCRIPTED_PATTERN_BANK_
#define _SCRIPTED_PATTERN_BANK_

#include "transport.h"

class scripted_pattern_bank;
class style_bank;
class script_tables;

// scripted instructions
#define END_     0x00, 0 // end [] => end of sequence
#define WT_      0x01, 1 // wt [x] => wait x ticks
#define	WB_      0x02, 1 // wb [x] => wait x beats
#define WBR_     0x03, 1 // wbr [x] => wait x bars
#define WH_      0x04, 0 // wh [] => wait half beat (rounded up)
#define UV_      0x05, 0 // uw [] => update variation
#define UT_      0x06, 0 // ut [] => update transcriptions
#define UPDATE_  0x07, 0 // update [] => update variation and transcriptions
#define ENTER_   0x08, 2 // enter [variation location] => entry point
#define CHANGE_  0x09, 2 // change [v e] => change variation at entry point
#define COND_    0x0a, 3 // cond [i v e] => conditional variation at entry point
#define AR_      0x0b, 3 // ar [key octave velocity] => arrange note, if no base then no sound
#define REL_     0x0c, 2 // ar [key velocity] => arrange note, if no base then no sound
#define KEY_     0x0d, 2 // key [key velocity] => keyon (velocity = 0 = keyoff)
#define HTV_     0x0e, 3 // htv [counter octave velocity] => hit top, if nothing played then no sound
#define HT_      0x0f, 2 // ht [counter octave] => hit top, if nothing played then no sound
#define HTBV_    0x10, 3 // htbv [counter octave velocity] => hit bottom, if nothing played then no sound
#define HTB_     0x11, 2 // htb [counter octave] => hit bottom, if nothing played then no sound
#define AOFF_    0x12, 0 // aoff [] => all keys off

#define END     0x00 // end [] => end of sequence
#define WT      0x01 // wt [x] => wait x ticks
#define	WB      0x02 // wb [x] => wait x beats
#define WBR     0x03 // wbr [x] => wait x bars
#define WH      0x04 // wh [] => wait half beat (rounded up)
#define UV      0x05 // uw [] => update variation
#define UT      0x06 // ut [] => update transcriptions
#define UPDATE  0x07 // update [] => update variation and transcriptions
#define ENTER   0x08 // enter [variation location] => entry point
#define CHANGE  0x09 // change [v e] => change variation at entry point
#define COND    0x0a // cond [i v e] => conditional variation at entry point
#define AR      0x0b // ar [key octave velocity] => arrange note, if no base then no sound
#define REL     0x0c // ar [key velocity] => arrange note, if no base then no sound
#define KEY     0x0d // key [key velocity] => keyon (velocity = 0 = keyoff)
#define HTV     0x0e // htv [counter octave velocity] => hit top, if nothing played then no sound
#define HT      0x0f // ht [counter octave] => hit top, if nothing played then no sound
#define HTBV    0x10 // htbv [counter octave velocity] => hit bottom, if nothing played then no sound
#define HTB     0x11 // htb [counter octave] => hit bottom, if nothing played then no sound
#define AOFF    0x12 // aoff [] => all keys off

class instruction {
public:
	int mnemonic;
	int arg1;
	int arg2;
	int arg3;
};

typedef instruction * instruction_pointer;

class scripted_pattern_cpp : public pattern {
private:
	char * style_pointer;
	char * name_pointer;
	instruction * ip;
public:
	virtual char * styleName (parameter_block * pb);
	virtual char * toString (parameter_block * pb);
	virtual void tick_code (arranger * arp);
	virtual void beat_code (arranger * arp);
	virtual void bar_code (arranger * arp);
	virtual void variation_code (arranger * arp);
	virtual void transcription_code (arranger * arp);
	virtual int get_tempo (void);
	void code (arranger * arp);
	void go_to (arranger * arp, int variation, int location);
	scripted_pattern_cpp (char * style_pointer, char * name_pointer, instruction * ip);
};

typedef scripted_pattern_cpp * scripted_pattern_cpp_pointer;

class script_tables {
public:
	int style_program_size;
	int multitimbral;
	scripted_pattern_cpp_pointer * algos;
	instruction_pointer * instruction_tables;
	STRING * style_names;
	STRING * pattern_names;
public:
	void read_scripted_pattern (SetupFileReader * fr, int factory_id);//, int track);
	void reset_instructions (int factory_id);
	script_tables (int multitimbral, int style_program_size);
	~ script_tables (void);
};

class scripted_pattern_bank : public pattern_bank {
private:
	script_tables * scripts;
	STRING directory;
	STRING msb_file_name;
	STRING msb_name;
	STRING lsb_name [128];
	STRING default_lsb_name;
	void extract_banks (SetupFileReader * fr);
	pattern * extract_program (SetupFileReader * fr, int lsb, int program, parameter_block * pb);
public:
	virtual pattern_pointer get_pattern (int lsb, int program, parameter_block * pb);
	virtual char * toString (void);
	virtual char * toString (int lsb);
	scripted_pattern_bank (STRING text, synthesiser * sth, int style_program_size);
	~ scripted_pattern_bank (void);
};

class style_bank : public bank {
private:
	script_tables * scripts;
	STRING directory;
	STRING msb_file_name;
	STRING msb_name;
	STRING lsb_name [128];
	STRING default_lsb_name;
	void extract_banks (SetupFileReader * fr);
	void extract_program (SetupFileReader * fr, int lsb, int program, parameter_block * pb);
public:
	virtual void program_change (int lsb, int program, parameter_block * pb);
	virtual char * toString (void);
	virtual char * toString (int lsb);
	style_bank (STRING text, synthesiser * sth, int style_program_size);
	~ style_bank (void);
};

#endif
