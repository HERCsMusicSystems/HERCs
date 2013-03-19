///////////////////////////////////////
// Copyright (C) 2003 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "data.h"
#include "info_display.h"

info_display :: info_display (config * cfg) {

	nrpn_msb = 127;
	nrpn_lsb = 127;
	data_lsb = 0;
	freq_msb = 0;
	freq_lsb = 0;
	vector_x = 0;
	vector_y = 0;

	pitch_id = 128;
	freq_id = 74;
	amp_id = 11;
	index_id = 71;
	attack_id = 73;
	decay_id = 76;
	sustain_id = 75;
	release_id = 72;
	speed_id = 77;
	vibrato_id = 1;
	tremolo_id = 1;
	lfo_index_id = 1;
	lfo_pan_id = 1;
	vector_x_id = 16;
	vector_y_id = 17;
	volume_id = 7;
	pan_id = 10;
	hold_id = 64;
	porta_id = 65;
	porta_time_id = 5;
	porta_mode_id = 84;

	part_number = 0;
	device_id_number = 0;
	midi_channel_number = 0;
	category_number = 0;
	program_number = 0;
	bank_msb_number = 0;
	bank_lsb_number = 0;

	algo_stripe_names = new STRING [cfg -> stripes];
	for (int ind = 0; ind < cfg -> stripes; ind++) {
		sprintf (algo_stripe_names [ind], "");
	}
	sprintf (part_name, "");
	sprintf (device_id_name, "");
	sprintf (midi_channel_name, "");
	sprintf (category_name, "");
	sprintf (program_name, "");
//	sprintf (program_value_name, "");
	sprintf (bank_msb_name, "");
	sprintf (bank_lsb_name, "");
	sprintf (control_parameter_name, "");
	sprintf (control_value_name, "");
	sprintf (system_parameter_name, "");
	sprintf (system_value_name, "");
	sprintf (edit_parameter_name, "");
	sprintf (edit_value_name, "");
	sprintf (algo_name, "");
//	update_system_data ();

}

info_display :: ~ info_display (void) {
	delete [] algo_stripe_names;
}

void info_display :: midi_control (int channel, int controller, int value) {
	if (midi_channel_number == 0x7f) return;
	if (midi_channel_number != channel && midi_channel_number != 16) return;
	switch (controller) {
	case 99: nrpn_msb = value; break;
	case 98: nrpn_lsb = value; break;
	case 38: data_lsb = value; break;
	case 6: midi_nrpn (value); data_lsb = 0; break;
	}
}
void info_display :: midi_programchange (int channel, int program) {}
void info_display :: midi_system_exclusive (midi_stream * line) {
	int selector;
	int sub_selector;
	int channel;
	char * cp;
	if (line -> check_system_exclusive ()) {
		selector = line -> get ();
		if (selector < 0x80) {
			channel = selector & 0xf;
			selector >>= 4;
			sub_selector = line -> get ();
			switch (selector) {
			case 1:
				if (midi_channel_number == channel || midi_channel_number == 0x10) {
					switch (sub_selector) {
					case 0x10: cp = algo_name; break;
					case 0x11: cp = program_name; break;
					case 0x12: cp = bank_msb_name; break;
					case 0x13: cp = bank_lsb_name; break;
					case 0x14: cp = category_name; break;
					case 0x15: cp = program_name; break;
					default: cp = algo_stripe_names [sub_selector]; break;
					}
					line -> get (cp);
				}
				break;
			case 3:
				if (sub_selector == device_id_number || sub_selector == 0x7f || device_id_number == 0x7f) {
					midi_channel_number = line -> get ();
					sprintf (system_parameter_name, "MIDI CHANNEL");
					switch (midi_channel_number) {
					case 0x10: sprintf (system_value_name, " = ALL"); break;
					case 0x7f: sprintf (system_value_name, " = OFF"); break;
					default: sprintf (system_value_name, " = %i", midi_channel_number + 1); break;
					}
//					sprintf (part_name, "SYSTEM");
				}
				break;
			}
		}
	}
	line -> get_f7 ();
}

void info_display :: midi_nrpn (int data_msb) {
	int selector = nrpn_lsb >> 4;
	int stripe = nrpn_lsb & 0xf;
	// positive       [0 .. 16383]
	// negative       [-8192 .. 8191]
	// very_negative  [-16383 .. 0]
	// msb128         [0 .. 128]
	// msb_negative    [-64 .. 63]
	int positive = data_msb;
	positive <<= 7;
	positive += data_lsb;
	int negative = positive;
	if (negative >= 8192) negative -= 16384;
	int msb128 = data_msb;
	if (data_msb == 127 && data_lsb == 127) msb128 = 128;
	int msb_negative = data_msb;
	if (msb_negative >= 64) msb_negative -= 128;
	int very_negative;
	int fm, fl;
	switch (nrpn_msb) {
	case 0:
		switch (data_msb) {
		case 124: sprintf (system_value_name, " = PITCH"); break;
		case 125: sprintf (system_value_name, " = CAT"); break;
		case 126: sprintf (system_value_name, " = PAT"); break;
		case 127: sprintf (system_value_name, " = OFF"); break;
		default: sprintf (system_value_name, " = %i", data_msb); break;
		}
		if (data_msb >= 124) data_msb += 4;
		switch (nrpn_lsb) {
		case 0:
			pitch_id = data_msb;
			strcpy (system_parameter_name, "PITCH");
			break;
		case 1:
			freq_id = data_msb;
			strcpy (system_parameter_name, "FREQ");
			break;
		case 2:
			amp_id = data_msb;
			strcpy (system_parameter_name, "AMP");
			break;
		case 3:
			index_id = data_msb;
			strcpy (system_parameter_name, "INDEX");
			break;
		case 4:
			attack_id = data_msb;
			strcpy (system_parameter_name, "ATTACK");
			break;
		case 5:
			decay_id = data_msb;
			strcpy (system_parameter_name, "DECAY");
			break;
		case 6:
			sustain_id = data_msb;
			strcpy (system_parameter_name, "SUSTAIN");
			break;
		case 7:
			release_id = data_msb;
			strcpy (system_parameter_name, "RELEASE");
			break;
		case 8:
			speed_id = data_msb;
			strcpy (system_parameter_name, "SPEED");
			break;
		case 9:
			vibrato_id = data_msb;
			strcpy (system_parameter_name, "VIBRATO");
			break;
		case 10:
			tremolo_id = data_msb;
			strcpy (system_parameter_name, "TREMOLO");
			break;
		case 11:
			lfo_index_id = data_msb;
			strcpy (system_parameter_name, "LFO INDEX");
			break;
		case 12:
			lfo_pan_id = data_msb;
			strcpy (system_parameter_name, "LFO PAN");
			break;
		case 13:
			vector_x_id = data_msb;
			strcpy (system_parameter_name, "VECTOR X");
			break;
		case 14:
			vector_y_id = data_msb;
			strcpy (system_parameter_name, "VECTOR Y");
			break;
		case 15:
			volume_id = data_msb;
			strcpy (system_parameter_name, "VOLUME");
			break;
		case 16:
			pan_id = data_msb;
			strcpy (system_parameter_name, "PAN");
			break;
		case 17:
			hold_id = data_msb;
			strcpy (system_parameter_name, "HOLD");
			break;
		case 18:
			porta_id = data_msb;
			strcpy (system_parameter_name, "PORTA");
			break;
		case 19:
			porta_time_id = data_msb;
			strcpy (system_parameter_name, "PORTA TIME");
			break;
		case 20:
			porta_mode_id = data_msb;
			strcpy (system_parameter_name, "PORTA MODE");
			break;
		default: break;
		}
//		sprintf (part_name, "SYSTEM");
		break;
	case 1:
//		sprintf (part_name, "CONTROLLERS");
		switch (nrpn_lsb) {
		case 0: sprintf (control_parameter_name, "PITCH"); break;
		case 1: sprintf (control_parameter_name, "FREQ"); break;
		case 2: sprintf (control_parameter_name, "AMP"); break;
		case 3: sprintf (control_parameter_name, "INDEX"); break;
		case 4: sprintf (control_parameter_name, "ATTACK"); break;
		case 5: sprintf (control_parameter_name, "DECAY"); break;
		case 6: sprintf (control_parameter_name, "SUSTAIN"); break;
		case 7: sprintf (control_parameter_name, "RELEASE"); break;
		case 8: sprintf (control_parameter_name, "SPEED"); break;
		case 9: sprintf (control_parameter_name, "VIBRATO"); break;
		case 10: sprintf (control_parameter_name, "TREMOLO"); break;
		case 11: sprintf (control_parameter_name, "LFO INDEX"); break;
		case 12: sprintf (control_parameter_name, "LFO PAN"); break;
		case 13:
			vector_x = msb128 - 64;
			sprintf (control_parameter_name, "VECTOR");
			break;
		case 14:
			vector_y = msb128 - 64;
			sprintf (control_parameter_name, "VECTOR");
			break;
		case 15: sprintf (control_parameter_name, "VOLUME"); break;
		case 16: sprintf (control_parameter_name, "PAN"); break;
		case 17: sprintf (control_parameter_name, "HOLD"); break;
		case 18: sprintf (control_parameter_name, "PORTA"); break;
		case 19: sprintf (control_parameter_name, "PORTA TIME"); break;
		case 20: sprintf (control_parameter_name, "PORTA MODE"); break;
		case 21: sprintf (control_parameter_name, "MODE"); break;
		default: break;
		}
		switch (nrpn_lsb) {
		case 13: case 14:
			sprintf (control_value_name, "= [%+2i %+2i]", vector_x, vector_y);
			break;
		case 17: case 18:
			sprintf (control_value_name, data_msb == 0 ? "= OFF" : "= ON");
			break;
		case 20:
			sprintf (control_value_name, data_msb == 0 ? "= FING" : "= FULL");
			break;
		case 21:
			sprintf (control_value_name, data_msb == 0 ? "= MONO" : "= POLY");
			break;
		default:
			sprintf (control_value_name, "= %5i", positive);//msb128);
			break;
		}
		break;
	case 2:
		switch (nrpn_lsb) {
		case 0: category_number = data_msb; break;
		case 1:
			bank_msb_number = data_msb;
			sprintf (edit_parameter_name, "BANK MSB");
			break;
		case 2:
			bank_lsb_number = data_msb;
			sprintf (edit_parameter_name, "BANK LSB");
			break;
		case 3:
			program_number = data_msb;
			sprintf (edit_parameter_name, "PROGRAM");
			break;
		case 4: sprintf (edit_parameter_name, "DEVICE ID"); break;
		case 5: sprintf (edit_parameter_name, "TUNE"); break;
		default: break;
		}
	case 3:
		sprintf (control_value_name, "= %5i", positive);
		switch (nrpn_lsb) {
		case 0:
			sprintf (control_parameter_name, data_msb == 0 ? "KEY OFF" : "KEY ON");
			sprintf (control_value_name, "= %3i", data_lsb);
			break;
		case 1: sprintf (control_parameter_name, "ALL SOUNDS OFF"); break;
		case 2: sprintf (control_parameter_name, "RESET ALL CONTROLLERS"); break;
		case 3: sprintf (control_parameter_name, "ALL KEYS OFF"); break;
		default: break;
		}
		break;
	case 16:
		sprintf (edit_parameter_name, "LFO RETRIGGER");
		sprintf (part_name, selector > 3 ? "PAN %i" : "STRIPE %i", stripe);
		sprintf (edit_value_name, data_msb == 0 ? "= OFF" : "= ON");
		break;
	case 17:
		sprintf (edit_parameter_name, "LFO SYNC TO");
		sprintf (part_name, selector > 3 ? "PAN %i" : "STRIPE %i", stripe);
		if (data_msb >= 16) sprintf (edit_value_name, "= %2i (STEREO)", data_msb & 0xf);
		else sprintf (edit_value_name, "= %2i (STRIPE)", data_msb & 0xf);
		break;
	case 18:
		sprintf (edit_parameter_name, "LFO SPEED");
		sprintf (part_name, selector > 3 ? "PAN %i" : "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %5i", positive);
		break;
	case 19:
		sprintf (edit_parameter_name, "LFO PHASE SHIFT");
		sprintf (part_name, selector > 3 ? "PAN %i" : "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %5i", positive);
		break;
	case 20:
		sprintf (edit_parameter_name, "LFO PULSE WIDTH");
		sprintf (part_name, selector > 3 ? "PAN %i" : "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %5i", negative);
		break;
	case 21:
		sprintf (edit_parameter_name, "LFO WAVE");
		sprintf (part_name, selector > 3 ? "PAN %i" : "STRIPE %i", stripe);
		switch (data_msb) {
		case 0: sprintf (edit_value_name, "= SIN"); break;
		case 1: sprintf (edit_value_name, "= TRI"); break;
		case 2: sprintf (edit_value_name, "= SQR"); break;
		case 3: sprintf (edit_value_name, "= RND"); break;
		case 4: sprintf (edit_value_name, "= S/H"); break;
		default: break;
		}
		break;
	case 32:
		sprintf (edit_parameter_name, "INDEX SHIFT");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %3i", msb128);
		break;
	case 33:
		sprintf (edit_parameter_name, "LFO INDEX");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %5i", positive);
		break;
	case 34:
		drop_eg_level (selector, "INDEX ");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %5i", negative);
		break;
	case 35:
		drop_eg_time (selector, "INDEX ");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %i", positive);
		break;
	case 40:
		freq_msb = data_msb;
		freq_lsb = data_lsb;
		fm = data_msb;
		fl = data_lsb;
		if (fm > 63) fm -= 128;
		if (fl > 63) {fl -= 128; fm++;}
		sprintf (edit_parameter_name, "FREQ SHIFT");
		sprintf (part_name, "STRIPE %i", stripe);
//		sprintf (edit_value_name, "= [%+2i %+2i]", msb_negative, data_lsb);
		sprintf (edit_value_name, "= [%+2i %+2i]", fm, fl);
		break;
	case 41:
		sprintf (edit_parameter_name, "LFO FREQ");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %5i", positive);
		break;
	case 42:
		drop_eg_level (selector, "FREQ ");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %5i", negative);
		break;
	case 43:
		drop_eg_time (selector, "FREQ ");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %i", positive);
		break;
	case 44:
		sprintf (edit_parameter_name, "FREQ RATIO");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %+2i", msb_negative);
		break;
	case 45:
		sprintf (edit_parameter_name, "SYNC");
		sprintf (part_name, "STRIPE %i", stripe);
		strcpy (edit_value_name, data_msb == 0 ? "= OFF" : "= ON");
		break;
	case 48:
		sprintf (edit_parameter_name, "AMP SHIFT");
		sprintf (part_name, "STRIPE %i", stripe);
		very_negative = positive;
		if (positive != 0) very_negative -= 16384;
		sprintf (edit_value_name, "= %5i", very_negative);
		break;
	case 49:
		sprintf (edit_parameter_name, "LFO AMP");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %5i", positive);
		break;
	case 50:
		drop_eg_level (selector, "AMP ");
		sprintf (part_name, "STRIPE %i", stripe);
		very_negative = positive;
		if (positive != 0) very_negative -= 16384;
		sprintf (edit_value_name, "= %5i", very_negative);
		break;
	case 51:
		drop_eg_time (selector, "AMP ");
		sprintf (part_name, "STRIPE %i", stripe);
		sprintf (edit_value_name, "= %i", positive);
		break;
	case 56:
		sprintf (edit_parameter_name, "PAN SHIFT");
		sprintf (part_name, "PAN %i", stripe);
		sprintf (edit_value_name, "= %+5i", negative);
		break;
	case 57:
		sprintf (edit_parameter_name, "LFO PAN");
		sprintf (part_name, "PAN %i", stripe);
		sprintf (edit_value_name, "= %5i", positive);
		break;
	case 58:
		drop_eg_level (selector, "PAN ");
		sprintf (part_name, "PAN %i", stripe);
		sprintf (edit_value_name, "= %5i", negative);
		break;
	case 59:
		drop_eg_time (selector, "PAN ");
		sprintf (part_name, "PAN %i", stripe);
		sprintf (edit_value_name, "= %i", positive);
		break;
	case 60:
		sprintf (edit_parameter_name, "PAN RND");
		sprintf (part_name, "PAN %i", stripe);
		sprintf (edit_value_name, "= %+5i", positive);
		break;
	case 63:
		switch (nrpn_lsb) {
		case 1:
			sprintf (edit_parameter_name, "FEEDBACK");
			sprintf (edit_value_name, " = %5i", positive);
			break;
		case 2:
			sprintf (edit_parameter_name, "DELAY TIME");
			sprintf (edit_value_name, " = %5i", positive);
			break;
		case 3:
			sprintf (edit_parameter_name, "DRY/WET");
			sprintf (edit_value_name, " = %+4i", negative);
			break;
		case 4:
			sprintf (edit_parameter_name, "CROSS DELAY");
			sprintf (edit_value_name, data_msb == 0 ? " = OFF" : " = ON");
			break;
		case 5:
			sprintf (edit_parameter_name, "DSP");
			sprintf (edit_value_name, data_msb == 0 ? " = OFF" : " = ON");
			break;
		}
//		sprintf (part_name, "DSP");
		break;
	default:
		sprintf (edit_parameter_name, "msb:%i lsb:%i data:[%i %i]", nrpn_msb, nrpn_lsb, data_msb, data_lsb);
		break;
	}
	if (nrpn_msb >= 24 && nrpn_msb <= 27) {
		sprintf (edit_parameter_name, "LFO ");
		switch (nrpn_msb) {
		case 24: strcat (edit_parameter_name, "SPEED "); break;
		case 25: strcat (edit_parameter_name, "PITCH "); break;
		case 26: strcat (edit_parameter_name, "VECTOR X "); break;
		case 27: strcat (edit_parameter_name, "VECTOR Y "); break;
		}
		if (selector < 4) sprintf (part_name, "STRIPE %i", stripe);
		else sprintf (part_name, "PAN %i", stripe);
		switch (selector & 0x3) {
		case 0: strcat (edit_parameter_name, "BP"); break;
		case 1: strcat (edit_parameter_name, "LC"); break;
		case 2: strcat (edit_parameter_name, "RC"); break;
		}
		if ((selector & 0x3) != 0) sprintf (edit_value_name, "= %+5i", negative);
		else sprintf (edit_value_name, "= %5i", positive);
	}
	if (nrpn_msb >= 64 && nrpn_msb < 127) {
		if (nrpn_msb >= 64 && nrpn_msb < 79) sprintf (edit_parameter_name, "INDEX ");
		if (nrpn_msb >= 80 && nrpn_msb < 95) sprintf (edit_parameter_name, "FREQ ");
		if (nrpn_msb >= 96 && nrpn_msb < 111) sprintf (edit_parameter_name, "AMP ");
		if (nrpn_msb >= 112 && nrpn_msb < 127) sprintf (edit_parameter_name, "PAN ");
		if (nrpn_msb < 112) sprintf (part_name, "STRIPE %i", stripe);
		else sprintf (part_name, "PAN %i", stripe);
		switch (nrpn_msb & 0xf) {
		case 0: strcat (edit_parameter_name, "SHIFT "); break;
		case 1: strcat (edit_parameter_name, "LFO "); break;
		case 2: strcat (edit_parameter_name, "ATTACK "); break;
		case 3: strcat (edit_parameter_name, "DECAY "); break;
		case 4: strcat (edit_parameter_name, "SUSTAIN "); break;
		case 5: strcat (edit_parameter_name, "RELEASE "); break;
		case 6: strcat (edit_parameter_name, "KEY "); break;
		case 7: strcat (edit_parameter_name, "VELOCITY "); break;
		case 8: strcat (edit_parameter_name, "VECTOR X "); break;
		case 9: strcat (edit_parameter_name, "VECTOR Y "); break;
		case 10: strcat (edit_parameter_name, "LFO X "); break;
		case 11: strcat (edit_parameter_name, "LFO Y "); break;
		case 12: strcat (edit_parameter_name, selector > 3 ? "KEY ATTACK EG SCAL" : "KEY EG SCAL "); break;
		case 13: strcat (edit_parameter_name, selector > 3 ? "VELOCITY ATTACK EG SCAL" : "VELOCITY EG SCAL "); break;
		case 14: strcat (edit_parameter_name, "PITCH "); break;
		}
		switch (selector) {
		case 0: strcat (edit_parameter_name, "BP"); break;
		case 1: strcat (edit_parameter_name, "LC"); break;
		case 2: strcat (edit_parameter_name, "RC"); break;
		}
		if (selector != 0) sprintf (edit_value_name, "= %+5i", negative);
		else sprintf (edit_value_name, "= %5i", positive);
	}
}

void info_display :: drop_eg_level (int selector, char * header) {
	strcpy (edit_parameter_name, header);
	switch (selector) {
	case 0: strcat (edit_parameter_name, "ATK/L"); break;
	case 1: strcat (edit_parameter_name, "ATK2/L"); break;
	case 2: strcat (edit_parameter_name, "SUSTAIN"); break;
	case 3: strcat (edit_parameter_name, "BASE/L"); break;
	}
}

void info_display :: drop_eg_time (int selector, char * header) {
	strcpy (edit_parameter_name, header);
	switch (selector) {
	case 0: strcat (edit_parameter_name, "ATTACK"); break;
	case 1: strcat (edit_parameter_name, "ATK2/T"); break;
	case 2: strcat (edit_parameter_name, "DECAY"); break;
	case 3: strcat (edit_parameter_name, "RELEASE"); break;
	}
}
