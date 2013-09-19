///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "transport.h"
#include "math.h"

void transport :: tempo (void) {
	int dx = horizontal * seconds;
	int dy = beats_per_seconds * 24;//ticks_per_beat;
	d2 = dy + dy;
	d0 = d2 - dx;
	int dd1 = d0 - dx;
	if (d1 == 0) d1 = dd1;
	d = (int) ((double) d * (double) dd1 / (double) d1);
	d1 = dd1;
}

void transport :: reset (void) {
	tick = 0; beat = 0; bar = 0;
	sub_tick = 0; sub_beat = 0;
	wait_type = 0; wait_sentinel = 0;
	d = d0;
}

void transport :: tempo (int beats_per_minute) {
	this -> beats_per_seconds = beats_per_minute;
	this -> seconds = 60;
	tempo ();
}

void transport :: tempo (int beats_per_seconds, int seconds) {
	this -> beats_per_seconds = beats_per_seconds;
	this -> seconds = seconds;
	tempo ();
}

void transport :: sub_division (int ticks_per_beat) {
	sub_tick += ticks_per_beat - this -> ticks_per_beat;
	this -> ticks_per_beat = ticks_per_beat;
}

void transport :: division (int beats_per_bar) {
	sub_beat += beats_per_bar - this -> beats_per_bar;
	this -> beats_per_bar = beats_per_bar;
}

void transport :: division (int beats_per_bar, int ticks_per_beat) {
	sub_tick += ticks_per_beat - this -> ticks_per_beat;
	this -> ticks_per_beat = ticks_per_beat;
	sub_beat += beats_per_bar - this -> beats_per_bar;
	this -> beats_per_bar = beats_per_bar;
}

void transport :: metrum (int top, int bottom) {division (top, 96 / bottom);}

void transport :: wt (void) {wait_type = 1; wait_sentinel = tick + 1;}
void transport :: wt (int ind) {wait_type = 1; wait_sentinel = tick + ind;}
void transport :: wait_beat (void) {wait_type = 2; wait_sentinel = beat + 1;}
void transport :: wait_beat (int ind) {wait_type = 2; wait_sentinel = beat + ind;}
void transport :: wait_bar (void) {wait_type = 3; wait_sentinel = bar + 1;}
void transport :: wait_bar (int ind) {wait_type = 3; wait_sentinel = bar + ind;}
void transport :: schedule_tick_code (void) {wait_type = 4;}
void transport :: start (void) {inactive = false;}
void transport :: pause (void) {wait_type = 0;}
void transport :: stop (void) {inactive = true; pause ();}

void transport :: signal (void) {
	tick++;
	if (--sub_tick <= 0) {
		sub_tick = ticks_per_beat;
		beat++;
		if (--sub_beat <= 0) {
			sub_beat = beats_per_bar;
			bar++;
		}
	}
	if (inactive) return;
	switch (wait_type) {
	case 1: if (wait_sentinel > tick) break; wait_type = 0; tick_code (); break;
	case 2: if (wait_sentinel > beat) break; wait_type = 0; beat_code (); break;
	case 3: if (wait_sentinel > bar) break; wait_type = 0; bar_code (); break;
	case 4: wait_type = 0; tick_code (); break;
	default: break;
	}
}

void transport :: move (void) {
	// d1 = 2 * dy - 2 * dx
	// d2 = 2 * dy
	if (d >= 0) {
		d += d1;
		signal ();
	} else d += d2;
}

void transport :: tick_code (void) {}
void transport :: beat_code (void) {}
void transport :: bar_code (void) {}

transport :: transport (int horizontal) {
	this -> horizontal = horizontal;
	reset ();
	d = 0;
	d1 = 0;
	division (4, 24);
	tempo (140);
	stop ();
	d = d0;
}

char * pattern :: styleName (parameter_block * pb) {return "SILENT STYLE";}
char * pattern :: toString (parameter_block * pb) {return "SILENT PATTERN";}
int pattern :: note_length (arranger * arp) {return (arp -> ticks_per_beat + 1) >> 1;}
void pattern :: on_retrigger (arranger * arp) {
	arp -> status = 0;
	arp -> sub_status = 0;
	arp -> octave = 0;
	arp -> counter = 0;
	arp -> reset ();
	arp -> wait_bar ();
}
void pattern :: on_hold_retrigger (arranger * arp) {}
void pattern :: on_start (arranger * arp) {}
void pattern :: on_pause (arranger * arp) {}
void pattern :: on_stop (arranger * arp) {}
void pattern :: tick_code (arranger * arp) {}
void pattern :: beat_code (arranger * arp) {}
void pattern :: bar_code (arranger * arp) {}
void pattern :: variation_code (arranger * arp) {arp -> update_variation ();}
void pattern :: transcription_code (arranger * arp) {arp -> update_transcriptions ();}
bool pattern :: no_continuous_play (void) {return true;}
int pattern :: get_division (void) {return 0;}
int pattern :: get_subdivision (void) {return 0;}
int pattern :: get_tempo (void) {return -1;}
//bool pattern :: transcriptions_buffered (void) {return true;}
//bool pattern :: variation_buffered (void) {return true;}
//bool pattern :: auto_keyoff (void) {return true;}

pattern silence_pattern;

void arranger :: set_variation (int variation) {
	private_variation = variation;
	if (head) head -> variation_code (this);
//	if (variation_buffered) return;
//	this -> variation = variation;
}
void arranger :: set_transcription (int key) {
	if (key == 0) {reset_transcriptions (); return;}
	private_base_note = key;
	transcriptions_ready = true;
	if (head) head -> transcription_code (this);
//	if (transcriptions_buffered) return;
//	update_transcriptions ();
}
void arranger :: set_transcription (int index, int key) {
	if (index < 0 || index > 63) return;
	private_transcriptions [index] = key;
}
void arranger :: reset_transcriptions (void) {
	private_base_note = 0;
	for (int ind = 0; ind < 64; ind++) private_transcriptions [ind] = ind;
	transcriptions_ready = true;
	if (head) head -> transcription_code (this);
//	if (transcriptions_buffered) return;
//	update_transcriptions ();
}
void arranger :: update_variation (void) {variation = private_variation;}
void arranger :: update_transcriptions (void) {
	if (transcriptions_ready) {
		base_note = private_base_note;
		for (int ind = 0; ind < 64; ind++)
			transcriptions [ind] = private_transcriptions [ind];
	}
	transcriptions_ready = false;
}
void arranger :: update (void) {
	update_variation ();
	update_transcriptions ();
}

void arranger :: tick_code (void) {pb -> pattern_head -> tick_code (this);}
void arranger :: beat_code (void) {pb -> pattern_head -> beat_code (this);}
void arranger :: bar_code (void) {pb -> pattern_head -> bar_code (this);}

void arranger :: key_on (int key, int velocity) {
	if (velocity != 0) pb -> key_code (key, velocity);
	else pb -> key_code (key);
}
void arranger :: key_off (int key) {pb -> key_code (key);}
void arranger :: key_off (void) {pb -> key_code ();}
int arranger :: top_key_index (void) {return 0;}
int arranger :: get_hit_key (int counter) {return 0;}
int arranger :: get_velocity (int counter) {return 0;}
bool arranger :: hit (int counter, int octave, int velocity) {
	int key = get_hit_key (counter);
	if (key < 0) return false;
	key += 12 * octave;
	if (key > 127) return false;
	//pb -> key_code (key, velocity);
	key_on (key, velocity);
	return true;
}

bool arranger :: hit (int counter, int octave) {
	int key = get_hit_key (counter);
	if (key < 0) return false;
	key += 12 * octave;
	if (key > 127) return false;
	pb -> key_code (key, get_velocity (counter));
	return true;
}

void arranger :: hit_top (int counter, int octave, int velocity) {
	int index = top_key_index ();
	if (counter > index) counter = index;
	int key = get_hit_key (counter);
	if (key < 0) return;
	key += 12 * octave;
	if (key > 127) return;
	//pb -> key_code (key, velocity);
	key_on (key, velocity);
}

void arranger :: hit_top (int counter, int octave) {
	int index = top_key_index ();
	if (counter > index) counter = index;
	int key = get_hit_key (counter);
	if (key < 0) return;
	key += 12 * octave;
	if (key > 127) return;
	//pb -> key_code (key, get_velocity (counter));
	key_on (key, get_velocity (counter));
}

void arranger :: hit_bottom (int counter, int octave, int velocity) {
	int index = top_key_index ();
	if (counter > index) counter = 0;
	int key = get_hit_key (counter);
	if (key < 0) return;
	key += 12 * octave;
	if (key > 127) return;
	key_on (key, velocity);
}

void arranger :: hit_bottom (int counter, int octave) {
	int index = top_key_index ();
	if (counter > index) counter = 0;
	int key = get_hit_key (counter);
	if (key < 0) return;
	key += 12 * octave;
	if (key > 127) return;
	key_on (key, get_velocity (counter));
}

void arranger :: arrange (int key, int octave, int velocity) {
	if (base_note == 0) return;
	if (key < 0 || key > 63) return;
	key = base_note + transcriptions [key] + octave * 12;
	if (key < 0) key = 0;
	if (key > 127) key = 127;
	key_on (key, velocity);
	//if (velocity != 0) pb -> key_code (key, velocity);
	//else pb -> key_code (key);
}

void arranger :: relative (int key, int velocity) {
	if (base_note == 0) return;
	key += base_note;
	if (key < 0) key = 0;
	if (key > 127) key = 127;
	key_on (key, velocity);
//	if (velocity != 0) pb -> key_code (key, velocity);
//	else pb -> key_code (key);
}

void arranger :: notify_key_on (int key, int velocity) {}
void arranger :: notify_key_off (int key) {}
void arranger :: notify_key_off (void) {}
void arranger :: notify_tempo (int beats_per_seconds, int seconds) {tempo (beats_per_seconds, seconds);}
void arranger :: notify_division (int beats_per_bar) {division (beats_per_bar);}
void arranger :: notify_division (int beats_per_bar, int ticks_per_beat) {
	division (beats_per_bar, ticks_per_beat);
//	key_off_ticks = pb -> pattern_head -> note_length (this);
}
void arranger :: notify_sub_division (int ticks_per_beat) {
	sub_division (ticks_per_beat);
//	key_off_ticks = pb -> pattern_head -> note_length (this);
}
void arranger :: notify_signal (void) {signal ();}
void arranger :: notify_switch (bool on) {}
void arranger :: notify_switch_loaded (bool on) {}
void arranger :: notify_hold (bool hold) {}
void arranger :: notify_variation (int variation) {}
void arranger :: notify_pattern_change (void) {head = pb -> pattern_head;}
void arranger :: notify_program_loaded (void) {
	controllers_parameter_block * cb = pb -> controllers;
	notify_division (cb -> beats_per_bar, cb -> ticks_per_beat);
	notify_tempo (cb -> beats_per_seconds, cb -> seconds);
	notify_pattern_change ();
	notify_hold (cb -> arpeggiator_hold);
	notify_switch_loaded (cb -> arpeggiator_switch);
	set_variation (cb -> variation);
	update_variation ();
}
arranger :: arranger (parameter_block * pb, int horizontal) : transport (horizontal) {
	this -> pb = pb;
	head = & silence_pattern;
	set_variation (0);
	reset_transcriptions ();
	update ();
}

arranger_pointer arpeggiator_pool :: get_arpeggiator (int ind) {return NULL;}
pattern_pointer arpeggiator_pool :: get_default_pattern (void) {return & silence_pattern;}
void arpeggiator_pool :: move (void) {}
arpeggiator_pool :: ~ arpeggiator_pool (void) {}

void arpeggio_pattern :: wait_beat (arranger * arp) {arp -> sub_status = 1;}
void arpeggio_pattern :: wait_beat (arranger * arp, int ind) {arp -> sub_status = ind;}
void arpeggio_pattern :: tick_code (arranger * arp) {
	arp -> key_off ();
	arp -> wait_beat (arp -> sub_status);
}

void arpeggio_pattern :: beat_code (arranger * arp) {
	arp -> wt (note_length (arp));
	wait_beat (arp);
	if (arp -> top_key_index () < 0 && no_continuous_play ()) return;
	code (arp);
}

void arpeggio_pattern :: bar_code (arranger * arp) {beat_code (arp);}

void arpeggio_pattern :: code (arranger * arp) {}

int arpeggiator_cpp :: top_key_index (void) {return arp_key_top - 1;}
int arpeggiator_cpp :: get_hit_key (int counter) {
	if (counter >= arp_key_top || counter < 0) return -1;
	int key = arp_keys [counter] + octave * 12;
	if (key > 127) key = 127;
	if (key < 0) key = 0;
	return key;
}
int arpeggiator_cpp :: get_velocity (int counter) {return arp_velocities [counter];}

void arpeggiator_cpp :: notify_key_on (int key, int velocity) {
	int i_to = 0;
	int i_from = 0;
	while (i_from < arp_key_top) {
		if (arp_hold [i_from]) {
			arp_keys [i_to] = arp_keys [i_from];
			arp_velocities [i_to] = arp_velocities [i_from];
			arp_hold [i_to++] = arp_hold [i_from];
		}
		i_from++;
	}
	arp_key_top = i_to;
	if (arp_key_top < arp_max_keys) arp_key_top++;
	i_to = arp_key_top - 1;
	i_from = i_to - 1;
	while (i_to > 0) {
		if (arp_keys [i_from] <= key) break;
		arp_velocities [i_to] = arp_velocities [i_from];
		arp_keys [i_to] = arp_keys [i_from];
		arp_hold [i_to--] = arp_hold [i_from--];
	}
	arp_keys [i_to] = key;
	arp_velocities [i_to] = velocity;
	arp_hold [i_to] = true;
	if (arp_key_top == 1) {
//		reset ();
//		wait_bar ();
		if (hold) pb -> pattern_head -> on_hold_retrigger (this);
		else pb -> pattern_head -> on_retrigger (this);
	}
}
void arpeggiator_cpp :: notify_key_off (int key) {
	if (arp_key_top < 1) return;
	int ind = arp_key_top - 1;
	while (ind >= 0 && arp_keys [ind] != key) ind--;
	if (ind < 0) return;
	if (hold) {arp_hold [ind] = false; return;}
	arp_key_top--;
	int i_from = ind + 1;
	while (ind < arp_key_top) {
		arp_keys [ind] = arp_keys [i_from];
		arp_velocities [ind] = arp_velocities [i_from];
		arp_hold [ind++] = arp_hold [i_from++];
	}
}
void arpeggiator_cpp :: notify_key_off (void) {
	if (hold) {
		for (int ind = 0; ind < arp_key_top; ind++) arp_hold [ind] = false;
		return;
	}
	arp_key_top = 0;
}
void arpeggiator_cpp :: notify_switch (bool on) {
	arp_key_top = 0;
	pb -> key_code ();
	if (on) {reset (); start (); wait_bar ();}
	else stop ();
}
void arpeggiator_cpp :: notify_switch_loaded (bool on) {
	arp_key_top = 0;
	if (on) {reset (); start (); wait_bar ();}
	else stop ();
}
void arpeggiator_cpp :: notify_hold (bool hold) {this -> hold = hold;}
void arpeggiator_cpp :: notify_variation (int variation) {set_variation (variation);}

arpeggiator_cpp :: arpeggiator_cpp (parameter_block * pb, int horizontal, int keys)
: arranger (pb, horizontal) {
	arp_keys = new int [keys];
	arp_velocities = new int [keys];
	arp_hold = new bool [keys];
	arp_key_top = 0;
	arp_max_keys = keys;
	hold = false;
}

arpeggiator_cpp :: ~ arpeggiator_cpp (void) {
	delete [] arp_keys;
	delete [] arp_velocities;
	delete [] arp_hold;
}

arranger_pointer arpeggiator_pool_cpp :: get_arpeggiator (int ind) {
	if (ind < 0 || ind >= sth -> multitimbral) return NULL;
	return arps [ind];
}

void arpeggiator_pool_cpp :: move (void) {
	for (int ind = 0; ind < multitimbral; ind++) arps [ind] -> move ();
}

arpeggiator_pool_cpp :: arpeggiator_pool_cpp (config * cfg, synthesiser * sth) {
	this -> sth = sth;
	multitimbral = cfg -> multitimbral;
	arps = new arpeggiator_cpp_pointer [multitimbral];
	int horizontal = cfg -> horizontal;
	if (cfg -> arranger_horizontal > 0) horizontal = cfg -> arranger_horizontal; //horizontal /= cfg -> arranger_samples;
	for (int ind = 0; ind < multitimbral; ind++)
		arps [ind] = new arpeggiator_cpp (sth -> data_blocks [ind], horizontal, cfg -> arranger_keys);
	sth -> connect_arpeggiator_pool (this);
}
arpeggiator_pool_cpp :: ~ arpeggiator_pool_cpp (void) {
	for (int ind = 0; ind < multitimbral; ind++)
		delete arps [ind];
	delete [] arps;
}

////////////
// VECTOR //
////////////

void vector :: notify_key_on (void) {}
void vector :: notify_key_off (void) {}
void vector :: notify_all_keys_off (void) {}
void vector :: notify_control_vector_x (int x) {}
void vector :: notify_control_vector_y (int y) {}
vector :: vector (int horizontal) : transport (horizontal) {}

vector * vector_pool :: get_vector (int ind) {return NULL;}
void vector_pool :: move (void) {}
vector_pool :: ~ vector_pool (void) {}

vector_pointer vector_pool_cpp :: get_vector (int ind) {
	if (ind < 0 || ind >= sth -> multitimbral) return NULL;
	return vects [ind];
}

void vector_pool_cpp :: move (void) {
	for (int ind = 0; ind < multitimbral; ind++) vects [ind] -> move ();
}

vector_pool_cpp :: vector_pool_cpp (config * cfg, synthesiser * sth) {
	this -> sth = sth;
	multitimbral = cfg -> multitimbral;
	vects = new vector_cpp_pointer [multitimbral];
	int horizontal = cfg -> horizontal;
	if (cfg -> vector_horizontal > 0) horizontal = cfg -> vector_horizontal;
	for (int ind = 0; ind < multitimbral; ind++)
		vects [ind] = new vector_cpp (sth -> data_blocks [ind], horizontal);
	sth -> connect_vector_pool (this);
}

vector_pool_cpp :: ~ vector_pool_cpp (void) {
	for (int ind = 0; ind < multitimbral; ind++)
		delete vects [ind];
	delete [] vects;
}

void vector_cpp :: start_recording (void) {
	cb -> vector_init_x = cb -> vector_x;
	cb -> vector_init_y = cb -> vector_y;
	cb -> vector_frame_count = 0;
	frame_counter = 0;
	recording = true;
	attacking = false;
	releasing = false;
	looping = false;
	loop_target = 0;
	rounding = false;
	frame_x_recorded = false;
	frame_y_recorded = false;
	tempo (cb -> vector_resolution << 7, 24 * cb -> vector_resolution_seconds);
	reset ();
	start ();
	wt (1);
}
void vector_cpp :: start_playback (void) {
	recording = false;
	releasing = false;
	looping = false;
	loop_target = 0;
	rounding = false;
	tempo (cb -> vector_resolution << 7, 24 * cb -> vector_resolution_seconds);
	reset ();
	start ();
	slope_time = cb -> vector_attack;
	if (slope_time != 0) slope_time = (int) ((double) cb -> vector_resolution * pow (2.0, (double) (slope_time - 8192) / (6.0 * (double) cb -> vector_resolution_seconds)));
	if (slope_time == 0) {attack_phase_finished (); return;}
	attacking = true;
	slope_time_counter = 0;
	start_x = cb -> vector_x;
	start_y = cb -> vector_y;
	wt (1);
}
void vector_cpp :: attack_phase_finished (void) {
	pb -> control_vector (cb -> vector_init_x, cb -> vector_init_y);
	frame_counter = 0;
	attacking = false;
	wt (pb -> vector_frames [0] . delay);
}
void vector_cpp :: start_loop (void) {
	slope_time = cb -> vector_loop_delay;
	if (slope_time != 0) slope_time = (int) ((double) cb -> vector_resolution * pow (2.0, (double) (slope_time - 8192) / (6.0 * (double) cb -> vector_resolution_seconds)));
	if (slope_time == 0) {loop_phase_finished (); return;}
	looping = true;
	loop_target = cb -> vector_loop_start;
	if (loop_target < 0 && loop_target >= cb -> vector_frame_count) loop_target = 0;
	slope_time_counter = 0;
	start_x = cb -> vector_x;
	start_y = cb -> vector_y;
	wt (1);
}
void vector_cpp :: loop_phase_finished (void) {
	int index = cb -> vector_loop_start;
	if (index < 0 || index >= cb -> vector_frame_count || index >= pb -> vector_size) return;
	pb -> control_vector (pb -> vector_frames [index] . x, pb -> vector_frames [index] . y);
	frame_counter = index;
	looping = false;
	if (++frame_counter < cb -> vector_frame_count) wt (pb -> vector_frames [frame_counter] . delay);
}
void vector_cpp :: notify_key_on (void) {
	key_counter++;
	switch (cb -> vector_trigger) {
	case 0: if (cb -> vector_switch && cb -> vector_frame_count > 0 && key_counter == 1) start_playback (); return;
	case 1: if (cb -> vector_switch && cb -> vector_frame_count > 0) start_playback (); return;
	default: start_recording (); return;
	}
}
void vector_cpp :: notify_key_off (void) {
	if (--key_counter > 0) return;
	notify_all_keys_off ();
}
void vector_cpp :: notify_all_keys_off (void) {
	cb -> vector_trigger &= 1;
	key_counter = 0;
	if (recording) pb -> send_vector_recorded_info ();
	recording = false;
	slope_time = cb -> vector_release;
	if (slope_time == 16383) {stop (); return;}
	slope_time = (int) ((double) cb -> vector_resolution * pow (2.0, (double) (slope_time - 8192) / (6.0 * (double) cb -> vector_resolution_seconds)));
	if (slope_time == 0) {pb -> control_vector (cb -> vector_init_x, cb -> vector_init_y); stop (); return;}
	releasing = true;
	slope_time_counter = 0;
	start_x = cb -> vector_x;
	start_y = cb -> vector_y;
	wt (1);
}
void vector_cpp :: notify_control_vector_x (int x) {
	if (! recording) return;
	pb -> vector_frames [cb -> vector_frame_count] . x = x;
	frame_x_recorded = true;
}
void vector_cpp :: notify_control_vector_y (int y) {
	if (! recording) return;
	pb -> vector_frames [cb -> vector_frame_count] . y = y;
	frame_y_recorded = true;
}
vector_cpp :: vector_cpp (parameter_block * pb, int horizontal) : vector (horizontal) {
	key_counter = 0;
	this -> pb = pb;
	this -> cb = pb -> controllers;
}
void vector_cpp :: tick_code (void) {
	vector_frame * fr;
	int x, y;
	if (looping) {
		if (++slope_time_counter < slope_time) {
			x = start_x * (slope_time - slope_time_counter);
			x += pb -> vector_frames [loop_target] . x * slope_time_counter;
			x /= slope_time;
			y = start_y * (slope_time - slope_time_counter);
			y += pb -> vector_frames [loop_target] . y * slope_time_counter;
			y /= slope_time;
			pb -> control_vector (x, y);
			wt (1);
		} else loop_phase_finished ();
		return;
	}
	if (attacking) {
		if (++slope_time_counter < slope_time) {
			x = start_x * (slope_time - slope_time_counter);
			x += cb -> vector_init_x * slope_time_counter;
			x /= slope_time;
			y = start_y * (slope_time - slope_time_counter);
			y += cb -> vector_init_y * slope_time_counter;
			y /= slope_time;
			pb -> control_vector (x, y);
			wt (1);
		} else attack_phase_finished ();
		return;
	}
	if (releasing) {
		if (++slope_time_counter < slope_time) {
			x = start_x * (slope_time - slope_time_counter);
			x += cb -> vector_init_x * slope_time_counter;
			x /= slope_time;
			y = start_y * (slope_time - slope_time_counter);
			y += cb -> vector_init_y * slope_time_counter;
			y /= slope_time;
			pb -> control_vector (x, y);
			wt (1);
		} else {
			pb -> control_vector (cb -> vector_init_x, cb -> vector_init_y);
			stop ();
			releasing = false;
		}
		return;
	}
	if (recording) {
		if (cb -> vector_frame_count >= pb -> vector_size) return;
		fr = & pb -> vector_frames [cb -> vector_frame_count];
		if (frame_x_recorded) {
			if (! frame_y_recorded) {
				if (cb -> vector_frame_count < 1) fr -> y = cb -> vector_init_y;
				else fr -> y = pb -> vector_frames [cb -> vector_frame_count - 1] . y;
			}
			fr -> delay = frame_counter;
			cb -> vector_frame_count++;
			frame_x_recorded = false;
			frame_y_recorded = false;
			frame_counter = 0; wt (1); return;
		}
		if (frame_y_recorded) {
			if (! frame_x_recorded) {
				if (cb -> vector_frame_count < 1) fr -> x = cb -> vector_init_x;
				else fr -> x = pb -> vector_frames [cb -> vector_frame_count - 1] . x;
			}
			fr -> delay = frame_counter;
			cb -> vector_frame_count++;
			frame_x_recorded = false;
			frame_y_recorded = false;
			frame_counter = 0; wt (1); return;
		}
		frame_counter++; wt (1); return;
	}
	if (rounding) {
		fr = & pb -> vector_frames [frame_counter];
		pb -> control_vector (fr -> x, fr -> y);
		wt (fr -> delay);
		frame_counter--;
		if (frame_counter <= cb -> vector_loop_start || frame_counter < 1) rounding = false;
		return;
	}
	fr = & pb -> vector_frames [frame_counter];
	pb -> control_vector (fr -> x, fr -> y);
	frame_counter++;
	if (cb -> vector_loop_type == 1) {
		if (frame_counter >= cb -> vector_loop_start + cb -> vector_loop_size) {
			start_loop ();
			return;
		}
	}
	if (cb -> vector_loop_type == 2) {
		if (frame_counter >= cb -> vector_loop_start + cb -> vector_loop_size) {
			rounding = true;
			frame_counter--;
			wt (fr -> delay);
			return;
		}
	}
	if (frame_counter < cb -> vector_frame_count) wt (pb -> vector_frames [frame_counter] . delay);
}
