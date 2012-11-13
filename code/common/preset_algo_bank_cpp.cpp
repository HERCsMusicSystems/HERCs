///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc_cpp.h"

extern algo_pointer get_fm_2_3_4_algo (int ind, int program);
extern algo_pointer get_ffm_2_3_algo (int ind, int program);
extern algo_pointer get_classic_algo (int ind, int program);
extern algo_pointer get_multifilter_algo (int ind, int program);
extern algo_pointer get_alarm_algo (int ind, int program);
extern algo_pointer get_classic_pro_algo (int ind, int program);
extern algo_pointer get_input_bypass_algo (int ind, int program);
extern algo_pointer get_tester_algo (int ind, int program);

algo_pointer preset_algo_bank_cpp :: get_algo (int lsb, int program, parameter_block * pb) {
	switch (lsb) {
	case 0: return get_fm_2_3_4_algo (pb -> hercules_number, program);
	case 1: return get_ffm_2_3_algo (pb -> hercules_number, program);
	case 2: return get_classic_algo (pb -> hercules_number, program);
	case 3: return get_multifilter_algo (pb -> hercules_number, program);
	case 4: return get_alarm_algo (pb -> hercules_number, program);
	case 5: return get_classic_pro_algo (pb -> hercules_number, program);
	case 6: return get_input_bypass_algo (pb -> hercules_number, program);
	case 7: return get_tester_algo (pb -> hercules_number, program);
	default: return NULL;
	}
}

char * preset_algo_bank_cpp :: toString (int lsb) {
	switch (lsb) {
	case 0: return "FM";
	case 1: return "FM Filtered";
	case 2: return "Classic Subtractive";
	case 3: return "Multifilters";
	case 4: return "Alarms";
	case 5: return "Classic Pro";
	case 6: return "Input";
	case 7: return "Testers";
	default: return "DEFAULT ALGO";
	}
}
preset_algo_bank_cpp :: preset_algo_bank_cpp (void) {}
preset_algo_bank_cpp :: ~ preset_algo_bank_cpp (void) {}

