///////////////////////////////////////
// Copyright (C) 2007 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc_cpp.h"
#include "algo_building_blocks.h"

class input_bypass_algo : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Input Bypass";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Mono Input";
		case 1: return "Left Input";
		case 2: return "Right Input";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		int * ap = osc -> amp;
//		float left = * osc -> left_in;
//		float right = * osc -> right_in;
		float left = osc -> left_in;
		float right = osc -> right_in;
		float mono = left + right * 0.5f;
		float wave = mono * (float) ap [0];
		wave += left * (float) ap [1];
		wave += right * (float) ap [2];
		wave += 0.5f;
		return (int) wave;
	}
} the_input_bypass_algo;

algo_pointer get_input_bypass_algo (int ind, int program) {
	return & the_input_bypass_algo;
}
