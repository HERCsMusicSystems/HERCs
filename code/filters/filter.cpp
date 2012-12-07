///////////////////////////////////////
// Copyright (C) 2008 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "filter.h"

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

static double compensation_table [100];
static bool compensation_table_not_ready = true;
double compensation (int cents) {
	if (compensation_table_not_ready) return 1.0;
	if (cents == 0.0) return 1.0;
	if (cents <= -50) return 0.0;
	if (cents >= 50) return 0.0;
	if (cents > 0) return compensation_table [50 + cents];
	return compensation_table [- cents];
}

filter :: filter (double sampling_freq, int key, double bandwidth) {
	reset ();
	configure (sampling_freq, key, bandwidth);
}

filter :: filter (double sampling_freq, double freq, double bandwidth) {
	reset ();
	configure (sampling_freq, freq, bandwidth);
}

void filter :: reconfigure (double sampling_freq, int key, double bandwidth) {
	reset ();
	configure (sampling_freq, key, bandwidth);
}

void filter :: reconfigure (double sampling_freq, double freq, double bandwidth) {
	reset ();
	configure (sampling_freq, freq, bandwidth);
}

void filter :: reconfigure (void) {compensation_table_not_ready = true;}

filter :: filter (void) {reset ();}

void filter :: reset (void) {
	semitone_multiplier = pow (2.0, 1.0 / 12.0);
	cent_multiplier = pow (2.0, 1.0 / 1200.0);
	cent_base_log = log (cent_multiplier);
	in0 = in1 = in2 = 0.0;
	out0 = out1 = out2 = 0.0;
	min_level = 0.0; max_level = 0.0;
	next_length = 0;
	last_delta = 0.5;
	amplitude = 0.0;
	average_amplitude = 0.0;
	length = 0.0;
	average_length = 0.0;
	frequency = 0.0;
	average_frequency = 0.0;
	tune = 0.0;
	cents = 0.0;
	last_cents = low_cents = high_cents = 0.0;
	accumulative_deviation = 0.0;
	vibrato_amplitude = 0.0;
	vibrato_speed = 0.0;
	if (! compensation_table_not_ready) return;
	compensation_table_not_ready = false;
	filter fx;
	double max;
	double freq = 1.0;
	double length = 32.0;
	double cent = pow (2.0, 1 / 1200.0);
	for (int index = 0; index < 100; index++) compensation_table [index] = 1.0;
	for (int index = 0; index < 50; index++) {
		max = 0.0;
		fx . reset ();
		for (int ind = 0; ind < 8192; ind++) {
			fx . move (sin (2.0 * M_PI * freq * (double) ind / 64.0));
			if (fx . amplitude > max) max = fx . amplitude;
		}
		compensation_table [50 + index] = max / 2.0;
//		sprintf (command, "amplitude [%i : %.6f : %.6f]", index, freq, max);
//		MessageBox (GetActiveWindow (), command, "INFO", MB_OK);
		freq *= cent;
	}
	freq = 1.0;
	for (int index = 0; index < 50; index++) {
		max = 0.0;
		fx . reset ();
		for (int ind = 0; ind < 8192; ind++) {
			fx . move (sin (2.0 * M_PI * freq * (double) ind / 64.0));
			if (fx . amplitude > max) max = fx . amplitude;
		}
		compensation_table [index] = max / 2.0;
//		sprintf (command, "amplitude [%i : %.6f : %.6f]", index, freq, max);
//		MessageBox (GetActiveWindow (), command, "INFO", MB_OK);
		freq /= cent;
	}
}

void filter :: configure (double sampling_freq, int key, double bandwidth) {
	centre_freq = 440.0 * pow (2.0, ((double) (key - 69)) / 12.0);
	this -> sampling_freq = sampling_freq;
	bandwidth = pow (2.0, bandwidth);
	BW = centre_freq * bandwidth - centre_freq;
	BW /= sampling_freq;
	F = centre_freq / sampling_freq;
	R = 1.0 - 3.0 * BW;
	COS = cos (2.0 * M_PI * F);
	K = (1.0 - 2.0 * R * COS + R * R) / (2.0 - 2.0 * COS);
	a0 = 1.0 - K;
	a1 = 2.0 * (K - R) * COS;
	a2 = R * R - K;
	b1 = 2.0 * R * COS;
	b2 = - (R * R);
}

void filter :: configure (double sampling_freq, double freq, double bandwidth) {
	centre_freq = freq;
	this -> sampling_freq = sampling_freq;
	bandwidth = pow (2.0, bandwidth);
	BW = centre_freq * bandwidth - centre_freq;
	BW /= sampling_freq;
	F = centre_freq / sampling_freq;
	R = 1.0 - 3.0 * BW;
	COS = cos (2.0 * M_PI * F);
	K = (1.0 - 2.0 * R * COS + R * R) / (2.0 - 2.0 * COS);
	a0 = 1.0 - K;
	a1 = 2.0 * (K - R) * COS;
	a2 = R * R - K;
	b1 = 2.0 * R * COS;
	b2 = - (R * R);
}

filter :: ~ filter (void) {}

void filter :: move (double sample) {
	// filter
	in2 = in1; in1 = in0;
	out2 = out1; out1 = out0;
	in0 = sample;
	out0 = in0 * a0 + in1 * a1 + in2 * a2 + out1 * b1 + out2 * b2;
	// amplitude
	if (out0 > max_level) max_level = out0;
	if (out0 < min_level) min_level = out0;
	// zero
	next_length++;
	if (out1 <= 0.0 && out0 > 0.0) {
		double delta = out0 / (out0 - out1);
		length = (double) next_length;
		length -= delta;
		length += last_delta;
		next_length = 0;
		last_delta = delta;
		average_length = average_length * 3.0 + length;
		average_length /= 4.0;
		frequency = length != 0.0 ? sampling_freq / length : 0.0;
		if (average_length != 0.0) {
			average_frequency = sampling_freq / average_length;
			tune = average_frequency / centre_freq;
		} else {
			average_frequency = 0.0;
			tune = 0.0;
		}
		amplitude = (max_level - min_level);
		cents = log (frequency / centre_freq) / cent_base_log;
//		differential = cents - last_cents;
		accumulative_deviation += cents;
		last_cents = cents;
		double comp = compensation ((int) cents);
		//cents = compensation ((int) cents);
//		amplitude *= compensation ((int) cents);
		comp = 1.0;
		if (comp != 0.0) amplitude /= comp;
		average_amplitude = average_amplitude * 3.0 + amplitude;
		average_amplitude /= 4.0;
		max_level = 0.0;
		min_level = 0.0;
	}
}
