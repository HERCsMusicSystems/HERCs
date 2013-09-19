///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc_cpp.h"
#include <math.h>

static float get_damp_fraction (int value, int horizontal) {
	if (value < 0) value = 0;
	if (value > 16383) value = 16383;
	float fraction = (float) value / 16384.0f;
	return exp ((48000.0f / (float) horizontal) / (1.0f / log (fraction)));
}

void dsp_algo_cpp :: code (dsp_line * dsp) {}
void dsp_algo_cpp :: parameter_change (dsp_line * dsp, int ind) {}
void dsp_algo_cpp :: all_parameters_changed (dsp_line * dsp) {
	dsp -> variables [2] = 0.0f;
	dsp -> variables [3] = 0.0f;
	dsp -> variables [4] = get_damp_fraction (dsp -> parameters [2], dsp -> horizontal);
	dsp -> variables [5] = 1.0f - dsp -> variables [4];
}

char * default_dsp_algo_cpp :: toString (parameter_block * pb) {return "C-DELAY";}
char * default_dsp_algo_cpp :: toString (parameter_block * pb, int ind) {
	switch (ind) {
	case 0: return "FEEDBACK";
	case 1: return "TIME";
	case 2: return "HIGH DAMP";
	default: return "NOT USED";
	}
}
int default_dsp_algo_cpp :: get_default (int ind) {
	switch (ind) {
	case 0: return 4096;
	case 1: return 8192;
	case 2: return 12800;
	default: return 0;
	}
}
void default_dsp_algo_cpp :: code (dsp_line * dsp) {
	int index = dsp -> indices [0];
	int sentinel = dsp -> indices [1];
	if (index >= sentinel) index = 0;
	float value = dsp -> line [index];
	dsp -> outputs [0] = value;
	value *= dsp -> variables [0];
	value += dsp -> inputs [0];
	value = value * dsp -> variables [5] + dsp -> variables [2] * dsp -> variables [4];
	dsp -> variables [2] = value;
	dsp -> line [index] = value;
	index++;
	if (index >= sentinel) index = 0;
	value = dsp -> line [index];
	dsp -> outputs [1] = value;
	value *= dsp -> variables [0];
	value += dsp -> inputs [1];
	value = value * dsp -> variables [5] + dsp -> variables [3] * dsp -> variables [4];
	dsp -> variables [3] = value;
	dsp -> line [index] = value;
	index++;
	dsp -> indices [0] = index;
}
void default_dsp_algo_cpp :: parameter_change (dsp_line * dsp, int ind) {
	float sub;
	int id;
	switch (ind) {
	case 0:
		sub = (float) dsp -> parameters [0];
		sub /= 16384.0f;
		dsp -> variables [0] = sub;
		break;
	case 1:
		sub = (float) dsp -> parameters [1];
		sub *= (float) (dsp -> horizontal << 1);
		sub /= 16384.0f;
		id = (int) sub;
		id >>= 1;
		id <<= 1;
		id++;
		if (id > dsp -> length) id = dsp -> length;
		dsp -> indices [1] = id;
		break;
	case 2:
		sub = get_damp_fraction (dsp -> parameters [2], dsp -> horizontal);
		dsp -> variables [4] = get_damp_fraction (dsp -> parameters [2], dsp -> horizontal);
		dsp -> variables [5] = 1.0f - dsp -> variables [4];
	default: break;
	}
}
void default_dsp_algo_cpp :: all_parameters_changed (dsp_line * dsp) {
	dsp -> indices [0] = 0;
	dsp_algo_cpp :: all_parameters_changed (dsp);
}
default_dsp_algo_cpp default_dsp_algo;

dsp_algo_pointer float_stereo_dsp_line_pool :: get_default_dsp_algo (void) {return & default_dsp_algo;}

void dsp_line :: create_routers (int ind) {
	if (ind < 1) {
		router_number = 0;
		routers = NULL;
		send_to = NULL;
		return;
	}
	router_number = ind;
	routers = new dsp_line_pointer [ind];
	send_to = new float [ind];
	for (int sub = 0; sub < ind; sub++) send_to [sub] = 0.0f;
}

void dsp_line :: remove_routers (void) {
	if (routers) delete [] routers;
	if (send_to) delete [] send_to;
}

void dsp_line :: ground (void) {for (int ind = 0; ind < length; ind++) line [ind] = 0.0f;}

#ifdef LINUX_OPERATING_SYSTEM
#include <fcntl.h>
#include <semaphore.h>
#endif

float_stereo_dsp_line_pool :: float_stereo_dsp_line_pool (config * cfg, synthesiser * sth) {
#ifdef MULTITHREADED
#ifdef LINUX_OPERATING_SYSTEM
	sem_t * s = sem_open ("HRCs", O_CREAT, 0000777, 1);
	sem_unlink ("HRCs");
	entry_semaphore = (void *) s;
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
	//entry_semaphore = CreateSemaphore (NULL, 1, 30000, NULL);
	entry_semaphore = CreateMutex (NULL, FALSE, NULL);
#endif
#endif
	int horizontal = cfg -> horizontal;
//	if (cfg -> overdrive > 1) horizontal *= cfg -> overdrive;
	configure (sth, cfg -> local_dsp, cfg -> global_dsp, cfg -> global_dsp_multi, cfg -> multitimbral, horizontal, cfg -> vertical, cfg -> head_room, cfg -> dco_blocking_freq, cfg -> dsp_variable_space, cfg -> dsp_line_space);
}

float_stereo_dsp_line_pool :: ~ float_stereo_dsp_line_pool (void) {
	delete [] dsp_inputs;
	delete [] dsp_outputs;
	delete [] dsp_input_ratios;
	delete [] dsp_output_ratios;
	delete [] volumes;
	delete [] global_volumes;
	delete [] dry_ratios;
	dsp_line_pointer dspp = lines;
	for (int ind = 0; ind < total_dsp_number; ind++) {
		delete [] dspp -> variables;
		delete [] dspp -> indices;
		delete [] dspp -> line;
		dspp -> remove_routers ();
		dspp++;
	}
	delete [] lines;
#ifdef MULTITHREADED
#ifdef LINUX_OPERATING_SYSTEM
	sem_close ((sem_t *) entry_semaphore);
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
	CloseHandle (entry_semaphore);
#endif
#endif
}

void float_stereo_dsp_line_pool :: configure (synthesiser * sth, int local_dsp, int global_dsp, bool global_dsp_multi, int multitimbral, int horizontal, int vertical, int head_room, int dco_blocking_freq, int data_size, int line_size) {
	int ind, sub, id;
	this -> head_room = (float) head_room;
	this -> multitimbral = multitimbral;
	this -> local_dsp = local_dsp;
	this -> global_dsp = global_dsp;
	this -> global_dsp_multi = global_dsp_multi;
	this -> all_dsp = local_dsp + global_dsp;
	total_local_dsp = local_dsp * multitimbral;
	all_dsp_multi = total_local_dsp + global_dsp * multitimbral;
	total_dsp_number = global_dsp;
	multi_step = multitimbral * 2 - 1;
	global_multi_step = global_dsp_multi ? multi_step : 1;
	global_multi_single_step = global_dsp_multi ? multitimbral : 1;
	if (global_dsp_multi) total_dsp_number *= multitimbral;
	total_dsp_number += total_local_dsp;
	total_input_output = total_dsp_number << 1;
	dco_blocking_fraction = 1.0f - ((float) dco_blocking_freq / (float) horizontal);
	dsp_inputs = new float [total_input_output];
	dsp_outputs = new float [total_input_output];
	dsp_input_ratios = new float [all_dsp_multi];
	dsp_output_ratios = new float [total_dsp_number];
	volumes = new float [multitimbral];
	dry_ratios = new float [multitimbral];
	global_volumes = new float [multitimbral];
	float vol = (float) pow (2.0, ((double) (12800 - 16383)) / 3328.0);
	for (ind = 0; ind < total_input_output; ind++) dsp_inputs [ind] = 0.0f;
	for (ind = 0; ind < all_dsp_multi; ind++) {
		dsp_input_ratios [ind] = ind < multitimbral ? 1.0f : 0.0f;
	}
	for (ind = 0; ind < total_dsp_number; ind++) {
		dsp_output_ratios [ind] = ind < total_local_dsp ? vol : 1.0f;
	}
	if (global_dsp > 0) {
		for (ind = 0; ind < multitimbral; ind++) dsp_input_ratios [total_local_dsp + ind] = 1.0f;
	}
	for (ind = 0; ind < multitimbral; ind++) {
		volumes [ind] = vol;
		dry_ratios [ind] = vol;
		global_volumes [ind] = 1.0f;
	}
	lines = new dsp_line [total_dsp_number];
	algos = new dsp_algo_cpp_pointer [total_dsp_number];
	dsp_line_pointer dspp = lines;
	for (ind = 0; ind < total_dsp_number; ind++) {
		algos [ind] = & default_dsp_algo;
		dspp -> sth = sth;
		dspp -> horizontal = horizontal;
		dspp -> vertical = vertical;
		dspp -> head_room = head_room;
		dspp -> variables = new float [data_size];
		dspp -> indices = new int [data_size];
		dspp -> line = new float [line_size];
		dspp -> length = line_size;
		dspp++;
	}
	dspp = lines;
	ind = 0;
	float * ip = dsp_inputs;
	float * op = dsp_outputs;
	while (ind < local_dsp) {
		for (sub = 0; sub < multitimbral; sub++) {
			dspp -> create_routers (all_dsp - ind - 1);
			for (id = 0; id < all_dsp - ind - 1; id++) {
				if (global_dsp_multi || id < local_dsp - ind - 1) dspp -> routers [id] = & lines [ind * multitimbral + sub + multitimbral * (id + 1)];
				else dspp -> routers [id] = & lines [total_local_dsp + ind - local_dsp + id + 1];
			}
			dspp -> inputs = ip;
			dspp -> outputs = op;
			ip += 2;
			op += 2;
			dspp++;
		}
		ind++;
	}
	while (ind < all_dsp) {
		for (sub = 0; sub < (global_dsp_multi ? multitimbral : 1); sub++) {
			dspp -> create_routers (all_dsp - ind - 1);
			for (id = 0; id < all_dsp - ind - 1; id++) {
				if (global_dsp_multi) dspp -> routers [id] = & lines [ind * multitimbral + sub + multitimbral * (id + 1)];
				else dspp -> routers [id] = & lines [total_local_dsp + ind - local_dsp + id + 1];
			}
			dspp -> inputs = ip;
			dspp -> outputs = op;
			ip += 2;
			op += 2;
			dspp++;
		}
		ind++;
	}
}

void float_stereo_dsp_line_pool :: add (int factory_id, int value, int * pan) {
	float delta_value = ((float) value) * ((float) * pan) / 8191.0f;
	float left = value - delta_value;
	float right = value + delta_value;
	global_left_out += dry_ratios [factory_id] * left;
	global_right_out += dry_ratios [factory_id] * right;
	float * fi = & dsp_inputs [factory_id << 1];
	float * fr = & dsp_input_ratios [factory_id];
	int ind;
	// wait ()
	for (ind = 0; ind < local_dsp; ind++) {
		* fi++ += left * * fr;
		* fi += right * * fr;
		fi += multi_step;
		fr += multitimbral;
	}
	if (! global_dsp_multi) {
		fi = & dsp_inputs [total_local_dsp << 1];
//		fr = & dsp_input_ratios [total_local_dsp];
	}
	for (ind = 0; ind < global_dsp; ind++) {
		* fi++ += left * * fr;
		* fi += right * * fr;
		fi += global_multi_step;
//		fr += global_multi_single_step;
		fr += multitimbral;
	}
	// signal ()
}

void float_stereo_dsp_line_pool :: multi_add (int factory_id, int value, int * pan, int sample) {
	float delta_value = ((float) value) * ((float) * pan) / 8191.0f;
	float left = value - delta_value;
	float right = value + delta_value;
	global_left_out += dry_ratios [factory_id] * left;
	global_right_out += dry_ratios [factory_id] * right;
	float * fi = & dsp_inputs [factory_id << 1];
	float * fr = & dsp_input_ratios [factory_id];
	int ind;
	// wait ()
#ifdef MULTITHREADED
#ifdef LINUX_OPERATING_SYSTEM
	sem_wait ((sem_t *) entry_semaphore);
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
	WaitForSingleObject (entry_semaphore, INFINITE);
#endif
#endif
	for (ind = 0; ind < local_dsp; ind++) {
		* fi++ += left * * fr;
		* fi += right * * fr;
		fi += multi_step;
		fr += multitimbral;
	}
	if (! global_dsp_multi) {
		fi = & dsp_inputs [total_local_dsp << 1];
//		fr = & dsp_input_ratios [total_local_dsp];
	}
	for (ind = 0; ind < global_dsp; ind++) {
		* fi++ += left * * fr;
		* fi += right * * fr;
		fi += global_multi_step;
//		fr += global_multi_single_step;
		fr += multitimbral;
	}
	// signal ()
#ifdef MULTITHREADED
#ifdef LINUX_OPERATING_SYSTEM
	sem_post ((sem_t *) entry_semaphore);
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
	//ReleaseSemaphore (entry_semaphore, 1, NULL);
	ReleaseMutex (entry_semaphore);
#endif
#endif
}

void float_stereo_dsp_line_pool :: move (void) {
	int ind, sub;
	dsp_line * dspp = lines;
	dsp_algo_cpp_pointer * ap = algos;
	float * ip, * op, ratio;
	for (ind = 0; ind < total_dsp_number; ind++) {
		(* ap++) -> code (dspp);
		for (sub = 0; sub < dspp -> router_number; sub++) {
			ip = dspp -> routers [sub] -> inputs;
			op = dspp -> outputs;
			ratio = dspp -> send_to [sub];
			* ip++ += * op++ * ratio;
			* ip += * op * ratio;
		}
		dspp++;
	}
	float * outp = dsp_outputs;
	float * ratiop = dsp_output_ratios;
	float * inp = dsp_inputs;
	for (ind = 0; ind < total_dsp_number; ind++) {
		global_left_out += * outp++ * * ratiop;
		global_right_out += * outp++ * * ratiop++;
		* inp++ = 0.0f;
		* inp++ = 0.0f;
	}
	global_left_out /= head_room; //262144.0f;
	global_right_out /= head_room; //262144.0f;
	left_out = global_left_out;
	right_out = global_right_out;
	global_left_out = 0.0f;
	global_right_out = 0.0f;
}

void float_stereo_dsp_line_pool :: multi_move (int samples) {
	move ();
	left_outs [samples] = left_out;
	right_outs [samples] = right_out;
}

void float_stereo_dsp_line_pool :: notify_algo_change (parameter_block * pb, int ind) {
	int sub;
	if (global_dsp_multi || ind < local_dsp) sub = ind * multitimbral + pb -> factory_id;
	else sub = total_local_dsp + ind - local_dsp;
	dsp_algo_cpp_pointer algo = (dsp_algo_cpp_pointer) (pb -> dsp [ind] -> algo_head);
	algos [sub] = algo;
	dsp_line * dsp = & lines [sub];
	for (int id = 0; id < 16; id++) algo -> parameter_change (dsp, id);
	algo -> all_parameters_changed (dsp);
	dsp -> ground ();
}
void float_stereo_dsp_line_pool :: notify_global_algo_change (synthesiser * sth, int ind) {
	int sub = total_local_dsp + ind;
	dsp_algo_cpp_pointer algo = (dsp_algo_cpp_pointer) (sth -> dsp [ind] -> algo_head);
	algos [sub] = algo;
	dsp_line * dsp = & lines [sub];
	for (sub = 0; sub < 16; sub++) algo -> parameter_change (dsp, sub);
	algo -> all_parameters_changed (dsp);
	dsp -> ground ();
}
void float_stereo_dsp_line_pool :: notify_volume_change_only (parameter_block * pb) {
	int value = pb -> controllers -> volume;
	float vol;
	if (value >= 3071) vol = (float) pow (2.0, ((double) (value - 16383)) / 3328.0);
	else vol = ((float) value) / 49136.0f;
	volumes [pb -> factory_id] = vol;
}
void float_stereo_dsp_line_pool :: notify_volume_change (parameter_block * pb) {
	notify_volume_change_only (pb);
	notify_dry_wet_change_all (pb);
}
void float_stereo_dsp_line_pool :: notify_global_volume_change (parameter_block * pb) {
	bool global_volume_multi = pb -> sth -> global_volume_multi;
	int value = global_volume_multi ? pb -> controllers -> global_volume : pb -> sth -> global_volume;
	float vol;
	if (value >= 3071) vol = (float) pow (2.0, ((double) (value - 16383)) / 3328.0);
	else vol = ((float) value) / 49136.0f;
	if (global_volume_multi) {
		global_volumes [pb -> factory_id] = vol;
		notify_dry_wet_change_all (pb);
	}
	else {
		for (int ind = 0; ind < multitimbral; ind++) {
			global_volumes [ind] = vol;
			notify_dry_wet_change_all (pb -> sth -> data_blocks [ind]);
		}
	}
}
void float_stereo_dsp_line_pool :: notify_dsp_change (parameter_block * pb, int nrpn_lsb) {
	int xxx = nrpn_lsb >> 4;
	xxx &= 0x7;
	if (xxx >= all_dsp) return;
	int nnnn = nrpn_lsb & 0xf;
	int ind = (global_dsp_multi || xxx < local_dsp) ? xxx * multitimbral + pb -> factory_id : total_local_dsp + xxx - local_dsp;
	lines [ind] . parameters [nnnn] = pb -> dsp [xxx] -> parameters [nnnn];
	algos [ind] -> parameter_change (& lines [ind], nnnn);
}
void float_stereo_dsp_line_pool :: notify_global_dsp_change (synthesiser * sth, int ind, int parameter) {
	int location = total_local_dsp + ind;
	dsp_line * dsp = & lines [location];
	dsp -> parameters [parameter] = sth -> dsp [ind] -> parameters [parameter];
	algos [ind] -> parameter_change (dsp, parameter);
}
void float_stereo_dsp_line_pool :: notify_dry_change (parameter_block * pb) {
	controllers_parameter_block * ctrl = pb -> controllers;
	int factory_id = pb -> factory_id;
	float global_volume = global_volumes [factory_id];
	float volume = volumes [factory_id];
	float global_input_volume = volume;
	volume *= global_volume;
//	int local_shift = pb -> local_dry_out + pb -> dry_wet_local_out_sens . get (ctrl -> dry_wet) + pb -> vector_x_local_out_sens . get (ctrl -> vector_x) + pb -> vector_y_local_out_sens . get (ctrl -> vector_y);
	int local_shift = pb -> local_dry_out . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
	global_input_volume *= (float) local_shift / 268402689.0f;
//	int global_shift = pb -> global_dry_out + pb -> dry_wet_global_out_sens . get (ctrl -> dry_wet) + pb -> vector_x_global_out_sens . get (ctrl -> vector_x) + pb -> vector_y_global_out_sens . get (ctrl -> vector_y);
	int global_shift = pb -> global_dry_out . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
	volume *= (float) global_shift / 268402689.0f;
	global_volume /= 16383.0f;
	float level = volume * (float) local_shift;
	if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
	// dry level
	dry_ratios [factory_id] = level;
	int ind;
	float * rp = & dsp_output_ratios [factory_id];
	dsp_parameter_block * dspp;
	// local dsp outputs
	for (ind = 0; ind < local_dsp; ind++) {
		dspp = pb -> dsp [ind];
//		local_shift = dspp -> wet_output + dspp -> dry_wet_out_sens . get (ctrl -> dry_wet) + dspp -> vector_x_out_sens . get (ctrl -> vector_x) + dspp -> vector_y_out_sens . get (ctrl -> vector_y);
		local_shift = dspp -> wet_output . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
		level = volume * (float) local_shift;
		if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
		* rp = dspp -> on ? level : 0.0f;
		rp += multitimbral;
	}
	// global dsp outputs
	if (! global_dsp_multi) rp = & dsp_output_ratios [total_local_dsp];
	while (ind < global_dsp) {
		dspp = pb -> dsp [ind];
//		local_shift = dspp -> wet_output + dspp -> dry_wet_out_sens . get (ctrl -> dry_wet) + dspp -> vector_x_out_sens . get (ctrl -> vector_x) + dspp -> vector_y_out_sens . get (ctrl -> vector_y);
		local_shift = dspp -> wet_output . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
		level = global_volume * (float) local_shift;
		if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
		* rp = dspp -> on ? level : 0.0f;
		rp += global_multi_single_step;
		ind++;
	}
	// dry to global dsp inputs
//	rp = & dsp_input_ratios [total_local_dsp + (global_dsp_multi ? factory_id : 0)];
	rp = & dsp_input_ratios [total_local_dsp + factory_id];
	for (ind = local_dsp; ind < all_dsp; ind++) {
//		local_shift = pb -> dsp_send [ind] + pb -> dry_wet_dsp_send_sens [ind] . get (ctrl -> dry_wet) + pb -> vector_x_dsp_send_sens [ind] . get (ctrl -> vector_x) + pb -> vector_y_dsp_send_sens [ind] . get (ctrl -> vector_y);
		local_shift = pb -> dsp_send [ind] . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
		level = global_input_volume * (float) local_shift;
		if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
		* rp = level;
//		rp += global_multi_single_step;
		rp += multitimbral;
	}
}
void float_stereo_dsp_line_pool :: notify_dry_wet_change_all (parameter_block * pb) {
	notify_dry_change (pb);
	for (int ind = 0; ind < all_dsp; ind++) notify_dry_wet_change (pb, ind);
}
void float_stereo_dsp_line_pool :: notify_dry_wet_change (parameter_block * pb, int ind) {
	int shift, sub_shift, wet_shift;
	float level;
	controllers_parameter_block * ctrl = pb -> controllers;
	int factory_id = pb -> factory_id;
	int location, sub_location;
	dsp_parameter_block * dspp;
	float volume = volumes [factory_id];
	float global_volume = global_volumes [factory_id];
	int sub;
	dsp_line * dsp;
	dsp_router_pointer dsprp;
	if (ind < local_dsp) {
		// local dsp
		location = ind * multitimbral + factory_id;
		// dsp input
//		shift = pb -> dsp_send [ind] + pb -> dry_wet_dsp_send_sens [ind] . get (ctrl -> dry_wet) + pb -> vector_x_dsp_send_sens [ind] . get (ctrl -> vector_x) + pb -> vector_y_dsp_send_sens [ind] . get (ctrl -> vector_y);
		shift = pb -> dsp_send [ind] . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
		level = (float) shift / 16383.0f;
		if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
		dsp_input_ratios [location] = level;
		// dsp output
		dspp = pb -> dsp [ind];
//		wet_shift = dspp -> wet_output + dspp -> dry_wet_out_sens . get (ctrl -> dry_wet) + dspp -> vector_x_out_sens . get (ctrl -> vector_x) + dspp -> vector_y_out_sens . get (ctrl -> vector_y);
		wet_shift = dspp -> wet_output . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
//		sub_shift = pb -> global_dry_out + pb -> dry_wet_global_out_sens . get (ctrl -> dry_wet) + pb -> vector_x_global_out_sens . get (ctrl -> vector_x) + pb -> vector_y_global_out_sens . get (ctrl -> vector_y);
		sub_shift = pb -> global_dry_out . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
		level = (float) wet_shift * (float) sub_shift * volume * global_volume / 268402689.0f;
		if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
		dsp_output_ratios [location] = dspp -> on ? level : 0.0f;
		// dsp to local dsp send
		dsp = & lines [location];
		sub = 0;
		while (sub < local_dsp - ind - 1) {
			dsprp = dspp -> routers [sub];
//			shift = dsprp -> send_level + dsprp -> dry_wet . get (ctrl -> dry_wet) + dsprp -> vector_x . get (ctrl -> vector_x) + dsprp -> vector_y . get (ctrl -> vector_y);
			shift = dsprp -> get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
			level = (float) shift / 16383.0f;
			if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
			dsp -> send_to [sub] = dspp -> on ? level : 0.0f;
			sub++;
		}
		// dsp to global dsp send
		while (sub < all_dsp - ind - 1) {
			sub_location = ind + sub + 1;
//			shift = pb -> dsp_send [sub_location] + pb -> dry_wet_dsp_send_sens [sub_location] . get (ctrl -> dry_wet) + pb -> vector_x_dsp_send_sens [sub_location] . get (ctrl -> vector_x) + pb -> vector_y_dsp_send_sens [sub_location] . get (ctrl -> vector_y);
			shift = pb -> dsp_send [sub_location] . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
			level = (float) wet_shift * (float) shift * volume / 268402689.0f;
			if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
			dsp -> send_to [sub] = dspp -> on ? level : 0.0f;
			sub++;
		}
		return;
	}
	// global dsp
	int multi_location = ind * multitimbral + factory_id;
	if (global_dsp_multi) location = multi_location;
	else location = total_local_dsp + ind - local_dsp;
	// dsp output
	dspp = pb -> dsp [ind];
//	wet_shift = dspp -> wet_output + dspp -> dry_wet_out_sens . get (ctrl -> dry_wet) + dspp -> vector_x_out_sens . get (ctrl -> vector_x) + dspp -> vector_y_out_sens . get (ctrl -> vector_y);
	wet_shift = dspp -> wet_output . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
	level = (float) wet_shift * global_volume / 16383.0f;
	if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
	dsp_output_ratios [location] = dspp -> on ? level : 0.0f;
	// dsp to global dsp send
	dsp = & lines [location];
	for (sub = 0; sub < all_dsp - ind - 1; sub++) {
		dsprp = dspp -> routers [sub];
//		shift = dsprp -> send_level + dsprp -> dry_wet . get (ctrl -> dry_wet) + dsprp -> vector_x . get (ctrl -> vector_x) + dsprp -> vector_y . get (ctrl -> vector_y);
		shift = dsprp -> get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
		level = (float) shift / 16383.0f;
		if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
		dsp -> send_to [sub] = dspp -> on ? level : 0.0f;
	}
	// dsp dry input
//	shift = pb -> dsp_send [ind] + pb -> dry_wet_dsp_send_sens [ind] . get (ctrl -> dry_wet) + pb -> vector_x_dsp_send_sens [ind] . get (ctrl -> vector_x) + pb -> vector_y_dsp_send_sens [ind] . get (ctrl -> vector_y);
	shift = pb -> dsp_send [ind] . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
//	sub_shift = pb -> local_dry_out + pb -> dry_wet_local_out_sens . get (ctrl -> dry_wet) + pb -> vector_x_local_out_sens . get (ctrl -> vector_x) + pb -> vector_y_local_out_sens . get (ctrl -> vector_y);
	sub_shift = pb -> local_dry_out . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
	level = (float) shift * (float) sub_shift * volume / 268402689.0f;
	if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
	dsp_input_ratios [multi_location] = level;
	// dsp wet input from other dsp
	sub_location = factory_id;
	for (sub = 0; sub < local_dsp; sub++) {
		dspp = pb -> dsp [sub];
//		sub_shift = dspp -> wet_output + dspp -> dry_wet_out_sens . get (ctrl -> dry_wet) + dspp -> vector_x_out_sens . get (ctrl -> vector_x) + dspp -> vector_y_out_sens . get (ctrl -> vector_y);
		sub_shift = dspp -> wet_output . get (ctrl -> dry_wet, ctrl -> vector_x, ctrl -> vector_y);
		level = (float) shift * (float) sub_shift * volume / 268402689.0f;
		if (level < 0.0f) level = 0.0f; if (level > 1.0f) level = 1.0f;
		lines [sub_location] . send_to [ind - sub - 1] = dspp -> on ? level : 0.0f;
		sub_location += multitimbral;
	}
}

void float_stereo_dsp_line_pool :: notify_program_loaded (parameter_block * pb) {
	int ind, sub;
	for (ind = 0; ind < local_dsp; ind++) {
		notify_algo_change (pb, ind);
		for (sub = 0; sub < 16; sub++) notify_dsp_change (pb, (ind << 4) | sub);
	}
	notify_volume_change_only (pb);
	notify_global_volume_change (pb);
//	notify_dry_change (pb);
//	notify_dry_wet_change_all (pb);
}

void float_stereo_dsp_line_pool :: notify_global_dsp_program_loaded (synthesiser * sth) {
	int ind, sub;
	for (ind = 0; ind < global_dsp; ind++) {
		notify_global_algo_change (sth, ind);
		for (sub = 0; sub < 16; sub++) notify_global_dsp_change (sth, ind, sub);
	}
}

void float_stereo_dsp_line_pool :: notify_multi_global_dsp_program_loaded (parameter_block * pb) {
	int ind, sub;
	for (ind = local_dsp; ind < all_dsp; ind++) {
		notify_algo_change (pb, ind);
		for (sub = 0; sub < 16; sub++) notify_dsp_change (pb, (ind << 4) | sub);
	}
	notify_volume_change_only (pb);
	notify_global_volume_change (pb);
}

void float_stereo_dsp_line_pool :: notify_dsp_ground (int factory_id) {
	dsp_line * dspp = & lines [factory_id];
	int sentinel = global_dsp_multi ? all_dsp : local_dsp;
	for (int ind = 0; ind < sentinel; ind++) {
		dspp -> ground ();
		dspp += multitimbral;
	}
}
