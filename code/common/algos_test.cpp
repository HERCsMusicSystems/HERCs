///////////////////////////////////////
// Copyright (C) 2007 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc_cpp.h"
#include "algo_building_blocks.h"

class filter_tester_algo : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filter Tester";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Noise/Filter";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		float previous_value, fraction;
		int res;
		FM_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SAWTOOTH (0, 2);
		accu = wave;
		COSINE_ANTIALIASED_SQUARE (2, 2);
		wave += accu;
		wave += (float) ((osc -> noise_in * ap [1]) >> 13);
		res = (int) (wave + 0.5f);

		fraction = (float) fp [1];

		float resonance = (float) ip [1];
//		resonance /= 1024.0f;
		resonance /= 4362.0f;
		resonance += 0.11f * resonance * fraction / 32139211.0f;
		wave -= resonance * (float) feedp [15];

		fraction /= 715827882.0f;

		int sentinel = osc -> pb -> controllers -> algo_parameters [0];
		if (sentinel > 8) sentinel = 8;
		for (int sub = 0; sub < sentinel; sub++) {
			previous_value = (float) feedp [16 + sub];
			wave = previous_value + (wave - previous_value) * fraction;
			res = (int) (wave + 0.5f);
			feedp [16 + sub] = res;
		}

		feedp [15] = res;

		return res;
	}
} the_filter_tester_algo;

class fpm_tester_algo : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "FM modulation tester";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "SINE with Feedback";
		case 1: return "extra modulator";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (0);
		osc -> abstract_freq_to_real (0, osc -> abstract_freq [0] + osc -> abstract_freq [1] - 8192);
		return (int) (wave + 0.5);
	}
} the_fpm_tester_algo;

algo_pointer get_tester_algo (int ind, int program) {
	if (ind < 4) return NULL;
	switch (program) {
	case 1: return & the_filter_tester_algo; break;
	case 2: return & the_fpm_tester_algo; break;
	default: break;
	}
	return NULL;
}
