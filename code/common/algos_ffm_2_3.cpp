///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc_cpp.h"
#include "algo_building_blocks.h"

static char * static_filter_parameter_name (int ind) {
	switch (ind) {
	case 1: return "High pass Filter and Noise"; break;
	case 2: return "Band pass Filter and Noise"; break;
	case 3: return "Band reject Filter and Noise"; break;
	default: return "Low pass Filter and Noise"; break;
	}
}

class ffm1_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered FM Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier F";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		FM_FEEDBACK (0);
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_ffm1_horizontal;

class ffm2_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered FM Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: return "Carrier F";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		FM_FEEDBACK (0);
		accu = wave;
		FM_FEEDBACK (2);
		wave += accu;
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_ffm2_horizontal;

class ffm2_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered FM Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		case 2: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		FM_FEEDBACK (2);
		FM_SINE (0);
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_ffm2_vertical;

class ffm3_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered FM Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: case 3: return "Carrier F";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		FM_FEEDBACK (0);
		accu = wave;
		FM_FEEDBACK (2);
		accu += wave;
		FM_FEEDBACK (3);
		wave += accu;
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_ffm3_horizontal;

class ffm3_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered FM Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		case 2: return "Top Modulator F => 1";
		case 3: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		FM_FEEDBACK (2);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		wave += accu;
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_ffm3_half_horizontal;

class ffm3_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered FM Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		case 2: case 3: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		FM_FEEDBACK (2);
		accu = wave;
		FM_FEEDBACK (3);
		wave += accu;
		FM_SINE (0);
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_ffm3_tree;

class ffm3_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered FM Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		case 2: return "Modulator => 1";
		case 3: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		FM_FEEDBACK (3);
		FM_SINE (2);
		FM_SINE (0);
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_ffm3_vertical;

class ffm3_pyramid : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered FM Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: return "Carrier";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		case 3: return "Top Modulator F => 1, 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		FM_FEEDBACK (3);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (2);
		wave += accu;
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_ffm3_pyramid;

class sawtooth_1 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sawtooth";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SAWTOOTH (0, 2);
		return (int) (wave + 0.5f);
	}
} the_sawtooth_1;

class filter_super_saw_1 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered Super Sawtooth";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SAWTOOTH (0, 2);
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_filter_super_saw_1;

class filter_super_saw_2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered Super Sawtooth";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		case 2: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SAWTOOTH (0, 2);
		accu = wave;
		COSINE_ANTIALIASED_SAWTOOTH (2, 2);
		wave += accu;
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_filter_super_saw_2;

class filter_super_saw_3 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered Super Sawtooth";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Sawtooth";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		case 2: return "Sawtooth";
		case 3: return "Sawtooth";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SAWTOOTH (0, 2);
		accu = wave;
		COSINE_ANTIALIASED_SAWTOOTH (2, 2);
		accu += wave;
		COSINE_ANTIALIASED_SAWTOOTH (3, 2);
		wave += accu;
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_filter_super_saw_3;

class filter_super_sqr_1 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered Super Square";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SQUARE (0, 2);
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_filter_super_sqr_1;

class filter_super_sqr_2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered Super Square";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		case 2: return "Square";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SQUARE (0, 2);
		accu = wave;
		COSINE_ANTIALIASED_SQUARE (2, 2);
		wave += accu;
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_filter_super_sqr_2;

class filter_super_sqr_3 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered Super Square";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		case 2: return "Square";
		case 3: return "Square";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SQUARE (0, 2);
		accu = wave;
		COSINE_ANTIALIASED_SQUARE (2, 2);
		accu += wave;
		COSINE_ANTIALIASED_SQUARE (3, 2);
		wave += accu;
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_filter_super_sqr_3;

class square_1 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Square";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SQUARE (0, 2);
		return (int) (wave + 0.5f);
	}
} the_square_1;

class filter_square : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Filtered Square";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Square";
		case 1: return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		COSINE_ANTIALIASED_SQUARE (0, 2);
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	STATE_NOISE_FILTER_GET_NAME (1)
} the_filter_square;

class classic : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Classic";}
	virtual char * toString (parameter_block * pb, int ind) {
		if (ind >= pb -> hercules_number) return "inactive";
		if (ind == 1) return static_filter_parameter_name (pb -> controllers -> algo_parameters [1]);
		if (ind >= 0 && ind < pb -> hercules_number) {
			switch (pb -> controllers -> algo_parameters [ind]) {
			case 1: return "Sawtooth"; break;
			case 2: return "Square"; break;
			case 3: return "Triangle"; break;
			default: return "Sine"; break;
			}
		}
		return "inactive";
	}
	virtual int code (oscillator * osc) {
		int * algo_ip = osc -> pb -> controllers -> algo_parameters;
		FM_DECLARATIONS;
		STATE_FILTER_DECLARATIONS;
		COSINE_ANTIALIAS_DECLARATIONS;
		ANTIALIAS_TRIANGLE_DECLARATIONS;
		switch (algo_ip [0]) {
		case 1: COSINE_ANTIALIASED_SAWTOOTH (0, 2); break;
		case 2: COSINE_ANTIALIASED_SQUARE (0, 2); break;
		case 3: ANTIALIASED_TRIANGLE (0); break;
		default: SINE (0); break;
		}
		accu = wave;
		for (int index = 2; index < osc -> pb -> hercules_number; index++) {
			switch (algo_ip [index]) {
			case 1: COSINE_ANTIALIASED_SAWTOOTH (index, 2); break;
			case 2: COSINE_ANTIALIASED_SQUARE (index, 2); break;
			case 3: ANTIALIASED_TRIANGLE (index); break;
			default: SINE (index); break;
			}
			accu += wave;
		}
		wave = accu;
		STATE_NOISE_FILTER (1, 16);
		STATE_NOISE_FILTER_WAVE (1);
		return (int) (wave + 0.5f);
	}
	virtual char * get_name (int ind) {
		switch (ind) {
		case 0: return "WAVEFORM 0";
		case 1: return "FILTER TYPE";
		case 2: return "WAVEFORM 2";
		case 3: return "WAVEFORM 3";
		case 4: return "WAVEFORM 4";
		case 5: return "WAVEFORM 5";
		case 6: return "WAVEFORM 6";
		case 7: return "WAVEFORM 7";
		case 8: return "WAVEFORM 8";
		case 9: return "WAVEFORM 9";
		case 10: return "WAVEFORM 10";
		case 11: return "WAVEFORM 11";
		case 12: return "WAVEFORM 12";
		case 13: return "WAVEFORM 13";
		case 14: return "WAVEFORM 14";
		case 15: return "WAVEFORM 15";
		default: return algo :: get_name (ind);
		}
	}
} the_classic;

class noise_multifilter : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Noise Multifilter";}
	virtual char * toString (parameter_block * pb, int ind) {return "Resonant Filter";}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		MULTIFILTER_DECLARATIONS;
		int y = 16;
		NOISE;
		for (int x = 0; x < osc -> stripes; x++) {
			FILTER (x, y);
			accu += filter_output;
			y++;
		}
		return (int) (accu + 0.5f);
	}
} the_noise_multifilter;

class fm1_horizontal_multifilter : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "FM 1 Horizontal Multifilter";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier F";
		default: return "Resonant Filter";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		MULTIFILTER_DECLARATIONS;
		int y = 17;
		FM_FEEDBACK (0);
		for (int x = 1; x < osc -> stripes; x++) {
			FILTER (x, y);
			accu += filter_output;
			y++;
		}
		return (int) (accu + 0.5f);
	}
} the_fm1_horizontal_multifilter;

class fm2_horizontal_multifilter : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "FM 2 Horizontal Multifilter";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 3: return "Carrier F";
		case 1: case 2: return "Resonant Filter";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		MULTIFILTER_DECLARATIONS;
		FM_FEEDBACK (0);
		accu = wave;
		FM_FEEDBACK (3);
		wave += accu;
		FILTER (1, 16);
		accu = filter_output;
		FILTER (2, 17);
		accu += filter_output + 0.5f;
		return (int) accu;
	}
} the_fm2_horizontal_multifilter;

class fm2_vertical_multifilter : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "FM 2 Vertical Multifilter";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: return "Resonant Filter";
		case 3: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		MULTIFILTER_DECLARATIONS;
		FM_FEEDBACK (0);
		FM_SINE (3);
		FILTER (1, 16);
		accu = filter_output;
		FILTER (2, 17);
		accu += filter_output + 0.5f;
		return (int) accu;
	}
} the_fm2_vertical_multifilter;

algo_pointer get_ffm_2_3_algo (int ind, int program) {
	if (ind > 4) ind = 4;
	switch (ind) {
	case 1:
		if (program == 0) return & the_ffm1_horizontal;
		break;
	case 3:
		switch (program) {
		case 0: return & the_ffm2_horizontal;
		case 1: return & the_ffm2_vertical;
		default: break;
		}
		break;
	case 4:
		switch (program) {
		case 0: return & the_ffm3_horizontal;
		case 1: return & the_ffm3_half_horizontal;
		case 2: return & the_ffm3_tree;
		case 3: return & the_ffm3_vertical;
		case 4: return & the_ffm3_pyramid;
		default: break;
		}
		break;
	default: break;
	}
	return NULL;
}

algo_pointer get_multifilter_algo (int ind, int program) {
	switch (program) {
	case 0: return & the_noise_multifilter;
	case 1: return & the_fm1_horizontal_multifilter;
	case 2: if (ind < 4) break; return & the_fm2_horizontal_multifilter;
	case 3: if (ind < 4) break; return & the_fm2_vertical_multifilter;
	default: break;
	}
	return NULL;
}

algo_pointer get_classic_algo (int ind, int program) {
	if (ind > 4) ind = 4;
	switch (ind) {
	case 1:
		switch (program) {
		case 0: case 1: return & the_square_1;
		case 2: return & the_sawtooth_1;
		default: break;
		}
		break;
	case 2:
		switch (program) {
		case 0: return & the_filter_square;
		case 1: return & the_filter_super_sqr_1;
		case 2: return & the_filter_super_saw_1;
		default: break;
		}
		break;
	case 3:
		switch (program) {
		case 0: return & the_classic;
		case 1: return & the_filter_super_sqr_2;
		case 2: return & the_filter_super_saw_2;
		default: break;
		}
		break;
	case 4:
		switch (program) {
		case 0: return & the_classic;
		case 1: return & the_filter_super_sqr_3;
		case 2: return & the_filter_super_saw_3;
		default: break;
		}
		break;
	default: break;
	}
	return NULL;
}

