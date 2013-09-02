///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _CONFIG
#define _CONFIG

#include "setup_file_reader.h"

class config;

#define CONFIG_MULTITIMBRAL 16
#define CONFIG_STRIPES 6
#define CONFIG_STEREO 1
#define CONFIG_LOCAL_DSP 1
#define CONFIG_GLOBAL_DSP 0
#define CONFIG_OSCILLATORS 64
#define CONFIG_PROCESSORS 1
#define CONFIG_KEY_STREAM 128
#define CONFIG_ARRANGER_KEYS 16
#define CONFIG_ARRANGER_HORIZONTAL 441
#define CONFIG_VECTOR_SIZE 1024
#define CONFIG_VECTOR_HORIZONTAL 441
#define CONFIG_CENTER_FREQ 330
#define CONFIG_HORIZONTAL 44100
#define CONFIG_VERTICAL 32768
#define CONFIG_HEAD_ROOM 262144
#define CONFIG_FEED_SPACE 128
#define CONFIG_DSP_LINE_SPACE 88200
#define CONFIG_DSP_VARIABLE_SPACE 16
#define CONFIG_STYLE_PROGRAM_SIZE 1024
#define CONFIG_PROLOG_HORIZONTAL 441
#define CONFIG_PROLOG_CONSOLE_HORIZONTAL 50
#define CONFIG_PROLOG_LIBRARY_LOAD ""
#define CONFIG_MIDI_IN -1
#define CONFIG_MIDI_OUT -1
#define CONFIG_CONSOLE_MIDI_IN -1
#define CONFIG_CONSOLE_MIDI_OUT -1
#define CONFIG_ROOT_DIRECTORY ""
#define CONFIG_PROLOG_ROOT_DIRECTORY ""
#define CONFIG_SERIAL_NUMBER ""
#define CONFIG_KEY ""
#define CONFIG_SAMPLING_FREQ 44100
#define CONFIG_LINEAR_PAN true
#define CONFIG_VOLUME_RESET true
#define CONFIG_GLOBAL_VOLUME_PRESENT true
#define CONFIG_GLOBAL_VOLUME_MULTI false
#define CONFIG_GLOBAL_DSP_MULTI false
#define CONFIG_VECTOR_AUTO_RETURN false
#define CONFIG_DEFAULT_ALGO_MSB 8
#define CONFIG_DEFAULT_ALGO_LSB 0
#define CONFIG_DEFAULT_PRESET_SOUND_MSB 0
#define CONFIG_DEFAULT_PRESET_SOUND_LSB 0
#define CONFIG_DEFAULT_USER_SOUND_MSB 1
#define CONFIG_DEFAULT_USER_SOUND_LSB 0
#define CONFIG_DEFAULT_PATTERN_MSB 6
#define CONFIG_DEFAULT_PATTERN_LSB 0
#define CONFIG_DEFAULT_STYLE_MSB 7
#define CONFIG_DEFAULT_STYLE_LSB 0
#define CONFIG_DEFAULT_DSP_ALGO_MSB 9
#define CONFIG_DEFAULT_DSP_ALGO_LSB 0
#define CONFIG_DEFAULT_PRESET_DSP_MSB 10
#define CONFIG_DEFAULT_PRESET_DSP_LSB 0
#define CONFIG_DEFAULT_USER_DSP_MSB 11
#define CONFIG_DEFAULT_USER_DSP_LSB 0
#define CONFIG_LATENCY_BLOCK 512
#define CONFIG_OSCILLOSCOPE_HORIZONTAL 40
#define CONFIG_OSCILLOSCOPE_RESOLUTION_SHIFT 7
#define CONFIG_OSCILLOSCOPE_HORIZONTAL_SHIFT 0
#define CONFIG_DCO_BLOCKING_FREQ 0
#define CONFIG_LAYOUT_SMALL false
#define CONFIG_MODULATION_WHEEL_ID 1
#define CONFIG_EXTERNAL_STORE_PROTECT_LEVEL 1
#define CONFIG_VST_CONTROLLERS_REDRAW false
#define CONFIG_VST_RECORD_MIDI false
#define CONFIG_VST_AUTOMATION true

class config {
private:
	char config_location [256];
	void read_config (SetupFileReader * fr);
	void store_config (SetupFileWriter * fw);
public:
	int multitimbral;                   // 16
	int stripes;                        // 6
	int stereo;                         // 1
	int local_dsp;                      // 1
	int global_dsp;                     // 0
	int oscillators;                    // 64
	int processors;                     // 1
	int key_stream;                     // 128
	int arranger_keys;                  // 16
	int arranger_horizontal;            // 441
	int vector_size;                    // 1024
	int vector_horizontal;              // 441
	int center_freq;                    // 330
	int horizontal;                     // 44100
	int vertical;                       // 32768
	int head_room;                      // 262144
	int feed_space;                     // 128 per stripe
	int dsp_line_space;                 // 88200 (2 * horizontal)
	int dsp_variable_space;             // 16
	int style_program_size;             // 1024
	int prolog_horizontal;              // 441
	int prolog_console_horizontal;      // 50
	char prolog_library_load [250];     // ""
	int midi_in;                        // -1
	int midi_out;                       // -1
	int console_midi_in;                // -1
	int console_midi_out;               // -1
	char root_directory [250];          // ""
	char prolog_root_directory [250];   // ""
	char serial_number [250];           // ""
	char key [250];                     // ""
	int sampling_freq;                  // 44100
	bool linear_pan;                    // true
	bool volume_reset;                  // true
	bool global_volume_present;         // true
	bool global_volume_multi;           // false
	bool global_dsp_multi;              // false
	bool vector_auto_return;            // false
	int default_algo_msb;               // added
	int default_algo_lsb;               // added
	int default_preset_sound_msb;       // added
	int default_preset_sound_lsb;       // added
	int default_user_sound_msb;         // added
	int default_user_sound_lsb;         // added
	int default_pattern_msb;            // added
	int default_pattern_lsb;            // added
	int default_style_msb;              // added
	int default_style_lsb;              // added
	int default_dsp_algo_msb;           // added
	int default_dsp_algo_lsb;           // added
	int default_preset_dsp_msb;         // added
	int default_preset_dsp_lsb;         // added
	int default_user_dsp_msb;           // added
	int default_user_dsp_lsb;           // added
	int latency_block;                  // 512
	int oscilloscope_horizontal;        // 40 (refresh per 10 seconds)
	int oscilloscope_resolution_shift;  // 7
	int oscilloscope_horizontal_shift;  // 0
	int dco_blocking_freq;              // 0
	bool layout_small;                  // false
	int modulation_wheel_id;            // 1
	int external_store_protect_level;   // 1 (0:protected, 1:automatic, 2:allowed)
	bool vst_controllers_redraw;        // false
	bool vst_record_midi;               // false
	bool vst_automation;                // true
	void store (void);
	void correct (void);
	bool correct_horizontal (void);
	bool config_not_found (void);
	config (char * file_name);
	config (void);
};

#endif

