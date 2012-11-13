///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _INFO_DISPLAY
#define _INFO_DISPLAY

#include "data.h"

class info_display : public midi_reader {
private:
	int nrpn_msb;
	int nrpn_lsb;
	int data_lsb;
	int vector_x;
	int vector_y;
	int pitch_id;
	int freq_id;
	int amp_id;
	int index_id;
	int attack_id;
	int decay_id;
	int sustain_id;
	int release_id;
	int speed_id;
	int vibrato_id;
	int tremolo_id;
	int lfo_index_id;
	int lfo_pan_id;
	int vector_x_id;
	int vector_y_id;
	int volume_id;
	int pan_id;
	int hold_id;
	int porta_id;
	int porta_time_id;
	int porta_mode_id;
public:
	int freq_msb;
	int freq_lsb;
public:
	int part_number;
	int device_id_number;
	int midi_channel_number;
	int category_number;
	int program_number;
	int bank_msb_number;
	int bank_lsb_number;
public:
	STRING part_name;
	STRING device_id_name;
	STRING midi_channel_name;
	STRING category_name;
	STRING program_name;
//	STRING program_value_name;
	STRING bank_msb_name;
	STRING bank_lsb_name;
	STRING control_parameter_name;
	STRING control_value_name;
	STRING system_parameter_name;
	STRING system_value_name;
	STRING edit_parameter_name;
	STRING edit_value_name;
	STRING algo_name;
	STRING * algo_stripe_names;
	info_display (config * cfg);
	~ info_display (void);
	virtual void midi_programchange (int channel, int program);
	virtual void midi_control (int channel, int controller, int value);
	virtual void midi_system_exclusive (midi_stream * line);
	void midi_nrpn (int data_msb);
private:
	void drop_eg_level (int selector, char * header);
	void drop_eg_time (int selector, char * header);
};

#endif