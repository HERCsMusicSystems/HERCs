///////////////////////////////////////
// Copyright (C) 2008 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _HERCs_FRONT_PANEL_BASE_
#define _HERCs_FRONT_PANEL_BASE_

#include "midi_stream.h"

enum {nrpn_name = 256, nrpn_name_character, nrpn_tune_key, nrpn_midi, nrpn_midi_extension, nrpn_device, nrpn_store};

typedef char string256 [256];
class hercs_front_panel : public midi_nrpn_reader {
public:
	int factory_id;
	int part_id;
	int part_ids [16];
	int stripe;
	int stereo_stripe;
	int dsp_id;
	int main_mode;
	int type;
	int sensitivity_type;
	int dsp_sensitivity_controller; // 0 = value, 1 = d/w, 2 = X, 3 = y
	int nrpn_msb, nrpn_lsb;
	int data;
	int range_min, range_max;
	int tune_key, name_character;
	bool msb_only;
	int bank_change_mode;
	int command_mode; // 0 = program change, 1 = reset, 2 = store
	int * bank_msb, * bank_lsb;
	int store_location;
	bool control_store_required;
	int preset_bank_msb, preset_bank_lsb, user_bank_msb, user_bank_lsb;
	int algo_bank_msb, algo_bank_lsb;
	int arp_bank_msb, arp_bank_lsb;
	int dsp_algo_bank_msb, dsp_algo_bank_lsb, preset_dsp_bank_msb, preset_dsp_bank_lsb, user_dsp_bank_msb, user_dsp_bank_lsb;
	int style_bank_msb, style_bank_lsb;
	int program_parameter, system_parameter, control_parameter, sensitivity_parameter, stripe_parameter, vector_parameter, lfo_parameter, arpeggiator_parameter, dsp_parameter, dsp_p_parameter, algo_parameter, transport_parameter, midi_parameter;
	char parameter_text [256];
	char status_text [256];
	char program_name [256];
	char bank_msb_name [256];
	char bank_lsb_name [256];
	char category_name [256];
	char algo_name [256];
	char pattern_name [256];
	char dsp_algo_name [256];
	char style_name [256];
	char algo_parameter_name [256];
	char dsp_parameter_name [256];
	string256 algo_parameter_names [16];
	string256 dsp_algo_parameter_names [8];
	int hercules_number, hercules_stereo_number;
	int total_dsp;
	int encoder_sensitivity;
	midi_stream * line;
	int panel_controllers_skip;
	int controller_ids [23];
public:
	virtual void midi_system_exclusive (midi_stream * line);
	virtual void midi_nrpn (int nrpn_msb, int nrpn_lsb, int data_ms, int data_lsb);
	void send_parameter_request (int channel, int extended_channel, int msb, int lsb);
	void send_parameter_request ();
	void send_panel_controllers_request (void);
	void send_extra_requests (int extended_channel);
	void send_device_request (int device);
public:
	virtual void console_print (char * text);
	virtual void console_operations (int a, int b, int c);
	virtual void text_operations (int selector, char * text);
	virtual void identity_operations (int * identity);
	virtual void error_operations (int channel, int code, int msb, int lsb, char * error);
public:
	virtual void change_main_mode (int mode);
	virtual void change_type (int type);
	virtual void change_sensitivity_type (int type);
	virtual void change_dsp_sensitivity_controller (int type);
	virtual void change_parameter (int parameter);
	virtual void change_parameter_name (int parameter, char * text);
	virtual void change_device (int device);
	virtual void change_stripe (int stripe);
	virtual void change_stereo_stripe (int stripe);
	virtual void change_dsp (int dsp);
	virtual void change_algo_parameter_base (int parameter);
	virtual void change_bank_type_selector (int type);
	virtual void change_command_mode (int mode);
	virtual void change_parameter_text (void);
	virtual void change_status_text (void);
	virtual void open_file_for_input (char * file_name);
	virtual void open_file_for_record (int seconds, char * file_name);
	virtual void update_controllers (int nrpn_lsb, int data_msb, int data_lsb, bool separate_vector_coordinates);
	void encoder_change (midi_stream * line, int delta);
	void controller_change (int controller_id, double value);
	void press_part_button (int button);
	void press_mode_button (int button);
	void press_type_button (int button);
	void press_sensitivity_button (int button);
	void press_dsp_sensitivity_button (int button);
	void press_parameter_button (int button);
	void press_bank_type_button (int button);
	void press_command_button (int button);
	void reset_command (void);
	void store_command (int location);
	virtual void calculate_nrpn_from_mode (void);
	virtual void calculate_mode_from_nrpn (void);
	void calculate_sensitivity_block_display (void);
	void calculate_lfo_sensitivity_block_display (double db);
	void calculate_dsp_display (void);
	void calculate_display (void);
	void calculate_status (void);
	void set_midi_out (midi_stream * line);
	void send_control_store_request (void);
	hercs_front_panel (void);
};

enum {
	mode_button_program = 0,
	mode_button_control,
	mode_button_stripe,
	mode_button_lfo,
	mode_button_sensitivity,
	mode_button_arpeggiator,
	mode_button_vector,
	mode_button_dsp,
	mode_button_dsp_parameter,
	mode_button_algo_parameter,
	mode_button_system,
	mode_button_midi,
	part_button_0, part_button_1, part_button_2, part_button_3, part_button_4, part_button_5, part_button_6, part_button_7, part_button_8, part_button_9,
	type_button_freq, type_button_amp, type_button_index, type_button_pan,
	sensitivity_button_lc, sensitivity_button_bp, sensitivity_button_rc,
	pb0, pb1, pb2, pb3, pb4, pb5, pb6, pb7, pb8, pb9, pb10, pb11, pb12, pb13, pb14, pb15, pb16, pb17,
	extra_button
};

enum {
	extra_dec_button = extra_button,
	extra_inc_button, extra_1_button, extra_8_button, extra_128_button,
	extra_reset_button, extra_enter_button, extra_store_button
};

class hercs_dx_type_panel : public hercs_front_panel {
public:
	int algo_parameter_base;
	int previous_algo_parameter_base;
	int previous_main_mode;
	int previous_dsp_id;
public:
	hercs_dx_type_panel (void);
	virtual void change_main_mode (int mode);
	virtual void change_type (int type);
	virtual void change_sensitivity_type (int type);
	virtual void change_dsp_sensitivity_controller (int type);
	virtual void change_parameter (int parameter);
	virtual void change_device (int device);
	virtual void change_stripe (int stripe);
	virtual void change_stereo_stripe (int stripe);
	virtual void change_dsp (int dsp);
	virtual void set_button (int button, double value);
	virtual void change_algo_parameter_base (int parameter);
	virtual void change_bank_type_selector (int type);
	virtual void change_command_mode (int mode);
	void press_part_button_dx (int button);
	void press_parameter_button_dx (int button);
	void press_zoom_button (int button);
	void set_dsp_parameter_names (void);
	void set_dsp_p_parameter_names (void);
	void set_indexed_parameter_names (int shift);
	void set_all_buttons (void);
	void set_type_buttons (void);
	void set_type_buttons (int mode); // 0 = inactive, 1 = full, 2 = stripe/stereo
	void set_sensitivity_buttons (void);
	void set_parameter_buttons (void);
	void set_parameter_buttons (int parameter);
	void set_part_buttons (void);
	void set_part_buttons (int part);
	void set_main_buttons (void);
	void set_command_buttons (void);
	void set_button_names (void);
	void set_zoom_buttons (void);
	virtual void calculate_nrpn_from_mode (void);
private:
	int get_system_parameter (void);
	int get_system_parameter (int parameter);
	int get_control_parameter (void);
	int get_type_parameter (void);
};

#endif
