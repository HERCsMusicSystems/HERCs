///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc.h"
#include "osc_cpp.h"
#include "math.h"
#include "algo_building_blocks.h"

unsigned int tune_freq_logs [16384 + 256]; // for tuning
unsigned int * freq_logs = & tune_freq_logs [128];
int amp_logs [16384];
double sine_wave [16384];
double fm_one_cycle;
double fm_three_cycles;
double chameleon_cycles;
int time_delays [16384];
int lfo_sine_wave [16385];
//int global_noise = 0;
//int * global_noise = NULL;

void fill_tables (int horizontal, int center_freq, int vertical) {
	int ind;
	double sub;
	double sample = 4294967296.0 / (double) horizontal;
	double center_sample = sample;
	center_sample *= (double) center_freq;
	for (ind = 0; ind < 16384; ind++) {
		sub = ind;
		sub *= 3.8349519697141030742952189737299e-4;
		sub = sin (sub);
		sine_wave [ind] = sub;
	}
	for (ind = 0; ind <= 16384; ind++) {
		sub = ind;
		sub *= 1.9174759848570515371476094868649e-4;
		sub = - cos (sub);
		sub *= 8192.0;
		sub += 0.5;
		if (sub > 8192.0) sub = 8192.0;
		lfo_sine_wave [ind] = (int) sub;
	}
	int freq = -8320;
	for (ind = 0; ind < 16640; ind++) { // 8320 in the middle
		sub = center_sample * pow (2.0, ((double) freq++) / 1536.0);
		sub += 0.5;
		tune_freq_logs [ind] = (unsigned int) sub;
	}
	int amp = 0;
	for (ind = 0; ind < 16383; ind++) {
		sub = (double) vertical;
		sub *= pow (2.0, ((double) amp--) / 1536.0);
		sub += 0.5;
		amp_logs [ind] = (int) sub;
	}
	amp_logs [16383] = 0;
	time_delays [0] = 0;
	freq = -8191;
	for (ind = 1; ind < 16384; ind++) {
		sub = horizontal;
		sub *= pow (2.0, ((double) freq++) / 768.0);
		sub += 0.5;
		time_delays [ind] = (int) sub;
	}
	fm_one_cycle = 4294967296.0 / (double) vertical;
	fm_three_cycles = fm_one_cycle * 3.0; //4294967296.0 * 3.0 / (double) vertical;
	chameleon_cycles = fm_one_cycle / 3.0; //4294967296.0 / (3.0 * (double) vertical);
//	antialias_amplitude = (float) amp_logs [5632];
//	antialias_amplitude *= 3.0f;
}

void adsr_control_block :: constructor (int size) {
	this -> size = size;
	index = size - 1;
	next_index = new int [size];
	x = new int [size];
	d0 = new int [size];
	d1 = new int [size];
	d2 = new int [size];
	y0 = new int [size];
	y1 = new int [size];
	y2 = new int [size];
	y_target = new int [size];
	for (int ind = 0; ind < size; ind++) {
		next_index [ind] = ind + 1;
		x [ind] = 0x7fffffff;
		d0 [ind] = 0;
		d1 [ind] = 0;
		d2 [ind] = 0;
		y0 [ind] = 0;
		y1 [ind] = 0;
		y2 [ind] = 0;
		y_target [ind] = 0;
	}
	next_index [size - 1] = size - 1;
	retrigger_index = 0;
	retrigger_x = x [0];
	retrigger_y = 0;
	retrigger_d = 0;
	retrigger_d1 = 0;
	retrigger_d2 = 0;
	wave = 0;
	target = 0;
}

void adsr_control_block :: destructor (void) {
	delete [] x;
	delete [] d0;
	delete [] d1;
	delete [] d2;
	delete [] y0;
	delete [] y1;
	delete [] y2;
	delete [] y_target;
}

void adsr_control_block :: adsr_constructor (void) {
	constructor (6);
	next_index [3] = 3;
}

void adsr_control_block :: lfo_constructor (void) {
	constructor (4);
	next_index [3] = 0;
	y0 [0] = -8192;
	y0 [1] = 8192; // 8191;
	y0 [2] = 8192; // 8191;
	y0 [3] = -8192;
}

void adsr_control_block :: key_glider_constructor (void) {
	constructor (1); //(2);
}

void algo_cpp :: data_constructor (oscillator * osc) {
	int * ip = osc -> feed;
	int ind = osc -> stripes;
	while (ind-- > 0) * (ip++) = 0;
}
void algo_cpp :: data_destructor (oscillator * osc) {}
int algo_cpp :: code (oscillator * osc) {
	FM_DECLARATIONS;
	for (int ind = 0; ind < osc -> stripes; ind++) {
		FM_FEEDBACK (ind);
		accu += wave;
	}
	return (int) (accu + 0.5f);
}
void algo_cpp :: retrigger (oscillator * osc) {}
void algo_cpp :: release (oscillator * osc) {}
void algo_cpp :: ground (oscillator * osc) {}
void algo_cpp :: notify_algo_parameter_changed (parameter_block * pb, int ind) {}
void algo_cpp :: notify_all_algo_parameters_changed (parameter_block * pb) {}

char * default_algo_cpp :: toString (parameter_block * pb) {return "DEFAULT";}
char * default_algo_cpp :: toString (parameter_block * pb, int ind) {return "sine";}
default_algo_cpp default_algo;

algo_pointer oscillator_pool_cpp :: get_default_algo (void) {return & default_algo;}
dsp_algo_pointer oscillator_pool_cpp :: get_default_dsp_algo (void) {return dsp -> get_default_dsp_algo ();}
preset_algo_bank * oscillator_pool_cpp :: create_preset_algo_bank (void) {return new preset_algo_bank_cpp ();}
algo_bank * oscillator_pool_cpp :: create_rom_player_bank (char * text, synthesiser * sth) {return new rom_player_bank (text, sth, center_freq, horizontal, vertical, head_room);}
algo_bank * oscillator_pool_cpp :: create_wavetable_bank (char * text, synthesiser * sth) {return new wavetable_bank_cpp (text, sth, vertical);}
preset_dsp_algo_bank * oscillator_pool_cpp :: create_preset_dsp_algo_bank (void) {return new preset_dsp_algo_bank_cpp ();}
pattern_bank * oscillator_pool_cpp :: create_scripted_pattern_bank (char * text, synthesiser * sth) {return new scripted_pattern_bank (text, sth, style_program_size);}
bank * oscillator_pool_cpp :: create_style_bank (char * text, synthesiser * sth) {return new style_bank (text, sth, style_program_size);}


void oscillator_pool_cpp :: initialize_math_support (void) {
//	global_noise = & rnd . out;
	fill_tables (horizontal, center_freq, vertical);
}

#include <omp.h>
#ifdef LINUX_OPERATING_SYSTEM
#include <pthread.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <semaphore.h>
#define THREAD void *
#define DLL_OPEN(name) dlopen (name, RTLD_LAZY)
#define GET_SYMBOL(dll, name) dlsym (dll, name)
#define DLL_CLOSE(dll) dlclose (dll)
#define SYSTEM_DELAY(miliseconds) usleep (miliseconds * 1000)
#define RUNNER_RETURN static void *
#define RUNNER_PARAMETER void *
#define RUNNER_RETURN_VALUE return 0;
#endif

#ifdef WINDOWS_OPERATING_SYSTEM
#include <windows.h>
#include <direct.h>
#include <process.h>
#define THREAD HINSTANCE
#define DLL_OPEN(name) LoadLibrary (name)
#define GET_SYMBOL(dll, name) GetProcAddress (dll, name)
#define DLL_CLOSE(dll) FreeLibrary (dll)
#define SYSTEM_DELAY(miliseconds) Sleep (miliseconds)
#define RUNNER_RETURN void
#define RUNNER_PARAMETER void *
#define RUNNER_RETURN_VALUE
#endif

void oscillator_pool_cpp_runner :: move (void) {
	osc_cpp -> move_oscillators (from, to, buffer_size);
}

RUNNER_RETURN oscillator_runner (RUNNER_PARAMETER parameters) {
	oscillator_pool_cpp_runner * r = (oscillator_pool_cpp_runner *) parameters;
	while (r -> active) {
#ifdef LINUX_OPERATING_SYSTEM
		sem_wait ((sem_t *) r -> enter_semaphore);
		if (r -> active) r -> move ();
		sem_post ((sem_t *) r -> exit_semaphore);
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
		WaitForSingleObject (r -> enter_semaphore, INFINITE);
		if (r -> active) r -> move ();
		ReleaseSemaphore (r -> exit_semaphore, 1, NULL);
#endif
	}
	r -> finished = true;
	RUNNER_RETURN_VALUE;
}

oscillator_pool_cpp_runner :: oscillator_pool_cpp_runner (oscillator_pool_cpp * osc_cpp, int from, int to) {
	this -> osc_cpp = osc_cpp;
	this -> from = from; this -> to = to;
	this -> active = true;
	this -> finished = false;
	this -> buffer_size = 1;
#ifdef LINUX_OPERATING_SYSTEM
	enter_semaphore = (void *) sem_open ("OSC_RUNNER_ENTER", O_CREAT, 0000777, 0);
	sem_unlink ("OSC_RUNNER_ENTER");
	exit_semaphore = (void *) sem_open ("OSC_RUNNER_EXIT", O_CREAT, 0000777, 0);
	sem_unlink ("OSC_RUNNER_EXIT");
	pthread_t threader;
	pthread_attr_t attr;
	pthread_attr_init (& attr);
	pthread_attr_setstacksize (& attr, 120 * 1024);
	pthread_attr_setdetachstate (& attr, PTHREAD_CREATE_DETACHED);
	pthread_create (& threader, & attr, oscillator_runner, this);
	pthread_attr_destroy (& attr);
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
	enter_semaphore = CreateSemaphore (NULL, 0, 30000, NULL);
	exit_semaphore = CreateSemaphore (NULL, 0, 30000, NULL);
	_beginthread (oscillator_runner, 0, this);
#endif
}

oscillator_pool_cpp_runner :: ~ oscillator_pool_cpp_runner (void) {
	active = false;
#ifdef LINUX_OPERATING_SYSTEM
	sem_post ((sem_t *) enter_semaphore);
	while (! finished) {usleep (20000); active = false; sem_post ((sem_t *) enter_semaphore);}
	sem_close ((sem_t *) enter_semaphore);
	sem_close ((sem_t *) exit_semaphore);
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
	ReleaseSemaphore (enter_semaphore, 1, NULL);
	while (! finished) {Sleep (20); active = false; ReleaseSemaphore (enter_semaphore, 1, NULL);}
	CloseHandle (enter_semaphore);
	CloseHandle (exit_semaphore);
#endif
}

//static oscillator_pool_cpp_runner * second_processor = NULL;

void oscillator_pool_cpp :: constructor (synthesiser * sth, int horizontal, int center_freq, int vertical, int head_room, int feed_space, dsp_line_pool * dsp, int style_program_size) {
	this -> style_program_size = style_program_size;
	this -> dsp = dsp;
	this -> horizontal = horizontal;      // this figurative code
	this -> center_freq = center_freq;    // is designed
	this -> vertical = vertical;          // to support
	this -> head_room = head_room;        // plugin loader
	initialize_math_support ();
	stripes = sth -> stripes;
	stripes2 = stripes + stripes;
	stripes3 = stripes2 + stripes;
	stereo = sth -> stereo;
	stripes_stereo = stripes + stereo;
	velocity_size = stripes3 + stereo;
	multitimbral = sth -> multitimbral;
	total_stripes = stripes * polyphony;
	total_stripes3 = total_stripes * 3;
	total_stripes9 = total_stripes3 * 3;
	slope_shift = total_stripes3 - stripes * 3;
	stripe_shift = total_stripes - stripes;
	stripe_shift4 = stripe_shift << 2;
	total_stereo = stereo * polyphony;
	adsr_size = total_stripes3 + total_stereo;
	lfo_size = stripes_stereo * multitimbral;
	key_glider_size = polyphony;
	total_adsr_size = adsr_size + lfo_size + key_glider_size;
	lfo_gate_gap = horizontal >> 11;
	lfo_gate_gap2 = lfo_gate_gap + lfo_gate_gap;

//	rotator = 0;

	adsr_x = new int [total_adsr_size];
	pan_x = & adsr_x [total_stripes3];
	lfo_x = & adsr_x [adsr_size];
	key_glide_x = & lfo_x [lfo_size];
	adsr_d = new int [total_adsr_size];
	pan_d = & adsr_d [total_stripes3];
	lfo_d = & adsr_d [adsr_size];
	key_glide_d = & lfo_d [lfo_size];
	adsr_delta = new int [total_adsr_size << 2];
	pan_delta = & adsr_delta [total_stripes3 << 2];
	lfo_delta = & adsr_delta [adsr_size << 2];
	key_glide_delta = & lfo_delta [lfo_size << 2];
	adsr_y = new int [total_adsr_size];
	pan_y = & adsr_y [total_stripes3];
	lfo_y = & adsr_y [adsr_size];
	key_glide_y = & lfo_y [lfo_size];
	lfo_out = new int [lfo_size];
	lfo_outs = new multi_line [lfo_size + 1];

	time = new unsigned int [total_stripes];
	delta_time = new unsigned int [total_stripes];
	freq = new int [adsr_size];
	amp = & freq [total_stripes];
	index = & amp [total_stripes];
	pan = & index [total_stripes];
	feed = new int [total_stripes * feed_space];
	freq_shift = new int [adsr_size];
	amp_shift = & freq_shift [total_stripes];
	index_shift = & amp_shift [total_stripes];
	pan_shift = & index_shift [total_stripes];
	lfo_freq_shift = new int [adsr_size];
	lfo_amp_shift = & lfo_freq_shift [total_stripes];
	lfo_index_shift = & lfo_amp_shift [total_stripes];
	lfo_pan_shift = & lfo_index_shift [total_stripes];
	ratio = new int [total_stripes];
	velocities = new int [adsr_size];
//	lfo_rnds = new int [polyphony];
	original_velocities = new int [polyphony];
	retriggers_required = new bool [multitimbral];
	freq_lfo_source = new int [adsr_size];
	amp_lfo_source = & freq_lfo_source [total_stripes];
	index_lfo_source = & amp_lfo_source [total_stripes];
	pan_lfo_source = & index_lfo_source [total_stripes];
	//@ index amp
//	index_amps = new bool [total_stripes];

	slopes = new int [adsr_size * 3];
	ctrl = new adsr_control_block [total_adsr_size];
	pan_ctrl = & ctrl [total_stripes3];
	lfo_ctrl = & ctrl [adsr_size];
	key_glide_ctrl = & lfo_ctrl [lfo_size];
	osc = new oscillator [polyphony];
	algo = new algo_cpp_pointer [polyphony];

	lfo_connections = new int [polyphony];

	amp_target = new int [total_stripes + total_stripes + total_stereo];
	index_target = & amp_target [total_stripes];
	pan_target = & index_target [total_stripes];

	int ind;
	for (ind = 0; ind < multitimbral; ind++) retriggers_required [ind] = true;
	int ip = 0;
	for (ind = total_stripes; ind > 0; ind--) {
		slopes [ip++] = 8192;
		slopes [ip++] = 128;
		slopes [ip++] = 128;
	}
	for (ind = total_stripes + total_stripes + total_stereo; ind > 0; ind--) {
		slopes [ip++] = 8192;
		slopes [ip++] = 0;
		slopes [ip++] = 0;
	}
	for (ind = 0; ind < total_stripes; ind++) {
		ratio [ind] = 1;
		freq_shift [ind] = 8192;
		amp_shift [ind] = 16383;
		index_shift [ind] = 0;
		//@ index amp
//		index_amps [ind] = false;
	}
	//@ index amp
//	index_amp_required = false;
	for (ind = 0; ind < total_stereo; ind++) {
		pan_shift [ind] = 0;
	}
	ip = 0;
	for (ind = 0; ind < adsr_size; ind++) {
		ctrl [ip++] . adsr_constructor ();
		velocities [ind] = 0;
	}
	for (ind = 0; ind < lfo_size; ind++) ctrl [ip++] . lfo_constructor ();
	for (ind = 0; ind < key_glider_size; ind++) ctrl [ip++] . key_glider_constructor ();
	ip = 0;
	for (ind = 0; ind < total_adsr_size; ind++) {
		adsr_x [ind] = 0x7fffffff;
		adsr_y [ind] = 0;
		adsr_d [ind] = 0;
		adsr_delta [ip++] = 0;
		adsr_delta [ip++] = 0;
		adsr_delta [ip++] = 0;
		adsr_delta [ip++] = 0;
	}
	for (ind = total_stripes; ind < total_stripes + total_stripes; ind++) {
		adsr_y [ind] = 16383;
	}
	oscillator * op = osc;
	ip = 0;
	int panp = 0;
	for (ind = 0; ind < polyphony; ind++) {
		lfo_connections [ind] = 0;
		algo [ind] = & default_algo;
		op -> sth = sth;
		op -> pb = sth -> data_blocks [0];
		op -> pool = this;
		op -> stripes = stripes;
		op -> stereo = stereo;
		op -> vertical = vertical;
		op -> extra = NULL;
		op -> time = & time [ip];
		op -> freq = & delta_time [ip];
		op -> abstract_freq = & freq [ip];
		op -> ratio = & ratio [ip];
//		op -> freq = & freq [ip];
		op -> amp = & amp [ip];
		op -> index = & index [ip];
		op -> pan = & pan [panp];
		op -> feed = & feed [ip * feed_space];
//		op -> left_in = & this -> left_in;
//		op -> right_in = & this -> right_in;
		op -> left_in = op -> right_in = 0.0F;
		ip += stripes;
		panp += stereo;
		op++;
	}
}

oscillator_pool_cpp :: oscillator_pool_cpp (config * cfg, synthesiser * sth, dsp_line_pool * dsp) :
	oscillator_pool (cfg, sth) {
	core_number = cfg -> processors;
	int horizontal = cfg -> horizontal;
	constructor (sth, horizontal, cfg -> center_freq, cfg -> vertical, cfg -> head_room, cfg -> feed_space, dsp, cfg -> style_program_size);
}

oscillator_pool_cpp :: ~ oscillator_pool_cpp (void) {
	delete [] adsr_x;
	delete [] adsr_d;
	delete [] adsr_delta;
	delete [] adsr_y;
	delete [] lfo_out;
	delete [] lfo_outs;

	delete [] time;
	delete [] delta_time;
	delete [] freq; // amp, index, pan
	delete [] feed;
	delete [] freq_shift; // amp, index, pan
	delete [] lfo_freq_shift;
	delete [] ratio;
	delete [] velocities;
//	delete [] lfo_rnds;
	delete [] original_velocities;
	delete [] retriggers_required;
	delete [] freq_lfo_source;
	//@ index amp
//	delete [] index_amps;

	delete [] slopes;
	for (int ind = 0; ind < total_adsr_size; ind++) ctrl [ind] . destructor ();
	delete [] ctrl;
	delete [] osc;
	delete [] algo;
	delete [] lfo_connections;

	delete [] amp_target;
}

void oscillator_pool_cpp :: move_oscillators (int from, int to, int samples) {for (int ind = from; ind < to; ind++) move_oscillator (ind, samples);}
void oscillator_pool_cpp :: move_oscillator (int counter, int samples) {
	// processors [1] or more
	for (int sample = 0; sample < samples; sample++) {
		int sub;
		multi_line * lfo_outs_ip;
		float freq_calculator, freq_sentinel;
		oscillator * op = & osc [counter];
		int key_glide = key_glide_y [counter];
		int stripe_data_shift = counter * stripes;
		int stereo_data_shift = counter * stereo;
		int * as = & amp_shift [stripe_data_shift];
		int * at = & amp_target [stripe_data_shift];
		int * is = & index_shift [stripe_data_shift];
		int * it = & index_target [stripe_data_shift];
		for (int ind = 0; ind < stripes; ind++) {
			if (* as != * at) {
				if (* as < * at) {sub = * at - * as; if (sub <= 1024) (* as)++; else if (sub <= 2048) (* as) += 2; else (* as) += 4;}
				else {sub = * as - * at; if (sub <= 1024) (* as)--; else if (sub <= 2048) (* as) -= 2; else (* as) -= 4;}
			}
			as++; at++;
			if (* is != *it) {
				if (* is < * it) {sub = * it - * is; if (sub <= 1024) (* is)++; else if (sub <= 2048) (* is) += 2; else (* is) += 4;}
				else {sub = * is - * it; if (sub <= 1024) (* is)--; else if (sub <= 2048) (* is) -= 2; else (* is) -= 4;}
			}
			is++; it++;
		}
		as = & pan_shift [stereo_data_shift];
		at = & pan_target [stereo_data_shift];
		for (int ind = 0; ind < stereo; ind++) {
			if (* as != * at) {
				if (* as < * at) {sub = * at - * as; if (sub <= 1024) (* as)++; else if (sub <= 2048) (* as) += 2; else (* as) += 4;}
				else {sub = * as - * at; if (sub <= 1024) (* as)--; else if (sub <= 2048) (* as) -= 2; else (* as) -= 4;}
			}
			as++; at++;
		}
//		lfo_yp = & lfo_out [lfo_connections [counter]];
		lfo_outs_ip = & lfo_outs [lfo_connections [counter]];
		int * freq_ip = & freq [stripe_data_shift]; int * amp_ip = & amp [stripe_data_shift]; int * index_ip = & index [stripe_data_shift];
		int * freq_shift_p = & freq_shift [stripe_data_shift]; int * amp_shift_p = & amp_shift [stripe_data_shift]; int * index_shift_p = & index_shift [stripe_data_shift];
		int * freq_adsr_yp = & adsr_y [stripe_data_shift]; int * amp_adsr_yp = freq_adsr_yp + total_stripes; int * index_adsr_yp = amp_adsr_yp + total_stripes;
		int * freq_lfo_shiftp = & lfo_freq_shift [stripe_data_shift]; int * amp_lfo_shiftp = & lfo_amp_shift [stripe_data_shift]; int * index_lfo_shiftp = & lfo_index_shift [stripe_data_shift];
		int * freq_slope_p = & slopes [3 * stripe_data_shift]; int * amp_slope_p = freq_slope_p + total_stripes3; int * index_slope_p = amp_slope_p + total_stripes3;
		int * freq_lfo_source_ip = & freq_lfo_source [stripe_data_shift]; int * amp_lfo_source_ip = & amp_lfo_source [stripe_data_shift]; int * index_lfo_source_ip = & index_lfo_source [stripe_data_shift];
		for (int ind = 0; ind < stripes; ind++) {
			int freq_sub = key_glide - (* (freq_slope_p++));
			if (freq_sub > 0) {freq_slope_p++; freq_sub = (freq_sub * (* (freq_slope_p++))) >> 7;}
			else {freq_sub = (freq_sub * (* (freq_slope_p++))) >> 7; freq_slope_p++;}
			int amp_sub = key_glide - (* (amp_slope_p++));
			if (amp_sub > 0) {amp_slope_p++; amp_sub = (amp_sub * (* (amp_slope_p++))) >> 7;}
			else {amp_sub = (amp_sub * (* (amp_slope_p++))) >> 7; amp_slope_p++;}
			int index_sub = key_glide - (* (index_slope_p++));
			if (index_sub > 0) {index_slope_p++; index_sub = (index_sub * (* (index_slope_p++))) >> 7;}
			else {index_sub = (index_sub * (* (index_slope_p++))) >> 7; index_slope_p++;}
			//int lfo_output = (* lfo_outs_ip++) [sample];
			//int lfo_output = (* (lfo_outs_ip + (* freq_lfo_source_ip++))) [sample];
			* (freq_ip++) = * (freq_shift_p++) + * (freq_adsr_yp++) + freq_sub + ((* (freq_lfo_shiftp++) * ((lfo_outs_ip [* freq_lfo_source_ip++]) [sample])) >> 14);
			* (amp_ip++) = * (amp_shift_p++) + * (amp_adsr_yp++) + amp_sub + ((* (amp_lfo_shiftp++) * ((lfo_outs_ip [* amp_lfo_source_ip++]) [sample])) >> 14);
			* (index_ip++) = * (index_shift_p++) + * (index_adsr_yp++) + index_sub + ((* (index_lfo_shiftp++) * ((lfo_outs_ip [* index_lfo_source_ip++]) [sample])) >> 14);
			//lfo_yp++;
		}
		int * pan_ip = & pan [stereo_data_shift];
		int * pan_shift_p = & pan_shift [stereo_data_shift];
		int * pan_adsr_yp = & adsr_y [total_stripes3 + stereo_data_shift];
		int * pan_lfo_shiftp = & lfo_pan_shift [stereo_data_shift];
		int * pan_slope_p = & slopes [3 * (total_stripes3 + stereo_data_shift)];
		int * pan_lfo_source_ip = & pan_lfo_source [stereo_data_shift];
		for (int ind = 0; ind < stereo; ind++) {
			int pan_sub = key_glide - (* (pan_slope_p++));
			if (pan_sub > 0) {pan_slope_p++; pan_sub = (pan_sub * (* (pan_slope_p++))) >> 7;}
			else {pan_sub = (pan_sub * (* (pan_slope_p++))) >> 7; pan_slope_p++;}
			* (pan_ip++) = * (pan_shift_p++) + * (pan_adsr_yp++) + pan_sub + ((* (pan_lfo_shiftp++) * ((lfo_outs_ip [* pan_lfo_source_ip++]) [sample])) >> 14);
		}
		freq_sentinel = (float) freq_logs [16383];
		int * freq_p = & freq [stripe_data_shift];
		int * ratio_p = & ratio [stripe_data_shift];
		unsigned int * delta_time_p = op -> freq;
		int * amp_p = op -> amp;
		int * index_p = op -> index;
		for (int ind = 0; ind < stripes; ind++) {
			int v = * (freq_p++); if (v > 16383) v = 16383; if (v < 0) v = 0;
			sub = * (ratio_p++);
			if (sub > 0) {
				freq_calculator = (float) freq_logs [v] * (float) sub;
				* (delta_time_p++) = freq_calculator > freq_sentinel ? (unsigned int) freq_sentinel : (unsigned int) freq_calculator;
			} else * (delta_time_p++) = freq_logs [v] >> - sub;
			v = * amp_p; if (v > 16383) v = 16383; if (v < 0) v = 0; * (amp_p++) = amp_logs [v];
			v = * index_p; if (v > 16383) v = 16383; if (v < 0) v = 0; * (index_p++) = v;
		}
		int * pan_p = op -> pan;
		int * multi_pan_p = & op -> multi_pan [sample * stereo];
		for (int ind = 0; ind < stereo; ind++) {int v = * (pan_p++); if (v > 8191) v = 8191; if (v < -8191) v = -8191; * (multi_pan_p++) = v;}
		op -> noise_in = rnd . outs [sample]; op -> left_in = this -> left_ins [sample]; op -> right_in = this -> right_ins [sample];
		op -> outputs [sample] = algo [counter] -> code (op);
//		dsp -> add (part_connections [counter], op -> outputs [sample], op -> pan);
		unsigned int * time_p = & time [stripe_data_shift]; delta_time_p = & delta_time [stripe_data_shift];
		for (int ind = 0; ind < stripes; ind++) {* (time_p++) += * (delta_time_p++);}
		//op++;
		for (int ind = 0; ind < stripes; ind++) {
			adsr_move (ind + stripe_data_shift, sample); // freq
			adsr_move (ind + stripe_data_shift + total_stripes, sample); // amp
			adsr_move (ind + stripe_data_shift + total_stripes + total_stripes, sample); // index
		}
		for (int ind = 0; ind < stereo; ind++) {
			adsr_move (ind + total_stripes3 + stereo_data_shift, sample); // pan
		}
//		lfo is outside this loop
		adsr_move (adsr_size + lfo_size + counter, sample);
	}
}

void oscillator_pool_cpp :: transfer_to_dsp (int sample) {
	oscillator * op = osc;
	int pan_address = sample * stereo;
	for (int ind = 0; ind < polyphony; ind++) {
		dsp -> add (part_connections [ind], op -> outputs [sample], & op -> multi_pan [pan_address]);
		op++;
	}
}

void oscillator_pool_cpp :: multi_move_oscillators (int samples) {
	if (core_number < 2) move_oscillators (0, polyphony, samples);
	else {
	#pragma omp parallel for
		for (int ind = 0; ind < polyphony; ind++) {
			//printf ("<%i %i %i %i> ", ind, omp_get_thread_num (), omp_get_num_threads (), samples);
			move_oscillator (ind, samples);
		}
		//printf ("\n");
	}
}

void oscillator_pool_cpp :: multi_move_parts (int samples) {
	for (int sample = 0; sample < samples; sample++) {
		int * lfo_out_ip = lfo_out;
		multi_line * lfo_outs_ip = lfo_outs;
		int * lfo_yp = lfo_y;
		adsr_control_block * ctrl_p = lfo_ctrl;
		for (int counter = 0; counter < lfo_size; counter++) {
			(* lfo_outs_ip++) [sample] = * lfo_out_ip;
			if ((ctrl_p++) -> wave == 0) * (lfo_out_ip++) = * (lfo_yp++);
			else * (lfo_out_ip++) = lfo_sine_wave [8192 + * (lfo_yp++)];
			adsr_move (counter + adsr_size, sample);
		}
		rnd . move ();
		rnd . outs [sample] = rnd . out;
	}
}

void oscillator_pool_cpp :: multi_move_dsp (int samples) {
	for (int sample = 0; sample < samples; sample++) {
		transfer_to_dsp (sample);
		dsp -> multi_move (sample);
	}
}

void oscillator :: abstract_freq_to_real (int ind, int abstract_freq) {
	if (abstract_freq > 16383) abstract_freq = 16383;
	if (abstract_freq < 0) abstract_freq = 0;
	int sub = ratio [ind];
	if (sub > 0) {
		float freq_calculator = (float) freq_logs [abstract_freq] * (float) sub;
		float freq_sentinel = (float) freq_logs [16383];
		freq [ind] = freq_calculator > freq_sentinel ? (unsigned int) freq_sentinel : (unsigned int) freq_calculator;
	} else freq [ind] = freq_logs [abstract_freq] >> - sub;
}

void oscillator_pool_cpp :: move (void) {
	// processors [0]
	int counter;
	int ind;
	int sub;
	int c2;
	int * adsr_xp = adsr_x;
	int * adsr_dp = adsr_d;
	int * adsr_deltap = adsr_delta;
	int * adsr_yp = adsr_y;
	adsr_control_block * ctrl_p;
	unsigned int * time_p = time;
	unsigned int * delta_time_p = delta_time;
	float freq_calculator, freq_sentinel;
	int * ip = freq;
	algo_cpp_pointer * ap = algo;
	oscillator * op = osc;
	int * slope_p = slopes;
	int * lfo_shiftp = lfo_freq_shift;
	int * shift_p = freq_shift;
	int * key_glide_yp;
	int * lfo_yp;
	int * ratio_p = ratio;
	//@ index amp
//	bool * index_amp_p;
//	int * amp_p;
	int dx, dy, y0, y1, y2, d0, d1, d2;

	// amplitude control filter
	int * as = amp_shift;
	int * at = amp_target;
	for (ind = total_stripes; ind > 0; ind--) {
		if (* as != * at) {
			if (* as < * at) {
				sub = * at - * as;
				if (sub <= 1024) (* as)++;
				else if (sub <= 2048) (* as) += 2;
				else (* as) += 4;
			} else {
				sub = * as - * at;
				if (sub <= 1024) (* as)--;
				else if (sub <= 2048) (* as) -= 2;
				else (* as) -= 4;
			}
		}
		as++; at++;
	}
	as = index_shift;
	for (ind = total_stripes; ind > 0; ind--) {
		if (* as != * at) {
			if (* as < * at) {
				sub = * at - * as;
				if (sub <= 1024) (* as)++;
				else if (sub <= 2048) (* as) += 2;
				else (* as) += 4;
			} else {
				sub = * as - * at;
				if (sub <= 1024) (* as)--;
				else if (sub <= 2048) (* as) -= 2;
				else (* as) -= 4;
			}
		}
		as++; at++;
	}
	as = pan_shift;
	for (ind = total_stereo; ind > 0; ind--) {
		if (* as != * at) {
			if (* as < * at) {
				sub = * at - * as;
				if (sub <= 1024) (* as)++;
				else if (sub <= 2048) (* as) += 2;
				else (* as) += 4;
			} else {
				sub = * as - * at;
				if (sub <= 1024) (* as)--;
				else if (sub <= 2048) (* as) -= 2;
				else (* as) -= 4;
			}
		}
		as++; at++;
	}
	// freq = (osc_freq_shift + ADSR + key * slope + LFO * sens) * ratio
	// amp = osc_amp_shift + ADSR + key * slope + LFO * sens
	// index = osc_index_shift + ADSR + key * slope + LFO * sens
	// pan = osc_pan_shift + ADSR + key * slope + LFO * sens
	// freq = log (freq)
	// amp = log (amp)
	// freq, amp, index keys * slope
	int * lfo_source_p = freq_lfo_source;
	for (c2 = 3; c2 > 0; c2--) {
		key_glide_yp = key_glide_y;
		for (counter = 0; counter < polyphony; counter++) {
			lfo_yp = & lfo_out [lfo_connections [counter]];
			for (ind = stripes; ind > 0; ind--) {
				sub = (* key_glide_yp) - (* (slope_p++));
				if (sub > 0) {
					slope_p++;
					sub = (sub * (* (slope_p++))) >> 7;
				} else {
					sub = (sub * (* (slope_p++))) >> 7;
					slope_p++;
				}
				* (ip++) = * (shift_p++) + * (adsr_yp++) + sub + ((* (lfo_shiftp++) * * (lfo_yp + * (lfo_source_p++))) >> 14);
			}
			key_glide_yp++;
		}
	}
	// pan keys * slope
	key_glide_yp = key_glide_y;
	for (counter = 0; counter < polyphony; counter++) {
		lfo_yp = & lfo_out [lfo_connections [counter] + stripes];
		for (ind = stereo; ind > 0; ind--) {
			sub = (* key_glide_yp) - (* (slope_p++));
			if (sub > 0) {
				slope_p++;
				sub = (sub * (* (slope_p++))) >> 7;
			} else {
				sub = (sub * (* (slope_p++))) >> 7;
				slope_p++;
			}
			* (ip++) = * (shift_p++) + * (adsr_yp++) + sub + ((* (lfo_shiftp++) * * (lfo_yp++)) >> 14);
		}
		key_glide_yp++;
	}

	// freq *= ratio; freq = log (freq);
	ip = freq;
	freq_sentinel = (float) freq_logs [16383];
	for (counter = total_stripes; counter > 0; counter--) {
		ind = * (ip++);
		if (ind > 16383) ind = 16383;
		if (ind < 0) ind = 0;
		sub = * (ratio_p++);

		if (sub > 0) {
			freq_calculator = (float) freq_logs [ind] * (float) sub;
			if (freq_calculator > freq_sentinel) * (delta_time_p++) = (unsigned int) freq_sentinel;
			else * (delta_time_p++) = (unsigned int) freq_calculator;
		} else * (delta_time_p++) = freq_logs [ind] >> -sub;
	}
	for (counter = total_stripes; counter > 0; counter--) {
		ind = * ip;
		if (ind > 16383) ind = 16383;
		if (ind < 0) ind = 0;
		* (ip++) = amp_logs [ind];
	}
	// inidexing amplitudes
	//@ index amp
//	if (index_amp_required) {
//		index_amp_p = index_amps;
//		amp_p = amp;
//		for (counter = total_stripes; counter > 0; counter--) {
//			ind = * ip;
//			if (ind > 16383) ind = 16383;
//			if (ind < 0) ind = 0;
//			* (ip++) = ind;
//			if (* (index_amp_p++)) * amp_p = (int) ((float) (* amp_p) * (float) ind / 16383.0f);
//			amp_p++;
//		}
//	} else {
	for (counter = total_stripes; counter > 0; counter--) {
		ind = * ip;
		if (ind > 16383) ind = 16383;
		if (ind < 0) ind = 0;
		* (ip++) = ind;
	}
//	}
	for (counter = total_stereo; counter > 0; counter--) {
		ind = * ip;
		if (ind > 8191) ind = 8191;
		if (ind < -8191) ind = -8191;
		* (ip++) = ind;
	}

	// call algo
	ip = part_connections;
	for (counter = polyphony; counter > 0; counter--) {
		op -> noise_in = rnd . out; op -> left_in = this -> left_in; op -> right_in = this -> right_in;
		dsp -> add (* (ip++), (* (ap++)) -> code (op), op -> pan);
		op++;
	}
	dsp -> move ();

	// time += freq
	//ip = freq;
	delta_time_p = delta_time;
	for (counter = total_stripes; counter > 0; counter --) * (time_p++) += * (delta_time_p++);

	// update ADSR, LFO, KEY GLIDE (Bresenham's algorithm for drawing lines)
	adsr_yp = adsr_y;
	for (counter = total_adsr_size; counter > 0; counter--) {
		ind = * adsr_xp;
		* adsr_xp = ind - 1;
		adsr_xp++;
		if (ind > 0) {
//		if ((* (adsr_xp++)) -- > 0) {
			if (* adsr_dp >= 0) {
				* (adsr_dp++) += * (adsr_deltap++);
				* (adsr_yp++) += * (adsr_deltap++);
				adsr_deltap += 2;
			} else {
				adsr_deltap += 2;
				* (adsr_dp++) += * (adsr_deltap++);
				* (adsr_yp++) += * (adsr_deltap++);
			}
		} else {
			adsr_xp--;
			ctrl_p = & ctrl [total_adsr_size - counter];
			ind = ctrl_p -> next_index [ctrl_p -> index];
			ctrl_p -> index = ind;
			switch (ctrl_p -> wave) {
			case 2: case 3:
				switch (ind) {
				case 0: case 2:
					dx = 2 + ctrl_p -> x [ind];
					y0 = ctrl_p -> target;
					dy = rnd . out - y0;
					ctrl_p -> target = rnd . out;
					y2 = dy / dx;
					y1 = y2;
					if (dy > 0) y1++;
					if (dy < 0) {y1--; dy = - dy;}
					dy %= dx;
					d2 = dy + dy;
					d0 = d2 - dx;
					d1 = d0 - dx;
					dx -= 2;
					* (adsr_xp++) = dx;
					* (adsr_dp++) = d0;
					* (adsr_deltap++) = d1;
					* (adsr_deltap++) = y1;
					* (adsr_deltap++) = d2;
					* (adsr_deltap++) = y2;
					* (adsr_yp++) = y0;
					break;
				default:
					* (adsr_xp++) = ctrl_p -> x [ind];
					* (adsr_dp++) = 0;
					* (adsr_deltap++) = 0;
					* (adsr_deltap++) = 0;
					* (adsr_deltap++) = 0;
					* (adsr_deltap++) = 0;
					* (adsr_yp++) = ctrl_p -> target;
					break;
				}
				break;
			default:
				* (adsr_xp++) = ctrl_p -> x [ind];
				* (adsr_dp++) = ctrl_p -> d0 [ind];
				* (adsr_deltap++) = ctrl_p -> d1 [ind];
				* (adsr_deltap++) = ctrl_p -> y1 [ind];
				* (adsr_deltap++) = ctrl_p -> d2 [ind];
				* (adsr_deltap++) = ctrl_p -> y2 [ind];
				* (adsr_yp++) = ctrl_p -> y0 [ind];
				break;
			}
		}
	}
	ip = lfo_out;
	lfo_yp = lfo_y;
	ctrl_p = lfo_ctrl;
	for (counter = lfo_size; counter > 0; counter--) {
		if ((ctrl_p++) -> wave == 0) * (ip++) = * (lfo_yp++);
		else * (ip++) = lfo_sine_wave [8192 + * (lfo_yp++)];
	}
	rnd . move ();
}

void oscillator_pool_cpp :: calculate_velocities (int oscillator, int velocity) {
	parameter_block * pb = osc [oscillator] . pb;
	original_velocities [oscillator] = velocity;
	int * ip = & velocities [oscillator * velocity_size];
	int ind;
	int random;
	stripe_parameter_block * spb;
	stereo_stripe_parameter_block * stpb;
	for (ind = 0; ind < stripes; ind++) {
		spb = pb -> stripes [ind];
		random = rnd . move () * (spb -> freq_rnd) / 16384;
		* (ip++) = spb -> freq_sens . velocity . get (velocity) + random;
	}
	for (ind = 0; ind < stripes; ind++) {
		spb = pb -> stripes [ind];
		random = spb -> amp_rnd;
		random = ((1 + random) >> 1) + rnd . move () * (random) / 16384;
		* (ip++) = spb -> amp_sens . velocity . get_negative (velocity) - random;
	}
	for (ind = 0; ind < stripes; ind++) {
		spb = pb -> stripes [ind];
		random = rnd . move () * (spb -> index_rnd) / 16384;
		* (ip++) = spb -> index_sens . velocity . get (velocity) + random;
	}
	for (ind = 0; ind < stereo; ind++) {
		stpb = pb -> stereo [ind];
		random = rnd . move () * (stpb -> rnd) / 16384;
		* (ip++) = stpb -> sens . velocity . get (velocity) + random;
	}
}

void oscillator_pool_cpp :: notify_oscillator_assign (parameter_block * pb, int oscillator) {
	if (osc [oscillator] . pb -> factory_id == pb -> factory_id) return;
	osc [oscillator] . pb = pb;
	algo_change (oscillator);
	lfo_connections [oscillator] = part_connections [oscillator] * stripes_stereo;
	load_ratios (oscillator);
	load_sensitivity (oscillator);
}

void oscillator_pool_cpp :: key_on (int oscillator, int key) {retrigger_key_glider (oscillator, false);}

void oscillator_pool_cpp :: key_on (int oscillator, int key, int velocity) {
	calculate_velocities (oscillator, velocity);
	retrigger_key_glider (oscillator, true);
	all_adsr_update_head (oscillator);
	adsr_switch (oscillator, 0);
	lfo_retrigger (oscillator);
	load_all_shifts (oscillator, true);
	load_lfo_all_shifts (oscillator);
	retrigger_times (oscillator);

}

void oscillator_pool_cpp :: key_on (parameter_block * pb, int oscillator, int key, int velocity) {
	notify_oscillator_assign (pb, oscillator);
	calculate_velocities (oscillator, velocity);
	retrigger_key_glider (oscillator, false);
	all_adsr_update_head (oscillator);
	adsr_retrigger (oscillator, 0);
	lfo_retrigger (oscillator);
	load_all_shifts (oscillator, true);
	load_lfo_all_shifts (oscillator);
	retrigger_times (oscillator);
}

void oscillator_pool_cpp :: notify_key_off (int oscillator) {
	all_adsr_update_tail (oscillator);
	adsr_switch (oscillator, 4);
	release_times (oscillator);
}

void oscillator_pool_cpp :: notify_ground (int oscillator) {
	calculate_velocities (oscillator, 100);
	all_adsr_update_tail (oscillator);
	adsr_retrigger (oscillator, 5);
	load_all_shifts (oscillator, true);
	load_lfo_all_shifts (oscillator);
	ground_times (oscillator);
}

void oscillator_pool_cpp :: retrigger_times (int index) {
	oscillator * o = & osc [index];
	parameter_block * pb = o -> pb;
	unsigned int * uip = & time [index * stripes];
	((algo_cpp *) pb -> algo_head) -> retrigger (o);
	for (int ind = 0; ind < stripes; ind++) {
		if (pb -> stripes [ind] -> retrigger) * uip = 0;
		uip++;
	}
}

void oscillator_pool_cpp :: release_times (int index) {
	oscillator * o = & osc [index];
	parameter_block * pb = o -> pb;
	((algo_cpp *) pb -> algo_head) -> release (o);
}

void oscillator_pool_cpp :: ground_times (int index) {
	oscillator * o = & osc [index];
	parameter_block * pb = o -> pb;
	((algo_cpp *) pb -> algo_head) -> ground (o);
}

void oscillator_pool_cpp :: retrigger_key_glider (int ind, bool skipper) {
	parameter_block * pb = osc [ind] . pb;
	controllers_parameter_block * cpb = pb -> controllers;
	int key = keys [ind];
	int last_key = pb -> last_key;
	int dx = time_delays [cpb -> porta_time];
	int ind2 = ind << 2;
	skipper &= cpb -> porta_mode == 0;
	if (! cpb -> porta || last_key < 0 || skipper || dx < 2) {
		key_glide_y [ind] = key;
		key_glide_x [ind] = 0x7fffffff;
		key_glide_d [ind] = 0;
		key_glide_delta [ind2++] = 0;
		key_glide_delta [ind2++] = 0;
		key_glide_delta [ind2++] = 0;
		key_glide_delta [ind2] = 0;
		key_glide_ctrl [ind] . y0 [0] = key;
		return;
	}
	int dy = key;
	if (cpb -> poly && cpb -> porta_mode == 0) {
		dy -= last_key;
		key_glide_y [ind] = last_key;
	} else {
		dy -= key_glide_y [ind];
	}
	int y2 = dy / dx;
	int y1 = y2;
	if (dy > 0) y1++;
	if (dy < 0) {y1--; dy = - dy;}
	dy %= dx;
	int d2 = dy + dy;
	int d0 = d2 - dx;
	int d1 = d0 - dx;

	key_glide_x [ind] = dx - 2;
	key_glide_d [ind] = d0;
	key_glide_delta [ind2++] = d1;
	key_glide_delta [ind2++] = y1;
	key_glide_delta [ind2++] = d2;
	key_glide_delta [ind2] = y2;
	key_glide_ctrl [ind] . y0 [0] = key;
}

static int get_stage (int stage, int y, adsr_control_block * ctrl) {
	if (stage != 0) return stage;
	if (ctrl -> x [0] == 0x7fffffff) return 0;
	if (ctrl -> x [1] == 0x7fffffff) return 0;
	int y_to = ctrl -> y_target [0];
	if (y_to != ctrl -> y0 [1]) return stage;
	int y_from = ctrl -> y_target [4];
	int y_to2 = ctrl -> y_target [1];
	if (y_from > y_to && y_to > y_to2 && y_to > y) return 1;
	if (y_from < y_to && y_to < y_to2 && y_to < y) return 1;
	return 0;
}

void oscillator_pool_cpp :: adsr_switch (int oscillator, int target_stage) {
	int counter;
	int ind = oscillator * stripes;
	adsr_control_block * ctrl_p = & ctrl [ind];
	int * adsr_xp = & adsr_x [ind];
	int * adsr_dp = & adsr_d [ind];
	int * adsr_yp = & adsr_y [ind];
	int * adsr_deltap = & adsr_delta [ind << 2];
	int dx, d0, d1, d2, y1, y2, dy;
	for (int c2 = 3; c2 > 0; c2--) {
		for (counter = stripes; counter > 0; counter--) {
			int stage = get_stage (target_stage, * adsr_yp, ctrl_p);
			ctrl_p -> index = stage;

			dx = ctrl_p -> x [stage];
			* (adsr_xp++) = dx;
			dx += 2;
//			dy = (ctrl_p++) -> y0 [stage + 1] - * (adsr_yp++);
			dy = (ctrl_p++) -> y_target [stage] - * (adsr_yp++);
			y2 = dy / dx;
			y1 = y2;
			if (dy > 0) y1++;
			if (dy < 0) {y1--; dy = - dy;}
			dy %= dx;
			d2 = dy + dy;
			d0 = d2 - dx;
			d1 = d0 - dx;

			* (adsr_dp++) = d0;
			* (adsr_deltap++) = d1;
			* (adsr_deltap++) = y1;
			* (adsr_deltap++) = d2;
			* (adsr_deltap++) = y2;
		}
		adsr_xp += stripe_shift;
		adsr_dp += stripe_shift;
		adsr_deltap += stripe_shift4;
		adsr_yp += stripe_shift;
		ctrl_p += stripe_shift;
	}
	ind = oscillator * stereo;
	adsr_xp = & pan_x [ind];
	adsr_dp = & pan_d [ind];
	adsr_deltap = & pan_delta [ind << 2];
	adsr_yp = & pan_y [ind];
	ctrl_p = & pan_ctrl [ind];
	for (counter = stereo; counter > 0; counter--) {
		int stage = get_stage (target_stage, * adsr_yp, ctrl_p);
		ctrl_p -> index = stage;

		dx = ctrl_p -> x [stage];
		* (adsr_xp++) = dx;
		dx += 2;
//		dy = (ctrl_p++) -> y0 [stage + 1] - * (adsr_yp++);
		dy = (ctrl_p++) -> y_target [stage] - * (adsr_yp++);
		y2 = dy / dx;
		y1 = y2;
		if (dy > 0) y1++;
		if (dy < 0) {y1--; dy = - dy;}
		dy %= dx;
		d2 = dy + dy;
		d0 = d2 - dx;
		d1 = d0 - dx;

		* (adsr_dp++) = d0;
		* (adsr_deltap++) = d1;
		* (adsr_deltap++) = y1;
		* (adsr_deltap++) = d2;
		* (adsr_deltap++) = y2;
	}
}

void oscillator_pool_cpp :: adsr_retrigger (int oscillator, int stage) {
	int counter;
	int ind = oscillator * stripes;
	adsr_control_block * ctrl_p = & ctrl [ind];
	int * adsr_xp = & adsr_x [ind];
	int * adsr_dp = & adsr_d [ind];
	int * adsr_yp = & adsr_y [ind];
	int * adsr_deltap = & adsr_delta [ind << 2];
	for (int c2 = 3; c2 > 0; c2--) {
		for (counter = stripes; counter > 0; counter--) {
			ctrl_p -> index = stage;
			* (adsr_xp++) = ctrl_p -> x [stage];
			* (adsr_dp++) = ctrl_p -> d0 [stage];
			* (adsr_deltap++) = ctrl_p -> d1 [stage];
			* (adsr_deltap++) = ctrl_p -> y1 [stage];
			* (adsr_deltap++) = ctrl_p -> d2 [stage];
			* (adsr_deltap++) = ctrl_p -> y2 [stage];
			* (adsr_yp++) = (ctrl_p++) -> y0 [stage];
		}
		adsr_xp += stripe_shift;
		adsr_dp += stripe_shift;
		adsr_deltap += stripe_shift4;
		adsr_yp += stripe_shift;
		ctrl_p += stripe_shift;
	}
	ind = oscillator * stereo;
	adsr_xp = & pan_x [ind];
	adsr_dp = & pan_d [ind];
	adsr_deltap = & pan_delta [ind << 2];
	adsr_yp = & pan_y [ind];
	ctrl_p = & pan_ctrl [ind];
	for (counter = stereo; counter > 0; counter--) {
		ctrl_p -> index = stage;
		* (adsr_xp++) = ctrl_p -> x [stage];
		* (adsr_dp++) = ctrl_p -> d0 [stage];
		* (adsr_deltap++) = ctrl_p -> d1 [stage];
		* (adsr_deltap++) = ctrl_p -> y1 [stage];
		* (adsr_deltap++) = ctrl_p -> d2 [stage];
		* (adsr_deltap++) = ctrl_p -> y2 [stage];
		* (adsr_yp++) = (ctrl_p++) -> y0 [stage];
	}
}

void oscillator_pool_cpp :: all_adsr_update_head (int ind) {
	int x, y;
	int index = ind * stripes;
	for (x = 0; x < 3; x++) {
		for (y = 0; y < stripes; y++) {
			adsr_update_head (ind, index++, x, y);
		}
		index += stripe_shift;
	}
	index = total_stripes3 + ind * stereo;
	for (x = 0; x < stereo; x++) {
		adsr_update_head (ind, index++, 3, x);
	}
}

void oscillator_pool_cpp :: all_adsr_update_tail (int ind) {
	int x, y;
	int index = ind * stripes;
	for (x = 0; x < 3; x++) {
		for (y = 0; y < stripes; y++) {
			adsr_update_tail (ind, index++, x, y);
		}
		index += stripe_shift;
	}
	index = total_stripes3 + ind * stereo;
	for (x = 0; x < stereo; x++) {
		adsr_update_tail (ind, index++, 3, x);
	}
}

void oscillator_pool_cpp :: adsr_update_head (int ind, int adsr_ind, int type, int stripe) {
	int key = keys [ind];
	int velocity = original_velocities [ind];
	parameter_block * pb = osc [ind] . pb;
	adsr_parameter_block * apb;
	controllers_parameter_block * cpb = pb -> controllers;
	sensitivity_parameter_block * spb;
	adsr_control_block * ctrl_pb = & ctrl [adsr_ind];
	switch (type) {
	case 0:
		apb = & pb -> stripes [stripe] -> freq_eg;
		spb = & pb -> stripes [stripe] -> freq_sens;
		break;
	case 1:
		apb = & pb -> stripes [stripe] -> amp_eg;
		spb = & pb -> stripes [stripe] -> amp_sens;
		break;
	case 2:
		apb = & pb -> stripes [stripe] -> index_eg;
		spb = & pb -> stripes [stripe] -> index_sens;
		break;
	default:
		apb = & pb -> stereo [stripe] -> eg;
		spb = & pb -> stereo [stripe] -> sens;
		break;
	}
	int dx, d0, d1, d2, y1, y2, dy;
	int y0 = apb -> level [3];
	if (type == 1) y0 = - y0;				// because of amp lut [0 .. 16383]
	int target;
	int index;
	int sub = 0;

	for (index = 0; index < 3; index++) {
		dx = apb -> time [index];
		target = apb -> level [index];
		if (type == 1) target = - target;	// because of amp lut [0 .. 16383]

		switch (index) {
		case 0: case 1: dx += spb -> attack . get (cpb -> attack) + spb -> key_attack_eg_scaling . get (key) + spb -> velocity_attack_eg_scaling . get (velocity); break;
		case 2:
			dx += spb -> decay . get (cpb -> decay) + spb -> key_eg_scaling . get (key) + spb -> velocity_eg_scaling . get (velocity);
			if (type != 1) target += spb -> sustain . get (cpb -> sustain);
			else target -= spb -> sustain . get_negative (cpb -> sustain); // amp lut
			break;
		}

		if (dx > 0 || index == 0) {
			if (dx < 0) dx = 0;
			if (dx > 16383) dx = 16383;
			dx = time_delays [dx];
			if (dx < 1) dx = 1;
			dy = target - y0;
			y2 = dy / dx;
			y1 = y2;
			if (dy > 0) y1++;
			if (dy < 0) {y1--; dy = - dy;}
			dy %= dx;
			d2 = dy + dy;
			d0 = d2 - dx;
			d1 = d0 - dx;
			ctrl_pb -> x [sub] = dx - 2;
			ctrl_pb -> d0 [sub] = d0;
			ctrl_pb -> d1 [sub] = d1;
			ctrl_pb -> d2 [sub] = d2;
			ctrl_pb -> y0 [sub] = y0;
			ctrl_pb -> y1 [sub] = y1;
			ctrl_pb -> y2 [sub] = y2;
			ctrl_pb -> y_target [sub] = target;
			sub++;
		}
		y0 = target;
	}

	while (sub < 4) {
		ctrl_pb -> x [sub] = 0x7fffffff;
		ctrl_pb -> d0 [sub] = 0;
		ctrl_pb -> d1 [sub] = 0;
		ctrl_pb -> d2 [sub] = 0;
		ctrl_pb -> y0 [sub] = y0;
		ctrl_pb -> y1 [sub] = 0;
		ctrl_pb -> y2 [sub] = 0;
		ctrl_pb -> y_target [sub] = 0;
		sub++;
	}
}

void oscillator_pool_cpp :: adsr_update_tail (int ind, int adsr_ind, int type, int stripe) {
	parameter_block * pb = osc [ind] . pb;
	adsr_parameter_block * apb;
	controllers_parameter_block * cpb = pb -> controllers;
	sensitivity_parameter_block * spb;
	adsr_control_block * ctrl_pb = & ctrl [adsr_ind];
	int velocity = original_velocities [ind];
	switch (type) {
	case 0:
		apb = & pb -> stripes [stripe] -> freq_eg;
		spb = & pb -> stripes [stripe] -> freq_sens;
		break;
	case 1:
		apb = & pb -> stripes [stripe] -> amp_eg;
		spb = & pb -> stripes [stripe] -> amp_sens;
		break;
	case 2:
		apb = & pb -> stripes [stripe] -> index_eg;
		spb = & pb -> stripes [stripe] -> index_sens;
		break;
	default:
		apb = & pb -> stereo [stripe] -> eg;
		spb = & pb -> stereo [stripe] -> sens;
		break;
	}
	int dx = apb -> time [3];
	dx += spb -> release . get (cpb -> release);
	dx += spb -> key_eg_scaling . get (keys [ind]);
	dx += spb -> velocity_eg_scaling . get (velocity); // original_velocities [ind]
	int target = apb -> level [3];
	if (type == 1) target = - target;		// because of amp lut [0 .. 16383]
	if (dx < 0) dx = 0;
	if (dx > 16383) dx = 16383;
	dx = time_delays [dx];
	if (dx < 1) dx = 1;

	ctrl_pb -> x [4] = dx;
	ctrl_pb -> d0 [4] = 0;
	ctrl_pb -> d1 [4] = 0;
	ctrl_pb -> d2 [4] = 0;
	ctrl_pb -> y0 [4] = 0;
	ctrl_pb -> y1 [4] = 0;
	ctrl_pb -> y2 [4] = 0;
	ctrl_pb -> y_target [4] = target;

	ctrl_pb -> x [5] = 0x7fffffff;
	ctrl_pb -> d0 [5] = 0;
	ctrl_pb -> d1 [5] = 0;
	ctrl_pb -> d2 [5] = 0;
	ctrl_pb -> y0 [5] = target;
	ctrl_pb -> y1 [5] = 0;
	ctrl_pb -> y2 [5] = 0;
	ctrl_pb -> y_target [5] = 0;
}

void oscillator_pool_cpp :: adsr_move (int ind, int sample) {
	int dx, dy, y0, y1, y2, d0, d1, d2;
	int sub = adsr_x [ind];
	adsr_x [ind] = sub - 1;
	int * adsr_deltap = & adsr_delta [ind << 2];
	if (sub > 0) {
		if (adsr_d [ind] >= 0) {
			adsr_d [ind] += * (adsr_deltap++);
			adsr_y [ind] += * (adsr_deltap++);
			adsr_deltap += 2;
		} else {
			adsr_deltap += 2;
			adsr_d [ind] += * (adsr_deltap++);
			adsr_y [ind] += * (adsr_deltap++);
		}
	} else {
		adsr_control_block * ctrl_p = & ctrl [ind];
		sub = ctrl_p -> next_index [ctrl_p -> index];
		ctrl_p -> index = sub;
		switch (ctrl_p -> wave) {
		case 2: case 3:
			switch (sub) {
			case 0: case 2:
				dx = 2 + ctrl_p -> x [sub];
				y0 = ctrl_p -> target;
				dy = rnd . outs [sample] - y0;
				ctrl_p -> target = rnd . outs [sample];
				y2 = dy / dx;
				y1 = y2;
				if (dy > 0) y1++;
				if (dy < 0) {y1--; dy = - dy;}
				dy %= dx;
				d2 = dy + dy;
				d0 = d2 - dx;
				d1 = d0 - dx;
				dx -= 2;
				adsr_x [ind] = dx;
				adsr_d [ind] = d0;
				* (adsr_deltap++) = d1;
				* (adsr_deltap++) = y1;
				* (adsr_deltap++) = d2;
				* (adsr_deltap++) = y2;
				adsr_y [ind] = y0;
				break;
			default:
				adsr_x [ind] = ctrl_p -> x [sub];
				adsr_d [ind] = 0;
				* (adsr_deltap++) = 0;
				* (adsr_deltap++) = 0;
				* (adsr_deltap++) = 0;
				* (adsr_deltap++) = 0;
				adsr_y [ind] = ctrl_p -> target;
				break;
			}
			break;
		default:
			adsr_x [ind] = ctrl_p -> x [sub];
			adsr_d [ind] = ctrl_p -> d0 [sub];
			* (adsr_deltap++) = ctrl_p -> d1 [sub];
			* (adsr_deltap++) = ctrl_p -> y1 [sub];
			* (adsr_deltap++) = ctrl_p -> d2 [sub];
			* (adsr_deltap++) = ctrl_p -> y2 [sub];
			adsr_y [ind] = ctrl_p -> y0 [sub];
			break;
		}
	}
}

void oscillator_pool_cpp :: notify_sensitivity_change (parameter_block * pb, int nrpn_msb, int xxx, int nnnn) {
	nrpn_msb &= 0xf;
	if (nrpn_msb != 6) return;
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_sensitivity (ind);
}

void oscillator_pool_cpp :: load_sensitivity (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	oscillator *= 3;
	int * slope_p = & slopes [stripes * oscillator];
	int ind;
	sub_parameter_block * spb;
	for (ind = 0; ind < stripes; ind++) {
		spb = & pb -> stripes [ind] -> freq_sens . key;
		* (slope_p++) = spb -> bp;
		* (slope_p++) = - spb -> lc;
		* (slope_p++) = spb -> rc;
	}
	slope_p += slope_shift;
	for (ind = 0; ind < stripes; ind++) {
		spb = & pb -> stripes [ind] -> amp_sens . key;
		* (slope_p++) = spb -> bp;
		* (slope_p++) = spb -> lc;
		* (slope_p++) = - spb -> rc;
	}
	slope_p += slope_shift;
	for (ind = 0; ind < stripes; ind++) {
		spb = & pb -> stripes [ind] -> index_sens . key;
		* (slope_p++) = spb -> bp;
		* (slope_p++) = - spb -> lc;
		* (slope_p++) = spb -> rc;
	}
	slope_p = & slopes [total_stripes9 + oscillator * stereo];
	for (ind = 0; ind < stereo; ind++) {
		spb = & pb -> stereo [ind] -> sens . key;
		* (slope_p++) = spb -> bp;
		* (slope_p++) = - spb -> lc;
		* (slope_p++) = spb -> rc;
	}
}

void oscillator_pool_cpp :: load_freq_shifts (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & freq_shift [oscillator * stripes];
	int * vp = & velocities [oscillator * velocity_size];
	for (int ind = 0; ind < stripes; ind++)
		* (ip++) = pb -> stripes [ind] -> freq_lemat + * (vp++) + 8192;
}

void oscillator_pool_cpp :: load_amp_shifts (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	stripe_parameter_block * spb;
	int * ip = & amp_target [oscillator * stripes];
	int * vp = & velocities [oscillator * velocity_size + stripes];
	for (int ind = 0; ind < stripes; ind++) {
		spb = pb -> stripes [ind];
		* (ip++) = - spb -> amp_lemat - * (vp++)
			+ spb -> amp_sens . key . get_negative ()
			+ ((1 + spb -> lfo_amp_lemat) >> 1);
	}
}

void oscillator_pool_cpp :: load_index_shifts (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
//	int * ip = & index_shift [oscillator * stripes];
	int * ip = & index_target [oscillator * stripes];
	int * vp = & velocities [oscillator * velocity_size + stripes2];
	for (int ind = 0; ind < stripes; ind++)
		* (ip++) = pb -> stripes [ind] -> index_lemat + * (vp++);
}

void oscillator_pool_cpp :: load_pan_shifts (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & pan_target [oscillator * stereo];
	int * vp = & velocities [oscillator * velocity_size + stripes3];
	for (int ind = 0; ind < stereo; ind++)
		* (ip++) = pb -> stereo [ind] -> pan_lemat + * (vp++);
}

void oscillator_pool_cpp :: load_all_shifts (int oscillator, bool unfiltered) {
	int ind;
	int sub;
	parameter_block * pb = osc [oscillator] . pb;
	stripe_parameter_block * spb;
	int * ip = & freq_shift [oscillator * stripes];
	int * at = & amp_target [oscillator * stripes];
	int * vp = & velocities [oscillator * velocity_size];
	//@ index amp
//	bool * bp = & index_amps [oscillator * stripes];
	for (ind = 0; ind < stripes; ind++) {
		* (ip++) = pb -> stripes [ind] -> freq_lemat + * (vp++) + 8192;
	}
	ip += stripe_shift;
	for (ind = 0; ind < stripes; ind++) {
		spb = pb -> stripes [ind];
		sub = - spb -> amp_lemat - * (vp++)
			+ spb -> amp_sens . key . get_negative ()
			+ ((1 + spb -> lfo_amp_lemat) >> 1);
		* (at++) = sub;
		if (unfiltered) * ip = sub;
		ip++;
	}
//	ip += stripe_shift;
	ip = & index_target [oscillator * stripes];
	for (ind = 0; ind < stripes; ind++) {
		* (ip++) = pb -> stripes [ind] -> index_lemat + * (vp++);
		//@ index amp
//		* (bp++) = pb -> stripes [ind] -> index_amp;
	}
	ip = & pan_shift [oscillator * stereo];
	at = & pan_target [oscillator * stereo];
	for (ind = 0; ind < stereo; ind++) {
		sub = pb -> stereo [ind] -> pan_lemat + * (vp++);
		* (at++) = sub;
		if (unfiltered) * ip = sub;
		ip++;
	}
	//@ index amp
//	check_index_amps ();
}

void oscillator_pool_cpp :: load_freq_lfo_source (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & freq_lfo_source [oscillator * stripes];
	for (int ind = 0; ind < stripes; ind++) {
		int source = pb -> stripes [ind] -> freq_lfo_source;
		* (ip++) = source >= 16 ? stripes + source - 16 : source;
	}
}

void oscillator_pool_cpp :: load_amp_lfo_source (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & amp_lfo_source [oscillator * stripes];
	for (int ind = 0; ind < stripes; ind++) {
		int source = pb -> stripes [ind] -> amp_lfo_source;
		* (ip++) = source >= 16 ? stripes + source - 16 : source;
	}
}

void oscillator_pool_cpp :: load_index_lfo_source (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & index_lfo_source [oscillator * stripes];
	for (int ind = 0; ind < stripes; ind++) {
		int source = pb -> stripes [ind] -> index_lfo_source;
		* (ip++) = source >= 16 ? stripes + source - 16 : source;
	}
}

void oscillator_pool_cpp :: load_pan_lfo_source (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & pan_lfo_source [oscillator * stereo];
	for (int ind = 0; ind < stereo; ind++) {
		int source = pb -> stereo [ind] -> pan_lfo_source;
		* (ip++) = source >= 16 ? stripes + source - 16 : source;
	}
}

void oscillator_pool_cpp :: load_all_lfo_source (int oscillator) {
	load_freq_lfo_source (oscillator);
	load_amp_lfo_source (oscillator);
	load_index_lfo_source (oscillator);
	load_pan_lfo_source (oscillator);
}

void oscillator_pool_cpp :: notify_freq_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_freq_shifts (ind);
}

void oscillator_pool_cpp :: notify_amp_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_amp_shifts (ind);
}

void oscillator_pool_cpp :: notify_index_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_index_shifts (ind);
}

//@ index amp
//void oscillator_pool_cpp :: notify_index_amp_change (parameter_block * pb, int ind) {
//	int factory_id = pb -> factory_id;
//	bool value = pb -> stripes [ind] -> index_amp;
//	for (int index = 0; index < polyphony; index++)
//		if (part_connections [index] == factory_id)
//			index_amps [index * stripes + ind] = value;
//	check_index_amps ();
//}


void oscillator_pool_cpp :: notify_pan_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_pan_shifts (ind);
}

void oscillator_pool_cpp :: notify_all_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id) {
			load_all_shifts (ind, false);
//			load_lfo_all_shifts (ind);
			notify_lfo_speeds_update (pb);
		}
}

void oscillator_pool_cpp :: notify_freq_lfo_source_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_freq_lfo_source (ind);
}

void oscillator_pool_cpp :: notify_amp_lfo_source_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_amp_lfo_source (ind);
}

void oscillator_pool_cpp :: notify_index_lfo_source_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_index_lfo_source (ind);
}

void oscillator_pool_cpp :: notify_pan_lfo_source_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_pan_lfo_source (ind);
}

void oscillator_pool_cpp :: notify_all_lfo_source_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_all_lfo_source (ind);
}

void oscillator_pool_cpp :: notify_lfo_freq_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_lfo_freq_shifts (ind);
}

void oscillator_pool_cpp :: notify_lfo_amp_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id) {
			load_lfo_amp_shifts (ind);
			load_amp_shifts (ind);
		}
}

void oscillator_pool_cpp :: notify_lfo_index_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_lfo_index_shifts (ind);
}

void oscillator_pool_cpp :: notify_lfo_pan_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_lfo_pan_shifts (ind);
}

void oscillator_pool_cpp :: notify_lfo_all_shifts_update (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_lfo_all_shifts (ind);
}

void oscillator_pool_cpp :: notify_ratio_change (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	for (int ind = 0; ind < polyphony; ind++)
		if (part_connections [ind] == factory_id)
			load_ratios (ind);
}

void oscillator_pool_cpp :: load_ratios (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * rp = & ratio [oscillator * stripes];
	for (int ind = 0; ind < stripes; ind++)
		* (rp++) = pb -> stripes [ind] -> multiplier;
}

void oscillator_pool_cpp :: load_lfo_freq_shifts (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & lfo_freq_shift [oscillator * stripes];
	for (int ind = 0; ind < stripes; ind++)
		* (ip++) = pb -> stripes [ind] -> lfo_freq_lemat;// + 1;
}

void oscillator_pool_cpp :: load_lfo_amp_shifts (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & lfo_amp_shift [oscillator * stripes];
	for (int ind = 0; ind < stripes; ind++)
		* (ip++) = - pb -> stripes [ind] -> lfo_amp_lemat;// - 1;
}

void oscillator_pool_cpp :: load_lfo_index_shifts (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & lfo_index_shift [oscillator * stripes];
	for (int ind = 0; ind < stripes; ind++)
		* (ip++) = pb -> stripes [ind] -> lfo_index_lemat;// + 1;
}

void oscillator_pool_cpp :: load_lfo_pan_shifts (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & lfo_pan_shift [oscillator * stereo];
	for (int ind = 0; ind < stereo; ind++)
		* (ip++) = pb -> stereo [ind] -> lfo_pan_lemat;// + 1;
}

void oscillator_pool_cpp :: load_lfo_all_shifts (int oscillator) {
	int ind;
	parameter_block * pb = osc [oscillator] . pb;
	int * ip = & lfo_freq_shift [oscillator * stripes];
	for (ind = 0; ind < stripes; ind++) {
		* (ip++) = pb -> stripes [ind] -> lfo_freq_lemat;// + 1;
	}
	ip += stripe_shift;
	for (ind = 0; ind < stripes; ind++) {
		* (ip++) = - pb -> stripes [ind] -> lfo_amp_lemat;// - 1;
	}
	ip += stripe_shift;
	for (ind = 0; ind < stripes; ind++) {
		* (ip++) = pb -> stripes [ind] -> lfo_index_lemat;// + 1;
	}
	ip = & lfo_pan_shift [oscillator * stereo];
	for (ind = 0; ind < stereo; ind++) {
		* (ip++) = pb -> stereo [ind] -> lfo_pan_lemat;// + 1;
	}
}

void oscillator_pool_cpp :: notify_lfo_speeds_update (parameter_block * pb) {
	int index = pb -> factory_id * stripes_stereo;
	int index2 = index << 2;
	adsr_control_block * apb = & lfo_ctrl [index];
	stripe_parameter_block * spb;
	stereo_stripe_parameter_block * stpb;
	int ind;
	int sync_to;
	int speed;
	int phase;
	int pulse;
	int wave;
	int stage;
	int breakpoint;
	int target;
	int retrigger_x, retrigger_y, retrigger_d, retrigger_d1, retrigger_d2, retrigger_index;
	int dx, d0, d1, d2, y1, y2, dy;
	int adx, ad0, ad1, ad2, ay1, ay2, ady;
	int bdx, bd0, bd1, bd2, by1, by2, bdy;
	int cdx, cd0, cd1, cd2, cy1, cy2, cdy;
	double last_x, last_X, new_x, new_X;
	float speeder;
	for (ind = 0; ind < stripes_stereo; ind++) {
		if (ind < stripes) {
			spb = pb -> stripes [ind];
			sync_to = spb -> lfo_sync;
			phase = spb -> phase_shift;
			pulse = spb -> pulse_width;
			wave = spb -> wave;
		} else {
			stpb = pb -> stereo [ind - stripes];
			sync_to = stpb -> lfo_sync;
			phase = stpb -> phase_shift;
			pulse = stpb -> pulse_width;
			wave = stpb -> wave;
		}
		if (sync_to < 16) speed = pb -> stripes [sync_to] -> lfo_speed_lemat;
		else speed = pb -> stereo [sync_to - 16] -> lfo_speed_lemat;
		if (speed < 0) speed = 0;
		if (speed > 16383) speed = 16383;
		speed = time_delays [16383 - speed];
		speed -= lfo_gate_gap2;
		speed >>= 1;

		speeder = (float) speed;
		speeder *= (float) pulse;
		speeder /= 8192.0f;
//		adx = speed + (speed * pulse) / 8192;
		adx = speed + (int) speeder;
		if (adx < 1) adx = 1;
		ady = 16383;
		ay2 = ady / adx;
		ay1 = ay2 + 1;
		ady %= adx;
		ad2 = ady + ady;
		ad0 = ad2 - adx;
		ad1 = ad0 - adx;
		adx -= 2;

//		bdx = speed - (speed * pulse) / 8192;
		bdx = speed - (int) speeder;
		if (bdx < 1) bdx = 1;
		bdy = 16383;
		by2 = 0 - bdy / bdx;
		by1 = by2 - 1;
		bdy %= bdx;
		bd2 = bdy + bdy;
		bd0 = bd2 - bdx;
		bd1 = bd0 - bdx;
		bdx -= 2;

		cdx = lfo_gate_gap;
		cdy = 16383;
		cy2 = cdy / cdx;
		cy1 = cy2 + 1;
		cdy %= cdx;
		cd2 = cdy + cdy;
		cd0 = cd2 - cdx;
		cd1 = cd0 - cdx;
		cdx -= 2;

		stage = apb -> index;
		last_X = apb -> x [stage];
		target = apb -> target;
		if (last_X > lfo_gate_gap2) {
			last_x = lfo_x [index];
			if (stage < 2) new_X = adx;
			else new_X = bdx;
			new_x = last_x * new_X;
			new_x /= last_X;
			new_x += 0.49;
			dx = (int) new_x;
			lfo_x [index] = dx;
			dx += 2;
			switch (stage) {
			case 0:
				if (apb -> wave == 2) dy = target;
				else dy = 8191;
				dy -= lfo_y [index];
//				dy = 8191 - lfo_y [index];
				y2 = dy / dx;
				y1 = y2;
				if (dy > 0) y1++;
				if (dy < 0) {y1--; dy = - dy;}
				dy %= dx;
				d2 = dy + dy;
				d0 = d2 - dx;
				d1 = d0 - dx;
				lfo_d [index++] = d0;
				lfo_delta [index2++] = d1;
				lfo_delta [index2++] = y1;
				lfo_delta [index2++] = d2;
				lfo_delta [index2++] = y2;
				break;
			case 2:
				if (apb -> wave == 2) dy = target;
				else dy = -8192;
				dy -= lfo_y [index];
//				dy = lfo_y [index] + 8192;
				y2 = dy / dx;
				y1 = y2;
				if (dy > 0) y1++;
				if (dy < 0) {y1--; dy = - dy;}
//				y2 = 0 - dy / dx;
//				y1 = y2 - 1;
				dy %= dx;
				d2 = dy + dy;
				d0 = d2 - dx;
				d1 = d0 - dx;
				lfo_d [index++] = d0;
				lfo_delta [index2++] = d1;
				lfo_delta [index2++] = y1;
				lfo_delta [index2++] = d2;
				lfo_delta [index2++] = y2;
				break;
			default:
				index++;
				index2 += 4;
				break;
			}
		} else {
			index++;
			index2 += 4;
		}

		breakpoint = 8192 + pulse;
		if (breakpoint < 1) breakpoint = 1;
		if (phase < breakpoint) {
			retrigger_index = 0;
			new_x = (double) phase;
			new_x *= (double) adx;
			new_x /= (double) breakpoint;
			retrigger_x = adx - (int) new_x;
			retrigger_y = (int) (0.49 + 16383.0 * new_x / (double) adx) - 8192;
			retrigger_d = 8191 - retrigger_y;
			retrigger_d %= 2 + retrigger_x;
			retrigger_d2 = retrigger_d + retrigger_d;
			retrigger_d = retrigger_d2 - retrigger_x - 2;
			retrigger_d1 = retrigger_d - retrigger_x - 2;
		} else {
			retrigger_index = 2;
			phase -= breakpoint;
			breakpoint = 16384 - breakpoint;
			if (breakpoint < 1) breakpoint = 1;
			new_x = (double) phase;
			new_x *= (double) bdx;
			new_x /= (double) breakpoint;
			retrigger_x = bdx - (int) new_x;
			retrigger_y = 8191 - (int) (0.49 + 16383.0 * new_x / (double) bdx);
			retrigger_d = retrigger_y + 8192;
			retrigger_d %= 2 + retrigger_x;
			retrigger_d2 = retrigger_d + retrigger_d;
			retrigger_d = retrigger_d2 - retrigger_x - 2;
			retrigger_d1 = retrigger_d - retrigger_x - 2;
		}

		switch (wave) {
		case 2: case 4:
//			retrigger_index++;
			if (retrigger_index++ < 2) retrigger_y = 8191;
			else retrigger_y = -8192;
			apb -> x [0] = cdx;
			apb -> d0 [0] = cd0;
			apb -> d1 [0] = cd1;
			apb -> d2 [0] = cd2;
//			apb -> y0 [0] = -8192;
			apb -> y1 [0] = cy1;
			apb -> y2 [0] = cy2;
			apb -> x [1] = adx;
			apb -> x [2] = cdx;
			apb -> d0 [2] = cd0;
			apb -> d1 [2] = cd1;
			apb -> d2 [2] = cd2;
//			apb -> y0 [2] = 8191;
			apb -> y1 [2] = - cy1;
			apb -> y2 [2] = - cy2;
			apb -> x [3] = bdx;
			break;
		default:
			apb -> x [0] = adx;
			apb -> d0 [0] = ad0;
			apb -> d1 [0] = ad1;
			apb -> d2 [0] = ad2;
//			apb -> y0 [0] = -8192;
			apb -> y1 [0] = ay1;
			apb -> y2 [0] = ay2;
			apb -> x [1] = cdx;
			apb -> x [2] = bdx;
			apb -> d0 [2] = bd0;
			apb -> d1 [2] = bd1;
			apb -> d2 [2] = bd2;
//			apb -> y0 [2] = 8191;
			apb -> y1 [2] = by1;
			apb -> y2 [2] = by2;
			apb -> x [3] = cdx;
			break;
		}
		apb -> retrigger_index = retrigger_index;
		apb -> retrigger_x = retrigger_x;
		apb -> retrigger_y = retrigger_y;
		apb -> retrigger_d = retrigger_d;
		apb -> retrigger_d1 = retrigger_d1;
		apb -> retrigger_d2 = retrigger_d2;
		switch (wave) {
		case 0: apb -> wave = 1; break;  // sine
		case 3: apb -> wave = 2; break;  // rnd
		case 4: apb -> wave = 3; break;  // s/h
		default: apb -> wave = 0; break; // tri/sqr
		}
		apb++;
	}
}

void oscillator_pool_cpp :: lfo_retrigger (int oscillator) {
	parameter_block * pb = osc [oscillator] . pb;
	int factory_id = pb -> factory_id;
	int index = factory_id * stripes_stereo;
	int index2 = index << 2;
	int retrigger_index;
	int sync_to;
	bool sync;
	bool retrigger_request = retriggers_required [factory_id];
	adsr_control_block * apb = & lfo_ctrl [index];
	int dx, dy, d0, d1, d2, y0, y1, y2;
	y0 = rnd . out;
	rnd . move ();
	int target = rnd . out;
	for (int ind = 0; ind < stripes_stereo; ind++) {
		if (ind < stripes) sync_to = pb -> stripes [ind] -> lfo_sync;
		else sync_to = pb -> stereo [ind - stripes] -> lfo_sync;
		if (sync_to < 16) sync = pb -> stripes [sync_to] -> lfo_retrigger;
		else sync = pb -> stereo [sync_to - 16] -> lfo_retrigger;
		if (sync || retrigger_request) {
			retrigger_index = apb -> retrigger_index;
			switch (apb -> wave) {
			case 2:
				dx = apb -> retrigger_x;
				lfo_x [index] = dx;
				dx += 2;
				apb -> target = target;
				dy = target - y0;
				y2 = dy / dx;
				y1 = y2;
				if (dy > 0) y1++;
				if (dy < 0) {y1--; dy = - dy;}
				dy %= dx;
				d2 = dy + dy;
				d0 = d2 - dx;
				d1 = d0 - dx;
				lfo_d [index] = d0;
				lfo_y [index++] = y0;
				lfo_delta [index2++] = d1;
				lfo_delta [index2++] = y1;
				lfo_delta [index2++] = d2;
				lfo_delta [index2++] = y2;
				break;
			case 3:
				apb -> target = target;
				lfo_x [index] = apb -> retrigger_x;
				lfo_d [index] = 0;
				lfo_y [index++] = target;
				lfo_delta [index2++] = 0;
				lfo_delta [index2++] = 0;
				lfo_delta [index2++] = 0;
				lfo_delta [index2++] = 0;
				break;
			default:
				lfo_x [index] = apb -> retrigger_x;
				lfo_d [index] = apb -> retrigger_d;
				lfo_y [index++] = apb -> retrigger_y;
				lfo_delta [index2++] = apb -> retrigger_d1;
				lfo_delta [index2++] = apb -> y1 [retrigger_index];
				lfo_delta [index2++] = apb -> retrigger_d2;
				lfo_delta [index2++] = apb -> y2 [retrigger_index];
				break;
			}
			apb -> index = retrigger_index;
		} else {
			index++;
			index2 += 4;
		}
		apb++;
	}
	retriggers_required [factory_id] = false;
}

void oscillator_pool_cpp :: notify_lfo_retrigger_required (parameter_block * pb) {
	retriggers_required [pb -> factory_id] = true;
}

void oscillator_pool_cpp :: notify_algo_change (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	algo_cpp_pointer * ap = algo;
	oscillator * oscp = osc;
	algo_cpp_pointer new_algo = (algo_cpp_pointer) pb -> algo_head;
	for (int ind = 0; ind < polyphony; ind++) {
		if (part_connections [ind] == factory_id) {
			(* ap) -> data_destructor (oscp);
			* ap = new_algo;
			new_algo -> data_constructor (oscp);
		}
		ap++;
		oscp++;
	}
	new_algo -> notify_all_algo_parameters_changed (pb);
}

void oscillator_pool_cpp :: notify_dsp_algo_change (parameter_block * pb, int ind) {
	dsp -> notify_algo_change (pb, ind);
}

//@ index amp
//void oscillator_pool_cpp :: check_index_amps (void) {
//	index_amp_required = false;
//	bool * bp = index_amps;
//	for (int ind = 0; ind < total_stripes; ind++)
//		if (* (bp++)) {index_amp_required = true; return;}
//}

void oscillator_pool_cpp :: algo_change (int ind) {
	oscillator * oscp = & osc [ind];
	parameter_block * pb = oscp -> pb;
	algo_cpp_pointer new_algo = (algo_cpp_pointer) pb -> algo_head;
	algo [ind] -> data_destructor (oscp);
	algo [ind] = new_algo;
	new_algo -> data_constructor (oscp);
	new_algo -> notify_all_algo_parameters_changed (pb);
}

void oscillator_pool_cpp :: notify_tune (int tune) {
	if (tune < -128) tune = -128;
	if (tune > 127) tune = 127;
	tune += 128;
	freq_logs = & tune_freq_logs [tune];
}

void oscillator_pool_cpp :: notify_lfo_wave_change (parameter_block * pb) {
	notify_lfo_speeds_update (pb);
	notify_lfo_retrigger_required (pb);
}

void oscillator_pool_cpp :: notify_lfo_pulse_width_change (parameter_block * pb) {
	notify_lfo_speeds_update (pb);
	notify_lfo_retrigger_required (pb);
}
void oscillator_pool_cpp :: notify_algo_parameter_changed (parameter_block * pb, int ind) {
	if (pb -> algo_head == NULL) return;
	algo_cpp_pointer a = (algo_cpp_pointer) pb -> algo_head;
	a -> notify_algo_parameter_changed (pb, ind);
}
void oscillator_pool_cpp :: notify_volume_change (parameter_block * pb) {dsp -> notify_volume_change (pb);}
void oscillator_pool_cpp :: notify_global_volume_change (parameter_block * pb) {dsp -> notify_global_volume_change (pb);}
void oscillator_pool_cpp :: notify_dsp_change (parameter_block * pb, int nrpn_lsb) {dsp -> notify_dsp_change (pb, nrpn_lsb);}
void oscillator_pool_cpp :: notify_dry_change (parameter_block * pb) {dsp -> notify_dry_change (pb);}
void oscillator_pool_cpp :: notify_dry_wet_change_all (parameter_block * pb) {dsp -> notify_dry_wet_change_all (pb);}
void oscillator_pool_cpp :: notify_dry_wet_change (parameter_block * pb, int ind) {dsp -> notify_dry_wet_change (pb, ind);}
void oscillator_pool_cpp :: notify_dsp_ground (parameter_block * pb) {dsp -> notify_dsp_ground (pb -> factory_id);}

void oscillator_pool_cpp :: notify_program_loaded (parameter_block * pb) {
	int factory_id = pb -> factory_id;
	int part_connection = factory_id * stripes_stereo;
	for (int ind = 0; ind < polyphony; ind++) {
		if (part_connections [ind] == factory_id) {
			osc [ind] . pb = pb;
			algo_change (ind);
			lfo_connections [ind] = part_connection;
			load_ratios (ind);
			load_sensitivity (ind);
		}
	}
	if (! pb -> controllers -> poly) lock_retrigger (pb -> locked_oscillator);
	notify_lfo_speeds_update (pb);
	notify_all_lfo_source_update (pb);
	notify_lfo_retrigger_required (pb);
//	notify_dsp_change (pb, 0);
//	notify_volume_change (pb);
	dsp -> notify_program_loaded (pb);
}

void oscillator_pool_cpp :: lock_retrigger (int index) {
	oscillator * op = & osc [index];
	for (int ind = 0; ind < op -> stripes; ind++) op -> time [ind] = 0;
}

void oscillator_pool_cpp :: notify_dsp_program_loaded (parameter_block * pb) {
	dsp -> notify_program_loaded (pb);
}

void oscillator_pool_cpp :: notify_global_dsp_program_loaded (synthesiser * sth) {
	dsp -> notify_global_dsp_program_loaded (sth);
}

void oscillator_pool_cpp :: notify_multi_global_dsp_program_loaded (parameter_block * pb) {
	dsp -> notify_multi_global_dsp_program_loaded (pb);
}
