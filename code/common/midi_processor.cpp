///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "osc.h"
#include "transport.h"

void synthesiser :: midi_keyoff (int channel, int key) {
	int midi_channel, channel_extension;
	parameter_block * pb;
	system_parameter_block * sb;
	for (int ind = 0; ind < multitimbral; ind++) {
		pb = data_blocks [ind];
		sb = pb -> system;
		midi_channel = sb -> midi_channel;
		channel_extension = sb -> midi_channel_extension;
		if ((midi_channel_extension == channel_extension || channel_extension == 0x7f) && (midi_channel == channel || midi_channel == 0x10))
			pb -> key_off (key);
	}
}

void synthesiser :: midi_keyon (int channel, int key, int velocity) {
	int midi_channel, channel_extension;
	parameter_block * pb;
	system_parameter_block * sb;
	for (int ind = 0; ind < multitimbral; ind++) {
		pb = data_blocks [ind];
		sb = pb -> system;
		midi_channel = sb -> midi_channel;
		channel_extension = sb -> midi_channel_extension;
		if ((midi_channel_extension == channel_extension || channel_extension == 0x7f) && (midi_channel == channel || midi_channel == 0x10))
			pb -> key_on (key, velocity);
	}
}

void parameter_block :: key_on (int key, int velocity) {
	if (controllers -> arpeggiator_switch) {
		if (arp == NULL) return;
		arp -> notify_key_on (key, velocity);
	} else key_code (key, velocity);
}

void parameter_block :: key_off (int key) {
	if (controllers -> arpeggiator_switch) {
		if (arp == NULL) return;
		arp -> notify_key_off (key);
	} else key_code (key);
}

void parameter_block :: key_off (void) {
	if (controllers -> arpeggiator_switch) {
		if (arp == NULL) return;
		arp -> notify_key_off ();
	} else key_code ();
}

void parameter_block :: key_code (int key, int velocity) {
	if (vect != NULL) vect -> notify_key_on ();
	key = controllers -> key_tuner [key];
	velocity <<= 7;
	int osc;
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	if (controllers -> poly) {
		osc = pool -> lock_oscillator (factory_id, key);
		pool -> unhold_oscillator (osc);
		pool -> notify_key_on (this, osc, key, velocity);
		last_key = key;
		return;
	}
	int size = queue -> key_on (key);
	if (size == 1) {
		pool -> notify_key_on (locked_oscillator, key, velocity);
		last_key = key;
		return;
	}
	pool -> notify_key_on (locked_oscillator, key);
	last_key = key;
}

void parameter_block :: key_code (int key) {
	if (vect != NULL) vect -> notify_key_off ();
	int osc;
//	key <<= 7;
	key = controllers -> key_tuner [key];
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	if (controllers -> poly) {
		osc = pool -> get_oscillator (factory_id, key);
		if (osc < 0) return;				// keyoff has no accompanied keyon
		if (controllers -> hold) pool -> hold_oscillator (osc); //pool -> hold [osc] = true;
		else {
//			pool -> locked [osc] = false;
			pool -> unlock_oscillator (osc);
			pool -> notify_key_off (osc);
		}
		return;
	}
	int sub = queue -> key_off (key);
	if (sub >= 0) {
		if (sub != key) {
			pool -> notify_key_on (locked_oscillator, sub);
			last_key = sub;
		}
		return;
	}
	if (controllers -> hold) pool -> hold_oscillator (locked_oscillator); //pool -> hold [locked_oscillator] = true;
	else pool -> notify_key_off (locked_oscillator);
}

void parameter_block :: key_code (void) {
	if (vect != NULL) vect -> notify_all_keys_off ();
	queue -> key_off ();
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	if (controllers -> poly) {
		for (int ind = 0; ind < pool -> polyphony; ind++) {
			if (pool -> part_connections [ind] == factory_id) {
				if (controllers -> hold) pool -> hold_oscillator (ind); //pool -> hold [ind] = true;
				else {
//					pool -> locked [ind] = false;
					pool -> unlock_oscillator (ind);
					pool -> notify_key_off (ind);
				}
			}
		}
		return;
	}
	if (controllers -> hold) pool -> hold_oscillator (locked_oscillator); //pool -> hold [locked_oscillator] = true;
	else pool -> notify_key_off (locked_oscillator);
}

void parameter_block :: loading_ground (void) {
	controllers -> hold = false;
	last_key = -1;
	queue -> key_off ();
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	for (int ind = 0; ind < pool -> polyphony; ind++) {
		if (pool -> part_connections [ind] == factory_id) {
			pool -> unlock_oscillator (ind);
			pool -> unhold_oscillator (ind);
			pool -> notify_ground (ind);
		}
	}
}

void parameter_block :: ground (void) {
	loading_ground ();
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> notify_dsp_ground (this);
}

void parameter_block :: reset_all_controllers (void) {}

void parameter_block :: poly (bool notifications_required) {
	// the purpose of notifications_required is to avoid notifications
	// when loading data from the file or voice init
	if (controllers -> poly) return;
	if (notifications_required) {
		controllers -> hold = false;
		hold_off ();
		key_off ();
		last_key = -1;
	}
	controllers -> poly = true;
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	pool -> return_oscillator (locked_oscillator);
}

void parameter_block :: mono (bool notifications_required) {
	// the purpose of notifications_required is to avoid notifications
	// when loading data from the file or voice init
	if (! controllers -> poly) return;
	if (notifications_required) {
		controllers -> hold = false;
		hold_off ();
		key_off ();
		last_key = -1;
	}
	controllers -> poly = false;
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	locked_oscillator = pool -> reserve_oscillator (factory_id);
	if (notifications_required) {
		pool -> notify_oscillator_assign (this, locked_oscillator);
		pool -> notify_ground (locked_oscillator);
		pool -> lock_retrigger (locked_oscillator);
	}
}

void parameter_block :: hold_off (void) {
	if (controllers -> hold) return;
	oscillator_pool * pool = sth -> pool;
	if (pool == NULL) return;
	if (controllers -> poly) {
		for (int ind = 0; ind < pool -> polyphony; ind++) {
			if (pool -> part_connections [ind] == factory_id && pool -> oscillator_holding (ind)) {
				pool -> unlock_oscillator (ind);
				pool -> unhold_oscillator (ind);
				pool -> notify_key_off (ind);
			}
		}
		return;
	}
	pool -> unhold_oscillator (locked_oscillator);
	pool -> notify_key_off (locked_oscillator);
}

void parameter_block :: control_vector_x (int x) {
	if (controllers -> vector_switch) {
		if (vect) vect -> notify_control_vector_x (x);
		if (controllers -> vector_trigger < 2) return;
	}
	controllers -> vector_x = x;
	relemat_vector_x ();
	add_all_shift_lemat ();
	notify_all_shifts_update ();
	add_lfo_all_lemat ();
	notify_lfo_all_shifts_update ();
	notify_dry_wet_change ();
}

void parameter_block :: control_vector_y (int y) {
	if (controllers -> vector_switch) {
		if (vect) vect -> notify_control_vector_y (y);
		if (controllers -> vector_trigger < 2) return;
	}
	controllers -> vector_y = y;
	relemat_vector_y ();
	add_all_shift_lemat ();
	notify_all_shifts_update ();
	add_lfo_all_lemat ();
	notify_lfo_all_shifts_update ();
	notify_dry_wet_change ();
}

void parameter_block :: control_vector (int x, int y) {
	if (controllers -> vector_x != x) {
		controllers -> vector_x = x;
		relemat_vector_x ();
		if (vector_auto_return) {
//			sth -> midi_out -> insert_channel_extension (system -> midi_channel_extension);
			sth -> midi_out -> insert_nrpn_14 ((system -> midi_channel_extension << 4) | system -> midi_channel & 0xf, 1, 13, x);
		}
	}
	if (controllers -> vector_y != y) {
		controllers -> vector_y = y;
		relemat_vector_y ();
		if (vector_auto_return) {
//			sth -> midi_out -> insert_channel_extension (system -> midi_channel_extension);
			sth -> midi_out -> insert_nrpn_14 ((system -> midi_channel_extension << 4) | system -> midi_channel & 0xf, 1, 14, y);
		}
	}
	add_all_shift_lemat ();
	notify_all_shifts_update ();
	add_lfo_all_lemat ();
	notify_lfo_all_shifts_update ();
	notify_dry_wet_change ();
}

void synthesiser :: midi_active_sensed (void) {for (int part = 0; part < multitimbral; part++) data_blocks [part] -> ground ();}

void synthesiser :: midi_control (int channel, int controller, int value) {
	parameter_block * pb;
	system_parameter_block * sb;
	controllers_parameter_block * cb;
	int midi_channel, channel_extension;
	int sub;
	for (int part = 0; part < multitimbral; part++) {
		pb = data_blocks [part];
		sb = pb -> system;
		cb = pb -> controllers;
		midi_channel = sb -> midi_channel;
		channel_extension = sb -> midi_channel_extension;
		if ((midi_channel_extension == channel_extension || channel_extension == 0x7f) && (midi_channel == channel || midi_channel == 0x10)) {
			sub = (value << 7) + sb -> data_lsb;
			if (controller == sb -> pitch) {cb -> pitch = sub; pb -> relemat_pitch (); pb -> add_all_shift_lemat (); pb -> notify_all_shifts_update ();}
			if (controller == sb -> freq) {cb -> freq = sub; pb -> relemat_freq_shift (); pb -> add_freq_lemat (); pb -> notify_freq_shifts_update ();}
			if (controller == sb -> amp) {cb -> amp = sub; pb -> relemat_amp_shift (); pb -> add_amp_lemat (); pb -> notify_amp_shifts_update ();}
			if (controller == sb -> index) {cb -> index = sub; pb -> relemat_index_shift (); pb -> add_index_lemat (); pb -> notify_index_shifts_update ();}
			if (controller == sb -> attack) {cb -> attack = sub; pb -> notify_attack_change ();}
			if (controller == sb -> decay) {cb -> decay = sub; pb -> notify_decay_change ();}
			if (controller == sb -> sustain) {cb -> sustain = sub; pb -> notify_sustain_change ();}
			if (controller == sb -> release) {cb -> release = sub; pb -> notify_release_change ();}
			if (controller == sb -> speed) {cb -> speed = sub; pb -> relemat_lfo_speed (); pb -> add_lfo_speed_lemat (); pb -> notify_lfo_speeds_update ();}
			if (controller == sb -> vibrato) {cb -> vibrato = sub; pb -> relemat_vibrato (); pb -> add_lfo_freq_lemat (); pb -> notify_lfo_freq_shifts_update ();}
			if (controller == sb -> tremolo) {cb -> tremolo = sub; pb -> relemat_tremolo (); pb -> add_lfo_amp_lemat (); pb -> notify_lfo_amp_shifts_update ();}
			if (controller == sb -> lfo_index) {cb -> lfo_index = sub; pb -> relemat_lfo_index (); pb -> add_lfo_index_lemat (); pb -> notify_lfo_index_shifts_update ();}
			if (controller == sb -> lfo_pan) {cb -> lfo_pan = sub; pb -> relemat_lfo_pan (); pb -> add_lfo_pan_lemat (); pb -> notify_lfo_pan_shifts_update ();}
			if (controller == sb -> vector_x) {pb -> control_vector_x (sub);}
			if (controller == sb -> vector_y) {pb -> control_vector_y (sub);}
			if (controller == sb -> volume) {cb -> volume = sub; pb -> notify_volume_change ();}
			if (controller == sb -> pan) {cb -> pan = sub; pb -> relemat_pan_shift (); pb -> add_pan_lemat (); pb -> notify_pan_shifts_update ();}
			if (controller == sb -> hold) {cb -> hold = value != 0; pb -> hold_off ();}
			if (controller == sb -> porta) {cb -> porta = value != 0; pb -> notify_porta_update ();}
			if (controller == sb -> porta_time) {cb -> porta_time = sub; pb -> notify_porta_update ();}
			if (controller == sb -> porta_mode) {cb -> porta_mode = value; pb -> notify_porta_update ();}
			if (controller == sb -> dry_wet) {cb -> dry_wet = sub; pb -> notify_dry_wet_change ();}
			if (controller == sb -> global_volume) {
				if (global_volume_present) {
					if (global_volume_multi) cb -> global_volume = sub;
					else global_volume = sub;
					pb -> notify_global_volume_change ();
				}
			}
			if (controller == 96) {pb -> rpn (value != 0 ? value : 1);}
			if (controller == 97) {pb -> rpn (value != 0 ? -value : -1);}
			if (controller == 98) {sb -> nrpn_lsb = value;}
			if (controller == 99) {sb -> nrpn_msb = value;}
			if (controller == 100) {sb -> rpn_lsb = value;}
			if (controller == 101) {sb -> rpn_msb = value;}
			if (controller == 38) {sb -> data_lsb = value;}
			if (controller == 6) {pb -> nrpn (value);}
			if (controller == 0) {sb -> bank_msb = value;}
			if (controller == 32) {sb -> bank_lsb = value;}
			if (controller == 120) {pb -> ground ();}
			if (controller == 121) {pb -> reset_all_controllers ();}
			if (controller == 123) {pb -> key_off ();}
			if (controller == 126) {pb -> mono (true);}
			if (controller == 127) {pb -> poly (true);}
			if ((controller >= 32 && controller < 64) || (controller >= 101 && controller < 120) || controller == 160) sb -> data_lsb = value;
			else sb -> data_lsb = 0;
		}
	}
}

void synthesiser :: midi_programchange (int channel, int program) {
	parameter_block * pb;
	system_parameter_block * sb;
	int midi_channel, channel_extension;
	for (int part = 0; part < multitimbral; part++) {
		pb = data_blocks [part];
		sb = pb -> system;
		midi_channel = sb -> midi_channel;
		channel_extension = sb -> midi_channel_extension;
		if ((midi_channel_extension == channel_extension || channel_extension == 0x7f) && (midi_channel == channel || midi_channel == 0x10)) {
			sb -> program = program;
			pb -> program_change ();
		}
	}
}

void synthesiser :: midi_parameter_request (int channel) {
	parameter_block * pb;
	int part_channel;
	int part_channel_extension;
	for (int part = 0; part < multitimbral; part++) {
		pb = data_blocks [part];
		part_channel = pb -> system -> midi_channel;
		part_channel_extension = pb -> system -> midi_channel_extension;
		if ((midi_channel_extension == part_channel_extension || part_channel_extension == 0x7f) && (part_channel == channel || part_channel == 0x10)) {
			send_midi_channel_extension (midi_channel_extension);
			pb -> send_parameter_value (channel);
		}
	}
}

extern void emit (midi_stream * line, char * text);
void synthesiser :: midi_system_exclusive (midi_stream * line) {
	line -> mark ();
	int channel;
	int part_channel;
	int part_channel_extension;
	int device_id;
	if (! line -> check_system_exclusive ()) {
		int ind, part, key, tune, sentinel;
		parameter_block * pb;
		// check if identity request
		line -> restore ();
		switch (line -> get ()) {
		case 0x7e:
			channel = line -> get ();
			switch (line -> get ()) {
			case 0x06:
				if (line -> get () == 0x01) {
					for (part = 0; part < multitimbral; part++) {
						pb = data_blocks [part];
						device_id = pb -> system -> device_id;
						if (device_id < 128 && (device_id == channel || device_id == 0x7f || channel == 0x7f))
							send_identity_reply (midi_channel_extension, device_id);
					}
				}
				break;
			case 0x08:
				switch (line -> get ()) {
				case 0x00:
					// scale bulk dump request
					send_scale_bulk_dump (channel, line -> get ());
					break;
				case 0x01:
					// scale bulk dump
					for (ind = 0; ind < 17; ind++) line -> get ();
					for (ind = 0; ind < 128; ind++) {
						tune = line -> get_int ();
						line -> get ();
						for (part = 0; part < multitimbral; part++) {
							pb = data_blocks [part];
							part_channel = pb -> system -> device_id;
							if (channel == part_channel || part_channel == 0x7f || channel == 0x7f) {
								pb -> controllers -> key_tuner [ind] = tune;
							}
						}
					}
					for (part = 0; part < multitimbral; part++) {
						pb = data_blocks [part];
						part_channel = pb -> system -> device_id;
						if (channel == part_channel || part_channel == 0x7f || channel == 0x7f) {
							pb -> key_off ();
						}
					}
					break;
				default: break;
				}
				break;
			default: break;
			}
			break;
		case 0x7f:
			channel = line -> get ();
			switch (line -> get ()) {
			case 0x08:
				switch (line -> get ()) {
				case 0x02:
					line -> get ();
					sentinel = line -> get ();
					for (ind = 0; ind < sentinel; ind++) {
						key = line -> get ();
						tune = line -> get_int ();
						line -> get ();
						for (part = 0; part < multitimbral; part++) {
							pb = data_blocks [part];
							part_channel = pb -> system -> device_id;
							if (channel == part_channel || channel == 0x7f || part_channel == 0x7f) {
								pb -> controllers -> key_tuner [key] = tune;
							}
						}
					}
					for (part = 0; part < multitimbral; part++) {
						pb = data_blocks [part];
						part_channel = pb -> system -> device_id;
						if (channel == part_channel || channel == 0x7f || part_channel == 0x7f) {
							pb -> key_off ();
						}
					}
					break;
				default: break;
				}
				break;
			default: break;
			}
			break;
		default: break;
		}
		line -> get_f7 ();
		return;
	}
	int selector = line -> get ();
	int sub_selector = line -> get ();
	if (selector > 0x7f || sub_selector > 0x7f) {
		line -> restore ();
		line -> get_f7 ();
		return;
	}
	line -> mark ();
	channel = selector & 0xf;
	selector >>= 4;
	parameter_block * pb;
	char path [256];
	char file_name [256];
	SetupFileReader * fr;
	SetupFileWriter * tc;
	int source, destination;
	for (int part = 0; part < multitimbral; part++) {
		line -> restore ();
		pb = data_blocks [part];
		part_channel = pb -> system -> midi_channel;
		part_channel_extension = pb -> system -> midi_channel_extension;
		if (((midi_channel_extension == part_channel_extension || part_channel_extension == 0x7f) && (part_channel == channel || part_channel == 0x10)) || ((selector & 0x6) == 0x2)) {
			switch (selector) {
			case 0x0:
				send_midi_channel_extension (midi_channel_extension);
				switch (sub_selector) {
				case 0x10: pb -> send_algo_string (channel); break;
				case 0x11: pb -> send_program_name (channel); break;
				case 0x12: pb -> send_bank_msb_name (channel); break;
				case 0x13: pb -> send_bank_lsb_name (channel); break;
				case 0x14: pb -> send_category_name (channel); break;
				case 0x15: pb -> send_pattern_name (channel); break;
				case 0x16: pb -> send_style_name (channel); break;
				default:
					if (sub_selector < 0x10) {pb -> send_stripe_string (channel, sub_selector); break;}
					if (sub_selector >= 0x18 && sub_selector < 0x18 + all_dsp) {pb -> send_dsp_string (channel, sub_selector - 0x18); break;}
					if (sub_selector >= 0x20) {send_message (channel, sub_selector, get_message (sub_selector)); break;}
					send_error (channel, 0x5, 0x7f, sub_selector, "unknown text request");
					break;
				}
				break;
			case 0x1:
				send_midi_channel_extension (midi_channel_extension);
				switch (sub_selector) {
				case 0x11: line -> get (pb -> name); break;
				default:
					send_error (channel, 0x6, 0x7f, sub_selector, "unknown text");
					break;
				}
				break;
			case 0x2:
				switch (channel) {
				case 0:
					device_id = pb -> system -> device_id;
					if (device_id > 127) break;
					if (device_id == sub_selector || device_id == 0x7f || sub_selector == 0x7f)
						pb -> send_midi_channel ();
					break;
				case 1:
					device_id = pb -> system -> device_id;
					if (device_id > 127) break;
					if (device_id == sub_selector || device_id == 0x7f || sub_selector == 0x7f)
						pb -> send_midi_channel_extension ();
					break;
				case 2:
					device_id = pb -> system -> device_id;
					if (device_id > 127) break;
					if (device_id == sub_selector || device_id == 0x7f || sub_selector == 0x7f)
						pb -> send_reserved_notes ();
					break;
				case 3:
					device_id = pb -> system -> device_id;
					if (device_id > 127) break;
					if (device_id == sub_selector || device_id == 0x7f || sub_selector == 0x7f)
						pb -> send_arranger_status ();
					break;
				case 4:
					process_transport_request (sub_selector, line);
					return;
				case 8:
					insert_prolog_command (sub_selector, line);
					line -> get_f7 ();
					return;
				default: break;
				}
				break;
			case 0x3:
				device_id = pb -> system -> device_id;
				if (device_id == sub_selector || device_id == 0x7f || sub_selector == 0x7f) {
					switch (channel) {
					case 0: pb -> system -> midi_channel = line -> get (); break;
					case 1: pb -> system -> midi_channel_extension = line -> get (); break;
					case 2:
						pb -> system -> reserved_notes = line -> get ();
						pb -> notify_reserved_notes_changed ();
						break;
					case 3: pb -> system -> arranger_assign = line -> get (); break;
					case 4: process_transport_data (sub_selector, line); break;
					default: break;
					}
				}
				break;
			case 0x4:
				send_midi_channel_extension (midi_channel_extension);
				switch (sub_selector) {
				case 0x00:
					if (midi_out == NULL) break;
					midi_out -> open_system_exclusive ();
					midi_out -> insert (0x50 | channel);
					midi_out -> insert (0);
					pb -> store_program (midi_out);
					midi_out -> insert_checksum ();
					midi_out -> close_system_exclusive ();
					break;
				case 0x01:
					if (midi_out == NULL) break;
					line -> get (file_name);
					sprintf (path, "%s%s", root_directory, file_name);
					fr = new SetupFileReader (path);
					if (fr -> file_not_found ()) send_error (channel, 2, 0x7f, 0x7f, path);
					else {
						midi_out -> open_system_exclusive ();
						midi_out -> insert (0x50 | channel);
						midi_out -> insert (1);
						emit (midi_out, file_name);
						transfer (fr, midi_out);
						midi_out -> insert_checksum ();
						midi_out -> close_system_exclusive ();
					}
					delete fr;
					break;
				case 0x03: store_configuration (); break;
				case 0x10: pb -> send_parameter_value (channel); break;
				case 0x21: pb -> send_indexed_category_name (channel, line -> get ()); break;
				case 0x22: pb -> send_indexed_algo_parameter_name (channel, line -> get ()); break;
				case 0x23: pb -> send_indexed_dsp_parameter_name (channel, line -> get ()); break;
				case 0x40:
					pb -> voice_init ();
					pb -> notify_program_loaded ();
					break;
				case 0x41:
					line -> get (file_name);
					pb -> bank_change (file_name);
					break;
				case 0x42:
					source = line -> get (); destination = line -> get ();
					pb -> eg_copy (channel, source, destination);
					break;
				case 0x50: pb -> send_algo_bulk_dump (channel); break;
				case 0x51: pb -> send_pattern_bulk_dump (channel); break;
				case 0x57: pb -> send_dsp_algo_bulk_dump (channel); break;
				default:
					if (sub_selector >= 0x58 && sub_selector < 0x60) pb -> send_dsp_algo_bulk_dump (channel, sub_selector & 0x7);
					else send_error (channel, 0x5, 0x7f, sub_selector, "unknown request");
					break;
				}
				break;
			case 0x5:
				switch (sub_selector) {
				case 0x00:
					if (! line -> check_checksum ()) break;
					pb -> read_program (line);
					break;
				case 0x01:
					if (! line -> check_checksum ()) break;
					line -> get (file_name);
					sprintf (path, "%s%s", root_directory, file_name);
					tc = new SetupFileWriter (path);
					transfer (line, tc);
					delete tc;
					break;
				case 0x11:
					pb -> read_vector_bulk_dump (line);
					break;
				case 0x50:
//					if (! line -> check_checksum ()) break;
					pb -> read_algo_bulk_dump (line);
					break;
				case 0x51:
//					if (! line -> check_checksum ()) break;
					pb -> read_pattern_bulk_dump (line);
					break;
				default:
					if (sub_selector >= 0x58 && sub_selector < 0x60) pb -> read_dsp_algo_bulk_dump (line, sub_selector & 0x7);
					else send_error (midi_channel_extension, channel, 0x6, 0x7f, sub_selector, "unknown data");
					break;
				}
				break;
			default: break;
			}
		}
	}
	line -> get_f7 ();
}

void synthesiser :: midi_timing_clock (void) {
	if (arps == NULL) return;
	parameter_block * pb;
	for (int ind = 0; ind < multitimbral; ind++) {
		pb = data_blocks [ind];
		if (pb -> controllers -> beats_per_seconds < 1)
			pb -> arp -> signal ();
	}
}

void synthesiser :: midi_start (void) {
	if (root == NULL) return;
	// to do
//	root -> startTransport ();
}

void synthesiser :: midi_continue (void) {
	if (root == NULL) return;
	// to do
//	root -> pauseTransport ();
}

void synthesiser :: midi_stop (void) {
	if (root == NULL) return;
	// to do
//	root -> stopTransport ();
}

void parameter_block :: rpn (int data) {
	switch (system -> rpn_msb) {
	case 0: default:
		switch (system -> rpn_lsb) {
		case 0: default:
			sth -> tune += data;
			notify_tune ();
			break;
		}
		break;
	}
}

void parameter_block :: nrpn (int data) {
	int positive = (data << 7) + system -> data_lsb;
	int negative = positive;
	if (negative > 8191) negative -= 16384;
	int very_negative = positive;
	if (very_negative != 0) very_negative -= 16384;
	int msb_negative = data;
	if (msb_negative >= 64) msb_negative -= 128;
	// positive       [0 .. 16383]
	// negative       [-8192 .. 8191]
	// very_negative  [-16383 .. 0]
	// msb128         [0 .. 128]
	// msb_negative    [-64 .. 63]
	int nrpn_msb = system -> nrpn_msb;
	int nrpn_lsb = system -> nrpn_lsb;
	int xxx = nrpn_lsb >> 4;
	int nnnn = nrpn_lsb & 0xf;
	int sub;
	sensitivity_parameter_block * spb = NULL;
	if (nrpn_msb == 17) {
		if (data < 16) {
			if (data >= hercules_number) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "lfo sync to non existing stripe");
				return;
			}
		} else {
			if ((data - 16) >= hercules_stereo_number) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "lfo sync to non existing stereo stripe");
				return;
			}
		}
	}
	if (nrpn_msb >= 16 && nrpn_msb < 28) {
		if (xxx < 4) {
			if (nnnn >= hercules_number) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "stripe lfo subscript wrong");
				return;
			}
		} else {
			if (nnnn >= hercules_stereo_number) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "pan lfo subscript wrong");
				return;
			}
		}
	}
	if (nrpn_msb >= 32 && nrpn_msb < 56) {
		if (nnnn >= hercules_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "stripe subscript wrong");
			return;
		}
	}
	if (nrpn_msb >= 56 && nrpn_msb < 63) {
		if (nnnn >= hercules_stereo_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "stereo stripes subscript wrong");
			return;
		}
	}
	if (nrpn_msb >= 64 && nrpn_msb < 112) {
		if (nnnn >= hercules_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "stripe sensitivity subscript wrong");
			return;
		}
	}
	if (nrpn_msb >= 64 && nrpn_msb < 80) spb = & stripes [nnnn] -> index_sens;
	if (nrpn_msb >= 80 && nrpn_msb < 96) spb = & stripes [nnnn] -> freq_sens;
	if (nrpn_msb >= 96 && nrpn_msb < 112) spb = & stripes [nnnn] -> amp_sens;
	if (nrpn_msb >= 112) {
		if (nnnn >= hercules_stereo_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "pan sensitivity subscript wrong");
			return;
		}
		spb = & stereo [nnnn] -> sens;
	}
	if ((nrpn_msb >= 16 && nrpn_msb < 28) || nrpn_msb >= 64) {
		if ((xxx & 0x3) == 3) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "sensitivity subscript wrong");
			return;
		}
	}
	switch (nrpn_msb) {
	case 0:
		if (data >= 124) data += 4;
		switch (nrpn_lsb) {
		case 0: system -> pitch = data; break;
		case 1: system -> freq = data; break;
		case 2: system -> amp = data; break;
		case 3: system -> index = data; break;
		case 4: system -> attack = data; break;
		case 5: system -> decay = data; break;
		case 6: system -> sustain = data; break;
		case 7: system -> release = data; break;
		case 8: system -> speed = data; break;
		case 9: system -> vibrato = data; break;
		case 10: system -> tremolo = data; break;
		case 11: system -> lfo_index = data; break;
		case 12: system -> lfo_pan = data; break;
		case 13: system -> vector_x = data; break;
		case 14: system -> vector_y = data; break;
		case 15: system -> volume = data; break;
		case 16: system -> pan = data; break;
		case 17: system -> hold = data; break;
		case 18: system -> porta = data; break;
		case 19: system -> porta_time = data; break;
		case 20: system -> porta_mode = data; break;
		case 21: system -> dry_wet = data; break;
		case 22:
			if (! sth -> global_volume_present) break;
			system -> global_volume = data;
			break;
		default:
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 5, nrpn_msb, nrpn_lsb, "unknown controller");
			break;
		}
		break;
	case 1:
		switch (nrpn_lsb) {
		case 0: controllers -> pitch = positive;
			relemat_pitch (); add_all_shift_lemat (); notify_all_shifts_update ();
			break;
		case 1: controllers -> freq = positive;
			relemat_freq_shift (); add_freq_lemat (); notify_freq_shifts_update ();
			break;
		case 2: controllers -> amp = positive;
			relemat_amp_shift (); add_amp_lemat (); notify_amp_shifts_update ();
			break;
		case 3: controllers -> index = positive;
			relemat_index_shift (); add_index_lemat (); notify_index_shifts_update ();
			break;
		case 4: controllers -> attack = positive; notify_attack_change (); break;
		case 5: controllers -> decay = positive; notify_decay_change (); break;
		case 6: controllers -> sustain = positive; notify_sustain_change (); break;
		case 7: controllers -> release = positive; notify_release_change (); break;
		case 8: controllers -> speed = positive;
			relemat_lfo_speed (); add_lfo_speed_lemat (); notify_lfo_speeds_update ();
			break;
		case 9: controllers -> vibrato = positive;
			relemat_vibrato (); add_lfo_freq_lemat (); notify_lfo_freq_shifts_update ();
			break;
		case 10: controllers -> tremolo = positive;
			relemat_tremolo (); add_lfo_amp_lemat (); notify_lfo_amp_shifts_update ();
			break;
		case 11: controllers -> lfo_index = positive;
			relemat_lfo_index (); add_lfo_index_lemat (); notify_lfo_index_shifts_update ();
			break;
		case 12: controllers -> lfo_pan = positive;
			relemat_lfo_pan (); add_lfo_pan_lemat (); notify_lfo_pan_shifts_update ();
			break;
		case 13: control_vector_x (positive); break;
		case 14: control_vector_y (positive); break;
		case 15: controllers -> volume = positive; notify_volume_change (); break;
		case 16: controllers -> pan = positive;
			relemat_pan_shift (); add_pan_lemat (); notify_pan_shifts_update ();
			break;
		case 17: controllers -> hold = data != 0; hold_off (); break; 
		case 18: controllers -> porta = data != 0; notify_porta_update (); break;
		case 19: controllers -> porta_time = positive; notify_porta_update (); break;
		case 20: controllers -> porta_mode = positive; notify_porta_update (); break;
		case 21: controllers -> dry_wet = positive; notify_dry_wet_change (); break;
		case 22:
			if (! sth -> global_volume_present) break;
			if (sth -> global_volume_multi) controllers -> global_volume = positive;
			else sth -> global_volume = positive;
			notify_global_volume_change ();
			break;
		case 23: if (data != 0) poly (true); else mono (true); break;
		default:
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 5, nrpn_msb, nrpn_lsb, "unknown controller");
			break;
		}
		break;
	case 2:
		switch (nrpn_lsb) {
		case 0: category = data; break;
		case 1: system -> bank_msb = data; break;
		case 2: system -> bank_lsb = data; break;
		case 3: system -> program = data; program_change (); break;
		case 4: system -> device_id = positive; break;
		case 5: sth -> tune = negative; notify_tune (); break;
		case 6: system -> arranger_assign = data; break;
		case 7: system -> reserved_notes = data; notify_reserved_notes_changed (); break;
		default: break;
		}
		break;
	case 3:
		switch (nrpn_lsb) {
		case 0:
			if (data != 0) key_on (system -> data_lsb, data);
			else key_off (system -> data_lsb);
			break;
		case 1: ground (); break;
		case 2: reset_all_controllers (); break;
		case 3: key_off (); break;
		default: break;
		}
		break;
	case 4: controllers -> key_tuner [nrpn_lsb] = positive; key_off (); break;
	case 6:
		switch (nrpn_lsb) {
		case 1: controllers -> vector_switch = data != 0; break;
		case 2: controllers -> vector_init_x = positive; break;
		case 3: controllers -> vector_init_y = positive; break;
		case 4:
			if (positive > vector_size) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "vector count out of range");
				return;
			}
			controllers -> vector_frame_count = positive;
			break;
		case 5:
			if (positive > vector_size) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "vector frame selector out of range");
				return;
			}
			controllers -> vector_frame_selector = positive;
			break;
		case 6: vector_frames [controllers -> vector_frame_selector] . delay = positive; break;
		case 7: vector_frames [controllers -> vector_frame_selector] . x = positive; break;
		case 8: vector_frames [controllers -> vector_frame_selector] . y = positive; break;
		case 9: controllers -> vector_resolution = positive; break;
		case 10: controllers -> vector_resolution_seconds = positive; break;
		case 11: controllers -> vector_trigger = data; break;
		case 12: controllers -> vector_attack = positive; break;
		case 13: controllers -> vector_release = positive; break;
		case 14: controllers -> vector_loop_type = data; break;
		case 15:
			if (positive >= vector_size) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "vector loop start point out of range");
				return;
			}
			controllers -> vector_loop_start = positive;
			break;
		case 16:
			if (positive > vector_size) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "vector loop size too big");
				return;
			}
			controllers -> vector_loop_size = positive;
			break;
		case 17: controllers -> vector_loop_delay = positive; break;
		default: break;
		}
		break;
	case 7:
		controllers -> algo_parameters [nrpn_lsb] = positive;
		notify_algo_parameter_changed (nrpn_lsb);
		break;
	case 8:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) dsp [xxx] -> wet_output . send_level = positive;
			else dsp [xxx] -> routers [sub - xxx - 1] -> send_level = positive;
		} else {
			if (sub == xxx) dsp [xxx] -> wet_output . dry_wet . bp = positive;
			else dsp [xxx] -> routers [sub - xxx - 1] -> dry_wet . bp = positive;
		}
		notify_dry_wet_change (xxx);
		break;
	case 9:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) dsp [xxx] -> wet_output . dry_wet . lc  = negative;
			else dsp [xxx] -> routers [sub - xxx - 1] -> dry_wet . lc = negative;
		} else {
			if (sub == xxx) dsp [xxx] -> wet_output . dry_wet . rc = negative;
			else dsp [xxx] -> routers [sub - xxx - 1] -> dry_wet . rc = negative;
		}
		notify_dry_wet_change (xxx);
		break;
	case 10:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) dsp [xxx] -> wet_output . vector_x . bp = positive;
			else dsp [xxx] -> routers [sub - xxx - 1] -> vector_x . bp = positive;
		} else {
			if (sub == xxx) dsp [xxx] -> wet_output . vector_y . bp = positive;
			else dsp [xxx] -> routers [sub - xxx - 1] -> vector_y . bp = positive;
		}
		notify_dry_wet_change (xxx);
		break;
	case 11:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) dsp [xxx] -> wet_output . vector_x . lc = negative;
			else dsp [xxx] -> routers [sub - xxx - 1] -> vector_x . lc = negative;
		} else {
			if (sub == xxx) dsp [xxx] -> wet_output . vector_x . rc = negative;
			else dsp [xxx] -> routers [sub - xxx - 1] -> vector_x . rc = negative;
		}
		notify_dry_wet_change (xxx);
		break;
	case 12:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) dsp [xxx] -> wet_output . vector_y . lc = negative;
			else dsp [xxx] -> routers [sub - xxx - 1] -> vector_y . lc = negative;
		} else {
			if (sub == xxx) dsp [xxx] -> wet_output . vector_y . rc = negative;
			else dsp [xxx] -> routers [sub - xxx - 1] -> vector_y . rc = negative;
		}
		notify_dry_wet_change (xxx);
		break;
	case 13:
		if (xxx >= hercules_all_dsp_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "dsp parameter does not exist");
			break;
		}
		switch (nnnn) {
		case 0: dsp_send [xxx] . send_level = positive; break;
		case 4: dsp_send [xxx] . dry_wet . bp = positive; break;
		case 5: dsp_send [xxx] . dry_wet . lc = negative; break;
		case 6: dsp_send [xxx] . dry_wet . rc = negative; break;
		case 8: dsp_send [xxx] . vector_x . bp = positive; break;
		case 9: dsp_send [xxx] . vector_x . lc = negative; break;
		case 10: dsp_send [xxx] . vector_x . rc = negative; break;
		case 12: dsp_send [xxx] . vector_y . bp = positive; break;
		case 13: dsp_send [xxx] . vector_y . lc = negative; break;
		case 14: dsp_send [xxx] . vector_y . rc = negative; break;
		default: sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 1, nrpn_msb, nrpn_lsb, "dsp parameter does not exist"); break;
		}
		notify_dry_wet_change (xxx);
		break;
	case 14:
		if (nrpn_lsb >= 32 && sth -> global_dsp < 1) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 7, nrpn_msb, nrpn_lsb, "no global dsp parameters");
			break;
		}
		switch (nrpn_lsb) {
		case 16: local_dry_out . send_level = positive; break;
		case 20: local_dry_out . dry_wet . bp = positive; break;
		case 21: local_dry_out . dry_wet . lc = negative; break;
		case 22: local_dry_out . dry_wet . rc = negative; break;
		case 24: local_dry_out . vector_x . bp = positive; break;
		case 25: local_dry_out . vector_x . lc = negative; break;
		case 26: local_dry_out . vector_x . rc = negative; break;
		case 28: local_dry_out . vector_y . bp = positive; break;
		case 29: local_dry_out . vector_y . lc = negative; break;
		case 30: local_dry_out . vector_y . rc = negative; break;
		case 32: global_dry_out . send_level = positive; break;
		case 36: global_dry_out . dry_wet . bp = positive; break;
		case 37: global_dry_out . dry_wet . lc = negative; break;
		case 38: global_dry_out . dry_wet . rc = negative; break;
		case 40: global_dry_out . vector_x . bp = positive; break;
		case 41: global_dry_out . vector_x . lc = negative; break;
		case 42: global_dry_out . vector_x . rc = negative; break;
		case 44: global_dry_out . vector_y . bp = positive; break;
		case 45: global_dry_out . vector_y . lc = negative; break;
		case 46: global_dry_out . vector_y . rc = negative; break;
		default:
			if (nrpn_lsb < hercules_all_dsp_number) {dsp [nrpn_lsb] -> on = (data != 0); notify_dry_wet_change (nrpn_lsb);}
			else sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 1, nrpn_msb, nrpn_lsb, "parameter does not exist");
			break;
		}
		notify_dry_change ();
		break;
	case 15:
		if (xxx >= hercules_all_dsp_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "dsp parameter does not exist");
			break;
		}
		dsp [xxx] -> parameters [nnnn] = positive;
		notify_dsp_change (nrpn_lsb);
		break;
	case 16:
		if (xxx == 0) stripes [nnnn] -> lfo_retrigger = (data != 0);
		else stereo [nnnn] -> lfo_retrigger = (data != 0);
		break;
	case 17:
		if (xxx == 0) {
			if (data > nnnn) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "sync to out of range");
				break;
			}
			if (nnnn >= hercules_number) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "non existent lfo selected");
				break;
			}
			stripes [nnnn] -> lfo_sync = data;
		} else if (xxx == 4) {
			if (data > nnnn + 16) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "sync to out of range");
				break;
			}
			if (nnnn >= hercules_stereo_number) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "non existent pan lfo selected");
				break;
			}
			stereo [nnnn] -> lfo_sync = data;
		} else {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "illegal lfo encoding");
			break;
		}
		notify_lfo_retrigger_required ();
		break;
	case 18:
		if (xxx == 0) stripes [nnnn] -> speed = positive;
		else stereo [nnnn] -> speed = positive;
		add_lfo_speed_lemat ();
		notify_lfo_speeds_update ();
		break;
	case 19:
		if (xxx == 0) stripes [nnnn] -> phase_shift = positive;
		else stereo [nnnn] -> phase_shift = positive;
		notify_lfo_speeds_update ();
		notify_lfo_retrigger_required ();
		break;
	case 20:
		if (xxx == 0) stripes [nnnn] -> pulse_width = negative;
		else stereo [nnnn] -> pulse_width = negative;
		notify_lfo_pulse_width_change ();
		break;
	case 21:
		if (xxx == 0) stripes [nnnn] -> wave = data;
		else stereo [nnnn] -> wave = data;
		notify_lfo_wave_change ();
		break;
	case 24:
		if (xxx < 4) stripes [nnnn] -> lfo_sens . speed . nrpn (xxx, positive, negative);
		else stereo [nnnn] -> lfo_sens . speed . nrpn (xxx, positive, negative);
		break;
	case 25:
		if (xxx < 4) stripes [nnnn] -> lfo_sens . pitch . nrpn (xxx, positive, negative);
		else stereo [nnnn] -> lfo_sens . pitch . nrpn (xxx, positive, negative);
		break;
	case 26:
		if (xxx < 4) stripes [nnnn] -> lfo_sens . vector_x . nrpn (xxx, positive, negative);
		else stereo [nnnn] -> lfo_sens . vector_x . nrpn (xxx, positive, negative);
		break;
	case 27:
		if (xxx < 4) stripes [nnnn] -> lfo_sens . vector_y . nrpn (xxx, positive, negative);
		else stereo [nnnn] -> lfo_sens . vector_y . nrpn (xxx, positive, negative);
		break;
	case 31:
		switch (nrpn_lsb) {
		case 1:
			controllers -> beats_per_seconds = positive;
			notify_tempo_change ();
			break;
		case 2: controllers -> seconds = positive; break;
		case 3:
			controllers -> beats_per_bar = positive;
			notify_division_change ();
			break;
		case 4:
			controllers -> ticks_per_beat = positive;
			notify_sub_division_change ();
			break;
		case 5:
			controllers -> arpeggiator_hold = (data != 0);
			notify_arpeggiator_hold ();
			break;
		case 6:
			controllers -> arpeggiator_switch = (data != 0);
			notify_arpeggiator_switch ();
			break;
		case 7:
			controllers -> variation = positive;
			notify_variation_change ();
			break;
		case 63:
			if (arp == NULL) break;
			arp -> set_transcription (data);
			break;
		default:
			if (nrpn_lsb < 64) {
				sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 1, nrpn_msb, nrpn_lsb, "parameter does not exist");
				return;
			}
			if (arp == NULL) break;
			arp -> set_transcription (nrpn_lsb - 64, data);
			break;
		}
		break;
	case 32: stripes [nnnn] -> index = positive; add_index_lemat (); notify_index_shifts_update (); break;
	case 33: stripes [nnnn] -> lfo_index = positive; add_lfo_index_lemat (); notify_lfo_index_shifts_update (); break;
	case 34: stripes [nnnn] -> index_eg . nrpn_level (xxx, negative); notify_adsr_level_update (nrpn_msb, xxx, nnnn); break;
	case 35: stripes [nnnn] -> index_eg . nrpn_time (xxx, positive); notify_adsr_time_update (nrpn_msb, xxx, nnnn); break;
	case 36: stripes [nnnn] -> index_rnd = positive; notify_rnd_update (); break;
	case 37:
		if ((data >= hercules_number && data < 16) || data >= 16 + hercules_stereo_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "index lfo sync to out of range");
			break;
		}
		stripes [nnnn] -> index_lfo_source = data;
		notify_index_lfo_source_update ();
		break;
	//@ index amp
//	case 36: stripes [nnnn] -> index_amp = (data != 0); notify_index_amp_change (nnnn); break;
	case 40: stripes [nnnn] -> freq = negative; add_freq_lemat (); notify_freq_shifts_update (); break;
	case 41: stripes [nnnn] -> lfo_freq = positive; add_lfo_freq_lemat (); notify_lfo_freq_shifts_update (); break;
	case 42: stripes [nnnn] -> freq_eg . nrpn_level (xxx, negative); notify_adsr_level_update (nrpn_msb, xxx, nnnn); break;
	case 43: stripes [nnnn] -> freq_eg . nrpn_time (xxx, positive); notify_adsr_time_update (nrpn_msb, xxx, nnnn); break;
	case 44: stripes [nnnn] -> freq_rnd = positive; notify_rnd_update (); break;
	case 45:
		if ((data >= hercules_number && data < 16) || data >= 16 + hercules_stereo_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "freq lfo sync to out of range");
			break;
		}
		stripes [nnnn] -> freq_lfo_source = data;
		notify_freq_lfo_source_update ();
		break;
	case 46: stripes [nnnn] -> multiplier = msb_negative; notify_ratio_change (); break;
	case 47: stripes [nnnn] -> retrigger = (data != 0); notify_retrigger_change (); break;
	case 48: stripes [nnnn] -> amp = very_negative; add_amp_lemat (); notify_amp_shifts_update (); break;
	case 49: stripes [nnnn] -> lfo_amp = positive; add_lfo_amp_lemat (); notify_lfo_amp_shifts_update (); break;
	case 50: stripes [nnnn] -> amp_eg . nrpn_level (xxx, very_negative); notify_adsr_level_update (nrpn_msb, xxx, nnnn); break;
	case 51: stripes [nnnn] -> amp_eg . nrpn_time (xxx, positive); notify_adsr_time_update (nrpn_msb, xxx, nnnn); break;
	case 52: stripes [nnnn] -> amp_rnd = positive; notify_rnd_update (); break;
	case 53:
		if ((data >= hercules_number && data < 16) || data >= 16 + hercules_stereo_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "amp lfo sync to out of range");
			break;
		}
		stripes [nnnn] -> amp_lfo_source = data;
		notify_amp_lfo_source_update ();
		break;
	case 56: stereo [nnnn] -> pan = negative; add_pan_lemat (); notify_pan_shifts_update (); break;
	case 57: stereo [nnnn] -> lfo = positive; add_lfo_pan_lemat (); notify_lfo_pan_shifts_update (); break;
	case 58: stereo [nnnn] -> eg . nrpn_level (xxx, negative); notify_adsr_level_update (nrpn_msb, xxx, nnnn); break;
	case 59: stereo [nnnn] -> eg . nrpn_time (xxx, positive); notify_adsr_time_update (nrpn_msb, xxx, nnnn); break;
	case 60: stereo [nnnn] -> rnd = positive; notify_rnd_update (); break;
	case 61:
		if ((data >= hercules_number && data < 16) || data >= 16 + hercules_stereo_number) {
			sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 3, nrpn_msb, nrpn_lsb, "pan lfo sync to out of range");
			break;
		}
		stereo [nnnn] -> pan_lfo_source = data;
		notify_pan_lfo_source_update ();
		break;
	case 64: case 80: case 96: case 112:
		spb -> shift . nrpn (xxx, positive, negative); break;
	case 65: case 81: case 97: case 113:
		spb -> lfo . nrpn (xxx, positive, negative); break;
	case 66: case 82: case 98: case 114:
		spb -> attack . nrpn (xxx, positive, negative); break;
	case 67: case 83: case 99: case 115:
		spb -> decay . nrpn (xxx, positive, negative); break;
	case 68: case 84: case 100: case 116:
		spb -> sustain . nrpn (xxx, positive, negative); break;
	case 69: case 85: case 101: case 117:
		spb -> release . nrpn (xxx, positive, negative); break;
	case 70: case 86: case 102: case 118:
		spb -> key . nrpn (xxx, positive, negative);
		break;
	case 71: case 87: case 103: case 119:
		spb -> velocity . nrpn (xxx, positive, negative); break;
	case 72: case 88: case 104: case 120:
		spb -> vector_x . nrpn (xxx, positive, negative); break;
	case 73: case 89: case 105: case 121:
		spb -> vector_y . nrpn (xxx, positive, negative); break;
	case 74: case 90: case 106: case 122:
		spb -> lfo_x . nrpn (xxx, positive, negative); break;
	case 75: case 91: case 107: case 123:
		spb -> lfo_y . nrpn (xxx, positive, negative); break;
	case 76: case 92: case 108: case 124:
		if (xxx < 4) spb -> key_eg_scaling . nrpn (xxx, positive, negative);
		else spb -> key_attack_eg_scaling . nrpn (xxx, positive, negative);
		break;
	case 77: case 93: case 109: case 125:
		if (xxx < 4) spb -> velocity_eg_scaling . nrpn (xxx, positive, negative);
		else spb -> velocity_attack_eg_scaling . nrpn (xxx, positive, negative);
		break;
	case 78: case 94: case 110: case 126:
		spb -> pitch . nrpn (xxx, positive, negative); break;
	default:
		sth -> send_error (system -> midi_channel_extension, system -> midi_channel & 0xf, 1, nrpn_msb, nrpn_lsb, "parameter does not exist");
		return;
		break;
	}
	if ((nrpn_msb >= 16 && nrpn_msb < 28) || nrpn_msb >= 64) notify_sensitivity_change (nrpn_msb, xxx, nnnn);
}

void sub_parameter_block :: nrpn (int selector, int positive, int negative) {
	selector &= 3;
	switch (selector) {
	case 1: lc = negative; return;
	case 2: rc = negative; return;
	default: bp = positive;
	}
}

void adsr_parameter_block :: nrpn_level (int selector, int value) {
	level [selector] = value;
}

void adsr_parameter_block :: nrpn_time (int selector, int value) {
	time [selector] = value;
}
