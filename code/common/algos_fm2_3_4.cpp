///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

//#include "osc.h"
#include "osc_cpp.h"
#include "algo_building_blocks.h"

class horizontal_algo : public default_algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Horizontal";}
};

class fm2_horizontal : public horizontal_algo {
	virtual char * toString (parameter_block * pb, int ind) {
		if (ind < 2) return "Carrier F";
		return "inactive";
	}
} the_fm2_horizontal;

class fm2_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		FM_SINE (0);
		return (int) (wave + 0.5);
	}
} the_fm2_vertical;

class fm3_horizontal : public horizontal_algo {
	virtual char * toString (parameter_block * pb, int ind) {
		if (ind < 3) return "Carrier F";
		return "inactive";
	}
} the_fm3_horizontal;

class fm3_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Top Modulator F => 1";
		case 2: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (2);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm3_half_horizontal;

class fm3_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		accu = wave;
		FM_FEEDBACK (2);
		wave += accu;
		FM_SINE (0);
		return (int) (wave + 0.5);
	}
} the_fm3_tree;

class fm3_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		FM_SINE (1);
		FM_SINE (0);
		return (int) (wave + 0.5);
	}
} the_fm3_vertical;

class fm3_pyramid : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Top Modulator F => 1, 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm3_pyramid;

class fm4_horizontal : public horizontal_algo {
	virtual char * toString (parameter_block * pb, int ind) {
		if (ind < 4) return "Carrier F";
		return "inactive";
	}
} the_fm4_horizontal;

class fm4_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Top Modulator F => 1";
		case 2: case 3: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (2);
		accu += wave;
		FM_FEEDBACK (3);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm4_half_horizontal;

class fm4_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: case 3: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		accu = wave;
		FM_FEEDBACK (2);
		accu += wave;
		FM_FEEDBACK (3);
		wave += accu;
		FM_SINE (0);
		return (int) (wave + 0.5f);
	}
} the_fm4_tree;

class fm4_tree_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: return "Top Modulator F => 1";
		case 3: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		accu = wave;
		FM_FEEDBACK (2);
		wave += accu;
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm4_tree_horizontal;

class fm4_bottom_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottom Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		FM_SINE (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm4_bottom_tree;

class fm4_half_pyramid : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Half Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Top Modulator F => 1, 2";
		case 3: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave;
		FM_FEEDBACK (3);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm4_half_pyramid;

class fm4_double_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Double Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: return "Carrier";
		case 1: return "Top Modulator F => 1";
		case 3: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		FM_SINE (2);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm4_double_decker;

class fm4_low_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Low Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		accu = wave;
		FM_FEEDBACK (2);
		FM_SINE (1);
		wave += accu;
		FM_SINE (0);
		return (int) (wave + 0.5);
	}
} the_fm4_low_tree;

class fm4_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "High Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: return "Top Modulator F => 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		accu = wave;
		FM_FEEDBACK (3);
		wave += accu;
		FM_SINE (1);
		FM_SINE (0);
		return (int) (wave + 0.5);
	}
} the_fm4_high_tree;

class fm4_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		FM_SINE (2);
		FM_SINE (1);
		FM_SINE (0);
		return (int) (wave + 0.5);
	}
} the_fm4_vertical;

class fm4_pyramid : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: return "Top Modulator F => 1, 2, 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave; wave = sub;
		FM_SINE (2);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm4_pyramid;

class fm4_bottleneck : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottleneck";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: case 3: return "Top Modulator F => 1, 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		accu = wave;
		FM_FEEDBACK (2);
		wave += accu;
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm4_bottleneck;

class fm4_spear : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Spear";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		FM_SINE (2);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm4_spear;

class fm5_horizontal : public horizontal_algo {
	virtual char * toString (parameter_block * pb, int ind) {
		if (ind < 5) return "Carrier F";
		return "inactive";
	}
} the_fm5_horizontal;

class fm5_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
			case 0: return "Carrier";
			case 1: return "Top Modulator F => 1";
			case 2: case 3: case 4: return "Carrier F";
			default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (2);
		accu += wave;
		FM_FEEDBACK (3);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm5_half_horizontal;

class fm5_tree_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: return "Top Modulator F => 1";
		case 3: return "Carrier F";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		accu = wave;
		FM_FEEDBACK (2);
		wave += accu;
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm5_tree_horizontal;

class fm5_bottom_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottom Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: return "Carrier F";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		FM_SINE (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm5_bottom_tree;

class fm5_half_pyramid : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Half Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Top Modulator F => 1, 2";
		case 3: return "Carrier F";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave;
		FM_FEEDBACK (3);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm5_half_pyramid;

class fm5_double_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Double Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: return "Carrier";
		case 1: return "Top Modulator F => 1";
		case 3: return "Top Modulator F => 3";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		FM_SINE (2);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm5_double_decker;

class fm5_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: case 3: return "Top Modulator F => 1";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		accu = wave;
		FM_FEEDBACK (2);
		accu += wave;
		FM_FEEDBACK (3);
		wave += accu;
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (4);
		wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm5_tree;

class fm5_low_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Low Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: return "Top Modulator F => 1";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		accu = wave;
		FM_FEEDBACK (2);
		FM_SINE (1);
		wave += accu;
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (4);
		wave += accu;
		return (int) (wave + 0.5);
	}
} the_fm5_low_tree;

class fm5_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "High Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: return "Top Modulator F => 2";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		accu = wave;
		FM_FEEDBACK (3);
		wave += accu;
		FM_SINE (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (4);
		wave += accu;
		return (int) (wave + 0.5);
	}
} the_fm5_high_tree;

class fm5_vertical4 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical 4";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		FM_SINE (2);
		FM_SINE (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (4);
		wave += accu;
		return (int) (wave + 0.5);
	}
} the_fm5_vertical4;

class fm5_pyramid3 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 3";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: return "Top Modulator F => 1, 2, 3";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave; wave = sub;
		FM_SINE (2);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm5_pyramid3;

class fm5_bottleneck2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottleneck";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: case 3: return "Top Modulator F => 1, 2";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		accu = wave;
		FM_FEEDBACK (2);
		wave += accu;
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm5_bottleneck2;

class fm5_spear2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Spear";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		FM_SINE (2);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm5_spear2;

class fm5_rooted_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: case 3: case 4: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); accu = wave;
		FM_FEEDBACK (2); accu += wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm5_rooted_horizontal;

class fm5_rooted_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: case 4: return "Top Modulator F => 1";
		default: return "inactice";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm5_rooted_half_horizontal;

class fm5_rooted_tree_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Tree Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: return "Top Modulator F => 2";
		case 4: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); wave += accu;
		FM_SINE (1); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm5_rooted_tree_horizontal;

class fm5_rooted_bottom_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Bottom Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm5_rooted_bottom_tree;

class fm5_rooted_double_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Double Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 3: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 4: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (4); FM_SINE (3); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm5_rooted_double_decker;

class fm5_rooted_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: case 4: return "Top Modulator F => 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm5_rooted_tree;

class fm5_rooted_low_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Low Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Top Modulator F => 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm5_rooted_low_tree;

class fm5_rooted_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted High Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: case 4: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave; FM_FEEDBACK (4); wave += accu;
		FM_SINE (2); FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm5_rooted_high_tree;

class fm5_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Modulator => 3";
		case 4: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); FM_SINE (2); FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm5_vertical;

class fm5_extended_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Extended Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 3: return "Carrier";
		case 1: case 2: return "Top Modulator F => 1";
		case 4: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); accu = wave;
		FM_FEEDBACK (2); wave += accu;
		FM_SINE (0); accu = wave;
		FM_FEEDBACK (4); FM_SINE (3); return (int) (wave + accu + 0.5f);
	}
} the_fm5_extended_tree;

class fm5_extended_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Extended Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 3: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 4: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (4); FM_SINE (3); return (int) (wave + accu + 0.5f);
	}
} the_fm5_extended_decker;

class fm5_extended_pyramid : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Extended Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 3: return "Carrier";
		case 2: return "Top Modulator F => 1, 2";
		case 4: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave;
		FM_FEEDBACK (4); FM_SINE (3); return (int) (wave + accu + 0.5f);
	}
} the_fm5_extended_pyramid;

class fm5_pyramid : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: case 3: return "Carrier";
		case 4: return "Top Modulator F => 1, 2, 3, 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); accu += wave; wave = sub;
		FM_SINE (3); return (int) (wave + accu + 0.5f);
	}
} the_fm5_pyramid;

class fm5_pyramid3_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 3 Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: case 4: return "Top Modulator F => 1, 2, 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave;
		FM_FEEDBACK (4); wave += accu; float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); return (int) (wave + accu + 0.5f);
	}
} the_fm5_pyramid3_horizontal;

class fm5_pyramid3_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 3 Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: return "Modulator => 1, 2, 3";
		case 4: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); return (int) (wave + accu + 0.5f);
	}
} the_fm5_pyramid3_vertical;

class fm5_pyramid2_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: case 3: case 4: return "Top Modulator F => 1, 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); wave += accu; float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm5_pyramid2_horizontal;

class fm5_pyramid2_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Top Modulator F => 1, 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); accu = wave;
		FM_FEEDBACK (4); wave += accu; float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm5_pyramid2_half_horizontal;

class fm5_pyramid2_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: case 4: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm5_pyramid2_tree;

class fm5_spear : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Spear";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Modulator => 3";
		case 4: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); FM_SINE (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm5_spear;

class fm6_horizontal : public horizontal_algo {
	virtual char * toString (parameter_block * pb, int ind) {
		if (ind < 6) return "Carrier F";
		return "inactive";
	}
} the_fm6_horizontal;

class fm6_half_horizontal1 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Half Horizontal 1";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Top Modulator F => 1";
		case 2: case 3: case 4: case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (2);
		accu += wave;
		FM_FEEDBACK (3);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm6_half_horizontal1;

class fm6_tree_horizontal2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Horizontal 2";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: return "Top Modulator F => 1";
		case 3: return "Carrier F";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		accu = wave;
		FM_FEEDBACK (2);
		wave += accu;
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm6_tree_horizontal2;

class fm6_half_horizontal2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Half Horizontal 2";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: return "Carrier F";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		FM_SINE (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm6_half_horizontal2;

class fm6_half_pyramid2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Half Pyramid 2";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Top Modulator F => 1, 2";
		case 3: return "Carrier F";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave;
		FM_FEEDBACK (3);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm6_half_pyramid2;

class fm6_double_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Double Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: return "Carrier";
		case 1: return "Top Modulator F => 1";
		case 3: return "Top Modulator F => 3";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (3);
		FM_SINE (2);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm6_double_decker;

class fm6_tree_horizontal3 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Horizontal 3";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: case 3: return "Top Modulator F => 1";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1);
		accu = wave;
		FM_FEEDBACK (2);
		accu += wave;
		FM_FEEDBACK (3);
		wave += accu;
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_tree_horizontal3;

class fm6_low_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Low Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: return "Top Modulator F => 1";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		accu = wave;
		FM_FEEDBACK (2);
		FM_SINE (1);
		wave += accu;
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		wave += accu;
		return (int) (wave + 0.5);
	}
} the_fm6_low_tree;

class fm6_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "High Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: return "Top Modulator F => 2";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2);
		accu = wave;
		FM_FEEDBACK (3);
		wave += accu;
		FM_SINE (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		wave += accu;
		return (int) (wave + 0.5);
	}
} the_fm6_high_tree;

class fm6_half_horizontal4 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Half Horizontal 4";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		FM_SINE (2);
		FM_SINE (1);
		FM_SINE (0);
		accu = wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		wave += accu;
		return (int) (wave + 0.5);
	}
} the_fm6_half_horizontal4;

class fm6_pyramid3 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 3";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: return "Top Modulator F => 1, 2, 3";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave; wave = sub;
		FM_SINE (2);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm6_pyramid3;

class fm6_bottleneck22_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottleneck 2 2 Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: case 3: return "Top Modulator F => 1, 2";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		accu = wave;
		FM_FEEDBACK (2);
		wave += accu;
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm6_bottleneck22_horizontal;

class fm6_spear2_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Spear 2 Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Carrier F";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3);
		FM_SINE (2);
		float sub = wave;
		FM_SINE (0);
		accu = wave; wave = sub;
		FM_SINE (1);
		accu += wave;
		FM_FEEDBACK (4);
		accu += wave;
		FM_FEEDBACK (5);
		accu += wave + 0.5f;
		return (int) accu;
	}
} the_fm6_spear2_horizontal;

class fm6_sided_rooted_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided Rooted Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: case 3: case 4: return "Top Modulator F => 1";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); accu = wave;
		FM_FEEDBACK (2); accu += wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_sided_rooted_horizontal;

class fm6_sided_rooted_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided Rooted Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: case 4: return "Top Modulator F => 1";
		case 5: return "Carrier F";
		default: return "inactice";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_sided_rooted_half_horizontal;

class fm6_sided_rooted_half_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided Rooted Half Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: return "Top Modulator F => 2";
		case 4: return "Top Modulator F => 1";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); wave += accu;
		FM_SINE (1); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_sided_rooted_half_tree;

class fm6_sided_low_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided Low Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Top Modulator F => 1";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_sided_low_tree;

class fm6_sided_rooted_double_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided Rooted Double Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 3: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 4: return "Top Modulator F => 4";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (4); FM_SINE (3); wave += accu;
		FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_sided_rooted_double_decker;

class fm6_sided_rooted_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided Rooted Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: case 4: return "Top Modulator F => 2";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_sided_rooted_tree;

class fm6_sided_middle_low_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided Middle Low Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Top Modulator F => 2";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_sided_middle_low_tree;

class fm6_sided_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided High Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: case 4: return "Top Modulator F => 3";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave; FM_FEEDBACK (4); wave += accu;
		FM_SINE (2); FM_SINE (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_sided_high_tree;

class fm6_vertical5 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical 5";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Modulator => 3";
		case 4: return "Top Modulator F => 4";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); FM_SINE (2); FM_SINE (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		return (int) (wave + 0.5f);
	}
} the_fm6_vertical5;

class fm6_tree_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 3: return "Carrier";
		case 1: case 2: return "Top Modulator F => 1";
		case 4: return "Top Modulator F => 4";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); accu = wave;
		FM_FEEDBACK (2); wave += accu;
		FM_SINE (0); accu = wave;
		FM_FEEDBACK (4); FM_SINE (3); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_tree_half_horizontal;

class fm6_decker_3_2_1 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Decker 3 2 1";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 3: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 4: return "Top Modulator F => 4";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (4); FM_SINE (3); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_decker_3_2_1;

class fm6_pyramid_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 3: return "Carrier";
		case 2: return "Top Modulator F => 1, 2";
		case 4: return "Top Modulator F => 4";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave;
		FM_FEEDBACK (4); FM_SINE (3); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid_half_horizontal;

class fm6_pyramid5 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: case 3: return "Carrier";
		case 4: return "Top Modulator F => 1, 2, 3, 4";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); accu += wave; wave = sub;
		FM_SINE (3); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid5;

class fm6_bottleneck23_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottleneck 2 3 Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: case 4: return "Top Modulator F => 1, 2, 3";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave;
		FM_FEEDBACK (4); wave += accu; float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_bottleneck23_horizontal;

class fm6_sided_vertical_spear3 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided Vertical Spear 3";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: return "Modulator => 1, 2, 3";
		case 4: return "Top Modulator F => 4";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_sided_vertical_spear3;

class fm6_bottleneck32_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottleneck 3 2 Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: case 3: case 4: return "Top Modulator F => 1, 2";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); wave += accu; float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_bottleneck32_horizontal;

class fm6_bottleneck_half_horizontal2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottleneck Half Horizontal 2";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Top Modulator F => 1, 2";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); accu = wave;
		FM_FEEDBACK (4); wave += accu; float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_bottleneck_half_horizontal2;

class fm6_bottleneck_tree_horizontal2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottleneck Tree Horizontal 2";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: case 4: return "Top Modulator F => 3";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_bottleneck_tree_horizontal2;

class fm6_sided_spear2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Sided Spear 2";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Modulator => 3";
		case 4: return "Top Modulator F => 4";
		case 5: return "Carrier F";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); FM_SINE (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave;
		FM_FEEDBACK (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_sided_spear2;

class fm6_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 2: case 3: case 4: case 5: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); accu = wave;
		FM_FEEDBACK (2); accu += wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); accu += wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_tree;

class fm6_tree_half_horizontal1 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Half Horizontal 1";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: case 4: case 5: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); accu += wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_tree_half_horizontal1;

class fm6_tree_tree_horizontal2 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Tree Horizontal 2";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: return "Top Modulator F => 2";
		case 4: case 5: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave; FM_FEEDBACK (3); wave += accu;
		FM_SINE (1); accu = wave;
		FM_FEEDBACK (4); accu += wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_tree_tree_horizontal2;

class fm6_tree_half_horizontal3 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Half Horizontal 3";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 4: case 5: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (4); accu += wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_tree_half_horizontal3;

class fm6_tree_sided_double_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Sided Double Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 3: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 4: return "Top Modulator F => 4";
		case 5: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (4); FM_SINE (3); accu += wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_tree_sided_double_decker;

class fm6_tree_sided_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Sided Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: case 4: return "Top Modulator F => 2";
		case 5: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (1); accu = wave;
		FM_SINE (5); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_tree_sided_tree;

class fm6_double_low_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Double low tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 4: return "Top Modulator F => 2";
		case 5: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (1); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_double_low_tree;

class fm6_double_low_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Double Low High Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: case 4: return "Top Modulator F => 3";
		case 5: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_double_low_high_tree;

class fm6_vertical_low_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical Low Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Modulator => 3";
		case 4: return "Top Modulator F => 4";
		case 5: return "Top Modulator F => 1";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); FM_SINE (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_vertical_low_tree;

class fm6_rooted_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: case 4: case 5: return "Top Modulator F => 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); accu += wave;
		FM_FEEDBACK (4); accu += wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_rooted_tree;

class fm6_rooted_tree_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Tree Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 4: case 5: return "Top Modulator F => 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); accu = wave;
		FM_FEEDBACK (4); accu += wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_rooted_tree_half_horizontal;

class fm6_double_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Double High Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: case 4: return "Top Modulator F => 3";
		case 5: return "Top Modulator F => 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (2); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_double_high_tree;

class fm6_middle_low_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Middle Low Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Modulator => 3";
		case 4: return "Top Modulator F => 4";
		case 5: return "Top Modulator F => 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); FM_SINE (2); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_middle_low_tree;

class fm6_rooted_tree_double_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Tree Double Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 4: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); accu = wave;
		FM_FEEDBACK (5); FM_SINE (4); wave += accu;
		FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_rooted_tree_double_decker;

class fm6_rooted_rooted_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Rooted Rooted Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: case 4: case 5: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave;
		FM_FEEDBACK (4); accu += wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (2); FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_rooted_rooted_tree;

class fm6_middle_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Middle High Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Modulator => 3";
		case 4: return "Top Modulator F => 4";
		case 5: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (2); FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_middle_high_tree;

class fm6_vertical_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical High Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Modulator => 3";
		case 4: case 5: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (3); FM_SINE (2); FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_vertical_high_tree;

class fm6_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Modulator => 3";
		case 4: return "Modulator => 4";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (5); FM_SINE (4); FM_SINE (3); FM_SINE (2); FM_SINE (1); FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_vertical;

class fm6_tree_tree_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Tree Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 4: return "Modulator => 1";
		case 2: case 3: return "Top Modulator F => 2";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); wave += accu;
		FM_SINE (1); accu = wave;
		FM_FEEDBACK (5); FM_SINE (4); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_tree_tree_vertical;

class fm6_tree_half_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Half Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: return "Carrier";
		case 1: case 4: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (5); FM_SINE (4); wave += accu;
		FM_SINE (0); return (int) (wave + 0.5f);
	}
} the_fm6_tree_half_decker;

class fm6_triple_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Triple Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: case 4: return "Carrier";
		case 1: return "Top Modulator F => 1";
		case 3: return "Top Modulator F => 3";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (3); FM_SINE (2); accu += wave;
		FM_FEEDBACK (5); FM_SINE (4); return (int) (wave + accu + 0.5f);
	}
} the_fm6_triple_decker;

class fm6_tree_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 4: return "Carrier";
		case 1: case 2: case 3: return "Top Modulator F => 1";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); accu = wave;
		FM_FEEDBACK (2); accu += wave;
		FM_FEEDBACK (3); wave += accu;
		FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); FM_SINE (4); return (int) (wave + accu + 0.5f);
	}
} the_fm6_tree_vertical;

class fm6_tree_half_horizontal_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Half Horizontal Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 4: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 3: return "Top Modulator F => 1";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); accu = wave;
		FM_FEEDBACK (3); wave += accu;
		FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); FM_SINE (4); return (int) (wave + accu + 0.5f);
	}
} the_fm6_tree_half_horizontal_vertical;

class fm6_top_tree_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Top Tree Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 4: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: case 3: return "Top Modulator F => 2";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); wave += accu;
		FM_SINE (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); FM_SINE (4); return (int) (wave + accu + 0.5f);
	}
} the_fm6_top_tree_vertical;

class fm6_vertical42 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical 4 2";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 4: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Modulator => 2";
		case 3: return "Top Modulator F => 3";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); FM_SINE (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); FM_SINE (4); return (int) (wave + accu + 0.5f);
	}
} the_fm6_vertical42;

class fm6_vertical33 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Vertical 3 3";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 3: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 4: return "Modulator => 4";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); FM_SINE (0); accu = wave;
		FM_FEEDBACK (5); FM_SINE (4); FM_SINE (3); return (int) (wave + accu + 0.5f);
	}
} the_fm6_vertical33;

class fm6_tree_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 3: return "Carrier";
		case 1: case 2: return "Top Modulator F => 1";
		case 4: case 5: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); accu = wave;
		FM_FEEDBACK (2); wave += accu;
		FM_SINE (0); float sub = wave;
		FM_FEEDBACK (4); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (3); return (int) (wave + sub + 0.5f);
	}
} the_fm6_tree_tree;

class fm6_pyramid_3_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 3 Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: case 4: return "Carrier";
		case 3: return "Top Modulator F => 1, 2, 3";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); accu += wave;
		FM_FEEDBACK (5); FM_SINE (4); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid_3_vertical;

class fm6_bottleneck22_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Bottleneck 2 2 Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 4: return "Carrier";
		case 2: case 3: return "Top Modulator F => 1, 2";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); accu = wave;
		FM_FEEDBACK (3); wave += accu; float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave;
		FM_FEEDBACK (5); FM_SINE (4); return (int) (wave + accu + 0.5f);
	}
} the_fm6_bottleneck22_vertical;

class fm6_spear2_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Spear 2 Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 3: case 4: return "Carrier";
		case 1: return "Modulator => 1, 4";
		case 2: return "Top Modulator F => 2";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (3); accu += wave;
		FM_FEEDBACK (5); FM_SINE (4); return (int) (wave + accu + 0.5f);
	}
} the_fm6_spear2_vertical;

class fm6_tree_vertical_3 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Tree Vertical 3";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 3: return "Carrier";
		case 1: return "Modulator => 1";
		case 2: return "Top Modulator F => 2";
		case 4: case 5: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); FM_SINE (1); FM_SINE (0); float sub = wave;
		FM_FEEDBACK (4); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (3); return (int) (wave + sub + 0.5f);
	}
} the_fm6_tree_vertical_3;

class fm6_pyramid_vertical_3 : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid Vertical 3";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: case 3: return "Carrier";
		case 1: return "Top Modulator F => 1, 3";
		case 4: return "Modulator => 4";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (2); accu += wave;
		FM_FEEDBACK (5); FM_SINE (4); FM_SINE (3); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid_vertical_3;

class fm6_pyramid_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: case 3: return "Carrier";
		case 1: return "Top Modulator F => 1, 3";
		case 4: case 5: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (2); accu += wave;
		FM_FEEDBACK (5); sub = wave;
		FM_FEEDBACK (4); wave += sub;
		FM_SINE (3); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid_tree;

class fm6_pyramid_pyramid : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 2: case 3: case 5: return "Carrier";
		case 1: return "Top Modulator F => 1, 3";
		case 4: return "Top Modulator F => 4, 6";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (1); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (2); accu += wave;
		FM_FEEDBACK (4); sub = wave;
		FM_SINE (3); accu += wave; wave = sub;
		FM_SINE (5); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid_pyramid;

class fm6_pyramid : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: case 3: case 4: return "Carrier";
		case 5: return "Top Modulator F => 1, 2, 3, 4, 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (5); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); accu += wave; wave = sub;
		FM_SINE (3); accu += wave; wave = sub;
		FM_SINE (4); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid;

class fm6_pyramid4_by_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 4 by Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: case 3: return "Carrier";
		case 4: case 5: return "Top Modulator F => 1, 2, 3, 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); float sub = wave;
		FM_FEEDBACK (5); sub += wave; wave = sub;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); accu += wave; wave = sub;
		FM_SINE (3); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid4_by_horizontal;

class fm6_pyramid4_by_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 4 by Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: case 3: return "Carrier";
		case 4: return "Modulator => 1, 2, 3, 4";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (5); FM_SINE (4); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); accu += wave; wave = sub;
		FM_SINE (3); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid4_by_vertical;

class fm6_pyramid3_by_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 3 by Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: case 4: case 5: return "Top Modulator F => 1, 2, 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); float sub = wave;
		FM_FEEDBACK (4); sub += wave;
		FM_FEEDBACK (5); sub += wave; wave = sub;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid3_by_horizontal;

class fm6_pyramid3_by_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 3 by Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: return "Modulator => 1, 2, 3";
		case 4: return "Top Modulator F => 4";
		case 5: return "Top Modulator F => 1, 2, 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); float sub = wave;
		FM_FEEDBACK (5); sub += wave; wave = sub;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid3_by_half_horizontal;

class fm6_pyramid3_by_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 3 by Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: return "Modulator => 1, 2, 3";
		case 4: case 5: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (3); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid3_by_tree;

class fm6_pyramid3_vertical : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 3 by Vertical";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: case 2: return "Carrier";
		case 3: return "Modulator => 1, 2, 3";
		case 4: return "Modulator => 4";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (5); FM_SINE (4); FM_SINE (3); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); accu += wave; wave = sub;
		FM_SINE (2); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid3_by_vertical;

class fm6_pyramid2_by_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 by Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: case 3: case 4: case 5: return "Top Modulator F => 1, 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (2); float sub = wave;
		FM_FEEDBACK (3); sub += wave;
		FM_FEEDBACK (4); sub += wave;
		FM_FEEDBACK (5); sub += wave; wave = sub;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid2_by_horizontal;

class fm6_pyramid2_by_half_horizontal : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 by Half Horizontal";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Top Modulator F => 3";
		case 4: case 5: return "Top Modulator F => 1, 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); float sub = wave;
		FM_FEEDBACK (4); sub += wave;
		FM_FEEDBACK (5); sub += wave; wave = sub;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid2_by_half_horizontal;

class fm6_pyramid2_by_half_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 by Half Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: case 4: return "Top Modulator F => 3";
		case 5: return "Top Modulator F => 1, 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave;
		FM_FEEDBACK (4); wave += accu;
		FM_SINE (2); float sub = wave;
		FM_FEEDBACK (5); sub += wave; wave = sub;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid2_by_half_tree;

class fm6_pyramid2_by_bottom_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 by Bottom Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Modulator => 3";
		case 4: return "Top Modulator F => 4";
		case 5: return "Top Modulator F => 1, 2";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); FM_SINE (2); float sub = wave;
		FM_FEEDBACK (5); sub += wave; wave = sub;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid2_by_bottom_tree;

class fm6_pyramid2_by_double_decker : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 by Double Decker";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: case 4: return "Modulator => 1, 2";
		case 3: return "Top Modulator F => 3";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); FM_SINE (2); float sub = wave;
		FM_FEEDBACK (5); FM_SINE (4); sub += wave; wave = sub;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid2_by_double_decker;

class fm6_pyramid2_by_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 by Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: case 4: case 5: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (3); accu = wave;
		FM_FEEDBACK (4); accu += wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid2_by_tree;

class fm6_pyramid2_by_middle_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 by Middle Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Modulator => 3";
		case 4: return "Top Modulator F => 4";
		case 5: return "Top Modulator F => 3";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); FM_SINE (3); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid2_by_middle_tree;

class fm6_pyramid2_by_high_tree : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Pyramid 2 by Hifh Tree";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Modulator => 3";
		case 4: case 5: return "Top Modulator F => 4";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (4); accu = wave;
		FM_FEEDBACK (5); wave += accu;
		FM_SINE (3); FM_SINE (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm6_pyramid2_by_high_tree;

class fm6_spear : public algo_cpp {
	virtual char * toString (parameter_block * pb) {return "Spear";}
	virtual char * toString (parameter_block * pb, int ind) {
		switch (ind) {
		case 0: case 1: return "Carrier";
		case 2: return "Modulator => 1, 2";
		case 3: return "Modulator => 3";
		case 4: return "Modulator => 4";
		case 5: return "Top Modulator F => 5";
		default: return "inactive";
		}
	}
	virtual int code (oscillator * osc) {
		FM_DECLARATIONS;
		FM_FEEDBACK (5); FM_SINE (4); FM_SINE (3); FM_SINE (2); float sub = wave;
		FM_SINE (0); accu = wave; wave = sub;
		FM_SINE (1); return (int) (wave + accu + 0.5f);
	}
} the_fm6_spear;

algo_pointer get_fm_2_3_4_algo (int ind, int program) {
	if (ind > 6 && program != 0) ind = 6;
	switch (ind) {
	case 2:
		switch (program) {
		case 0: return & the_fm2_horizontal;
		case 1: return & the_fm2_vertical;
		default: break;
		}
		break;
	case 3:
		switch (program) {
		case 0: return & the_fm3_horizontal;
		case 1: return & the_fm3_half_horizontal;
		case 2: return & the_fm3_tree;
		case 3: return & the_fm3_vertical;
		case 4: return & the_fm3_pyramid;
		default: break;
		}
		break;
	case 4:
		switch (program) {
		case 0: return & the_fm4_horizontal;
		case 1: return & the_fm4_half_horizontal;
		case 2: return & the_fm4_tree_horizontal;
		case 3: return & the_fm4_bottom_tree;
		case 4: return & the_fm4_half_pyramid;
		case 5: return & the_fm4_double_decker;
		case 6: return & the_fm4_tree;
		case 7: return & the_fm4_low_tree;
		case 8: return & the_fm4_high_tree;
		case 9: return & the_fm4_vertical;
		case 10: return & the_fm4_pyramid;
		case 11: return & the_fm4_bottleneck;
		case 12: return & the_fm4_spear;
		default: break;
		}
		break;
	case 5:
		switch (program) {
		case 0: return & the_fm5_horizontal;
		case 1: return & the_fm5_half_horizontal;
		case 2: return & the_fm5_tree_horizontal;
		case 3: return & the_fm5_bottom_tree;
		case 4: return & the_fm5_half_pyramid;
		case 5: return & the_fm5_double_decker;
		case 6: return & the_fm5_tree;
		case 7: return & the_fm5_low_tree;
		case 8: return & the_fm5_high_tree;
		case 9: return & the_fm5_vertical4;
		case 10: return & the_fm5_pyramid3;
		case 11: return & the_fm5_bottleneck2;
		case 12: return & the_fm5_spear2;
		case 13: return & the_fm5_rooted_horizontal;
		case 14: return & the_fm5_rooted_half_horizontal;
		case 15: return & the_fm5_rooted_tree_horizontal;
		case 16: return & the_fm5_rooted_bottom_tree;
		case 17: return & the_fm5_rooted_double_decker;
		case 18: return & the_fm5_rooted_tree;
		case 19: return & the_fm5_rooted_low_tree;
		case 20: return & the_fm5_rooted_high_tree;
		case 21: return & the_fm5_vertical;
		case 22: return & the_fm5_extended_tree;
		case 23: return & the_fm5_extended_decker;
		case 24: return & the_fm5_extended_pyramid;
		case 25: return & the_fm5_pyramid;
		case 26: return & the_fm5_pyramid3_horizontal;
		case 27: return & the_fm5_pyramid3_vertical;
		case 28: return & the_fm5_pyramid2_horizontal;
		case 29: return & the_fm5_pyramid2_half_horizontal;
		case 30: return & the_fm5_pyramid2_tree;
		case 31: return & the_fm5_spear;
		default: break;
		}
		break;
	case 6:
		switch (program) {
		case 0: return & the_fm6_horizontal;
		case 1: return & the_fm6_half_horizontal1;
		case 2: return & the_fm6_tree_horizontal2;
		case 3: return & the_fm6_half_horizontal2;
		case 4: return & the_fm6_half_pyramid2;
		case 5: return & the_fm6_double_decker;
		case 6: return & the_fm6_tree_horizontal3;
		case 7: return & the_fm6_low_tree;
		case 8: return & the_fm6_high_tree;
		case 9: return & the_fm6_half_horizontal4;
		case 10: return & the_fm6_pyramid3;
		case 11: return & the_fm6_bottleneck22_horizontal;
		case 12: return & the_fm6_spear2_horizontal;
		case 13: return & the_fm6_sided_rooted_horizontal;
		case 14: return & the_fm6_sided_rooted_half_horizontal;
		case 15: return & the_fm6_sided_rooted_half_tree;
		case 16: return & the_fm6_sided_low_tree;
		case 17: return & the_fm6_sided_rooted_double_decker;
		case 18: return & the_fm6_sided_rooted_tree;
		case 19: return & the_fm6_sided_middle_low_tree;
		case 20: return & the_fm6_sided_high_tree;
		case 21: return & the_fm6_vertical5;
		case 22: return & the_fm6_tree_half_horizontal;
		case 23: return & the_fm6_decker_3_2_1;
		case 24: return & the_fm6_pyramid_half_horizontal;
		case 25: return & the_fm6_pyramid5;
		case 26: return & the_fm6_bottleneck23_horizontal;
		case 27: return & the_fm6_sided_vertical_spear3;
		case 28: return & the_fm6_bottleneck32_horizontal;
		case 29: return & the_fm6_bottleneck_half_horizontal2;
		case 30: return & the_fm6_bottleneck_tree_horizontal2;
		case 31: return & the_fm6_sided_spear2;
		case 32: return & the_fm6_tree;
		case 33: return & the_fm6_tree_half_horizontal1;
		case 34: return & the_fm6_tree_tree_horizontal2;
		case 35: return & the_fm6_tree_half_horizontal3;
		case 36: return & the_fm6_tree_sided_double_decker;
		case 37: return & the_fm6_tree_sided_tree;
		case 38: return & the_fm6_double_low_tree;
		case 39: return & the_fm6_double_low_high_tree;
		case 40: return & the_fm6_vertical_low_tree;
		case 41: return & the_fm6_rooted_tree;
		case 42: return & the_fm6_rooted_tree_half_horizontal;
		case 43: return & the_fm6_double_high_tree;
		case 44: return & the_fm6_middle_low_tree;
		case 45: return & the_fm6_rooted_tree_double_decker;
		case 46: return & the_fm6_rooted_rooted_tree;
		case 47: return & the_fm6_middle_high_tree;
		case 48: return & the_fm6_vertical_high_tree;
		case 49: return & the_fm6_vertical;
		case 50: return & the_fm6_tree_tree_vertical;
		case 51: return & the_fm6_tree_half_decker;
		case 52: return & the_fm6_triple_decker;
		case 53: return & the_fm6_tree_vertical;
		case 54: return & the_fm6_tree_half_horizontal_vertical;
		case 55: return & the_fm6_top_tree_vertical;
		case 56: return & the_fm6_vertical42;
		case 57: return & the_fm6_vertical33;
		case 58: return & the_fm6_tree_tree;
		case 59: return & the_fm6_pyramid_3_vertical;
		case 60: return & the_fm6_bottleneck22_vertical;
		case 61: return & the_fm6_spear2_vertical;
		case 62: return & the_fm6_tree_vertical_3;
		case 63: return & the_fm6_pyramid_vertical_3;
		case 64: return & the_fm6_pyramid_tree;
		case 65: return & the_fm6_pyramid_pyramid;
		case 66: return & the_fm6_pyramid;
		case 67: return & the_fm6_pyramid4_by_horizontal;
		case 68: return & the_fm6_pyramid4_by_vertical;
		case 69: return & the_fm6_pyramid3_by_horizontal;
		case 70: return & the_fm6_pyramid3_by_half_horizontal;
		case 71: return & the_fm6_pyramid3_by_tree;
		case 72: return & the_fm6_pyramid3_by_vertical;
		case 73: return & the_fm6_pyramid2_by_horizontal;
		case 74: return & the_fm6_pyramid2_by_half_horizontal;
		case 75: return & the_fm6_pyramid2_by_half_tree;
		case 76: return & the_fm6_pyramid2_by_bottom_tree;
		case 77: return & the_fm6_pyramid2_by_double_decker;
		case 78: return & the_fm6_pyramid2_by_tree;
		case 79: return & the_fm6_pyramid2_by_middle_tree;
		case 80: return & the_fm6_pyramid2_by_high_tree;
		case 81: return & the_fm6_spear;
		default: break;
		}
		break;
	default: break;
	}
	return NULL;
}
