///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"

void synthesizer :: send_identity_reply (int extension, int channel) {
	if (midi_out == NULL) return;
	midi_out -> insert_channel_extension (extension);
	midi_out -> open_generic_system_exclusive ();
	midi_out -> insert (0x7e);
	midi_out -> insert (channel); //midi_out -> insert (0x7f);
	midi_out -> insert (0x06);
	midi_out -> insert (0x02);
	midi_out -> insert_manufacturers_id ();
	midi_out -> insert_product_id ();
	midi_out -> insert_product_version ();
	midi_out -> close_system_exclusive ();
}

void synthesizer :: send_scale_bulk_dump (int device_id, int program) {
	if (midi_out == NULL) return;
	parameter_block * pb;
	controllers_parameter_block * cb;
	int device, part, ind, tune;
	for (part = 0; part < multitimbral; part++) {
		pb = data_blocks [part];
		device = pb -> system -> device_id;
		if (device == device_id || device == 0x7f || device_id == 0x7f) {
			cb = pb -> controllers;
			midi_out -> open_generic_system_exclusive ();
			midi_out -> insert (0x7e);
			midi_out -> insert (device);
			midi_out -> insert (0x08);
			midi_out -> insert (0x01);
			midi_out -> insert (program & 0x7f);
			midi_out -> insert ("HERCs scale dump");
			for (ind = 0; ind < 128; ind++) {
				tune = cb -> key_tuner [ind];
				midi_out -> insert (tune >> 7);
				midi_out -> insert (tune & 0x7f);
				midi_out -> insert (0);
			}
			midi_out -> insert_data_xor ();
			midi_out -> close_system_exclusive ();
		}
	}
}

void synthesizer :: send_midi_channel_extension (int extension) {
	if (midi_out == NULL) return;
	midi_out -> insert_channel_extension (extension);
}

void synthesizer :: send_message (int msb, int extension, int channel, int lsb, char * text) {
	if (midi_out == NULL) return;
	msb &= 0x70;
	channel &= 0x0f;
	msb |= channel;
	lsb &= 0x7f;
	midi_out -> insert_channel_extension (extension);
	midi_out -> open_system_exclusive ();
	midi_out -> insert (msb);
	midi_out -> insert (lsb);
	midi_out -> insert (text);
	midi_out -> close_system_exclusive ();
}

void synthesizer :: send_message (int msb, int channel, int lsb, char * text) {
	if (midi_out == NULL) return;
	msb &= 0x70;
	channel &= 0x0f;
	msb |= channel;
	lsb &= 0x7f;
	midi_out -> open_system_exclusive ();
	midi_out -> insert (msb);
	midi_out -> insert (lsb);
	midi_out -> insert (text);
	midi_out -> close_system_exclusive ();
}

void synthesizer :: send_message (int channel, int id, char * message) {
	send_message (0x10, channel, id, message);
}

void synthesizer :: send_error (int channel, int error, int msb, int lsb, char * text) {
	if (midi_out == NULL) return;
	channel &= 0x0f;
	channel |= 0x70;
	midi_out -> open_system_exclusive ();
	midi_out -> insert (channel);
	midi_out -> insert (error);
	midi_out -> insert (msb);
	midi_out -> insert (lsb);
	midi_out -> insert (text);
	midi_out -> close_system_exclusive ();
}

void synthesizer :: send_error (int extension, int channel, int error, int msb, int lsb, char * text) {
	if (midi_out == NULL) return;
	channel &= 0x0f;
	channel |= 0x70;
	midi_out -> insert_channel_extension (extension);
	midi_out -> open_system_exclusive ();
	midi_out -> insert (channel);
	midi_out -> insert (error);
	midi_out -> insert (msb);
	midi_out -> insert (lsb);
	midi_out -> insert (text);
	midi_out -> close_system_exclusive ();
}

void synthesizer :: send_transport_data (int sub_selector) {
	if (midi_out == NULL) return;
	if (root == NULL) return;
	int sub = -1;
	switch (sub_selector) {
	case 0x8: sub = root -> getTransportBeatsPerSeconds (); break;
	case 0x9: sub = root -> getTransportSeconds (); break;
	case 0xa: sub = root -> getTransportBeatsPerBar (); break;
	case 0xb: sub = root -> getTransportTicksPerBeat (); break;
	default: break;
	}
	if (sub < 0) return;
	midi_out -> open_system_exclusive ();
	midi_out -> insert (0x34);
	midi_out -> insert (sub_selector);
	midi_out -> insert (sub & 0x7f);
	sub >>= 7;
	midi_out -> insert (sub & 0x7f);
	midi_out -> close_system_exclusive ();
}

void synthesizer :: process_transport_request (int sub_selector, midi_stream * line) {
	if (root == NULL) return;
	int top;
	switch (sub_selector) {
	case 0x0: root -> stopTransport (); break;
	case 0x1: root -> startTransport (); break;
	case 0x2: root -> pauseTransport (); break;
	case 0xc: top = line -> get (); root -> transportMetrum (top, line -> get ()); break;
	default: send_transport_data (sub_selector); break;
	}
}

void synthesizer :: process_transport_data (int sub_selector, midi_stream * line) {
	if (root == NULL) return;
	switch (sub_selector) {
	case 0x8:
		if (transport_seconds != 0) root -> transportTempo (line -> get_int (), transport_seconds);
		else root -> transportTempo (line -> get_int ());
		transport_seconds = 0;
		break;
	case 0x9: transport_seconds = line -> get_int (); break;
	case 0xa: root -> transportDivision (line -> get_int ()); break;
	case 0xb: root -> transportTickDivision (line -> get_int ()); break;
	}
}

void parameter_block :: send_algo_string (int channel) {
	sth -> send_message (0x10, channel, 0x10, algo_head -> toString (this));
}

void parameter_block :: send_stripe_string (int channel, int stripe) {
	sth -> send_message (0x10, channel, stripe, algo_head -> toString (this, stripe));
}

void parameter_block :: send_dsp_string (int channel, int ind) {
	sth -> send_message (0x10, channel, 0x18 + ind, dsp [ind] -> algo_head -> toString (this));
}

void parameter_block :: send_program_name (int channel) {
	sth -> send_message (0x10, channel, 0x11, name);
}

void parameter_block :: send_bank_msb_name (int channel) {
	bank * bptr = sth -> banks [system -> bank_msb];
	if (bptr == NULL) sth -> send_message (0x10, channel, 0x12, "NOT INSTALLED");
	else sth -> send_message (0x10, channel, 0x12, bptr -> toString ());
}

void parameter_block :: send_bank_lsb_name (int channel) {
	bank * bptr = sth -> banks [system -> bank_msb];
	if (bptr == NULL) sth -> send_message (0x10, channel, 0x13, "NOT INSTALLED");
	else sth -> send_message (0x10, channel, 0x13, bptr -> toString (system -> bank_lsb));
}

void parameter_block :: send_category_name (int channel) {
	sth -> send_message (0x10, channel, 0x14, sth -> get_category (category));
}

void parameter_block :: send_style_name (int channel) {
	sth -> send_message (0x10, channel, 0x16, pattern_head -> styleName (this));
}

void parameter_block :: send_pattern_name (int channel) {
	sth -> send_message (0x10, channel, 0x15, pattern_head -> toString (this));
}

void parameter_block :: send_indexed_algo_parameter_name (int channel, int ind) {
	midi_stream * midi_out = sth -> midi_out;
	if (midi_out == NULL) return;
	ind &= 0xff;
	if (ind > 0x7f) {
		for (ind = 0; ind < 128; ind++) {
			midi_out -> open_system_exclusive ();
			midi_out -> insert (0x50 | channel);
			midi_out -> insert (0x22);
			midi_out -> insert (ind);
			midi_out -> insert (algo_head -> get_name (ind));
			midi_out -> close_system_exclusive ();
		}
		return;
	}
	midi_out -> open_system_exclusive ();
	midi_out -> insert (0x50 | channel);
	midi_out -> insert (0x22);
	midi_out -> insert (ind);
	midi_out -> insert (algo_head -> get_name (ind));
	midi_out -> close_system_exclusive ();
}

void parameter_block :: send_indexed_dsp_parameter_name (int channel, int ind) {
	midi_stream * midi_out = sth -> midi_out;
	if (midi_out == NULL) return;
	ind &= 0xff;
	int xxx = ind >> 4;
	int nnnn = ind & 0xf;
	if (xxx >= 8) {
		for (xxx = 0; xxx < hercules_all_dsp_number; xxx++)
			for (nnnn = 0; nnnn < 16; nnnn++) {
				midi_out -> open_system_exclusive ();
				midi_out -> insert (0x50 | channel);
				midi_out -> insert (0x23);
				midi_out -> insert ((xxx << 4) + nnnn);
				midi_out -> insert (dsp [xxx] -> algo_head -> toString (this, nnnn));
				midi_out -> close_system_exclusive ();
			}
		return;
	}
	if (xxx >= hercules_all_dsp_number) {
		sth -> send_error (channel, 7, 0x7f, ind, "dsp parameter does not exist");
		return;
	}
	midi_out -> open_system_exclusive ();
	midi_out -> insert (0x50 | channel);
	midi_out -> insert (0x23);
	midi_out -> insert (ind);
	midi_out -> insert (dsp [xxx] -> algo_head -> toString (this, nnnn));
	midi_out -> close_system_exclusive ();
}

void parameter_block :: send_indexed_category_name (int channel, int ind) {
	midi_stream * midi_out = sth -> midi_out;
	if (midi_out == NULL) return;
	ind &= 0xff;
	if (ind < 128 && ind >= sth -> category_pointer) {
		sth -> send_error (channel, 3, 0x7f, ind, "category not installed");
		return;
	}
	channel &= 0xf;
	if (ind < 128) {
		midi_out -> open_system_exclusive ();
		midi_out -> insert (0x50 | channel);
		midi_out -> insert (0x21);
		midi_out -> insert (ind);
		midi_out -> insert (sth -> categories [ind]);
		midi_out -> close_system_exclusive ();
		return;
	}
	for (int sub = 0; sub < sth -> category_pointer; sub++) {
		midi_out -> open_system_exclusive ();
		midi_out -> insert (0x50 | channel);
		midi_out -> insert (0x21);
		midi_out -> insert (sub);
		midi_out -> insert (sth -> categories [sub]);
		midi_out -> close_system_exclusive ();
	}
}

void parameter_block :: send_midi_channel (void) {
	midi_stream * midi_out = sth -> midi_out;
	if (midi_out == NULL) return;
	midi_out -> open_system_exclusive ();
	midi_out -> insert (0x30);
	midi_out -> insert (system -> device_id);
	midi_out -> insert (system -> midi_channel);
	midi_out -> close_system_exclusive ();
}

void parameter_block :: send_midi_channel_extension (void) {
	midi_stream * midi_out = sth -> midi_out;
	if (midi_out == NULL) return;
	midi_out -> open_system_exclusive ();
	midi_out -> insert (0x31);
	midi_out -> insert (system -> device_id);
	midi_out -> insert (system -> midi_channel_extension);
	midi_out -> close_system_exclusive ();
}

void parameter_block :: send_arranger_status (void) {
	midi_stream * midi_out = sth -> midi_out;
	if (midi_out == NULL) return;
	midi_out -> open_system_exclusive ();
	midi_out -> insert (0x33);
	midi_out -> insert (system -> device_id);
	midi_out -> insert (system -> arranger_assign);
	midi_out -> close_system_exclusive ();
}

void parameter_block :: send_reserved_notes (void) {
	midi_stream * midi_out = sth -> midi_out;
	if (midi_out == NULL) return;
	midi_out -> open_system_exclusive ();
	midi_out -> insert (0x32);
	midi_out -> insert (system -> device_id);
	midi_out -> insert (system -> reserved_notes);
	midi_out -> close_system_exclusive ();
}

void parameter_block :: send_vector_recorded_info (void) {
	midi_stream * midi_out = sth -> midi_out;
	if (midi_out == NULL) return;
	int channel = system -> midi_channel & 0xf;
	midi_out -> open_system_exclusive ();
	midi_out -> insert (0x50 | channel);
	midi_out -> insert (0x12);
	midi_out -> close_system_exclusive ();
}

void parameter_block :: send_sub_parameter_block (int channel, int nrpn_msb, int nrpn_lsb, sub_parameter_block * pb) {
	int xxx = nrpn_lsb >> 4;
	xxx &= 0x3;
	int msb;
	switch (xxx) {
	case 0: msb = pb -> bp; break;
	case 1: msb = pb -> lc; break;
	case 2: msb = pb -> rc; break;
	default: sth -> send_error (channel, 3, nrpn_msb, nrpn_lsb, "control processor subscript wrong"); return; break;
	}
	int lsb = msb & 0x7f;
	msb >>= 7;
	msb &= 0x7f;
	sth -> midi_out -> INSERT_NRPN (channel, nrpn_msb, nrpn_lsb, msb, lsb);
}

void parameter_block :: send_adsr_level (int channel, int nrpn_msb, int nrpn_lsb, adsr_parameter_block * pb) {
	int xxx = nrpn_lsb >> 4;
	if (xxx > 3) {sth -> send_error (channel, 3, nrpn_msb, nrpn_lsb, "adsr level subscript wrong"); return;}
	int msb = pb -> level [xxx];
	int lsb = msb& 0x7f;
	msb >>= 7;
	msb &= 0x7f;
	sth -> midi_out -> INSERT_NRPN (channel, nrpn_msb, nrpn_lsb, msb, lsb);
}

void parameter_block :: send_adsr_time (int channel, int nrpn_msb, int nrpn_lsb, adsr_parameter_block * pb) {
	int xxx = nrpn_lsb >> 4;
	if (xxx > 3) {sth -> send_error (channel, 3, nrpn_msb, nrpn_lsb, "adsr time subscript wrong"); return;}
	int msb = pb -> time [xxx];
	int lsb = msb & 0x7f;
	msb >>= 7;
//	msb &= 0x7f;
	sth -> midi_out -> INSERT_NRPN (channel, nrpn_msb, nrpn_lsb, msb, lsb);
}

void parameter_block :: send_parameter_value (int channel) {
	if (sth -> midi_out == NULL) return;
	int nrpn_msb = system -> nrpn_msb;
	int nrpn_lsb = system -> nrpn_lsb;
	int xxx = nrpn_lsb >> 4;
	int nnnn = nrpn_lsb & 0x0f;
	int data_msb = -70000;
	int data_lsb = -70000;
	int sub = 0;
	bool error = false;
	if (nrpn_msb >= 16 && nrpn_msb < 28) {
		if (xxx < 4) {if (nnnn >= hercules_number) error = true;}
		else {if (nnnn >= hercules_stereo_number) error = true;}
	}
	if (nrpn_msb >= 32 && nrpn_msb <= 53) {if (nnnn >= hercules_number) error = true;}
	if (nrpn_msb >= 56 && nrpn_msb <= 60) {if (nnnn >= hercules_stereo_number) error = true;}
//	if (nrpn_msb >= 64 && nrpn_msb < 79) {if (nnnn >= hercules_number) error = true;}
//	if (nrpn_msb >= 80 && nrpn_msb < 95) {if (nnnn >= hercules_number) error = true;}
//	if (nrpn_msb >= 96 && nrpn_msb < 111) {if (nnnn >= hercules_number) error = true;}
	if (nrpn_msb >= 64 && nrpn_msb < 112) {if (nnnn >= hercules_number) error = true;}
	if (nrpn_msb >= 112 && nrpn_msb < 127) {if (nnnn >= hercules_stereo_number) error = true;}
	if (error) {sth -> send_error (channel, 3, nrpn_msb, nrpn_lsb, "stripe subscript wrong"); return;}
	switch (nrpn_msb) {
	case 0:
		switch (nrpn_lsb) {
		case 0: data_msb = system -> pitch; break;
		case 1: data_msb = system -> freq; break;
		case 2: data_msb = system -> amp; break;
		case 3: data_msb = system -> index; break;
		case 4: data_msb = system -> attack; break;
		case 5: data_msb = system -> decay; break;
		case 6: data_msb = system -> sustain; break;
		case 7: data_msb = system -> release; break;
		case 8: data_msb = system -> speed; break;
		case 9: data_msb = system -> vibrato; break;
		case 10: data_msb = system -> tremolo; break;
		case 11: data_msb = system -> lfo_index; break;
		case 12: data_msb = system -> lfo_pan; break;
		case 13: data_msb = system -> vector_x; break;
		case 14: data_msb = system -> vector_y; break;
		case 15: data_msb = system -> volume; break;
		case 16: data_msb = system -> pan; break;
		case 17: data_msb = system -> hold; break;
		case 18: data_msb = system -> porta; break;
		case 19: data_msb = system -> porta_time; break;
		case 20: data_msb = system -> porta_mode; break;
		case 21: data_msb = system -> dry_wet; break;
		case 22:
			if (! sth -> global_volume_present) {
				sth -> send_error (channel, 1, nrpn_msb, nrpn_lsb, "global volume does not exist");
				return;
			}
			data_msb = system -> global_volume;
			break;
		default: sth -> send_error (channel, 1, nrpn_msb, nrpn_lsb, "system parameter does not exist"); return;
		}
		if (data_msb > 127) data_msb -= 4;
		break;
	case 1:
		switch (nrpn_lsb) {
		case 0: sub = controllers -> pitch; break;
		case 1: sub = controllers -> freq; break;
		case 2: sub = controllers -> amp; break;
		case 3: sub = controllers -> index; break;
		case 4: sub = controllers -> attack; break;
		case 5: sub = controllers -> decay; break;
		case 6: sub = controllers -> sustain; break;
		case 7: sub = controllers -> release; break;
		case 8: sub = controllers -> speed; break;
		case 9: sub = controllers -> vibrato; break;
		case 10: sub = controllers -> tremolo; break;
		case 11: sub = controllers -> lfo_index; break;
		case 12: sub = controllers -> lfo_pan; break;
		case 13: sub = controllers -> vector_x; break;
		case 14: sub = controllers -> vector_y; break;
		case 15: sub = controllers -> volume; break;
		case 16: sub = controllers -> pan; break;
		case 17: sub = (controllers -> hold ? 127 : 0); break;
		case 18: sub = (controllers -> porta ? 127 : 0); break;
		case 19: sub = controllers -> porta_time; break;
		case 20: sub = controllers -> porta_mode; break;
		case 21: sub = controllers -> dry_wet; break;
		case 22:
			if (! sth -> global_volume_present) {
				sth -> send_error (channel, 1, nrpn_msb, nrpn_lsb, "global volume parameter does not exist");
				return;
			}
			if (sth -> global_volume_multi) sub = controllers -> global_volume;
			else sub = sth -> global_volume;
			break;
		case 23: sub = (controllers -> poly ? 127 : 0); break;
		default: sth -> send_error (channel, 1, nrpn_msb, nrpn_lsb, "control parameter does not exist"); return;
		}
		switch (nrpn_lsb) {
		case 17: case 18: case 23:
			data_msb = sub;
			break;
		default:
			data_msb = sub >> 7;
			data_lsb = sub & 0x7f;
			break;
		}
		break;
	case 2:
		switch (nrpn_lsb) {
		case 0: data_msb = category; break;
		case 1: data_msb = system -> bank_msb; break;
		case 2: data_msb = system -> bank_lsb; break;
		case 3: data_msb = system -> program; break;
		case 4:
			data_msb = system -> device_id;
			data_lsb = data_msb & 0x7f;
			data_msb >>= 7;
			break;
		case 5:
			data_msb = sth -> tune;
			data_lsb = data_msb & 0x7f;
			data_msb >>= 7;
			data_msb &= 0x7f;
			break;
		case 6: data_msb = system -> arranger_assign; break;
		case 7: data_msb = system -> reserved_notes; break;
		default: sth -> send_error (channel, 1, nrpn_msb, nrpn_lsb, "control parameter does not exist"); return;
		}
		break;
	case 4:
		data_msb = controllers -> key_tuner [nrpn_lsb];
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		break;
	case 6:
		switch (nrpn_lsb) {
		case 1: data_msb = controllers -> vector_switch ? 127 : 0; break;
		case 2: data_msb = controllers -> vector_init_x; break;
		case 3: data_msb = controllers -> vector_init_y; break;
		case 4: data_msb = controllers -> vector_frame_count; break;
		case 5: data_msb = controllers -> vector_frame_selector; break;
		case 6: data_msb = vector_frames [controllers -> vector_frame_selector] . delay; break;
		case 7: data_msb = vector_frames [controllers -> vector_frame_selector] . x; break;
		case 8: data_msb = vector_frames [controllers -> vector_frame_selector] . y; break;
		case 9: data_msb = controllers -> vector_resolution; break;
		case 10: data_msb = controllers -> vector_resolution_seconds; break;
		case 11: data_msb = controllers -> vector_trigger; break;
		case 12: data_msb = controllers -> vector_attack; break;
		case 13: data_msb = controllers -> vector_release; break;
		case 14: data_msb = controllers -> vector_loop_type; break;
		case 15: data_msb = controllers -> vector_loop_start; break;
		case 16: data_msb = controllers -> vector_loop_size; break;
		case 17: data_msb = controllers -> vector_loop_delay; break;
		default: break;
		}
		switch (nrpn_lsb) {
		case 1: case 11: case 14: break;
		default:
			if (nrpn_lsb < 1 || nrpn_lsb > 17) break;
			data_lsb = data_msb & 0x7f;
			data_msb >>= 7;
			data_msb &= 0x7f;
			break;
		}
		break;
	case 7:
		data_msb = controllers -> algo_parameters [nrpn_lsb];
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		data_msb &= 0x7f;
		break;
	case 8:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (channel, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . send_level;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> send_level;
		} else {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . dry_wet . bp;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> dry_wet . bp;
		}
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		data_msb &= 0x7f;
		break;
	case 9:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (channel, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . dry_wet . lc;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> dry_wet . lc;
		} else {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . dry_wet . rc;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> dry_wet . rc;
		}
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		data_msb &= 0x7f;
		break;
	case 10:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (channel, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . vector_x . bp;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> vector_x . bp;
		} else {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . vector_y . bp;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> vector_y . bp;
		}
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		data_msb &= 0x7f;
		break;
	case 11:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (channel, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . vector_x . lc;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> vector_x . lc;
		} else {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . vector_x . rc;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> vector_x . rc;
		}
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		data_msb &= 0x7f;
		break;
	case 12:
		sub = nnnn & 0x7;
		if (xxx >= hercules_all_dsp_number || sub < xxx || sub >= hercules_all_dsp_number || (sub >= hercules_local_dsp_number && xxx < hercules_local_dsp_number)) {
			sth -> send_error (channel, 7, nrpn_msb, nrpn_lsb, "dsp router parameter does not exist");
			return;
		}
		if (nnnn < 8) {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . vector_y . lc;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> vector_y . lc;
		} else {
			if (sub == xxx) data_msb = dsp [xxx] -> wet_output . vector_y . rc;
			else data_msb = dsp [xxx] -> routers [sub - xxx - 1] -> vector_y . rc;
		}
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		data_msb &= 0x7f;
		break;
	case 13:
		if (xxx >= hercules_all_dsp_number) {
			sth -> send_error (channel, 3, nrpn_msb, nrpn_lsb, "dsp parameter does not exist");
			return;
		}
		switch (nnnn) {
		case 0: data_msb = dsp_send [xxx] . send_level; break;
		case 4: data_msb = dsp_send [xxx] . dry_wet . bp; break;
		case 5: data_msb = dsp_send [xxx] . dry_wet . lc; break;
		case 6: data_msb = dsp_send [xxx] . dry_wet . rc; break;
		case 8: data_msb = dsp_send [xxx] . vector_x . bp; break;
		case 9: data_msb = dsp_send [xxx] . vector_x . lc; break;
		case 10: data_msb = dsp_send [xxx] . vector_x . rc; break;
		case 12: data_msb = dsp_send [xxx] . vector_y . bp; break;
		case 13: data_msb = dsp_send [xxx] . vector_y . lc; break;
		case 14: data_msb = dsp_send [xxx] . vector_y . rc; break;
		default: sth -> send_error (channel, 1, nrpn_msb, nrpn_lsb, "dsp parameter does not exist"); return;
		}
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		data_msb &= 0x7f;
		break;
	case 14:
		if (nrpn_lsb >= 32 && sth -> global_dsp < 1) {
			sth -> send_error (channel, 7, nrpn_msb, nrpn_lsb, "no global dsp parameters");
			return;
		}
		switch (nrpn_lsb) {
		case 16: data_msb = local_dry_out . send_level; break;
		case 20: data_msb = local_dry_out . dry_wet . bp; break;
		case 21: data_msb = local_dry_out . dry_wet . lc; break;
		case 22: data_msb = local_dry_out . dry_wet . rc; break;
		case 24: data_msb = local_dry_out . vector_x . bp; break;
		case 25: data_msb = local_dry_out . vector_x . lc; break;
		case 26: data_msb = local_dry_out . vector_x . rc; break;
		case 28: data_msb = local_dry_out . vector_y . bp; break;
		case 29: data_msb = local_dry_out . vector_y . lc; break;
		case 30: data_msb = local_dry_out . vector_y . rc; break;
		case 32: data_msb = global_dry_out . send_level; break;
		case 36: data_msb = global_dry_out . dry_wet . bp; break;
		case 37: data_msb = global_dry_out . dry_wet . lc; break;
		case 38: data_msb = global_dry_out . dry_wet . rc; break;
		case 40: data_msb = global_dry_out . vector_x . bp; break;
		case 41: data_msb = global_dry_out . vector_x . lc; break;
		case 42: data_msb = global_dry_out . vector_x . rc; break;
		case 44: data_msb = global_dry_out . vector_y . bp; break;
		case 45: data_msb = global_dry_out . vector_y . lc; break;
		case 46: data_msb = global_dry_out . vector_y . rc; break;
		default:
			if (nrpn_lsb < hercules_all_dsp_number) data_msb = (dsp [nrpn_lsb] -> on ? 127 : 0);
			else {
				sth -> send_error (channel, 1, nrpn_msb, nrpn_lsb, "parameter does not exist");
				return;
			}
		}
		if (nrpn_lsb < 8) break;
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		data_msb &= 0x7f;
		break;
	case 15:
		if (xxx >= hercules_all_dsp_number) {
			sth -> send_error (channel, 3, nrpn_msb, nrpn_lsb, "dsp parameter does not exist");
			return;
		}
		data_msb = dsp [xxx] -> parameters [nnnn];
		data_lsb = data_msb & 0x7f;
		data_msb >>= 7;
		data_msb &= 0x7f;
		break;
	case 16:
		if (xxx == 0) data_msb = (stripes [nnnn] -> lfo_retrigger ? 127 : 0);
		else data_msb = (stereo [nnnn] -> lfo_retrigger ? 127 : 0);
		break;
	case 17:
		if (xxx == 0) data_msb = stripes [nnnn] -> lfo_sync;
		else data_msb = stereo [nnnn] -> lfo_sync;
		break;
	case 18:
		if (xxx == 0) sub = stripes [nnnn] -> speed;
		else sub = stereo [nnnn] -> speed;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 19:
		if (xxx == 0) sub = stripes [nnnn] -> phase_shift;
		else sub = stereo [nnnn] -> phase_shift;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 20:
		if (xxx == 0) sub = stripes [nnnn] -> pulse_width;
		else sub = stereo [nnnn] -> pulse_width;
		sub &= 0x3fff;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 21:
		if (xxx == 0) data_msb = stripes [nnnn] -> wave;
		else data_msb = stereo [nnnn] -> wave;
		break;

	case 24: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, xxx < 4 ? & stripes [nnnn] -> lfo_sens . speed : & stereo [nnnn] -> lfo_sens . speed); return;
	case 25: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, xxx < 4 ? & stripes [nnnn] -> lfo_sens . pitch : & stereo [nnnn] -> lfo_sens . pitch); return;
	case 26: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, xxx < 4 ? & stripes [nnnn] -> lfo_sens . vector_x : & stereo [nnnn] -> lfo_sens . vector_x); return;
	case 27: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, xxx < 4 ? & stripes [nnnn] -> lfo_sens . vector_y : & stereo [nnnn] -> lfo_sens . vector_y); return;

	case 31:
		switch (nrpn_lsb) {
		case 1:
			sub = controllers -> beats_per_seconds;
			data_msb = sub >> 7;
			data_lsb = sub & 0x7f;
			break;
		case 2:
			sub = controllers -> seconds;
			data_msb = sub >> 7;
			data_lsb = sub & 0x7f;
			break;
		case 3:
			sub = controllers -> beats_per_bar;
			data_msb = sub >> 7;
			data_lsb = sub & 0x7f;
			break;
		case 4:
			sub = controllers -> ticks_per_beat;
			data_msb = sub >> 7;
			data_lsb = sub & 0x7f;
			break;
		case 5: data_msb = controllers -> arpeggiator_hold ? 127 : 0; break;
		case 6: data_msb = controllers -> arpeggiator_switch ? 127 : 0; break;
		case 7:
			sub = controllers -> variation;
			data_msb = sub >> 7;
			data_lsb = sub & 0x7f;
			break;
		default: sth -> send_error (channel, 1, nrpn_msb, nrpn_lsb, "dsp parameter does not exist"); return;
		}
		break;

	case 32:
		sub = stripes [nnnn] -> index;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 33:
		sub = stripes [nnnn] -> lfo_index;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 34: send_adsr_level (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_eg); return;
	case 35: send_adsr_time (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_eg); return;
	case 36:
		sub = stripes [nnnn] -> index_rnd;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 37: data_msb = stripes [nnnn] -> index_lfo_source; break;
	//@ index amp
//	case 36:
//		data_msb = (stripes [nnnn] -> index_amp ? 127 : 0);
//		break;

	case 40:
		sub = stripes [nnnn] -> freq;
		data_msb = sub >> 7;
		data_msb &= 0x7f;
		data_lsb = sub & 0x7f;
		break;
	case 41:
		sub = stripes [nnnn] -> lfo_freq;
		data_msb = sub >> 7;
//		data_msb &= 0x7f;
		data_lsb = sub & 0x7f;
		break;
	case 42: send_adsr_level (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_eg); return;
	case 43: send_adsr_time (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_eg); return;
	case 44:
		sub = stripes [nnnn] -> freq_rnd;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 45: data_msb = stripes [nnnn] -> freq_lfo_source; break;
	case 46:
		data_msb = stripes [nnnn] -> multiplier;
		data_msb &= 0x7f;
		break;
	case 47:
		data_msb = (stripes [nnnn] -> retrigger ? 127 : 0);
		break;

	case 48:
		sub = stripes [nnnn] -> amp;
		data_msb = sub >> 7;
		data_msb &= 0x7f;
		data_lsb = sub & 0x7f;
		break;
	case 49:
		sub = stripes [nnnn] -> lfo_amp;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 50: send_adsr_level (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_eg); return;
	case 51: send_adsr_time (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_eg); return;
	case 52:
		sub = stripes [nnnn] -> amp_rnd;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 53: data_msb = stripes [nnnn] -> amp_lfo_source; break;

	case 56:
		sub = stereo [nnnn] -> pan;
		data_msb = sub >> 7;
		data_msb &= 0x7f;
		data_lsb = sub & 0x7f;
		break;
	case 57:
		sub = stereo [nnnn] -> lfo;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 58: send_adsr_level (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> eg); return;
	case 59: send_adsr_time (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> eg); return;
	case 60:
		sub = stereo [nnnn] -> rnd;
		data_msb = sub >> 7;
		data_lsb = sub & 0x7f;
		break;
	case 61: data_msb = stereo [nnnn] -> pan_lfo_source; break;

	case 64: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . shift); return;
	case 65: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . lfo); return;
	case 66: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . attack); return;
	case 67: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . decay); return;
	case 68: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . sustain); return;
	case 69: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . release); return;
	case 70: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . key); return;
	case 71: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . velocity); return;
	case 72: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . vector_x); return;
	case 73: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . vector_y); return;
	case 74: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . lfo_x); return;
	case 75: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . lfo_y); return;
	case 76: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . key_eg_scaling); return;
	case 77: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . velocity_eg_scaling); return;
	case 78: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> index_sens . pitch); return;

	case 80: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . shift); return;
	case 81: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . lfo); return;
	case 82: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . attack); return;
	case 83: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . decay); return;
	case 84: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . sustain); return;
	case 85: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . release); return;
	case 86: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . key); return;
	case 87: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . velocity); return;
	case 88: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . vector_x); return;
	case 89: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . vector_y); return;
	case 90: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . lfo_x); return;
	case 91: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . lfo_y); return;
	case 92: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . key_eg_scaling); return;
	case 93: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . velocity_eg_scaling); return;
	case 94: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> freq_sens . pitch); return;

	case 96: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . shift); return;
	case 97: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . lfo); return;
	case 98: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . attack); return;
	case 99: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . decay); return;
	case 100: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . sustain); return;
	case 101: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . release); return;
	case 102: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . key); return;
	case 103: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . velocity); return;
	case 104: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . vector_x); return;
	case 105: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . vector_y); return;
	case 106: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . lfo_x); return;
	case 107: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . lfo_y); return;
	case 108: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . key_eg_scaling); return;
	case 109: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . velocity_eg_scaling); return;
	case 110: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stripes [nnnn] -> amp_sens . pitch); return;

	case 112: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . shift); return;
	case 113: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . lfo); return;
	case 114: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . attack); return;
	case 115: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . decay); return;
	case 116: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . sustain); return;
	case 117: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . release); return;
	case 118: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . key); return;
	case 119: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . velocity); return;
	case 120: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . vector_x); return;
	case 121: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . vector_y); return;
	case 122: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . lfo_x); return;
	case 123: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . lfo_y); return;
	case 124: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . key_eg_scaling); return;
	case 125: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . velocity_eg_scaling); return;
	case 126: send_sub_parameter_block (channel, nrpn_msb, nrpn_lsb, & stereo [nnnn] -> sens . pitch); return;

	case 127: return;
	default: sth -> send_error (channel, 1, nrpn_msb, nrpn_lsb, "parameter not found"); return;
	}
	if (data_msb > -70000) {
		if (data_lsb > -70000) sth -> midi_out -> INSERT_NRPN (channel, nrpn_msb, nrpn_lsb, data_msb, data_lsb);
		else sth -> midi_out -> INSERT_NRPN (channel, nrpn_msb, nrpn_lsb, data_msb);
	}
}
