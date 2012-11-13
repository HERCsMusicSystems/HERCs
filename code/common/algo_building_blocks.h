///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _ALGO_BUILDING_BLOCKS
#define _ALGO_BUILDING_BLOCKS
#include "osc.h"
#include <math.h>

HERCs_SDK_EXPORT extern double sine_wave [];
HERCs_SDK_EXPORT extern double fm_one_cycle;
HERCs_SDK_EXPORT extern double fm_three_cycles;
HERCs_SDK_EXPORT extern double chameleon_cycles;
HERCs_SDK_EXPORT extern int amp_logs [16384];
//HERCs_SDK_EXPORT extern int * global_noise;
#define FM_DECLARATIONS \
	unsigned int * tp = osc -> time;\
	unsigned int * fp = osc -> freq;\
	int * ap = osc -> amp;\
	int * ip = osc -> index;\
	int * feedp = osc -> feed;\
	float wave;\
	float accu = 0.0f;
#define FM_FEEDBACK(ind)\
	wave = (float) sine_wave [(tp [ind] + feedp [ind] * ip [ind]) >> 18];\
	feedp [ind] = (int) (0.5f + wave * 131072.0);\
	wave *= (float) ap [ind];
#define FM_SINE(ind)\
	wave = (float) sine_wave [(tp [ind] + (int) (wave * fm_three_cycles)) >> 18] * (float) ap [ind];
#define CH_SINE(ind)\
	wave = (float) sine_wave [(tp [ind] + (int) (wave * chameleon_cycles)) >> 18] * (float) ap [ind];
#define SINE(ind)\
	wave = (float) sine_wave [tp [ind] >> 18] * (float) ap [ind];
#define RETURN_WAVE return (int) (wave + 0.5f);
#define ANTIALIAS_DECLARATIONS \
	int antialias_amplitude;\
	int antialias_feedback;\
	int antialias_freq;
#define COSINE_ANTIALIAS_DECLARATIONS \
	unsigned int antialias_step;
#define ANTIALIASED_SAWTOOTH(ind)\
	antialias_feedback = 5632;\
	antialias_amplitude = 5888;\
	antialias_freq = osc -> freq [ind] >> 16;\
	if (antialias_freq > 0x400) {\
		antialias_freq -= 0x400;\
		antialias_freq >>= 2;\
		antialias_feedback -= antialias_freq;\
		antialias_amplitude += antialias_freq;\
	} else {if (antialias_freq < 0x180) antialias_feedback -= 0x180 + antialias_freq;}\
	antialias_amplitude = amp_logs [antialias_amplitude];\
	wave = (float) sine_wave [(tp [ind] + feedp [ind] * antialias_feedback) >> 18];\
	feedp [ind] = (int) (0.5f + wave * 131072.0);\
	wave = (float) sine_wave [(tp [ind] + (int) (wave * fm_three_cycles * (float) antialias_amplitude)) >> 18] * (float) ap [ind];
#define ANTIALIASED_SQUARE(ind)\
	antialias_feedback = 5376;\
	antialias_amplitude = 5888;\
	antialias_freq = osc -> freq [ind] >> 16;\
	if (antialias_freq > 0x400) {\
		antialias_freq -= 0x400;\
		antialias_freq >>= 1;\
		antialias_feedback -= antialias_freq;\
		antialias_amplitude += antialias_freq;\
	} else {if (antialias_freq < 0x180) antialias_feedback -= 0x180 + antialias_freq;}\
	antialias_amplitude = amp_logs [antialias_amplitude];\
	wave = (float) sine_wave [((tp [ind] << 1) + feedp [ind] * antialias_feedback) >> 18];\
	feedp [ind] = (int) (0.5f + wave * 131072.0);\
	wave = (float) sine_wave [(tp [ind] + (int) (wave * fm_three_cycles * (float) antialias_amplitude)) >> 18] * (float) ap [ind];
#define COSINE_ANTIALIASED_SQUARE(ind, parameter)\
	antialias_step = fp [ind] * parameter;\
	if (tp [ind] < antialias_step) wave = (float) ap [ind] * (float) sine_wave [12288 - (int) (8192.0f * (float) tp [ind] / (float) antialias_step)];\
	else {\
		if (tp [ind] < 0x80000000) wave = (float) ap [ind];\
		else {\
			if (tp [ind] < 0x80000000 + antialias_step) wave = (float) ap [ind] * (float) sine_wave [4096 + (int) (8192.0f * (float) (tp [ind] - 0x80000000) / (float) antialias_step)];\
			else wave = (float) - ap [ind];\
		}\
	}
#define COSINE_ANTIALIASED_SAWTOOTH(ind, parameter)\
	antialias_step = fp [ind] * parameter;\
	if (tp [ind] < antialias_step) wave = (float) ap [ind] * (float) sine_wave [12288 - (int) (8192.0f * (float) tp [ind] / (float) antialias_step)];\
	else {\
		wave = (float) ap [ind];\
		wave = -2.0f * wave * (float) (tp [ind] - antialias_step) / (4294967296.0f - (float) antialias_step) + wave;\
	}

#define ANTIALIAS_TRIANGLE_DECLARATIONS\
	unsigned int triangle_pointer;\
	float triangle_sine_antialias_factor;\
	float triangle_antialias_factor;
#define ANTIALIASED_TRIANGLE(INDEX)\
		triangle_pointer = tp [INDEX];\
		if (triangle_pointer >= 3221225472u) {\
			wave = (float) triangle_pointer;\
			wave -= 4294967296.0f;\
			wave *= 0.000000000931322574615478515625f;\
		} else {\
			if (triangle_pointer >= 1073741824u) {\
				wave = (float) triangle_pointer;\
				wave -= 2147483648.0f;\
				wave *= -0.000000000931322574615478515625f;\
			} else {\
				wave = (float) triangle_pointer;\
				wave *= 0.000000000931322574615478515625f;\
			}\
		}\
		triangle_sine_antialias_factor = (float) osc -> freq [INDEX];\
		triangle_sine_antialias_factor -= 134217728;\
		if (triangle_sine_antialias_factor < 0.0f) triangle_sine_antialias_factor = 0.0f;\
		if (triangle_sine_antialias_factor > 402653184.0f) triangle_sine_antialias_factor = 402653184.0f;\
		triangle_sine_antialias_factor /= 402653184.0f;\
		triangle_antialias_factor = 1.0f - triangle_sine_antialias_factor;\
		wave *= triangle_antialias_factor;\
		wave += triangle_sine_antialias_factor * (float) sine_wave [tp [INDEX] >> 18];\
		wave *= (float) ap [INDEX];

// 715827880 178956970 536870910
// 536870912.0f 134217728.0f 402653184.0f

// division by 8192 * 8192 * 2 ^ 1/3 = 106528681.30999083087598364751396
#define filter_freq_adjuster 9.3871433280026403424681023305523e-9f;
#define FILTER_DECLARATIONS \
	bool filter_switch;\
	float filter_in;\
	float freq_square;\
	float filter_output;\
	float filter_diff;\
	float filter_delta;
#define STATE_FILTER_DECLARATIONS \
	bool filter_switch;\
	float filter_in;\
	float freq;\
	float reso;\
	float low;\
	float high;\
	float band;\
	float reject;
#define MOOG_LADDER_DECLARATIONS \
	float ladder_in, ladder_out, ladder_freq;
#define MULTIFILTER_DECLARATIONS \
	float freq_square;\
	float filter_output;\
	float filter_diff;\
	float filter_delta;
#define SMOOTH_FILTER_EXTRA_DECLARATIONS unsigned int as, at, sub; unsigned int * ufeedp = (unsigned int *) osc -> feed;
#define NOISE_FILTER(i1, i2)\
	filter_switch = osc -> pb -> stripes [i1] -> retrigger;\
	filter_in = (float) ((osc -> noise_in * ap [i1]) >> 13);\
	if (filter_switch) filter_in += wave;\
	freq_square = (float) (osc -> freq [i1] >> 16);\
	if (freq_square > 20000) freq_square = 20000;\
	freq_square *= freq_square;\
	freq_square *= filter_freq_adjuster;\
	filter_output = (float) feedp [i1];\
	filter_diff = filter_in - filter_output;\
	filter_diff *= freq_square;\
	filter_delta = feedp [i2] + filter_diff;\
	filter_delta *= 0.5f + ((float) ip [i1]) / 32766.0f;\
	filter_output += filter_delta;\
	feedp [i1] = (int) (filter_output + 0.5f);\
	feedp [i2] = (int) (filter_delta + 0.5f);\
	if (! filter_switch) wave += filter_output;\
	else wave = filter_output;
#define STATE_NOISE_FILTER(i1, i2)\
	filter_switch = osc -> pb -> stripes [i1] -> retrigger;\
	filter_in = (float) ((osc -> noise_in * ap [i1]) >> 13);\
	if (filter_switch) filter_in += wave;\
	freq = (float) osc -> freq [i1];\
	freq /= 2734261102.30572636f;\
	freq = 2.0f * sin (freq);\
	reso = 2.0f - (float) ip [i1] / 8192.0f;\
	low = (float) feedp [i1];\
	band = (float) feedp [i2];\
	low = low + freq * band;\
	high = filter_in - low - reso * band;\
	band = freq * high + band;\
	low = low + freq * band;\
	high = filter_in - low - reso * band;\
	band = freq * high + band;\
	reject = high + low;\
	feedp [i1] = (int) (low + 0.5f);\
	feedp [i2] = (int) (band + 0.5f);
#define STATE_NOISE_FILTER_WAVE(i1)\
	switch (osc -> pb -> controllers -> algo_parameters [i1]) {\
	case 0: {if (filter_switch) wave = low; else wave += low;} break;\
	case 1: {if (filter_switch) wave = high; else wave += high;} break;\
	case 2: {if (filter_switch) wave = band; else wave += band;} break;\
	case 3: {if (filter_switch) wave = reject; else wave += reject;} break;\
	default: if (filter_switch) wave = low; else wave += low; break;\
	}
#define STATE_NOISE_FILTER_GET_NAME(i1)\
	virtual char * get_name (int ind) {\
		if (ind == i1) return "FILTER TYPE";\
		return algo :: get_name (ind);\
	}

#define NOISE_AMP(ind)\
	wave = (float) ((osc -> noise_in * ap [ind]) >> 13);
#define NOISE wave = (float) ((osc -> noise_in * amp_logs [0]) >> 13);
#define MOOG_LADDER_NOISE_FILTER(index, ix)\
	filter_switch = osc -> pb -> stripes [index] -> retrigger;\
	filter_in = (float) ((osc -> noise_in * ap [index]) >> 13);\
	if (filter_switch) filter_in += wave;\
	filter_delta = (float) (fp [index]) / 2147483648.0f;\
	ladder_freq = 1.0f - filter_delta;\
	freq_square = filter_delta * filter_delta;\
	filter_diff = (float) ip [index] / 4096.0f;\
	filter_diff *= 1.0f - 0.15f * freq_square;\
	ladder_in = filter_in - (float) feedp [ix + 7] * filter_diff;\
	ladder_in *= 0.35013f * freq_square * freq_square;\
	ladder_out = ladder_in + 0.3f * (float) feedp [ix + 0] + ladder_freq * (float) feedp [ix + 1];\
	feedp [ix + 0] = (int) (ladder_in + 0.5f);\
	feedp [ix + 1] = (int) (ladder_out + 0.5f);\
	ladder_in = ladder_out;\
	ladder_out = ladder_in + 0.3f * (float) feedp [ix + 2] + ladder_freq * (float) feedp [ix + 3];\
	feedp [ix + 2] = (int) (ladder_in + 0.5f);\
	feedp [ix + 3] = (int) (ladder_out + 0.5f);\
	ladder_in = ladder_out;\
	ladder_out = ladder_in + 0.3f * (float) feedp [ix + 4] + ladder_freq * (float) feedp [ix + 5];\
	feedp [ix + 4] = (int) (ladder_in + 0.5f);\
	feedp [ix + 5] = (int) (ladder_out + 0.5f);\
	ladder_in = ladder_out;\
	ladder_out = ladder_in + 0.3f * (float) feedp [ix + 6] + ladder_freq * (float) feedp [ix + 7];\
	feedp [ix + 6] = (int) (ladder_in + 0.5f);\
	feedp [ix + 7] = (int) (ladder_out + 0.5f);\
	filter_output = ladder_out;\
	if (! filter_switch) wave += filter_output;\
	else wave = filter_output;
#define FILTER(i1, i2)\
	freq_square = (float) (osc -> freq [i1] >> 16);\
	if (freq_square > 20000) freq_square = 20000;\
	freq_square *= freq_square;\
	freq_square *= filter_freq_adjuster;\
	filter_output = (float) feedp [i1];\
	filter_diff = wave * 0.25f - filter_output;\
	filter_diff *= freq_square;\
	filter_delta = feedp [i2] + filter_diff;\
	filter_delta *= 0.5f + ((float) ip [i1]) / 32766.0f;\
	filter_output += filter_delta;\
	feedp [i1] = (int) (filter_output + 0.5f);\
	feedp [i2] = (int) (filter_delta + 0.5f);\
	filter_output = (float) ((ap [i1] * (int) filter_output) >> 13);
#define ROM_WAVE_FILTER(i1, i2, i3)\
	freq_square = (float) (osc -> freq [i1] >> 16);\
	if (freq_square > 20000) freq_square = 20000;\
	freq_square *= freq_square;\
	freq_square *= filter_freq_adjuster;\
	filter_output = (float) feedp [i3];\
	filter_diff = wave * 0.25f - filter_output;\
	filter_diff *= freq_square;\
	filter_delta = feedp [i2] + filter_diff;\
	filter_delta *= 0.5f + ((float) ip [i1]) / 32766.0f;\
	filter_output += filter_delta;\
	feedp [i3] = (int) (filter_output + 0.5f);\
	feedp [i2] = (int) (filter_delta + 0.5f);\
	filter_output = (float) ((ap [i1] * (int) filter_output) >> 13);
#define ROM_WAVE_SMOOTH_FILTER(i1, i2, i3, i4)\
	at = osc -> freq [i1] >> 14;\
	as = ufeedp [i4];\
	if (as != at) {\
		if (as < at) {\
			sub = at - as;\
			if (sub <= 1024) as++;\
			else if (sub <= 2048) as += 2;\
			else as += 4;\
		} else {\
			sub = as - at;\
			if (sub <= 1024) as--;\
			else if (sub <= 2048) as -= 2;\
			else as -= 4;\
		}\
	}\
	ufeedp [i4] = as;\
	freq_square = (float) (as >> 2);\
	if (freq_square > 20000) freq_square = 20000;\
	freq_square *= freq_square;\
	freq_square *= filter_freq_adjuster;\
	filter_output = (float) feedp [i3];\
	filter_diff = wave / 4.0f - filter_output;\
	filter_diff *= freq_square;\
	filter_delta = feedp [i2] + filter_diff;\
	filter_delta *= 0.5f + ((float) ip [i1]) / 32766.0f;\
	filter_output += filter_delta;\
	feedp [i3] = (int) (filter_output + 0.5f);\
	feedp [i2] = (int) (filter_delta + 0.5f);\
	filter_output = (float) ((ap [i1] * (int) filter_output) >> 13);
#define SMOOTH_STRIPE_INIT(i1, i3) osc -> feed [i3] = osc -> freq [i1] >> 14
#define SMOOTH_NOISE_FILTER(i1, i2, i3)\
	at = osc -> freq [i1] >> 14;\
	as = ufeedp [i3];\
	if (as != at) {\
		if (as < at) {\
			sub = at - as;\
			if (sub <= 1024) as++;\
			else if (sub <= 2048) as += 2;\
			else as += 4;\
		} else {\
			sub = as - at;\
			if (sub <= 1024) as--;\
			else if (sub <= 2048) as -= 2;\
			else as -= 4;\
		}\
	}\
	ufeedp [i3] = as;\
	filter_switch = osc -> pb -> stripes [i1] -> retrigger;\
	filter_in = (float) ((osc -> noise_in * ap [i1]) >> 13);\
	if (filter_switch) filter_in += wave;\
	freq_square = (float) (as >> 2);\
	if (freq_square > 20000) freq_square = 20000;\
	freq_square *= freq_square;\
	freq_square *= filter_freq_adjuster;\
	filter_output = (float) feedp [i1];\
	filter_diff = filter_in - filter_output;\
	filter_diff *= freq_square;\
	filter_delta = feedp [i2] + filter_diff;\
	filter_delta *= 0.5f + ((float) ip [i1]) / 32766.0f;\
	filter_output += filter_delta;\
	feedp [i1] = (int) (filter_output + 0.5f);\
	feedp [i2] = (int) (filter_delta + 0.5f);\
	if (! filter_switch) wave += filter_output;\
	else wave = filter_output;

#define FILTER_INIT(filter_output_index, filter_speed_index)\
	osc -> feed [filter_output_index] = 0;\
	osc -> feed [filter_speed_index] = 0;
#define SECOND_STAGE_FILTER_DECLARATIONS\
	float feedback_factor;
#define SECOND_STAGE_FILTER(stripe_index, filter_output_index, filter_speed_index)\
	feedback_factor = freq_square / 16.0f;\
	if (feedback_factor > 0.5f) feedback_factor = 0.5f;\
	filter_output = (float) feedp [filter_output_index];\
	filter_diff = wave - filter_output;\
	filter_diff *= freq_square;\
	filter_delta = feedp [filter_speed_index] + filter_diff;\
	filter_delta *= 0.5f - feedback_factor;\
	filter_output += filter_delta;\
	feedp [filter_output_index] = (int) (filter_output + 0.5f);\
	feedp [filter_speed_index] = (int) (filter_diff + 0.5f);\
	wave = filter_output;

#define THIRD_STAGE_FILTER(stripe_index, filter_output_index, filter_speed_index)\
	feedback_factor = freq_square / 12.0f;\
	if (feedback_factor > 0.5f) feedback_factor = 0.5f;\
	filter_output = (float) feedp [filter_output_index];\
	filter_diff = wave - filter_output;\
	filter_diff *= freq_square;\
	filter_delta = feedp [filter_speed_index] + filter_diff;\
	filter_delta *= 0.5f - feedback_factor;\
	filter_output += filter_delta;\
	feedp [filter_output_index] = (int) (filter_output + 0.5f);\
	feedp [filter_speed_index] = (int) (filter_diff + 0.5f);\
	wave = filter_output;


#endif

