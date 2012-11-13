///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////


#include "data.h"

void parameter_block :: relemat_freq_shift (void) {
	stripe_parameter_block * pb;
	int freq = controllers -> freq;
	for (int ind = 0; ind < hercules_number; ind++) {
		pb = stripes [ind];
		pb -> freq_shift_lemat = pb -> freq_sens . shift . get (freq);
	}
}

void parameter_block :: relemat_amp_shift (void) {
	stripe_parameter_block * pb;
	int amp = controllers -> amp;
	for (int ind = 0; ind < hercules_number; ind++) {
		pb = stripes [ind];
		pb -> amp_shift_lemat = pb -> amp_sens . shift . get_negative (amp);
	}
}

void parameter_block :: relemat_index_shift (void) {
	stripe_parameter_block * pb;
	int index = controllers -> index;
	for (int ind = 0; ind < hercules_number; ind++) {
		pb = stripes [ind];
		pb -> index_shift_lemat = pb -> index_sens . shift . get (index);
	}
}

void parameter_block :: relemat_pan_shift (void) {
	stereo_stripe_parameter_block * pb;
	int pan = controllers -> pan;
	for (int ind = 0; ind < hercules_stereo_number; ind++) {
		pb = stereo [ind];
		pb -> pan_shift_lemat = pb -> sens . shift . get (pan);
	}
}

void parameter_block :: relemat_pitch (void) {
	stripe_parameter_block * sp;
	stereo_stripe_parameter_block * stp;
	int pitch = controllers -> pitch;
	int ind;
	for (ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> freq_pitch_lemat = sp -> freq_sens . pitch . get (pitch) >> 6;
		sp -> amp_pitch_lemat = sp -> amp_sens . pitch . get_negative (pitch);
		sp -> index_pitch_lemat = sp -> index_sens . pitch . get (pitch);
		sp -> lfo_speed_pitch_lemat = sp -> lfo_sens . pitch . get (pitch);
	}
	for (ind = 0; ind < hercules_stereo_number; ind++) {
		stp = stereo [ind];
		stp -> pan_pitch_lemat = stp -> sens . pitch . get (pitch);
		stp -> lfo_speed_pitch_lemat = stp -> lfo_sens . pitch . get (pitch);
	}
}

void parameter_block :: relemat_vector_x (void) {
	stripe_parameter_block * sp;
	stereo_stripe_parameter_block * stp;
	int vector_x = controllers -> vector_x;
	int ind;
	for (ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> freq_vector_x_lemat = sp -> freq_sens . vector_x . get (vector_x);
		sp -> amp_vector_x_lemat = sp -> amp_sens . vector_x . get_negative (vector_x);
		sp -> index_vector_x_lemat = sp -> index_sens . vector_x . get (vector_x);
		sp -> lfo_freq_vector_x_lemat = sp -> freq_sens . lfo_x . get (vector_x) >> 6;
		sp -> lfo_amp_vector_x_lemat = sp -> amp_sens . lfo_x . get (vector_x);
		sp -> lfo_index_vector_x_lemat = sp -> index_sens . lfo_x . get (vector_x);
		sp -> lfo_speed_vector_x_lemat = sp -> lfo_sens . vector_x . get (vector_x);
	}
	for (ind = 0; ind < hercules_stereo_number; ind++) {
		stp = stereo [ind];
		stp -> pan_vector_x_lemat = stp -> sens . vector_x . get (vector_x);
		stp -> lfo_pan_vector_x_lemat = stp -> sens . lfo_x . get (vector_x);
		stp -> lfo_speed_vector_x_lemat = stp -> lfo_sens . vector_x . get (vector_x);
	}
}

void parameter_block :: relemat_vector_y (void) {
	stripe_parameter_block * sp;
	stereo_stripe_parameter_block * stp;
	int vector_y = controllers -> vector_y;
	int ind;
	for (ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> freq_vector_y_lemat = sp -> freq_sens . vector_y . get (vector_y);
		sp -> amp_vector_y_lemat = sp -> amp_sens . vector_y . get_negative (vector_y);
		sp -> index_vector_y_lemat = sp -> index_sens . vector_y . get (vector_y);
		sp -> lfo_freq_vector_y_lemat = sp -> freq_sens . lfo_y . get (vector_y) >> 6;
		sp -> lfo_amp_vector_y_lemat = sp -> amp_sens . lfo_y . get (vector_y);
		sp -> lfo_index_vector_y_lemat = sp -> index_sens . lfo_y . get (vector_y);
		sp -> lfo_speed_vector_y_lemat = sp -> lfo_sens . vector_y . get (vector_y);
	}
	for (ind = 0; ind < hercules_stereo_number; ind++) {
		stp = stereo [ind];
		stp -> pan_vector_y_lemat = stp -> sens . vector_y . get (vector_y);
		stp -> lfo_pan_vector_y_lemat = stp -> sens . lfo_y . get (vector_y);
		stp -> lfo_speed_vector_y_lemat = stp -> sens . vector_y . get (vector_y);
	}
}

void parameter_block :: relemat_vibrato (void) {
	stripe_parameter_block * sp;
	int vibrato = controllers -> vibrato;
	for (int ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> lfo_freq_lfo_lemat = sp -> freq_sens . lfo . get (vibrato) >> 6;
	}
}

void parameter_block :: relemat_tremolo (void) {
	stripe_parameter_block * sp;
	int tremolo = controllers -> tremolo;
	for (int ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> lfo_amp_lfo_lemat = sp -> amp_sens . lfo . get (tremolo);
	}
}

void parameter_block :: relemat_lfo_index (void) {
	stripe_parameter_block * sp;
	int lfo_index = controllers -> lfo_index;
	for (int ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> lfo_index_lfo_lemat = sp -> index_sens . lfo . get (lfo_index);
	}
}

void parameter_block :: relemat_lfo_pan (void) {
	stereo_stripe_parameter_block * sp;
	int lfo_pan = controllers -> lfo_pan;
	for (int ind = 0; ind < hercules_stereo_number; ind++) {
		sp = stereo [ind];
		sp -> lfo_pan_lfo_lemat = sp -> sens . lfo . get (lfo_pan);
	}
}

void parameter_block :: relemat_lfo_speed (void) {
	stripe_parameter_block * sp;
	stereo_stripe_parameter_block * stp;
	int speed = controllers -> speed;
	int ind;
	for (ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> lfo_speed_speed_lemat = sp -> lfo_sens . speed . get (speed);
	}
	for (ind = 0; ind < hercules_stereo_number; ind++) {
		stp = stereo [ind];
		stp -> lfo_speed_speed_lemat = stp -> lfo_sens . speed . get (speed);
	}
}

void parameter_block :: add_freq_lemat (void) {
	stripe_parameter_block * sp;
	for (int ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> freq_lemat = sp -> freq + sp -> freq_pitch_lemat + sp -> freq_shift_lemat + sp -> freq_vector_x_lemat + sp -> freq_vector_y_lemat;
	}
}

void parameter_block :: add_amp_lemat (void) {
	stripe_parameter_block * sp;
	for (int ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> amp_lemat = sp -> amp + sp -> amp_pitch_lemat + sp -> amp_shift_lemat + sp -> amp_vector_x_lemat + sp -> amp_vector_y_lemat;
	}
}

void parameter_block :: add_index_lemat (void) {
	stripe_parameter_block * sp;
	for (int ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> index_lemat = sp -> index + sp -> index_pitch_lemat + sp -> index_shift_lemat + sp -> index_vector_x_lemat + sp -> index_vector_y_lemat;
	}
}

void parameter_block :: add_pan_lemat (void) {
	stereo_stripe_parameter_block * sp;
	for (int ind = 0; ind < hercules_stereo_number; ind++) {
		sp = stereo [ind];
		sp -> pan_lemat = sp -> pan + sp -> pan_pitch_lemat + sp -> pan_shift_lemat + sp -> pan_vector_x_lemat + sp -> pan_vector_y_lemat;
	}
}

void parameter_block :: add_all_shift_lemat (void) {
	stripe_parameter_block * sp;
	stereo_stripe_parameter_block * stp;
	int ind;
	for (ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> freq_lemat = sp -> freq + sp -> freq_pitch_lemat + sp -> freq_shift_lemat + sp -> freq_vector_x_lemat + sp -> freq_vector_y_lemat;
		sp -> amp_lemat = sp -> amp + sp -> amp_pitch_lemat + sp -> amp_shift_lemat + sp -> amp_vector_x_lemat + sp -> amp_vector_y_lemat;
		sp -> index_lemat = sp -> index + sp -> index_pitch_lemat + sp -> index_shift_lemat + sp -> index_vector_x_lemat + sp -> index_vector_y_lemat;
		sp -> lfo_speed_lemat = sp -> speed + sp -> lfo_speed_pitch_lemat + sp -> lfo_speed_speed_lemat + sp -> lfo_speed_vector_x_lemat + sp -> lfo_speed_vector_y_lemat;
	}
	for (ind = 0; ind < hercules_stereo_number; ind++) {
		stp = stereo [ind];
		stp -> pan_lemat = stp -> pan + stp -> pan_pitch_lemat + stp -> pan_shift_lemat + stp -> pan_vector_x_lemat + stp -> pan_vector_y_lemat;
		stp -> lfo_speed_lemat = stp -> speed + stp -> lfo_speed_pitch_lemat + stp -> lfo_speed_speed_lemat + stp -> lfo_speed_vector_x_lemat + stp -> lfo_speed_vector_y_lemat;
	}
}

void parameter_block :: add_lfo_speed_lemat (void) {
	stripe_parameter_block * sp;
	stereo_stripe_parameter_block * stp;
	int ind;
	for (ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> lfo_speed_lemat = sp -> speed + sp -> lfo_speed_pitch_lemat + sp -> lfo_speed_speed_lemat + sp -> lfo_speed_vector_x_lemat + sp -> lfo_speed_vector_y_lemat;
	}
	for (ind = 0; ind < hercules_stereo_number; ind++) {
		stp = stereo [ind];
		stp -> lfo_speed_lemat = stp -> speed + stp -> lfo_speed_pitch_lemat + stp -> lfo_speed_speed_lemat + stp -> lfo_speed_vector_x_lemat + stp -> lfo_speed_vector_y_lemat;
	}
}

void parameter_block :: add_lfo_freq_lemat (void) {
	stripe_parameter_block * sp;
	for (int ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> lfo_freq_lemat = sp -> lfo_freq + sp -> lfo_freq_lfo_lemat + sp -> lfo_freq_vector_x_lemat + sp -> lfo_freq_vector_y_lemat;
	}
}

void parameter_block :: add_lfo_amp_lemat (void) {
	stripe_parameter_block * sp;
	for (int ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> lfo_amp_lemat = sp -> lfo_amp + sp -> lfo_amp_lfo_lemat + sp -> lfo_amp_vector_x_lemat + sp -> lfo_amp_vector_y_lemat;
	}
}

void parameter_block :: add_lfo_index_lemat (void) {
	stripe_parameter_block * sp;
	for (int ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> lfo_index_lemat = sp -> lfo_index + sp -> lfo_index_lfo_lemat + sp -> lfo_index_vector_x_lemat + sp -> lfo_index_vector_y_lemat;
	}
}

void parameter_block :: add_lfo_pan_lemat (void) {
	stereo_stripe_parameter_block * sp;
	for (int ind = 0; ind < hercules_stereo_number; ind++) {
		sp = stereo [ind];
		sp -> lfo_pan_lemat = sp -> lfo + sp -> lfo_pan_lfo_lemat + sp -> lfo_pan_vector_x_lemat + sp -> lfo_pan_vector_y_lemat;
	}
}

void parameter_block :: add_lfo_all_lemat (void) {
	stripe_parameter_block * sp;
	stereo_stripe_parameter_block * stp;
	int ind;
	for (ind = 0; ind < hercules_number; ind++) {
		sp = stripes [ind];
		sp -> lfo_freq_lemat = sp -> lfo_freq + sp -> lfo_freq_lfo_lemat + sp -> lfo_freq_vector_x_lemat + sp -> lfo_freq_vector_y_lemat;
		sp -> lfo_amp_lemat = sp -> lfo_amp + sp -> lfo_amp_lfo_lemat + sp -> lfo_amp_vector_x_lemat + sp -> lfo_amp_vector_y_lemat;
		sp -> lfo_index_lemat = sp -> lfo_index + sp -> lfo_index_lfo_lemat + sp -> lfo_index_vector_x_lemat + sp -> lfo_index_vector_y_lemat;
	}
	for (ind = 0; ind < hercules_stereo_number; ind++) {
		stp = stereo [ind];
		stp -> lfo_pan_lemat = stp -> lfo + stp -> lfo_pan_lfo_lemat + stp -> lfo_pan_vector_x_lemat + stp -> lfo_pan_vector_y_lemat;
	}
}

void parameter_block :: relemat_all (void) {
	relemat_freq_shift ();
	relemat_amp_shift ();
	relemat_index_shift ();
	relemat_pan_shift ();

	relemat_pitch ();
	relemat_vector_x ();
	relemat_vector_y ();

	relemat_lfo_speed ();
	relemat_vibrato ();
	relemat_tremolo ();
	relemat_lfo_index ();
	relemat_lfo_pan ();

	add_freq_lemat ();
	add_amp_lemat ();
	add_index_lemat ();
	add_pan_lemat ();

	add_lfo_speed_lemat ();
	add_lfo_freq_lemat ();
	add_lfo_amp_lemat ();
	add_lfo_index_lemat ();
	add_lfo_pan_lemat ();
}
