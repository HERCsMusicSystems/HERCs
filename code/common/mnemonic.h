///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _MNEMONIC
#define _MNEMONIC


#define PROGRAM          0x01
#define NAME             0x02
#define CATEGORY         0x03
#define ALGO             0x04
#define PITCH            0x05
#define FREQ             0x06
#define AMP              0x07
#define INDEX            0x08
#define ATTACK           0x09
#define DECAY            0x0a
#define SUSTAIN          0x0b
#define RELEASE          0x0c
#define SPEED            0x0d
#define VIBRATO          0x0e
#define TREMOLO          0x0f
#define LFO_INDEX        0x10
#define VECTOR_X         0x11
#define VECTOR_Y         0x12
#define VOLUME           0x13
#define PAN              0x14
#define HOLD_ON          0x15
#define PORTA_ON         0x16
#define PORTA_TIME       0x17
#define PORTA_MODE       0x18
#define POLY             0x19
#define MONO             0x1a
#define SYNC_ALL         0x1b
#define DELAY_ON         0x1c
#define DELAY_OFF        0x1d
#define DELAY_CROSS_ON   0x1e
#define DELAY_CROSS_OFF  0x1f
#define DRY_WET          0x1e
#define GLOBAL_VOLUME    0x1f
#define DELAY            0x20
#define STRIPE           0x21
#define SYNC             0x22
#define FREQ_RATIO       0x23
#define LFO_FREQ         0x24
#define LFO_AMP          0x25
#define FREQ_LFO_SOURCE  0x02
#define AMP_LFO_SOURCE   0x03
#define INDEX_LFO_SOURCE 0x01
#define LFO_SOURCE       0X01
#define LFO_SPEED        0x26
#define LFO_PHASE_SHIFT  0x27
#define LFO_PULSE_WIDTH  0x28
#define LFO_SYNC         0x29
#define LFO_SYNC_TO      0x2a
#define LFO_WAVE         0x2b
#define FREQ_EG          0x2c
#define FREQ_EG_LEVEL    0x2d
#define FREQ_EG_TIME     0x2e
#define AMP_EG           0x30
#define AMP_EG_LEVEL     0x31
#define AMP_EG_TIME      0x32
#define INDEX_EG         0x33
#define INDEX_EG_LEVEL   0x34
#define INDEX_EG_TIME    0x35
#define EG               0x36
#define EG_LEVEL         0x37
#define EG_TIME          0x38
#define PITCH_SENS                 0x39
#define SENS                       0x3a
#define LFO_SENS                   0x3b
#define ATTACK_SENS                0x3c
#define DECAY_SENS                 0x3d
#define SUSTAIN_SENS               0x3e
#define RELEASE_SENS               0x3f
#define KEY_SENS                   0x40
#define VELOCITY_SENS              0x41
#define VECTOR_X_SENS              0x42
#define VECTOR_Y_SENS              0x43
#define LFO_X_SENS                 0x44
#define LFO_Y_SENS                 0x45
#define KEY_ATTACK_EG_SCALING      0x15
#define KEY_EG_SCALING             0x46
#define VELOCITY_ATTACK_EG_SCALING 0x16
#define VELOCITY_EG_SCALING        0x47
#define FREQ_PITCH_SENS            0x48
#define FREQ_SENS                  0x49
#define FREQ_LFO_SENS              0x4a
#define FREQ_ATTACK_SENS           0x4b
#define FREQ_DECAY_SENS            0x4c
#define FREQ_SUSTAIN_SENS          0x4d
#define FREQ_RELEASE_SENS          0x4e
#define FREQ_KEY_SENS              0x4f
#define FREQ_VELOCITY_SENS         0x50
#define FREQ_VECTOR_X_SENS         0x51
#define FREQ_VECTOR_Y_SENS         0x52
#define FREQ_LFO_X_SENS            0x53
#define FREQ_LFO_Y_SENS            0x54
#define FREQ_KEY_ATTACK_EG_SCALING 0x04
#define FREQ_KEY_EG_SCALING        0x55
#define FREQ_VELOCITY_ATTACK_EG_SCALING 0x05
#define FREQ_VELOCITY_EG_SCALING   0x56
#define AMP_PITCH_SENS             0x57
#define AMP_SENS                   0x58
#define AMP_LFO_SENS               0x59
#define AMP_ATTACK_SENS            0x5a
#define AMP_DECAY_SENS             0x5b
#define AMP_SUSTAIN_SENS           0x5c
#define AMP_RELEASE_SENS           0x5d
#define AMP_KEY_SENS               0x5e
#define AMP_VELOCITY_SENS          0x5f
#define AMP_VECTOR_X_SENS          0x60
#define AMP_VECTOR_Y_SENS          0x61
#define AMP_LFO_X_SENS             0x62
#define AMP_LFO_Y_SENS             0x63
#define AMP_KEY_ATTACK_EG_SCALING  0x11
#define AMP_KEY_EG_SCALING         0x64
#define AMP_VELOCITY_ATTACK_EG_SCALING 0x12
#define AMP_VELOCITY_EG_SCALING    0x65
#define INDEX_PITCH_SENS           0x66
#define INDEX_SENS                 0x67
#define INDEX_LFO_SENS             0x68
#define INDEX_ATTACK_SENS          0x69
#define INDEX_DECAY_SENS           0x6a
#define INDEX_SUSTAIN_SENS         0x6b
#define INDEX_RELEASE_SENS         0x6c
#define INDEX_KEY_SENS             0x6d
#define INDEX_VELOCITY_SENS        0x6e
#define INDEX_VECTOR_X_SENS        0x6f
#define INDEX_VECTOR_Y_SENS        0x70
#define INDEX_LFO_X_SENS           0x71
#define INDEX_LFO_Y_SENS           0x72
#define INDEX_KEY_ATTACK_EG_SCALING 0x13
#define INDEX_KEY_EG_SCALING       0x73
#define INDEX_VELOCITY_ATTACK_EG_SCALING 0x14
#define INDEX_VELOCITY_EG_SCALING  0x74
#define LFO_SPEED_SENS             0x75
#define LFO_PITCH_SENS             0x76
#define LFO_VECTOR_X_SENS          0x77
#define LFO_VECTOR_Y_SENS          0x78
#define STEREO           0x79
#define RND              0x7a
#define FREQ_RND         0x7a
#define LFO              0x7b
#define AMP_RND          0x7c
#define INDEX_RND        0x7d
#define KEY              0x7e
#define LFO_PAN          0x7f
#define EARTH            0x00

#define SETUP                          0x48
#define DEVICE                         0x49
#define CHANNEL                        0x4a
#define CHANNEL_EXTENSION              0x76
#define MESSAGE                        0x4b
#define CATEGORIES                     0x4c
#define CONTROLS                       0x4d
#define BANKS                          0x4e
#define PRESET_ALGO                    0x4f
#define PRESET_PATTERN                 0x50
#define PRESET_DSP_ALGO                0x51
#define ROM_PLAYER                     0x52
#define WAVETABLE                      0x53
#define PATTERN                        0x54
#define STYLE                          0x55
#define SOUND                          0x56
#define USER_SOUND                     0x57
#define DSP                            0x58
#define USER_DSP                       0x59
#define PLUGIN                         0x5a
#define TEMPO                          0x5b
#define DIVISION                       0x5c
#define ARPEGGIATOR_ON                 0x5d
#define ARPEGGIATOR_HOLD               0x5e
#define VARIATION                      0x5f
#define DIRECT_OUTPUT                  0x60
#define DIRECT_OUTPUT_DRY_WET_SENS     0x61
#define DIRECT_OUTPUT_VECTOR_X_SENS    0x62
#define DIRECT_OUTPUT_VECTOR_Y_SENS    0x63
#define DSP_ON                         0x64
#define DSP_OFF                        0x65
#define DIRECT_INPUT                   0x66
#define DIRECT_INPUT_DRY_WET_SENS      0x67
#define DIRECT_INPUT_VECTOR_X_SENS     0x68
#define DIRECT_INPUT_VECTOR_Y_SENS     0x69
#define SEND_TO                        0x6a
#define SEND_TO_DRY_WET_SENS           0x6b
#define SEND_TO_VECTOR_X_SENS          0x6c
#define SEND_TO_VECTOR_Y_SENS          0x6d
#define PARAMETER                      0x6e
#define TUNE                           0x6f

#define ARRANGER_ASSIGN                0x70
#define RESERVED_NOTES                 0x71
#define GLOBAL_DSP                     0x72
#define ALGO_PARAMETER                 0x73

#define MSB              0x54
#define LSB              0x55
#define LOCATION         0x56
#define FILE             0x57
#define DEFAULT          0x58

//@ index amp
//#define INDEX_AMP        0x02

#define HOLD             0x15
#define PORTA            0x16


extern void drop_controller (SetupFileWriter * tc, int ind, int sub, char * text);
extern void drop_controller (SetupFileWriter * tc, int index, int ind, int sub, char * text);
extern void drop_controller (SetupFileWriter * tc, bool ind, bool sub, char * text);

extern void drop_controller (midi_stream * line, int ind, int sub, int id);
extern void drop_short_controller (midi_stream * line, int ind, int sub, int id);
extern void drop_controller (midi_stream * line, int index, int ind, int sub, int id);
extern void drop_controller (midi_stream * line, bool ind, bool sub, int id);

extern bool copy_one (SetupFileReader * fr, midi_stream * line, int id);
extern bool copy_short (SetupFileReader * fr, midi_stream * line, int id);
extern bool copy_two (SetupFileReader * fr, midi_stream * line, int id);
extern bool copy_three (SetupFileReader * fr, midi_stream * line, int id);
extern bool copy_eight (SetupFileReader * fr, midi_stream * line, int id);

extern void copy_eight (midi_stream * line, SetupFileWriter * tc, char * id);
extern void copy_eight_very_negative (midi_stream * line, SetupFileWriter * tc, char * id);
extern void copy_two_positive (midi_stream * line, SetupFileWriter * tc, char * id);
extern void copy_two_negative (midi_stream * line, SetupFileWriter * tc, char * id);
extern void copy_two_very_negative (midi_stream * line, SetupFileWriter * tc, char * id);
extern void copy_three (midi_stream * line, SetupFileWriter * tc, char * id);

extern void emit (midi_stream * line, int data);
extern void emit (midi_stream * line, int id, int data);
extern void emit (midi_stream * line, char * text);

/*
extern char file_system_data [1048576];
extern int file_system_pointer;
extern int file_pointer;
extern int flash_ref;
extern rtems_unsigned32 flash_size;

extern bool delay_ground_flag;

#define GSI file_system_data [file_pointer++]
#define FSP & file_system_data [file_pointer]
#define FP file_system_data [file_pointer]

extern void skip_on_string (void);
extern int get_int (void);
extern int get_negative (void);
extern int get_short_negative (void);
extern int get_very_negative (void);

extern int find_file (char * file_name);
extern void pre_read_program_file (synthesizer * sth, parameter_block * pb);
extern void post_read_program_file (synthesizer * sth, parameter_block * pb);
extern void read_program_file (synthesizer * sth, parameter_block * pb);
*/
#endif

