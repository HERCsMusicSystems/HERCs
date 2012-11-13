///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _DATA
#define _DATA

#include "setup_file_reader.h"
#include "config.h"
#include "hercs_prolog_sdk.h"

class HERCs_SDK_EXPORT sub_parameter_block;
class HERCs_SDK_EXPORT adsr_parameter_block;
class HERCs_SDK_EXPORT sensitivity_parameter_block;
class HERCs_SDK_EXPORT lfo_sensitivity_parameter_block;
class HERCs_SDK_EXPORT stripe_parameter_block;
class HERCs_SDK_EXPORT system_parameter_block;
class HERCs_SDK_EXPORT stereo_stripe_parameter_block;
class HERCs_SDK_EXPORT controllers_parameter_block;
class HERCs_SDK_EXPORT oscillator_pool;
class HERCs_SDK_EXPORT algo;
class HERCs_SDK_EXPORT arranger;
class HERCs_SDK_EXPORT arpeggiator_pool;
class HERCs_SDK_EXPORT vector;
class HERCs_SDK_EXPORT vector_pool;
class HERCs_SDK_EXPORT pattern;
class HERCs_SDK_EXPORT dsp_algo;
class HERCs_SDK_EXPORT dsp_router;
class HERCs_SDK_EXPORT dsp_parameter_block;
class HERCs_SDK_EXPORT key_queue;
class HERCs_SDK_EXPORT parameter_block;
class HERCs_SDK_EXPORT bank;
class HERCs_SDK_EXPORT synthesizer;
class HERCs_SDK_EXPORT algo_bank;
class HERCs_SDK_EXPORT pattern_bank;
class HERCs_SDK_EXPORT dsp_algo_bank;
class HERCs_SDK_EXPORT preset_algo_bank;
class HERCs_SDK_EXPORT preset_pattern_bank;
class HERCs_SDK_EXPORT preset_dsp_algo_bank;
class HERCs_SDK_EXPORT sound_bank;
class HERCs_SDK_EXPORT user_sound_bank;
class HERCs_SDK_EXPORT dsp_bank;
class HERCs_SDK_EXPORT user_dsp_bank;





#define STRING_SIZE 128
typedef char STRING [STRING_SIZE];
HERCs_SDK_EXPORT extern void string_copy (char * to, char * from);


class sub_parameter_block {
public:
	int bp;
	int lc;
	int rc;
	void voice_init (int selector);
	void voice_init (void);
	void dry_init (void);
	void wet_init (void);
	int get (int ind);
	int get_negative (int ind);
	int get_negative (void);
	void nrpn (int selector, int positive, int negative);
	bool read (SetupFileReader * fr);
	void read (midi_stream * line);
	void drop (SetupFileWriter * tc, int selector, char * header);
	void drop (SetupFileWriter * tc, int bp_selector, int selector, char * header);
	void drop (midi_stream * line, int selector, int id);
	void drop (midi_stream * line, int bp_selector, int selector, int id);
	void drop_indexed (SetupFileWriter * tc, int index, char * header);
	void drop_indexed (midi_stream * line, int index, int id);
	void drop_dry (SetupFileWriter * tc, char * header);
	void drop_dry (midi_stream * line, int id);
	void drop_wet (SetupFileWriter * tc, char * header);
	void drop_wet (midi_stream * line, int id);
};

class adsr_parameter_block {
public:
	int level [4];
	int time [4];
	void voice_init (int selector);
	void nrpn_level (int selector, int value);
	void nrpn_time (int selector, int value);
	bool read (SetupFileReader * fr);
	bool read_level (SetupFileReader * fr);
	bool read_time (SetupFileReader * fr);
	void read (midi_stream * line);
	void read_level (midi_stream * line);
	void read_time (midi_stream * line);
	void read_amp (midi_stream * line);
	void read_amp_level (midi_stream * line);
	void drop (SetupFileWriter * tc, int selector, char * header);
	void drop (midi_stream * line, int selector, int id);
};

class sensitivity_parameter_block {
public:
	sub_parameter_block pitch;
	sub_parameter_block shift;
	sub_parameter_block lfo;
	sub_parameter_block attack;
	sub_parameter_block decay;
	sub_parameter_block sustain;
	sub_parameter_block release;
	sub_parameter_block key;
	sub_parameter_block velocity;
	sub_parameter_block vector_x;
	sub_parameter_block vector_y;
	sub_parameter_block lfo_x;
	sub_parameter_block lfo_y;
	sub_parameter_block key_eg_scaling;
	sub_parameter_block velocity_eg_scaling;
	void voice_init (int selector);
};

class lfo_sensitivity_parameter_block {
public:
	sub_parameter_block speed;
	sub_parameter_block pitch;
	sub_parameter_block vector_x;
	sub_parameter_block vector_y;
	void voice_init (void);
};

class stripe_parameter_block {
public:
//	STRING algo_name;
	bool retrigger;
	int multiplier;
	int freq;
	int amp;
	int index;
	//@ index amp
//	bool index_amp;
	adsr_parameter_block freq_eg;
	adsr_parameter_block amp_eg;
	adsr_parameter_block index_eg;
	int freq_rnd;
	int amp_rnd;
	int index_rnd;
	bool lfo_retrigger;
	int lfo_freq;
	int lfo_amp;
	int lfo_index;
	int speed;
	int phase_shift;
	int pulse_width;
	int wave;
	int lfo_sync;
	int freq_lfo_source;
	int amp_lfo_source;
	int index_lfo_source;
	sensitivity_parameter_block freq_sens;
	sensitivity_parameter_block amp_sens;
	sensitivity_parameter_block index_sens;
	lfo_sensitivity_parameter_block lfo_sens;
	void voice_init (int ind, int selector);
// lemat
public:
	int freq_pitch_lemat;
	int freq_shift_lemat;
	int freq_vector_x_lemat;
	int freq_vector_y_lemat;
	int freq_lemat;

	int amp_pitch_lemat;
	int amp_shift_lemat;
	int amp_vector_x_lemat;
	int amp_vector_y_lemat;
	int amp_lemat;

	int index_pitch_lemat;
	int index_shift_lemat;
	int index_vector_x_lemat;
	int index_vector_y_lemat;
	int index_lemat;

	int lfo_freq_lfo_lemat;
	int lfo_freq_vector_x_lemat;
	int lfo_freq_vector_y_lemat;
	int lfo_freq_lemat;

	int lfo_amp_lfo_lemat;
	int lfo_amp_vector_x_lemat;
	int lfo_amp_vector_y_lemat;
	int lfo_amp_lemat;

	int lfo_index_lfo_lemat;
	int lfo_index_vector_x_lemat;
	int lfo_index_vector_y_lemat;
	int lfo_index_lemat;

	int lfo_speed_pitch_lemat;
	int lfo_speed_speed_lemat;
	int lfo_speed_vector_x_lemat;
	int lfo_speed_vector_y_lemat;
	int lfo_speed_lemat;
};
typedef stripe_parameter_block * stripe_parameter_block_pointer;

class system_parameter_block {
public:

	int device_id;                 // 0 .. 126, 127 = all, 16383 = off
	int midi_channel;              // 0 .. 15, 16 = all, 127 = off
	int midi_channel_extension;    // 0 .. 126, 127 = all, default = 127
	int arranger_assign;           // 0 .. 126 = track, 127 = off
	int reserved_notes;            // default = 0 => all oscillators reserved

	int bank_msb;
	int bank_lsb;
	int program;

	int nrpn_msb;
	int nrpn_lsb;
	int data_lsb;
	int rpn_msb;
	int rpn_lsb;

	int pitch;
	int freq;
	int amp;
	int index;
	int attack;
	int decay;
	int sustain;
	int release;
	int speed;
	int vibrato;
	int tremolo;
	int lfo_index;
	int lfo_pan;
	int volume;
	int pan;
	int vector_x;
	int vector_y;
	int hold;
	int porta;
	int porta_time;
	int porta_mode;
	int dry_wet;
	int global_volume;

	system_parameter_block (void);

};

class stereo_stripe_parameter_block {
public:
	int pan;
	int lfo;
	adsr_parameter_block eg;
	int rnd;
	bool lfo_retrigger;
	int lfo_sync;
	int speed;
	int phase_shift;
	int pulse_width;
	int wave;
	int pan_lfo_source;
	sensitivity_parameter_block sens;
	lfo_sensitivity_parameter_block lfo_sens;
	void voice_init (int ind);
// lemat
public:
	int pan_pitch_lemat;
	int pan_shift_lemat;
	int pan_vector_x_lemat;
	int pan_vector_y_lemat;
	int pan_lemat;

	int lfo_pan_lfo_lemat;
	int lfo_pan_vector_x_lemat;
	int lfo_pan_vector_y_lemat;
	int lfo_pan_lemat;

	int lfo_speed_pitch_lemat;
	int lfo_speed_speed_lemat;
	int lfo_speed_vector_x_lemat;
	int lfo_speed_vector_y_lemat;
	int lfo_speed_lemat;
};
typedef stereo_stripe_parameter_block * stereo_stripe_parameter_block_pointer;

class vector_frame {
public:
	int delay;
	int x, y;
};
typedef vector_frame * vector_frame_pointer;

class controllers_parameter_block {
public:
	int pitch;
	int freq;
	int amp;
	int index;
	int attack;
	int decay;
	int sustain;
	int release;
	int speed;
	int vibrato;
	int tremolo;
	int lfo_index;
	int lfo_pan;
	int vector_x;
	int vector_y;
	int volume;
	int pan;
	bool hold;
	bool porta;
	int porta_time;
	int porta_mode;
	int dry_wet;
	int global_volume;
	bool poly;

	bool volume_reset;
	void reset_all_controllers (void);
	void voice_init (void);

	bool arpeggiator_switch;
	bool arpeggiator_hold;
	int beats_per_seconds;
	int seconds;
	int beats_per_bar;
	int ticks_per_beat;
	int variation;

	bool vector_switch;
	int vector_init_x;
	int vector_init_y;
	int vector_frame_count;
	int vector_frame_selector;
	int vector_resolution;
	int vector_resolution_seconds;
	int vector_trigger;
	int vector_attack;
	int vector_release;
	int vector_loop_type;
	int vector_loop_start;
	int vector_loop_size;
	int vector_loop_delay;

	int key_tuner [128];
	int algo_parameters [128];
};

class algo {
public:
	virtual char * toString (parameter_block * pb);
	virtual char * toString (parameter_block * pb, int ind);
//	virtual void control (int lsb, int program);
	virtual char * get_name (int ind);
	virtual int get_default (int ind);
//	virtual void notify_algo_parameter_changed (parameter_block * pb, int ind);
//	virtual void notify_all_algo_parameters_changed (parameter_block * pb);
};

typedef algo * algo_pointer;
HERCs_SDK_EXPORT extern algo silence;

class pattern {
public:
	virtual char * styleName (parameter_block * pb);
	virtual char * toString (parameter_block * pb);
	virtual int note_length (arranger * arp);
	virtual void on_retrigger (arranger * arp);
	virtual void on_hold_retrigger (arranger * arp);
	virtual void on_start (arranger * arp);
	virtual void on_pause (arranger * arp);
	virtual void on_stop (arranger * arp);
	virtual void tick_code (arranger * arp);
	virtual void beat_code (arranger * arp);
	virtual void bar_code (arranger * arp);
	virtual void variation_code (arranger * arp);
	virtual void transcription_code (arranger * arp);
	virtual bool no_continuous_play (void);      // true if code not called without keys played
	virtual int get_division (void);             // returns beats per bar or 0 if does not care
	virtual int get_subdivision (void);          // returns ticks per beat or 0 if does not care
	virtual int get_tempo (void);                // returns beats per second or -1 if does not care
//	virtual bool transcriptions_buffered (void); // true if the pattern wishes to controll transcription changes
//	virtual bool variation_buffered (void);      // true if the pattern wishes to controll variation changes
//	virtual bool auto_keyoff (void);             // true if the pattern does not keyoff notes
};

typedef pattern * pattern_pointer;
HERCs_SDK_EXPORT extern pattern silence_pattern;

class dsp_algo {
public:
	virtual char * toString (parameter_block * pb);
	virtual char * toString (parameter_block * pb, int ind);
	virtual int get_default (int ind);
};

typedef dsp_algo * dsp_algo_pointer;
HERCs_SDK_EXPORT extern dsp_algo silence_dsp_algo;

class dsp_router {
public:
	int send_level;
	sub_parameter_block dry_wet;
	sub_parameter_block vector_x;
	sub_parameter_block vector_y;
	void voice_init (int level);
	void dry_init (void);
	void wet_init (void);
	int get (int ctrl_dry_wet, int ctrl_vector_x, int ctrl_vector_y);
};

typedef dsp_router * dsp_router_pointer;

class dsp_parameter_block {
public:
	synthesizer * sth;
	STRING algo_msb;
	int algo_program;
	dsp_algo_pointer algo_head;
	bool on;
	dsp_router wet_output;
	dsp_router_pointer * routers;
	int router_number;
	int parameters [16];
	bool algo_notification_not_required;
	void voice_init (void);
	dsp_parameter_block (synthesizer * sth, int ind, int dsp_number);
	~ dsp_parameter_block (void);
};

typedef dsp_parameter_block * dsp_parameter_block_pointer;

class key_queue {
public:
	int * queue;
	int size;
	int top;
	int key_on (int key);
	int key_off (int key);
	void key_off (void);
	key_queue (int size);
	~ key_queue (void);
};

class parameter_block {
public:
	STRING name;
	int category;
	algo_pointer algo_head;
	STRING algo_msb;
	int algo_lsb;
	int algo_program;
	pattern_pointer pattern_head;
	arranger * arp;
	vector * vect;
	STRING pattern_msb;
	int pattern_lsb;
	int pattern_program;
	int factory_id;

	dsp_router local_dry_out;
	dsp_router global_dry_out;
	dsp_router * dsp_send;

	controllers_parameter_block * controllers;
	stripe_parameter_block * * stripes;
	stereo_stripe_parameter_block * * stereo;
	system_parameter_block * system;
	vector_frame * vector_frames;
	dsp_parameter_block * * dsp;
	int hercules_number;
	int hercules_stereo_number;
	int vector_size;
	bool vector_auto_return;
	int hercules_local_dsp_number;
	int hercules_global_dsp_number;
	int hercules_all_dsp_number;

	void voice_init (void);
	void dsp_voice_init (bool global);
	void local_dsp_voice_init (void);
	void global_dsp_voice_init (void);
	void reset_global_dsp_parameters (void);
	parameter_block (synthesizer * sth, int stripes, int stereo, int vector_size, bool vector_auto_return, int queue, int factory_id, int local_dsp, int global_dsp);
	~ parameter_block (void);
// midi related
private:
	bool algo_notification_not_required;
	bool pattern_notification_not_required;
public:
	key_queue * queue;
	int locked_oscillator;
	int last_key;
	synthesizer * sth;
	void nrpn (int data);
	void rpn (int data);
	void key_on (int key, int velocity);
	void key_off (int key);
	void key_off (void);
	void key_code (int key, int velocity);
	void key_code (int key);
	void key_code (void);
	void loading_ground (void);
	void ground (void);
	void reset_all_controllers (void);
	void poly (bool notifications_required);
	void mono (bool notifications_required);
	void hold_off (void);
	void control_vector_x (int x);
	void control_vector_y (int y);
	void control_vector (int x, int y);
	void send_sub_parameter_block (int channel, int nrpn_msb, int nrpn_lsb, sub_parameter_block * pb);
	void send_adsr_level (int channel, int nrpn_msb, int nrpn_lsb, adsr_parameter_block * pb);
	void send_adsr_time (int channel, int nrpn_msb, int nrpn_lsb, adsr_parameter_block * pb);
	void send_parameter_value (int channel);
	void send_algo_string (int channel);
	void send_stripe_string (int channel, int stripe);
	void send_dsp_string (int channel, int ind);
	void send_program_name (int channel);
	void send_bank_msb_name (int channel);
	void send_bank_lsb_name (int channel);
	void send_category_name (int channel);
	void send_style_name (int channel);
	void send_pattern_name (int channel);
	void send_indexed_category_name (int channel, int ind);
	void send_indexed_algo_parameter_name (int channel, int ind);
	void send_indexed_dsp_parameter_name (int channel, int ind);
	void send_algo_bulk_dump (int channel);
	void send_pattern_bulk_dump (int channel);
	void send_dsp_algo_bulk_dump (int channel);
	void send_dsp_algo_bulk_dump (int channel, int ind);
	void send_vector_bulk_dump (int channel);
	void send_midi_channel (void);
	void send_midi_channel_extension (void);
	void send_vector_recorded_info (void);
	void send_arranger_status (void);
	void send_reserved_notes (void);
// notifications to oscillator pool
public:
	void notify_retrigger_change (void);
	void notify_ratio_change (void);
	void notify_freq_shifts_update (void);
	void notify_amp_shifts_update (void);
	void notify_index_shifts_update (void);
	//@ index amp
//	void notify_index_amp_change (int ind);
	void notify_rnd_update (void);
	void notify_pan_shifts_update (void);
	void notify_all_shifts_update (void);
	void notify_freq_lfo_source_update (void);
	void notify_amp_lfo_source_update (void);
	void notify_index_lfo_source_update (void);
	void notify_pan_lfo_source_update (void);
	void notify_all_lfo_source_update (void);
	void notify_lfo_freq_shifts_update (void);
	void notify_lfo_amp_shifts_update (void);
	void notify_lfo_index_shifts_update (void);
	void notify_lfo_pan_shifts_update (void);
	void notify_lfo_all_shifts_update (void);
	void notify_lfo_speeds_update (void);
	void notify_lfo_retrigger_required (void);
	void notify_lfo_wave_change (void);
	void notify_lfo_pulse_width_change (void);
	void notify_sensitivity_change (int nrpn_msb, int xxx, int nnnn);
	void notify_algo_change (void);
	void notify_pattern_change (void);
	void notify_dsp_algo_change (int ind);
	void notify_attack_change (void);
	void notify_decay_change (void);
	void notify_sustain_change (void);
	void notify_release_change (void);
	void notify_adsr_level_update (int nrpn_msb, int xxx, int nnnn);
	void notify_adsr_time_update (int nrpn_msb, int xxx, int nnnn);
	void notify_porta_update (void);
	void notify_tune (void);
	void notify_reserved_notes_changed (void);
	void notify_algo_parameter_changed (int ind);
	void notify_volume_change (void);
	void notify_global_volume_change (void);
//	void notify_poly_mono_change (void);
	void notify_dsp_change (int nrpn_lsb);
	void notify_dry_change (void);
	void notify_dry_wet_change (void);
	void notify_dry_wet_change (int ind);
	void notify_tempo_change (void);
	void notify_division_change (void);
	void notify_sub_division_change (void);
	void notify_metrum_change (void);
	void notify_arpeggiator_switch (void);
	void notify_arpeggiator_hold (void);
	void notify_variation_change (void);
	void notify_arpeggiator_loaded (void);
	void notify_program_loaded (void);
	void notify_dsp_program_loaded (bool global);
// actions
public:
	void program_change (void);
	void bank_change (char * msb);
	void algo_change (char * msb, int lsb, int program);
	void eg_copy (int channel, int source, int destination);
//	void algo_control (int lsb, int program);
	void pattern_change (char * msb, int lsb, int program);
	void dsp_algo_change (char * msb, int lsb, int program);
	void read_algo_bulk_dump (midi_stream * line);
	void read_pattern_bulk_dump (midi_stream * line);
	void read_dsp_algo_bulk_dump (midi_stream * line, int ind);
	void read_vector_bulk_dump (midi_stream * line);
	void read_program (SetupFileReader * fr);
	void read_program (midi_stream * line);
	void read_dsp_program (SetupFileReader * fr, bool global);
	void read_dsp_program (midi_stream * line, bool global);
	void store_program (SetupFileWriter * tc);
	void store_program (midi_stream * line);
	void store_dsp_program (SetupFileWriter * tc, bool global);
	void store_dsp_program (midi_stream * line, bool global);
	void transfer_program (SetupFileReader * fr, midi_stream * line);
	void transfer_program (midi_stream * line, SetupFileWriter * tc);
// lemat
public:
	void relemat_freq_shift (void);
	void relemat_amp_shift (void);
	void relemat_index_shift (void);
	void relemat_pan_shift (void);

	void relemat_pitch (void);
	void relemat_vector_x (void);
	void relemat_vector_y (void);

	void relemat_lfo_speed (void);
	void relemat_vibrato (void);
	void relemat_tremolo (void);
	void relemat_lfo_index (void);
	void relemat_lfo_pan (void);

	void add_freq_lemat (void);
	void add_amp_lemat (void);
	void add_index_lemat (void);
	void add_pan_lemat (void);
	void add_all_shift_lemat (void);

	void add_lfo_speed_lemat (void);
	void add_lfo_freq_lemat (void);
	void add_lfo_amp_lemat (void);
	void add_lfo_index_lemat (void);
	void add_lfo_pan_lemat (void);
	void add_lfo_all_lemat (void);

	void relemat_all (void);
};
typedef parameter_block * parameter_block_pointer;

class bank {
private:
	STRING config_string;
	STRING plugin_parameter_string;
	int type;
public:
	virtual void program_change (int lsb, int program, parameter_block * pb);
	virtual char * toString (void);
	virtual char * toString (int lsb);
	virtual ~ bank (void);
	friend class synthesizer;
};

class synthesizer : public midi_reader {
public:
	STRING root_directory;
	STRING setup_file_name;
	STRING control_file_name;
	bool no_setup_store;
	bool no_control_store;
	parameter_block * * data_blocks;
	dsp_parameter_block * * dsp;
	int multitimbral;
	int stripes;
	int stereo;
	int vector_size;
	bool vector_auto_return;
	int local_dsp;
	int global_dsp;
	int all_dsp;
	bool volume_reset;
	bool global_volume_multi;
	bool global_volume_present;
	int global_volume;

	int transport_seconds;
	int transport_ticks_per_beat;

	int tune;                            // -8192 .. 8191 (some oscillator pools recognize only [-64 .. 64] range)

	bank * banks [128];
	STRING categories [128];
	int category_pointer;
	STRING messages [96];

	midi_stream * midi_out;
	oscillator_pool * pool;
	arpeggiator_pool * arps;
	vector_pool * vects;

	PrologRoot * root;

	synthesizer (config * cfg);
	~ synthesizer (void);
	void voice_init (void);
	void notify_tune (void);
	void notify_global_dsp_program_loaded (void);

	void reset_categories (void);
	void insert_category (char * name);
	char * get_category (int ind);
	void reset_messages (void);
	void set_message (int ind, char * message);
	char * get_message (int ind);
	void reset_banks (void);
	void destroy_banks (void);
	algo_pointer get_default_algo (void);
	algo_pointer get_silence_algo (void);
	pattern_pointer get_default_pattern (void);
	pattern_pointer get_silence_pattern (void);
	dsp_algo_pointer get_default_dsp_algo (void);
	dsp_algo_pointer get_silence_dsp_algo (void);

//	void program_change (parameter_block * pb);
public:
	void connect_prolog_root (PrologRoot * root);
	void disconnect_prolog_root (void);
	void insert_prolog_command (int sub_selector, midi_stream * line);
public:
	void connect_midi_out (midi_stream * line);
	void disconnect_midi_out (void);
	void connect_oscillator_pool (oscillator_pool * pool);
	void disconnect_oscillator_pool (void);
	void connect_arpeggiator_pool (arpeggiator_pool * arps);
	void disconnect_arpeggiator_pool (void);
	void connect_vector_pool (vector_pool * vects);
	void disconnect_vector_pool (void);
	void send_midi_channel_extension (int extension);
	void send_message (int msb, int extension, int channel, int lsb, char * text);
	void send_message (int msb, int channel, int lsb, char * text);
	void send_message (int channel, int id, char * message);
	void send_error (int channel, int error, int msb, int lsb, char * text);
	void send_error (int extension, int channel, int error, int msb, int lsb, char * text);
	void send_identity_reply (int extension, int channel);
	void send_scale_bulk_dump (int device, int program);
public:
	void send_transport_data (int sub_selector);
	void process_transport_request (int sub_selector, midi_stream * line);
	void process_transport_data (int sub_selector, midi_stream * line);
private:
	void constructor (int multitimbral, int stripes, int stereo, int vector_size, bool vector_auto_return, int queue, char * root_directory, bool volume_reset, bool global_dsp_multi, int local_dsp, int global_dsp, bool gobal_volume_multi, bool global_volume_present);
	bool sub_configure (SetupFileReader * fr);
public: // from midi reader
	virtual void midi_keyoff (int channel, int key);
	virtual void midi_keyon (int channel, int key, int velocity);
	virtual void midi_control (int channel, int controller, int value);
	virtual void midi_programchange (int channel, int program);
	virtual void midi_system_exclusive (midi_stream * line);
	virtual void midi_parameter_request (int channel);
	virtual void midi_timing_clock (void);
	virtual void midi_start (void);
	virtual void midi_continue (void);
	virtual void midi_stop (void);
	virtual void midi_active_sensed (void);
private:
	void sub_store_control (SetupFileWriter * tc);
	void sub_store_control (midi_stream * line);
// configuration store and read
private:
	void store_control (SetupFileWriter * tc);
	void store_setup (SetupFileWriter * tc);
public:
	void configure (void);                            // default setup and control
	void configure (char * setup, char * control);    // setup and control
	void configure_setup (void);                      // default setup
	void configure_control (void);                    // default control
	void configure_setup (char * setup);              // setup
	void configure_control (char * control);          // control
	bool configure (char * name);                     // setup or config
	bool store_setup (void);                          // if setup
	bool store_control (void);                        // if control
	void store_setup (char * setup);                  // store setup
	void store_control (char * setup);                // store control
	void store_configuration (void);                  // called by midi message
	// midi procedures never used
	// however might be usefull for DSP version
	bool store_setup (midi_stream * line);            // if setup export setup
	bool store_control (midi_stream * line);          // if control export control
	void store_configuration (midi_stream * line);    // if setup or control
public:
	void transfer (SetupFileReader * fr, midi_stream * line);
	void transfer (midi_stream * line, SetupFileWriter * tc);
	void transfer_program (SetupFileReader * fr, midi_stream * line);
	void transfer_program (midi_stream * line, SetupFileWriter * tc);
	void transfer_setup_file (SetupFileReader * fr, midi_stream * line);
	void transfer_setup_file (midi_stream * line, SetupFileWriter * tc);
	void transfer_program_map (SetupFileReader * fr, midi_stream * line);
	void transfer_program_map (midi_stream * line, SetupFileWriter * tc);
	void transfer_rom (SetupFileReader * fr, midi_stream * line);
	void transfer_rom (midi_stream * line, SetupFileWriter * tc);
	void transfer_wave (SetupFileReader * fr, midi_stream * line);
	void transfer_wave (midi_stream * line, SetupFileWriter * tc);
	void transfer_wavetable (SetupFileReader * fr, midi_stream * line);
	void transfer_wavetable (midi_stream * line, SetupFileWriter * tc);
	void transfer_arpeggio (SetupFileReader * fr, midi_stream * line);
	void transfer_arpeggio (midi_stream * line, SetupFileWriter * tc);
	void transfer_style (SetupFileReader * fr, midi_stream * line);
	void transfer_style (midi_stream * line, SetupFileWriter * tc);
	void transfer_text_file (SetupFileReader * fr, midi_stream * line);
	void transfer_text_file (midi_stream * line, SetupFileWriter * tc);
	void transfer_key_file (SetupFileReader * fr, midi_stream * line);
	void transfer_key_file (midi_stream * line, SetupFileWriter * tc);
};

class algo_bank : public bank {
public:
	virtual void program_change (int lsb, int program, parameter_block * pb);
	virtual algo_pointer get_algo (int lsb, int program, parameter_block * pb);
};

class pattern_bank : public bank {
public:
	virtual void program_change (int lsb, int program, parameter_block * pb);
	virtual pattern_pointer get_pattern (int lsb, int program, parameter_block * pb);
};

class dsp_algo_bank : public bank {
public:
	virtual char * toString (void);
	virtual char * toString (int lsb);
	virtual void program_change (int lsb, int program, parameter_block * pb);
	virtual dsp_algo_pointer get_dsp_algo (int program, parameter_block * pb);
};

class preset_algo_bank : public algo_bank {
public:
	virtual char * toString (void);
};

class preset_pattern_bank : public pattern_bank {
public:
	virtual pattern_pointer get_pattern (int lsb, int program, parameter_block * pb);
	virtual char * toString (void);
	virtual char * toString (int lsb);
	preset_pattern_bank (void);
	~ preset_pattern_bank (void);
};

class preset_dsp_algo_bank : public dsp_algo_bank {
public:
	virtual char * toString (void);
};

class sound_bank : public bank {
private:
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
	sound_bank (STRING text, synthesizer * sth);
};

class user_sound_bank : public bank {
private:
	STRING directory;
public:
	virtual void program_change (int lsb, int program, parameter_block * pb);
	virtual char * toString (void);
	virtual char * toString (int lsb);
	user_sound_bank (STRING text, synthesizer * sth);
};

class dsp_bank : public bank {
private:
	bool global;
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
	dsp_bank (STRING text, synthesizer * sth);
};

class user_dsp_bank : public bank {
private:
	bool global;
	STRING directory;
	void extract_dsp_type (SetupFileReader * fr);
public:
	virtual void program_change (int lsb, int program, parameter_block * pb);
	virtual char * toString (void);
	virtual char * toString (int lsb);
	user_dsp_bank (STRING text, synthesizer * sth);
};


#endif
