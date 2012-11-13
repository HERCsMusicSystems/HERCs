///////////////////////////////////////
// Copyright (C) 2004 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "transport.h"

#include "osc.h"

static noise random;

class up_only_pattern : public arpeggio_pattern {
private:
	int oct_sentinel;
	STRING name;
public:
	virtual char * toString (parameter_block * pb) {return name;}
	virtual void code (arranger * arp) {
		if (arp -> hit (arp -> counter++, 0)) return;
		arp -> counter = 0;
		if (arp -> octave < oct_sentinel) arp -> octave++;
		else arp -> octave = 0;
		arp -> hit (arp -> counter++, 0);
	}
	up_only_pattern (int octaves, STRING name) {
		oct_sentinel = octaves - 1;
		string_copy (this -> name, name);
	}
};

class down_only_pattern : public arpeggio_pattern {
private:
	int oct_sentinel;
	STRING name;
public:
	virtual char * toString (parameter_block * pb) {return name;}
	virtual void code (arranger * arp) {
		if (arp -> hit (arp -> top_key_index () - arp -> counter++, 0)) return;
		arp -> counter = 0;
		if (arp -> octave > oct_sentinel) arp -> octave--;
		else arp -> octave = 0;
		arp -> hit (arp -> top_key_index () - arp -> counter++, 0);
	}
	down_only_pattern (int octaves, STRING name) {
		oct_sentinel = 1 - octaves;
		string_copy (this -> name, name);
	}
};

class up_down_dup_pattern : public arpeggio_pattern {
private:
	int oct_sentinel;
	STRING name;
public:
	virtual char * toString (parameter_block * pb) {return name;}
	virtual void code (arranger * arp) {
		if (arp -> status == 0) {
			if (arp -> hit (arp -> counter++, 0)) return;
			arp -> counter = 0;
			if (arp -> octave < oct_sentinel) {
				arp -> octave++;
				arp -> hit (arp -> counter++, 0);
			} else {
				arp -> status = 1;
				arp -> hit (arp -> top_key_index () - arp -> counter++, 0);
			}
		} else {
			if (arp -> hit (arp -> top_key_index () - arp -> counter++, 0)) return;
			arp -> counter = 0;
			if (arp -> octave > 0) {
				arp -> octave--;
				arp -> hit (arp -> top_key_index () - arp -> counter++, 0);
			} else {
				arp -> status = 0;
				arp -> hit (arp -> counter++, 0);
			}
		}
	}
	up_down_dup_pattern (int octaves, STRING name) {
		oct_sentinel = octaves - 1;
		string_copy (this -> name, name);
	}
};

class up_down_pattern : public arpeggio_pattern {
private:
	int oct_sentinel;
	STRING name;
public:
	virtual char * toString (parameter_block * pb) {return name;}
	virtual void code (arranger * arp) {
		if (arp -> status == 0) {
			if (arp -> hit (arp -> counter++, 0)) return;
			arp -> counter = 0;
			if (arp -> octave < oct_sentinel) {
				arp -> octave++;
				arp -> hit (arp -> counter++, 0);
			} else {
				arp -> status = 1;
				arp -> counter++;
				if (arp -> hit (arp -> top_key_index () - arp -> counter++, 0)) return;
				if (arp -> octave > 0) arp -> octave--;
				else arp -> status = 0;
				arp -> counter = 0;
				arp -> hit (arp -> top_key_index () - arp -> counter++, 0);
			}
		} else {
			if (arp -> hit (arp -> top_key_index () - arp -> counter++, 0)) return;
			arp -> counter = 0;
			if (arp -> octave > 0) {
				arp -> octave--;
				arp -> hit (arp -> top_key_index () - arp -> counter++, 0);
			} else {
				arp -> status = 0;
				arp -> counter++;
				if (arp -> hit (arp -> counter++, 0)) return;
				if (arp -> octave < oct_sentinel) arp -> octave++;
				else arp -> status = 1;
				arp -> counter = 0;
				arp -> hit (arp -> counter++, 0);
			}
		}
	}
	up_down_pattern (int octaves, STRING name) {
		oct_sentinel = octaves - 1;
		string_copy (this -> name, name);
	}
};

class random_dup_pattern : public arpeggio_pattern {
private:
	int octaves;
	STRING name;
public:
	virtual char * toString (parameter_block * pb) {return name;}
	virtual void code (arranger * arp) {
		arp -> octave = random . move (0, octaves);
		int ind = random . move (0, arp -> top_key_index () + 1);
		arp -> hit (ind, 0);
	}
	random_dup_pattern (int octaves, STRING name) {
		this -> octaves = octaves;
		string_copy (this -> name, name);
	}
};

class random_no_dup_pattern : public arpeggio_pattern {
private:
	int octaves;
	STRING name;
public:
	virtual char * toString (parameter_block * pb) {return name;}
	virtual void code (arranger * arp) {
		arp -> octave = random . move (0, octaves);
		int modulo = arp -> top_key_index () + 1;
		int ind = random . move (1, modulo);
		ind += arp -> counter;
		while (ind >= modulo) ind -= modulo;
		arp -> hit (ind, 0);
		arp -> counter = ind;
	}
	random_no_dup_pattern (int octaves, STRING name) {
		this -> octaves = octaves;
		string_copy (this -> name, name);
	}
};

up_only_pattern the_up_1 (1, "Up 1 Octave");
up_only_pattern the_up_2 (2, "Up 2 Octaves");
up_only_pattern the_up_3 (3, "Up 3 Octaves");
up_only_pattern the_up_4 (4, "Up 4 Octaves");
down_only_pattern the_down_1 (1, "Down 1 Octave");
down_only_pattern the_down_2 (2, "Down 2 Octaves");
down_only_pattern the_down_3 (3, "Down 3 Octaves");
down_only_pattern the_down_4 (4, "Down 4 Octaves");
up_down_dup_pattern the_up_down_dup_1 (1, "Up Down Duplicate 1 Octave");
up_down_dup_pattern the_up_down_dup_2 (2, "Up Down Duplicate 2 octaves");
up_down_dup_pattern the_up_down_dup_3 (3, "Up Down Duplicate 3 octaves");
up_down_dup_pattern the_up_down_dup_4 (4, "Up Down Duplicate 4 octaves");
up_down_pattern the_up_down_1 (1, "Up Down 1 Octave");
up_down_pattern the_up_down_2 (2, "Up Down 2 Octaves");
up_down_pattern the_up_down_3 (3, "Up Down 3 Octaves");
up_down_pattern the_up_down_4 (4, "Up Down 4 Octaves");
random_dup_pattern the_random_dup_1 (1, "Random Duplicate 1 Octave");
random_dup_pattern the_random_dup_2 (2, "Random Duplicate 2 Octaves");
random_dup_pattern the_random_dup_3 (3, "Random Duplicate 3 Octaves");
random_dup_pattern the_random_dup_4 (4, "Random Duplicate 4 Octaves");
random_no_dup_pattern the_random_1 (1, "Random 1 Octave");
random_no_dup_pattern the_random_2 (2, "Random 2 Octaves");
random_no_dup_pattern the_random_3 (3, "Random 3 Octaves");
random_no_dup_pattern the_random_4 (4, "Random 4 Octaves");

pattern_pointer get_preset_pattern (int ind) {
	switch (ind) {
	case 0: return & the_up_1;
	case 1: return & the_up_2;
	case 2: return & the_up_3;
	case 3: return & the_up_4;
	case 4: return & the_down_1;
	case 5: return & the_down_2;
	case 6: return & the_down_3;
	case 7: return & the_down_4;
	case 8: return & the_up_down_1;
	case 9: return & the_up_down_2;
	case 10: return & the_up_down_3;
	case 11: return & the_up_down_4;
	case 12: return & the_up_down_dup_1;
	case 13: return & the_up_down_dup_2;
	case 14: return & the_up_down_dup_3;
	case 15: return & the_up_down_dup_4;
	case 16: return & the_random_dup_1;
	case 17: return & the_random_dup_2;
	case 18: return & the_random_dup_3;
	case 19: return & the_random_dup_4;
	case 20: return & the_random_1;
	case 21: return & the_random_2;
	case 22: return & the_random_3;
	case 23: return & the_random_4;
	default: return NULL;
	}
}
