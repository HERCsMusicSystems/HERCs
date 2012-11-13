///////////////////////////////////////
// Copyright (C) 2004 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "osc_cpp.h"
#include "math.h"

extern double sine_wave [];

class preset_delay_dsp_algo : public default_dsp_algo_cpp {
public:
	virtual char * toString (parameter_block * pb) {return "DELAY";}
	virtual void parameter_change (dsp_line * dsp, int ind) {
		float sub;
		int id;
		switch (ind) {
//		case 0:
//			sub = (float) dsp -> parameters [0];
//			sub /= 16384.0f;
//			dsp -> variables [0] = sub;
//			break;
		case 1:
			sub = (float) dsp -> parameters [1];
			sub *= (float) (dsp -> horizontal << 1);
			sub /= 16384.0f;
			id = (int) sub;
			id >>= 1;
			id <<= 1;
			if (id > dsp -> length) id = dsp -> length;
			dsp -> indices [1] = id;
			break;
//		case 2:
//			sub = get_damp_fraction (dsp -> parameters [2], dsp -> horizontal);
//			dsp -> variables [4] = get_damp_fraction (dsp -> parameters [2], dsp -> horizontal);
//			dsp -> variables [5] = 1.0f - dsp -> variables [4];
//			break;
		default: default_dsp_algo_cpp :: parameter_change (dsp, ind); break;
		}
	}
} the_preset_delay_dsp_algo;

// parameters
// indices [0] => from
// indices [1] => to
// indices [2] => speed
// indices [3] => time
// indices [4] => phase shift
// indices [5] => wave
// indices [6] => negative phase
// indices [7] => negative phase channel
// variables [0] => feedback
// variables [3] => depth
#define CHORUS_LINE_SIZE 8192

class preset_chorus_algo : public default_dsp_algo_cpp {
public:
	virtual char * toString (parameter_block * pb) {return "CHORUS";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "FEEDBACK";
		case 1: return "DELAY";
		case 2: return "SPEED";
		case 3: return "DEPTH";
		case 4: return "CHANNEL PHASE SHIFT";
		case 5: return "WAVE (0=sine else=tri)";
		case 6: return "NEGATE (if ~ 0)";
		case 7: return "CHANNEL NEGATE (if ~ 0)";
		default: return "NOT USED";
		}
	}
	virtual int get_default (int ind) {
		switch (ind) {
		case 2: return 8192;
		default: return 0;
		}
	}
	virtual void parameter_change (dsp_line * dsp, int ind) {
		float sub;
		int id;
		switch (ind) {
		case 0:
			sub = (float) dsp -> parameters [0];
			sub /= 16384.0f;
			dsp -> variables [0] = sub;
			break;
		case 1:
			sub = (float) dsp -> parameters [1];
			sub *= (float) (dsp -> horizontal << 1);
			sub /= 327680.0f;
			id = 2 + (int) sub;
			id >>= 1;
			id <<= 1;
			id = dsp -> indices [1] - id;
			while (id < 0) id += CHORUS_LINE_SIZE;
			if (id > dsp -> length) id = dsp -> length;
			dsp -> indices [0] = id;
			break;
		case 2:
			sub = (float) dsp -> parameters [2];
			sub -= 8192.0f;
			sub /= 768.0f;
			sub = (float) pow (2.0, (double) sub);
			sub *= 4294967296.0f;
			sub /= (float) dsp -> horizontal;
			dsp -> indices [2] = (int) sub;
			break;
		case 3:
			sub = (float) dsp -> parameters [3];
//			sub /= 128.0f;
			sub *= (float) dsp -> horizontal;
			sub /= 4194304.0f;
			dsp -> variables [3] = sub;
			break;
		case 4:
			sub = (float) dsp -> parameters [4];
			sub *= 262144.0f;
			dsp -> indices [4] = (int) sub;
			break;
		case 5: dsp -> indices [5] = dsp -> parameters [5]; break;
		case 6: dsp -> indices [6] = dsp -> parameters [6]; break;
		case 7: dsp -> indices [7] = dsp -> parameters [7]; break;
		default: break;
		}
	}
	virtual void all_parameters_changed (dsp_line * dsp) {
		dsp -> indices [0] = 0;
		dsp -> indices [1] = 2;
		dsp -> indices [3] = 0;
	}
	virtual void code (dsp_line * dsp) {
		float left_shift, right_shift;
		if (dsp -> indices [5] == 0) {
			left_shift = dsp -> variables [3] * (float) (sine_wave [((unsigned int) dsp -> indices [3]) >> 18] + 1.0);
			right_shift = dsp -> variables [3] * (float) (sine_wave [((unsigned int) (dsp -> indices [3] + dsp -> indices [4])) >> 18] + 1.0);
		} else {
			unsigned int a1 = (unsigned int) dsp -> indices [3];
			unsigned int a2 = a1 + (unsigned int) dsp -> indices [4];
			float b1 = (float) a1;
			float b2 = (float) a2;
			b1 /= 1073741824.0f;
			b2 /= 1073741824.0f;
			if (b1 > 2.0f) b1 = 4.0f - b1;
			if (b2 > 2.0f) b2 = 4.0f - b2;
			left_shift = dsp -> variables [3] * b1;
			right_shift = dsp -> variables [3] * b2;
		}
//		left_shift >>= 1; left_shift <<= 1;
//		right_shift >>= 1; right_shift <<=1; right_shift++;
		int left_index = (int) left_shift;
		int right_index = (int) right_shift;
		float left_fraction = left_shift - (float) left_index;
		float right_fraction = right_shift - (float) right_index;
		left_index <<= 1; right_index <<= 1; right_index++;

		int from = dsp -> indices [0];
		int to = dsp -> indices [1];
		if (from >= CHORUS_LINE_SIZE) from = 0;
		if (to >= CHORUS_LINE_SIZE) to = 0;
//		left_shift = from - left_shift;
//		right_shift = from - right_shift;
//		while (left_shift < 0) left_shift += CHORUS_LINE_SIZE;
//		while (right_shift < 0) right_shift += CHORUS_LINE_SIZE;
		left_index = from - left_index;
		right_index = from - right_index;
		if (left_index < 0) left_index += CHORUS_LINE_SIZE;
		if (right_index < 0) right_index += CHORUS_LINE_SIZE;

		float multiplier = dsp -> indices [6] == 0 ? 1.0f : -1.0f;
//		float value = dsp -> line [left_shift];
		float value = dsp -> line [left_index];
		left_index -= 2;
		if (left_index < 0) left_index += CHORUS_LINE_SIZE;
		value += (dsp -> line [left_index] - value) * left_fraction;
//		value = (value * (1.0 - left_fraction) + dsp -> line [left_index] * left_fraction);
		dsp -> outputs [0] = value;
		value *= dsp -> variables [0];
		value += dsp -> inputs [0] * multiplier;
		dsp -> line [to++] = value;

		if (dsp -> indices [7] != 0) multiplier *= -1.0f;
//		value = dsp -> line [right_shift];
		value = dsp -> line [right_index];
		right_index -= 2;
		if (right_index < 0) right_index += CHORUS_LINE_SIZE;
		value += (dsp -> line [right_index] - value) * right_fraction;
//		value = value * (1.0 - right_fraction) + dsp -> line [right_index] * right_fraction;
		dsp -> outputs [1] = value;
		value *= dsp -> variables [0];
		value += dsp -> inputs [1] * multiplier;
		dsp -> line [to++] = value;

		dsp -> indices [0] = from + 2;
		dsp -> indices [1] = to;
		dsp -> indices [3] += dsp -> indices [2];
	}
} the_preset_chorus_dsp_algo;

dsp_algo_pointer get_preset_dsp_algo (int ind) {
	switch (ind) {
	case 1: return & the_preset_delay_dsp_algo;
	case 2: return & the_preset_chorus_dsp_algo;
	default: return NULL;
	}
	return NULL;
}

