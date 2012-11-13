///////////////////////////////////////
// Copyright (C) 2004 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"

extern pattern_pointer get_preset_pattern (int ind);

char * preset_pattern_bank :: toString (void) {return "PRESET PATTERN BANK";}
char * preset_pattern_bank :: toString (int lsb) {return "ARPEGGIO PATTERNS";}

pattern_pointer preset_pattern_bank :: get_pattern (int lsb, int program, parameter_block * pb) {
	return get_preset_pattern (program);
}

preset_pattern_bank :: preset_pattern_bank (void) {}
preset_pattern_bank :: ~ preset_pattern_bank (void) {}

