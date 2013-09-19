///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _ROM_PLAYER_BANK_
#define _ROM_PLAYER_BANK_

class rom_wave {
public:
	STRING wave_name;
	int data_size;
	int loop_end, loop_size;
	int * data;
	bool tail_present;
	rom_wave * next;
	// resolution related
	float ratio;
	float mega_vertical;
	int bit_shift;
	int bit_mask;
	int bit_value;
	void calculate_ratio (int bits, int center_freq, int key, int horizontal, int hercs_vertical);
	rom_wave * find (char * name);
	void read_scripted_waveform (SetupFileReader * fr, int center_freq, int key, int vertical, int head_room);
	rom_wave (char * name, rom_wave * next, int center_freq, int key, int horizontal, int vertical, int head_room, int loop_start, int loop_end, bool tail_present);
	rom_wave (char * name, rom_wave * next, int center_freq, int key, int horizontal, int vertical, int head_room);
	~ rom_wave (void);
};

typedef rom_wave * rom_wave_pointer;

class rom_player_bank;

class rom_player_algo : public algo_cpp {
public:
	rom_player_bank * bank;
	rom_wave_pointer * wave_head;
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
public:
	virtual void data_constructor (oscillator * osc);
	virtual void retrigger (oscillator * osc);
	virtual void release (oscillator * osc);
	virtual char * toString (parameter_block * pb);
	virtual char * toString (parameter_block * pb, int ind);
	virtual int code (oscillator * osc);
	virtual char * get_name (int ind);
	virtual int get_default (int ind);
	virtual void notify_algo_parameter_changed (parameter_block * pb, int ind);
	virtual void notify_all_algo_parameters_changed (parameter_block * pb);
	rom_player_algo (rom_player_bank * bank, rom_wave_pointer * head, int vertical);
};

typedef rom_player_algo * rom_player_algo_pointer;

class rom_player_bank : public algo_bank {
private:
	int center_freq, horizontal, vertical, head_room, stripes, multitimbral;
	STRING directory;
	STRING msb_name;
	rom_wave * wave_head;
	rom_wave_pointer * wave_table;
	rom_player_algo_pointer * algos;
public:
	STRING file_name;
private:
	void configure (SetupFileReader * fr, char * directory);
public:
	void read_program (SetupFileReader * fr, int program, rom_wave_pointer * rp, char * lsb_name);
	virtual algo_pointer get_algo (int lsb, int program, parameter_block * pb);
	virtual char * toString (void);
	virtual char * toString (int lsb);
public:
	rom_wave * import_wave (char * name, int key, int loop_start, int loop_end, bool tail_present);
	rom_player_bank (char * text, synthesiser * sth, int center_freq, int horizontal, int vertical, int head_room);
	~ rom_player_bank (void);
};

#endif
