///////////////////////////////////////
// Copyright (C) 2004 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc_cpp.h"

extern dsp_algo_pointer get_preset_dsp_algo (int ind);

dsp_algo_pointer preset_dsp_algo_bank_cpp :: get_dsp_algo (int program, parameter_block * pb) {return get_preset_dsp_algo (program);}