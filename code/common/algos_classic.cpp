///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc_cpp.h"
#include "algo_building_blocks.h"

class sawtooth_alarm : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "SAWTOOTH ALARM";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIASED_SAWTOOTH (0);
		RETURN_WAVE;
	}
} the_sawtooth_alarm;

class square_alarm : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "SQUARE ALARM";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIASED_SQUARE (0);
		RETURN_WAVE;
	}
} the_square_alarm;

class classic_pro_square : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SQUARE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIASED_SQUARE (0);
		NOISE_FILTER (1, 2);
		RETURN_WAVE;
	}
} the_classic_pro_square;

class classic_pro_sawtooth : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SAWTOOTH";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIASED_SAWTOOTH (0);
		NOISE_FILTER (1, 2);
		RETURN_WAVE;
	}
} the_classic_pro_sawtooth;

class classic_pro_sine : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SINE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SINE (0);
		NOISE_FILTER (1, 2);
		RETURN_WAVE;
	}
} the_classic_pro_sine;

/////////////////
// CLASSIC PRO //
/////////////////

#define LADDER_FILTER_SECTION\
		NOISE_FILTER (1, 3);\
		float pre_filter_signal = filter_in;\
		int filter_type = osc -> pb -> controllers -> algo_parameters [124];\
		if (filter_type > 0) {\
			SECOND_STAGE_FILTER (1, 5, 6);\
			if (filter_type > 1) {\
				THIRD_STAGE_FILTER (1, 7, 8);\
			}\
		}\
		if (osc -> pb -> controllers -> algo_parameters [123] != 0) return (int) (pre_filter_signal - wave + 0.5f);\
		if (feedp [10] == 0) {\
			feedp [10] = osc -> pb -> controllers -> algo_parameters [121];\
			feedp [9] = (int) (wave + 0.5f);\
			RETURN_WAVE;\
		}\
		feedp [10]--;\
		return feedp [9];

#define FILTER_SECTION\
		filter_delta = 0.0f;\
		feedback_factor = 0.0f;\
		freq_square = 0.0f;\
		filter_diff = 0.0f;\
		float previous_value;\
		filter_switch = osc -> pb -> stripes [1] -> retrigger;\
		filter_in = (float) ((osc -> noise_in * ap [1]) >> 13);\
		if (filter_switch) filter_in += wave;\
		filter_output = filter_in;\
		int res = (int) (filter_output + 0.5f);\
		float fraction = (float) fp [1];\
		if (fraction > 500000000.0f) fraction = 500000000.0f;\
		float resonance = (float) ip [1];\
		resonance /= 4600.0f;\
		resonance += 0.09f * resonance * fraction / 32139211.0f;\
		filter_output -= resonance * (float) feedp [15];\
		fraction /= 715827882.0f;\
		int filter_type = osc -> pb -> controllers -> algo_parameters [124];\
		int sentinel = 4;\
		for (int sub = 0; sub < sentinel; sub++) {\
			previous_value = (float) feedp [16 + sub];\
			filter_output = previous_value + (filter_output - previous_value) * fraction;\
			res = (int) (filter_output + 0.5f);\
			feedp [16 + sub] = res;\
		}\
		feedp [15] = res;\
		filter_output *= 2.0f;\
		if (filter_switch) wave = filter_output;\
		else wave += filter_output;\
		RETURN_WAVE;

#define SMOOTH_LADDER_FILTER_SECTION\
		at = osc -> freq [1];\
		as = ufeedp [4];\
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
		ufeedp [4] = as;\
		filter_delta = 0.0f;\
		feedback_factor = 0.0f;\
		freq_square = 0.0f;\
		filter_diff = 0.0f;\
		float previous_value;\
		filter_switch = osc -> pb -> stripes [1] -> retrigger;\
		filter_in = (float) ((* global_noise * ap [1]) >> 13);\
		if (filter_switch) filter_in += wave;\
		filter_output = filter_in;\
		int res = (int) (filter_output + 0.5f);\
		float fraction = (float) fp [1];\
		float resonance = (float) ip [1];\
		resonance /= 4362.0f;\
		resonance += 0.11f * resonance * fraction / 32139211.0f;\
		filter_output -= resonance * (float) feedp [15];\
		fraction /= 715827882.0f;\
		int filter_type = osc -> pb -> controllers -> algo_parameters [124];\
		int sentinel = 4;\
		for (int sub = 0; sub < sentinel; sub++) {\
			previous_value = (float) feedp [16 + sub];\
			filter_output = previous_value + (filter_output - previous_value) * fraction;\
			res = (int) (filter_output + 0.5f);\
			feedp [16 + sub] = res;\
		}\
		feedp [15] = res;\
		if (filter_switch) wave = filter_output;\
		else wave += filter_output;\
		RETURN_WAVE;

#define SMOOTH_FILTER_SECTION\
		SMOOTH_NOISE_FILTER (1, 3, 4);\
		float pre_filter_signal = filter_in;\
		int filter_type = osc -> pb -> controllers -> algo_parameters [124];\
		if (filter_type > 0) {\
			SECOND_STAGE_FILTER (1, 5, 6);\
			if (filter_type > 1) {\
				THIRD_STAGE_FILTER (1, 7, 8);\
			}\
		}\
		if (osc -> pb -> controllers -> algo_parameters [123] != 0) return (int) (pre_filter_signal - wave + 0.5f);\
		if (feedp [10] == 0) {\
			feedp [10] = osc -> pb -> controllers -> algo_parameters [121];\
			feedp [9] = (int) (wave + 0.5f);\
			RETURN_WAVE;\
		}\
		feedp [10]--;\
		return feedp [9];

#define CONDITIONAL_SAWTOOTH(ind)\
	if (step == 16383) {ANTIALIASED_SAWTOOTH (ind);}\
	else {COSINE_ANTIALIASED_SAWTOOTH (ind, step);}

#define CONDITIONAL_SQUARE(ind)\
	if (step == 16383) {ANTIALIASED_SQUARE (ind);}\
	else {COSINE_ANTIALIASED_SQUARE (ind, step);}

class classic_algo_cpp : public algo_cpp {
public:
	virtual char * get_name (int ind) {
		switch (ind) {
		case 118: return "ANTI-ALIAS";
		case 121: return "DOWNSAMPLING";
		case 123: return "FILTER TYPE";
		case 124: return "SLOPE";
		default: return algo_cpp :: get_name (ind);
		}
	}
	virtual int get_default (int ind) {
		switch (ind) {
		case 118: return 2;
		default: return algo_cpp :: get_default (ind);
		}
	}
};

class classic_pro_square_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SQUARE & SQUARE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave += accu;
		FILTER_SECTION
	}
	/*virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
//		ANTIALIASED_SQUARE (0);
		COSINE_ANTIALIASED_SQUARE (0, step);
		accu = wave;
		ANTIALIASED_SQUARE (2);
		wave += accu;
		FILTER_SECTION
	}*/
} the_classic_pro_square_square;

class classic_pro_sawtooth_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SAWTOOTH & SQUARE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_sawtooth_square;

class classic_pro_sine_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SINE & SQUARE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		SINE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_sine_square;

class classic_pro_triangle_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO TRIANGLE & SQUARE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 3: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_triangle_square;

class classic_pro_square_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SQUARE & SAWTOOTH";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_square_sawtooth;

class classic_pro_sawtooth_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SAWTOOTH & SAWTOOTH";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Sm Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave += accu;
		FILTER_SECTION
	}
	/*virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		COSINE_ANTIALIASED_SAWTOOTH (0, step);
		accu = wave;
		ANTIALIASED_SAWTOOTH (2);
		wave += accu;
		FILTER_SECTION
	}*/
} the_classic_pro_sawtooth_sawtooth;

class classic_pro_sine_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SINE & SAWTOOTH";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		SINE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_sine_sawtooth;

class classic_pro_triangle_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO TRIANGLE & SAWTOOTH";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_triangle_sawtooth;

class classic_pro_square_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SQUARE & SINE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		SINE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_square_sine;

class classic_pro_sawtooth_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SAWTOOTH & SINE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		SINE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_sawtooth_sine;

class classic_pro_sine_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SINE & SINE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		SINE (0);
		accu = wave;
		SINE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_sine_sine;

class classic_pro_triangle_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO TRIANGLE & SINE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		SINE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_triangle_sine;

class classic_pro_square_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SQUARE & TRIANGLE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_square_triangle;

class classic_pro_sawtooth_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SAWTOOTH & TRIANGLE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_sawtooth_triangle;

class classic_pro_sine_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SINE & TRIANGLE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		SINE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_sine_triangle;

class classic_pro_triangle_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO TRIANGLE & TRIANGLE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave += accu;
		FILTER_SECTION
	}
} the_classic_pro_triangle_triangle;

////////////////////////
// SMOOTH CLASSIC PRO //
////////////////////////

class smooth_classic_pro_square_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH SQUARE & SQUARE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_square_square;

class smooth_classic_pro_sawtooth_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH SAWTOOTH & SQUARE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_sawtooth_square;

class smooth_classic_pro_sine_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SINE & SQUARE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		SINE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_sine_square;

class smooth_classic_pro_triangle_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO TRIANGLE & SQUARE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_triangle_square;

class smooth_classic_pro_square_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SQUARE & SAWTOOTH";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_square_sawtooth;

class smooth_classic_pro_sawtooth_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SAWTOOTH & SAWTOOTH";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Sm Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_sawtooth_sawtooth;

class smooth_classic_pro_sine_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SINE & SAWTOOTH";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		SINE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_sine_sawtooth;

class smooth_classic_pro_triangle_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO TRIANGLE & SAWTOOTH";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_triangle_sawtooth;

class smooth_classic_pro_square_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SQUARE & SINE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		SINE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_square_sine;

class smooth_classic_pro_sawtooth_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SAWTOOTH & SINE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		SINE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_sawtooth_sine;

class smooth_classic_pro_sine_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SINE & SINE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		SINE (0);
		accu = wave;
		SINE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_sine_sine;

class smooth_classic_pro_triangle_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO TRIANGLE & SINE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		SINE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_triangle_sine;

class smooth_classic_pro_square_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SQUARE & TRIANGLE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_square_triangle;

class smooth_classic_pro_sawtooth_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SAWTOOTH & TRIANGLE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_sawtooth_triangle;

class smooth_classic_pro_sine_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO SINE & TRIANGLE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		SINE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_sine_triangle;

class smooth_classic_pro_triangle_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO TRIANGLE & TRIANGLE";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave += accu;
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_triangle_triangle;

//////////////////////////////////
//                              //
//     WITH RING MODULATOR      //
//                              //
//////////////////////////////////

class classic_pro_ring_square_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_square_square;

class classic_pro_ring_sawtooth_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_sawtooth_square;

class classic_pro_ring_sine_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		SINE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_sine_square;

class classic_pro_ring_triangle_square : public classic_algo_cpp {

	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_triangle_square;

class classic_pro_ring_square_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_square_sawtooth;

class classic_pro_ring_sawtooth_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_sawtooth_sawtooth;

class classic_pro_ring_sine_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		SINE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		FILTER_SECTION
	}
} the_classic_pro_ring_sine_sawtooth;

class classic_pro_ring_triangle_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_triangle_sawtooth;

class classic_pro_ring_square_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		SINE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_square_sine;

class classic_pro_ring_sawtooth_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		SINE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_sawtooth_sine;

class classic_pro_ring_sine_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		SINE (0);
		accu = wave;
		SINE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_sine_sine;

class classic_pro_ring_triangle_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		SINE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_triangle_sine;

class classic_pro_ring_square_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_square_triangle;

class classic_pro_ring_sawtooth_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_sawtooth_triangle;

class classic_pro_ring_sine_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		SINE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_sine_triangle;

class classic_pro_ring_triangle_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC PRO RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		//MOOG_LADDER_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave *= accu;
		wave /= amp_logs [0];
		FILTER_SECTION
	}
} the_classic_pro_ring_triangle_triangle;

//////////////////
// SMOOTH RINGS //
//////////////////

class smooth_classic_pro_ring_square_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_square_square;

class smooth_classic_pro_ring_sawtooth_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_sawtooth_square;

class smooth_classic_pro_ring_sine_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		SINE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_sine_square;

class smooth_classic_pro_ring_triangle_square : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		CONDITIONAL_SQUARE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_triangle_square;

class smooth_classic_pro_ring_square_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_square_sawtooth;

class smooth_classic_pro_ring_sawtooth_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_sawtooth_sawtooth;

class smooth_classic_pro_ring_sine_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		SINE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_sine_sawtooth;

class smooth_classic_pro_ring_triangle_sawtooth : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		CONDITIONAL_SAWTOOTH (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_triangle_sawtooth;

class smooth_classic_pro_ring_square_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		SINE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_square_sine;

class smooth_classic_pro_ring_sawtooth_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		SINE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_sawtooth_sine;

class smooth_classic_pro_ring_sine_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		SINE (0);
		accu = wave;
		SINE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_sine_sine;

class smooth_classic_pro_ring_triangle_sine : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Sine";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		SINE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_triangle_sine;

class smooth_classic_pro_ring_square_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SQUARE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_square_triangle;

class smooth_classic_pro_ring_sawtooth_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		int step = osc -> pb -> controllers -> algo_parameters [118];
		CONDITIONAL_SAWTOOTH (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_sawtooth_triangle;

class smooth_classic_pro_ring_sine_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sine";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		SINE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_sine_triangle;

class smooth_classic_pro_ring_triangle_triangle : public classic_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "CLASSIC SMOOTH RING";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Triangle";
		case 1: return "Resonant Filter & Noise";
		case 2: return "Triangle";
		default: return "inactive";
		}
	}
	virtual void data_constructor (oscillator * osc) {
		SMOOTH_STRIPE_INIT (1, 4);
		FILTER_INIT (1, 3);
		FILTER_INIT (5, 6);
		FILTER_INIT (7, 8); osc -> feed [9] = 0; osc -> feed [10] = 0;
		osc -> feed [15] = 0; osc -> feed [16] = 0; osc -> feed [17] = 0; osc -> feed [18] = 0; osc -> feed [19] = 0;
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FILTER_DECLARATIONS;
		SMOOTH_FILTER_EXTRA_DECLARATIONS;
		SECOND_STAGE_FILTER_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		ANTIALIASED_TRIANGLE (0);
		accu = wave;
		ANTIALIASED_TRIANGLE (2);
		wave *= accu;
		wave /= amp_logs [0];
		SMOOTH_FILTER_SECTION
	}
} the_smooth_classic_pro_ring_triangle_triangle;



algo_pointer get_alarm_algo (int ind, int program) {
	if (ind < 1) return NULL;
	switch (program) {
	case 0: return & the_square_alarm;
	case 1: return & the_sawtooth_alarm;
	default: return NULL;
	}
}

algo_pointer get_classic_pro_algo (int ind, int program) {
	if (ind > 3) ind = 3;
	switch (ind) {
	case 1: return get_alarm_algo (1, program);
	case 2:
		switch (program) {
		case 0: return & the_classic_pro_square;
		case 1: return & the_classic_pro_sawtooth;
		case 2: return & the_classic_pro_sine;
		default: break;
		}
		break;
	case 3:
		switch (program) {
		case 0: return & the_classic_pro_square_square;
		case 1: return & the_classic_pro_sawtooth_square;
		case 2: return & the_classic_pro_sine_square;
		case 3: return & the_classic_pro_triangle_square;
		case 4: return & the_classic_pro_square_sawtooth;
		case 5: return & the_classic_pro_sawtooth_sawtooth;
		case 6: return & the_classic_pro_sine_sawtooth;
		case 7: return & the_classic_pro_triangle_sawtooth;
		case 8: return & the_classic_pro_square_sine;
		case 9: return & the_classic_pro_sawtooth_sine;
		case 10: return & the_classic_pro_sine_sine;
		case 11: return & the_classic_pro_triangle_sine;
		case 12: return & the_classic_pro_square_triangle;
		case 13: return & the_classic_pro_sawtooth_triangle;
		case 14: return & the_classic_pro_sine_triangle;
		case 15: return & the_classic_pro_triangle_triangle;
		case 32: return & the_smooth_classic_pro_square_square;
		case 33: return & the_smooth_classic_pro_sawtooth_square;
		case 34: return & the_smooth_classic_pro_sine_square;
		case 35: return & the_smooth_classic_pro_triangle_square;
		case 36: return & the_smooth_classic_pro_square_sawtooth;
		case 37: return & the_smooth_classic_pro_sawtooth_sawtooth;
		case 38: return & the_smooth_classic_pro_sine_sawtooth;
		case 39: return & the_smooth_classic_pro_triangle_sawtooth;
		case 40: return & the_smooth_classic_pro_square_sine;
		case 41: return & the_smooth_classic_pro_sawtooth_sine;
		case 42: return & the_smooth_classic_pro_sine_sine;
		case 43: return & the_smooth_classic_pro_triangle_sine;
		case 44: return & the_smooth_classic_pro_square_triangle;
		case 45: return & the_smooth_classic_pro_sawtooth_triangle;
		case 46: return & the_smooth_classic_pro_sine_triangle;
		case 47: return & the_smooth_classic_pro_triangle_triangle;
		case 64: return & the_classic_pro_ring_square_square;
		case 65: return & the_classic_pro_ring_sawtooth_square;
		case 66: return & the_classic_pro_ring_sine_square;
		case 67: return & the_classic_pro_ring_triangle_square;
		case 68: return & the_classic_pro_ring_square_sawtooth;
		case 69: return & the_classic_pro_ring_sawtooth_sawtooth;
		case 70: return & the_classic_pro_ring_sine_sawtooth;
		case 71: return & the_classic_pro_ring_triangle_sawtooth;
		case 72: return & the_classic_pro_ring_square_sine;
		case 73: return & the_classic_pro_ring_sawtooth_sine;
		case 74: return & the_classic_pro_ring_sine_sine;
		case 75: return & the_classic_pro_ring_triangle_sine;
		case 76: return & the_classic_pro_ring_square_triangle;
		case 77: return & the_classic_pro_ring_sawtooth_triangle;
		case 78: return & the_classic_pro_ring_sine_triangle;
		case 79: return & the_classic_pro_ring_triangle_triangle;
		case 96: return & the_smooth_classic_pro_ring_square_square;
		case 97: return & the_smooth_classic_pro_ring_sawtooth_square;
		case 98: return & the_smooth_classic_pro_ring_sine_square;
		case 99: return & the_smooth_classic_pro_ring_triangle_square;
		case 100: return & the_smooth_classic_pro_ring_square_sawtooth;
		case 101: return & the_smooth_classic_pro_ring_sawtooth_sawtooth;
		case 102: return & the_smooth_classic_pro_ring_sine_sawtooth;
		case 103: return & the_smooth_classic_pro_ring_triangle_sawtooth;
		case 104: return & the_smooth_classic_pro_ring_square_sine;
		case 105: return & the_smooth_classic_pro_ring_sawtooth_sine;
		case 106: return & the_smooth_classic_pro_ring_sine_sine;
		case 107: return & the_smooth_classic_pro_ring_triangle_sine;
		case 108: return & the_smooth_classic_pro_ring_square_triangle;
		case 109: return & the_smooth_classic_pro_ring_sawtooth_triangle;
		case 110: return & the_smooth_classic_pro_ring_sine_triangle;
		case 111: return & the_smooth_classic_pro_ring_triangle_triangle;
		default: break;
		}
		break;
	default: break;
	}
	return NULL;
}
