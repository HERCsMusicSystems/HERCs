///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _WAVETABLE__BANK_
#define _WAVETABLE_BANK_

#include "string.h"

class wave_cell {
public:
	int vertical;
	STRING wave_name;
	float data [32768]; // 128 waves * 256 samples;
	wave_cell * next;
	wave_cell * find (char * name);
	void read_wave (SetupFileReader * fr);
	wave_cell (char * name, wave_cell * next, int vertical);
};

class wavetable_location {
public:
	bool inactive;
	wave_cell * from_cell;
	wave_cell * to_cell;
	int from_index;
	int index_length;
public:
	void ground (void);
	void set (int from, int to, wave_cell * fc, wave_cell * tc);
	float get (unsigned int time, unsigned int freq, float amp, int index);
};

class wavetable_bank_cpp;

class wavetable_algo : public algo_cpp {
public:
	wavetable_bank_cpp * bank;
	wavetable_location * wavetable_head;
	STRING lsb_names [16];
	float vertical;
	bool filter_on_2;
	int waves_to;
	int default_0;
	bool vector_processing;
public:
	void waves (int number);
	void filtered_waves (int number);
	void reset_waves (void);
	void reset_waves (int index);
	virtual char * toString (parameter_block * pb);
	virtual char * toString (parameter_block * pb, int ind);
	virtual void data_constructor (oscillator * osc);
	virtual int code (oscillator * osc);
	virtual char * get_name (int ind);
	virtual int get_default (int ind);
	virtual void notify_algo_parameter_changed (parameter_block * pb, int ind);
	virtual void notify_all_algo_parameters_changed (parameter_block * pb);
	wavetable_algo (wavetable_bank_cpp * bank, wavetable_location * head, int vertical);
};

typedef wavetable_algo * wavetable_algo_pointer;

class wavetable_bank_cpp : public algo_bank {
private:
	int vertical, stripes, multitimbral;
	STRING directory;
	STRING msb_name;
	wave_cell * cell_head;
	wavetable_location * wavetables;
	wavetable_algo_pointer * algos;
public:
	STRING file_name;
private:
	void configure (SetupFileReader * fr, char * directory);
public:
	void read_program (SetupFileReader * fr, int program, wavetable_location * wl, char * lsb_name);
	virtual algo_pointer get_algo (int lsb, int program, parameter_block * pb);
	virtual char * toString (void);
	virtual char * toString (int lsb);
	wave_cell * import_wave (char * name);
	wavetable_bank_cpp (char * text, synthesiser * sth, int vertical);
	~ wavetable_bank_cpp (void);
};

#endif
