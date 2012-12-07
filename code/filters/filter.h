///////////////////////////////////////
// Copyright (C) 2008 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _FILTER_DEFINITIONS_
#define _FILTER_DEFINITIONS_


class filter {
private:
	double sampling_freq;
	double semitone_multiplier;
	double cent_multiplier;
	double cent_base_log;
	double centre_freq;
	double BW;
	double F, R, COS, K;
	double a0, a1, a2, b0, b1, b2;
	double min_level, max_level;
	double last_delta;
	int next_length;
	double high_cents, low_cents, last_cents;
public:
	double in0, in1, in2;
	double out0, out1, out2;
	double amplitude;
	double average_amplitude;
	double frequency;
	double average_frequency;
	double length;
	double average_length;
	double tune;
	double cents;
	double accumulative_deviation;
	double vibrato_amplitude;
	double vibrato_speed;
public:
	void reset (void);
	void move (double sample);
	filter (double sampling_freq, int key, double bandwidth = 0.01); // bandwidth = 2 means one octave
	filter (double sampling_freq, double freq, double bandwidth = 0.01);
	filter (void);
	void configure (double sampling_freq, int key, double bandwidth = 0.01); // bandwidth = 2 means one octave
	void configure (double sampling_freq, double freq, double bandwidth = 0.01);
	static void reconfigure (void);
	void reconfigure (double sampling_freq, int key, double bandwidth = 0.01);
	void reconfigure (double sampling_freq, double freq, double bandwidth = 0.01);
	~ filter (void);
};

typedef filter * filter_pointer;


#endif
