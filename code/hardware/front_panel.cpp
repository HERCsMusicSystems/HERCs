///////////////////////////////////////
// Copyright (C) 2008 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "front_panel.h"

#include <wx/wx.h>

hercs_front_panel :: hercs_front_panel (void) {
	factory_id = 0;
//	part = 0;
	stripe = 0;
	stereo_stripe = 0;
	dsp_id = 0;
	main_mode = 0;
	type = 0;
	sensitivity_type = 1;
	dsp_sensitivity_controller = 0;
	nrpn_msb = nrpn_lsb = 127;
	data = 0;
	range_min = 0; range_max = 16383; msb_only = false;
	tune_key = 64; name_character = 0;
	store_location = 0;
	control_store_required = false;
	program_parameter = system_parameter = control_parameter = sensitivity_parameter = stripe_parameter = vector_parameter = lfo_parameter = arpeggiator_parameter = dsp_parameter = dsp_p_parameter = algo_parameter = transport_parameter = midi_parameter = 0;
	bank_change_mode = 0;
	command_mode = 0;
	bank_msb = & preset_bank_msb; bank_lsb = & preset_bank_lsb;
	preset_bank_msb = preset_bank_lsb = user_bank_msb = user_bank_lsb = 0;
	algo_bank_msb = algo_bank_lsb = 0;
	arp_bank_msb = arp_bank_lsb = 0;
	dsp_algo_bank_msb = dsp_algo_bank_lsb = preset_dsp_bank_msb = preset_dsp_bank_lsb = user_dsp_bank_msb = user_dsp_bank_lsb = 0;
	style_bank_msb = style_bank_lsb = 0;
	strcpy (parameter_text, "");
	strcpy (status_text, "");
	strcpy (program_name, "<NOT INSTALLED>");
	strcpy (bank_msb_name, "<NOT INSTALLED>");
	strcpy (bank_lsb_name, "<NOT INSTALLED>");
	strcpy (category_name, "??:<NOT INSTALLED>");
	strcpy (algo_name, "<NOT INSTALLED>");
	strcpy (pattern_name, "<NOT INSTALLED>");
	strcpy (dsp_algo_name, "<NOT INSTALLED>");
	strcpy (style_name, "<NOT INSTALLED>");
	strcpy (algo_parameter_name, "<UNKNOWN>");
	strcpy (dsp_parameter_name, "<UNKNOWN>");
	part_id = 0;
	for (int ind = 0; ind < 16; ind++) {strcpy (algo_parameter_names [ind], ""); part_ids [ind] = ind;}
	for (int ind = 0; ind < 8; ind++) {strcpy (dsp_algo_parameter_names [ind], "");}
	encoder_sensitivity = 1;
}

void hercs_front_panel :: calculate_mode_from_nrpn (void) {}
void hercs_front_panel :: calculate_nrpn_from_mode (void) {}

void hercs_front_panel :: calculate_sensitivity_block_display (void) {
	char * type_name;
	switch ((nrpn_msb >> 4) & 0x3) {
		case 0: type_name = "INDEX"; break;
		case 1: type_name = "FREQ"; break;
		case 2: type_name = "AMP"; break;
		case 3: type_name = "PAN"; break;
		default: type_name = "UNKNOWN"; break;
	}
	char * parameter_name;
	switch (nrpn_msb & 0xf) {
		case 0: parameter_name = "SHIFT"; break;
		case 1: parameter_name = "LFO"; break;
		case 2: parameter_name = "ATTACK"; break;
		case 3: parameter_name = "DECAY"; break;
		case 4: parameter_name = "SUSTAIN"; break;
		case 5: parameter_name = "RELEASE"; break;
		case 6: parameter_name = "KEY"; break;
		case 7: parameter_name = "VELOCITY"; break;
		case 8: parameter_name = "VECTOR X"; break;
		case 9: parameter_name = "VECTOR Y"; break;
		case 10: parameter_name = "LFO VECTOR X"; break;
		case 11: parameter_name = "LFO VECTOR Y"; break;
		case 12: if (nrpn_lsb >= 64) parameter_name = "KEY ATTACK EG SCAL"; else parameter_name = "KEY EG SCAL"; break;
		case 13: if (nrpn_lsb >= 64) parameter_name = "VELOCITY ATTACK EG SCAL"; else parameter_name = "VELOCITY EG SCAL"; break;
		case 14: parameter_name = "PITCH"; break;
		default: parameter_name = "UNKNOWN"; break;
	}
	double db = (double) data / 128.0;
	switch ((nrpn_lsb >> 4) & 0x3) {
		case 0: sprintf (parameter_text, "%s %s SENS BP=%7.3f", type_name, parameter_name, db); break;
		case 1: sprintf (parameter_text, "%s %s SENS LC=%+7.3f", type_name, parameter_name, db); break;
		case 2: sprintf (parameter_text, "%s %s SENS RC=%+7.3f", type_name, parameter_name, db); break;
		default: sprintf (parameter_text, "UNKNOWN"); break;
	}
}

void hercs_front_panel :: calculate_dsp_display (void) {
	int dsp_original = (nrpn_lsb >> 4) & 0x7;
	int dsp_target = nrpn_lsb & 0x7;
	bool dsp_switch = (nrpn_lsb & 0x8) != 0;
	double db = (double) data / 128.0;
	char * type_name, * controller_name, * sensitivity_name;
	bool sensitivity_bp = false;
	switch (nrpn_msb) {
		case 8:
			if (dsp_switch) {
				if (dsp_original == dsp_target) sprintf (parameter_text, "DSP %i OUTPUT LEVEL DRY/WET SENS BP=%7.3f", dsp_original + 1, db);
				else sprintf (parameter_text, "DSP %i TO DSP %i SEND LEVEL DRY/WET SENS BP=%7.3f", dsp_original + 1, dsp_target + 1, db);
			} else {
				if (dsp_original == dsp_target) sprintf (parameter_text, "DSP %i OUTPUT LEVEL=%7.3f", dsp_original + 1, db);
				else sprintf (parameter_text, "DSP %i TO DSP %i SEND LEVEL=%7.3f", dsp_original + 1, dsp_target + 1, db);
			}
			break;
		case 9:
			if (dsp_original == dsp_target) sprintf (parameter_text, "DSP %i OUTPUT LEVEL DRY/WET SENS %s=%+7.3f", dsp_original + 1, dsp_switch ? "RC" : "LC", db);
			else sprintf (parameter_text, "DSP %i TO DSP %i SEND LEVEL DRY/WET SENS %s=%+7.3f", dsp_original + 1, dsp_target + 1, dsp_switch ? "RC" : "LC", db);
			break;
		case 10:
			if (dsp_original == dsp_target) sprintf (parameter_text, "DSP %i OUTPUT LEVEL VECTOR %s SENS BP=%7.3f", dsp_original + 1, dsp_switch ? "Y" : "X", db);
			else sprintf (parameter_text, "DSP %i TO DSP %i SEND LEVEL VECTOR %s SENS BP=%7.3f", dsp_original + 1, dsp_target + 1, dsp_switch ? "Y" : "X", db);
			break;
		case 11: case 12:
			if (dsp_original == dsp_target) sprintf (parameter_text, "DSP %i OUTPUT LEVEL VECTOR %s SENS %s=%+7.3f", dsp_original + 1, nrpn_msb == 11 ? "X" : "Y", dsp_switch ? "RC" : "LC", db);
			else sprintf (parameter_text, "DSP %i TO DSP %i SEND LEVEL VECTOR %s SENS %s=%+7.3f", dsp_original + 1, dsp_target + 1, nrpn_msb == 11 ? "X" : "Y", dsp_switch ? "RC" : "LC", db);
			break;
		case 13: switch (nrpn_lsb & 0xf) {
			case 0: sprintf (parameter_text, "DRY TO DSP %i SEND=%7.3f", dsp_original + 1, db); break;
			case 4: sprintf (parameter_text, "DRY TO DSP %i SEND DRY/WET SENS BP=%7.3f", dsp_original + 1, db); break;
			case 5: sprintf (parameter_text, "DRY TO DSP %i SEND DRY/WET SENS LC=%+7.3f", dsp_original + 1, db); break;
			case 6: sprintf (parameter_text, "DRY TO DSP %i SEND DRY/WET SENS RC=%+7.3f", dsp_original + 1, db); break;
			case 8: sprintf (parameter_text, "DRY TO DSP %i SEND VECTOR X SENS BP=%7.3f", dsp_original + 1, db); break;
			case 9: sprintf (parameter_text, "DRY TO DSP %i SEND VECTOR X SENS LC=%+7.3f", dsp_original + 1, db); break;
			case 10: sprintf (parameter_text, "DRY TO DSP %i SEND VECTOR X SENS RC=%+7.3f", dsp_original + 1, db); break;
			case 12: sprintf (parameter_text, "DRY TO DSP %i SEND VECTOR Y SENS BP=%7.3f", dsp_original + 1, db); break;
			case 13: sprintf (parameter_text, "DRY TO DSP %i SEND VECTOR Y SENS LC=%+7.3f", dsp_original + 1, db); break;
			case 14: sprintf (parameter_text, "DRY TO DSP %i SEND VECTOR Y SENS RC=%+7.3f", dsp_original + 1, db); break;
			default: sprintf (parameter_text, "UNKNOWN=%+7.3f", db); break;
			}
			break;
		case 14:
			if (nrpn_lsb < 8) {sprintf (parameter_text, "DSP %i %s", nrpn_lsb + 1, data == 0 ? "OFF" : "ON"); break;}
			if (nrpn_lsb == 16) {sprintf (parameter_text, "LOCAL LEVEL=%7.3f", db); break;}
			if (nrpn_lsb == 32) {sprintf (parameter_text, "GLOBAL LEVEL=%7.3f", db); break;}
			type_name = "UNKNOWN";
			if ((nrpn_lsb & 0x10) != 0) type_name = "LOCAL";
			if ((nrpn_lsb & 0x20) != 0) type_name = "GLOBAL";
			switch ((nrpn_lsb >> 2) & 0x3) {
				case 1: controller_name = "DRY/WET"; break;
				case 2: controller_name = "VECTOR X"; break;
				case 3: controller_name = "VECTOR Y"; break;
				default: controller_name = "UNKNOWN"; break;
			}
			switch (nrpn_lsb & 0x3) {
				case 0: sensitivity_name = "BP"; sensitivity_bp = true; break;
				case 1: sensitivity_name = "LC"; break;
				case 2: sensitivity_name = "RC"; break;
				default: sensitivity_name = "UNKNOWN"; break;
			}
			if (sensitivity_bp) sprintf (parameter_text, "%s LEVEL %s SENS BP=%7.3f", type_name, controller_name, db);
			else sprintf (parameter_text, "%s LEVEL %s SENS %s=%+7.3f", type_name, controller_name, sensitivity_name, db);
//			sprintf (parameter_text, "%02X %02X", nrpn_msb, nrpn_lsb);
			break;
		default: sprintf (parameter_text, "UNKNOWN"); break;
	}
}

void hercs_front_panel :: calculate_lfo_sensitivity_block_display (double db) {
	int selector = (nrpn_lsb >> 4) & 0x3;
	char * pan_name = nrpn_lsb >= 0x40 ? (char *) "PAN " : (char *) "";
	char * parameter_name;
	switch (nrpn_msb) {
		case 24: parameter_name = ""; break;
		case 25: parameter_name = "PITCH "; break;
		case 26: parameter_name = "VECTOR X "; break;
		case 27: parameter_name = "VECTOR Y "; break;
		default: parameter_name = "UNKNOWN "; break;
	}
	switch (selector) {
		case 0: sprintf (parameter_text, "%sLFO SPEED %sSENS BP=%7.3f", pan_name, parameter_name, db); break;
		case 1: sprintf (parameter_text, "%sLFO SPEED %sSENS LC=%+7.3f", pan_name, parameter_name, db); break;
		case 2: sprintf (parameter_text, "%sLFO SPEED %sSENS RC=%+7.3f", pan_name, parameter_name, db); break;
		default: sprintf (parameter_text, "UNKNOWN"); break;
	}
}

void hercs_front_panel :: calculate_display (void) {
//	wxMessageBox (wxString :: Format (_T ("CALCULATED [%i %i]"), nrpn_msb, nrpn_lsb), _T ("INFO"), wxOK, NULL);
	if (command_mode == 2) {sprintf (parameter_text, "STORE LOCATION ? = %i", store_location); return;}
	if (nrpn_msb >= 64 && nrpn_msb < 128) {calculate_sensitivity_block_display (); return;}
	char command [256];
	char * back = & command [128];
	double dbp = (double) data / 128.0;
	double db = (double) (data - 8192) / 128.0;
	int ind;
	switch (nrpn_msb) {
		case 0:
			switch (data) {
				case 124: sprintf (command, "PITCH"); break;
				case 125: sprintf (command, "CHAN. AFT."); break;
				case 126: sprintf (command, "POLY. AFT."); break;
				case 127: sprintf (command, "OFF"); break;
				default: sprintf (command, "%i", data); break;
			}
			switch (nrpn_lsb) {
				case 0: sprintf (parameter_text, "PITCH=%s", command); break;
				case 1: sprintf (parameter_text, "FREQ=%s", command); break;
				case 2: sprintf (parameter_text, "AMP=%s", command); break;
				case 3: sprintf (parameter_text, "INDEX=%s", command); break;
				case 4: sprintf (parameter_text, "ATTACK=%s", command); break;
				case 5: sprintf (parameter_text, "DECAY=%s", command); break;
				case 6: sprintf (parameter_text, "SUSTAIN=%s", command); break;
				case 7: sprintf (parameter_text, "RELEASE=%s", command); break;
				case 8: sprintf (parameter_text, "LFO SPEED=%s", command); break;
				case 9: sprintf (parameter_text, "VIBRATO=%s", command); break;
				case 10: sprintf (parameter_text, "TREMOLO=%s", command); break;
				case 11: sprintf (parameter_text, "LFO INDEX=%s", command); break;
				case 12: sprintf (parameter_text, "LFO PAN=%s", command); break;
				case 13: sprintf (parameter_text, "VECTOR X=%s", command); break;
				case 14: sprintf (parameter_text, "VECTOR Y=%s", command); break;
				case 15: sprintf (parameter_text, "VOLUME=%s", command); break;
				case 16: sprintf (parameter_text, "PAN=%s", command); break;
				case 17: sprintf (parameter_text, "HOLD=%s", command); break;
				case 18: sprintf (parameter_text, "PORTA=%s", command); break;
				case 19: sprintf (parameter_text, "PORTA TIME=%s", command); break;
				case 20: sprintf (parameter_text, "PORTA MODE=%s", command); break;
				case 21: sprintf (parameter_text, "DRY/WET=%s", command); break;
				case 22: sprintf (parameter_text, "GLOBAL VOLUME=%s", command); break;
				default: sprintf (parameter_text, "UNKNOWN CONTROL=%s", command); break;
			}
			break;
		case 1:
			switch (nrpn_lsb) {
				case 0: sprintf (parameter_text, "PITCH=%+7.3f", db); break;
				case 1: sprintf (parameter_text, "FREQ=%+7.3f", db); break;
				case 2: sprintf (parameter_text, "AMP=%+7.3f", db); break;
				case 3: sprintf (parameter_text, "INDEX=%+7.3f", db); break;
				case 4: sprintf (parameter_text, "ATTACK=%+7.3f", db); break;
				case 5: sprintf (parameter_text, "DECAY=%+7.3f", db); break;
				case 6: sprintf (parameter_text, "SUSTAIN=%+7.3f", db); break;
				case 7: sprintf (parameter_text, "RELEASE=%+7.3f", db); break;
				case 8: sprintf (parameter_text, "LFO SPEED=%+7.3f", db); break;
				case 9: sprintf (parameter_text, "VIBRATO=%+7.3f", db); break;
				case 10: sprintf (parameter_text, "TREMOLO=%+7.3f", db); break;
				case 11: sprintf (parameter_text, "LFO INDEX=%+7.3f", db); break;
				case 12: sprintf (parameter_text, "LFO PAN=%+7.3f", db); break;
				case 13: sprintf (parameter_text, "VECTOR X=%+7.3f", db); break;
				case 14: sprintf (parameter_text, "VECTOR Y=%+7.3f", db); break;
				case 15: sprintf (parameter_text, "VOLUME=%7.3f", dbp); break;
				case 16: sprintf (parameter_text, "PAN=%+7.3f", db); break;
				case 17: sprintf (parameter_text, "HOLD=%s", data == 0 ? "OFF" : "ON"); break;
				case 18: sprintf (parameter_text, "PORTA=%s", data == 0 ? "OFF" : "ON"); break;
				case 19: sprintf (parameter_text, "PORTA TIME=%7.3f [%7.2f sec]", dbp, pow (2.0, (double) (data - 8192) / 768.0)); break;
				case 20: sprintf (parameter_text, "PORTA MODE=%s", data == 0 ? "FING" : "FULL"); break;
				case 21: sprintf (parameter_text, "DRY/WET=%+7.3f", db); break;
				case 22: sprintf (parameter_text, "GLOBAL VOLUME=%7.3f", dbp); break;
				case 23: sprintf (parameter_text, "%s", data ==  0 ? "MONO" : "POLY"); break;
				default: sprintf (parameter_text, "UNKNOWN CONTROL=%+7.3f", db); break;
			}
			break;
		case 2:
			switch (nrpn_lsb) {
			case 0: sprintf (parameter_text, "CATEGORY=[%3i] %s", data, category_name); break;
			case 1: sprintf (parameter_text, "BANK MSB=%i %s", data, bank_msb_name); break;
			case 2: sprintf (parameter_text, "BANK LSB=%i %s", data, bank_lsb_name); break;
			case 3:
				switch (bank_change_mode) {
				case 0: case 1: sprintf (parameter_text, "%03i [%c%c] %s", data, category_name [0], category_name [1], program_name); break;
				case 2: sprintf (parameter_text, "ALGO %03i %s", data, algo_name); break;
				case 3: sprintf (parameter_text, "PATTERN %03i %s", data, pattern_name); break;
				case 4: sprintf (parameter_text, "DSP %i %03i ALGO %s", dsp_algo_bank_lsb + 1, data, dsp_algo_name); break;
				case 5: sprintf (parameter_text, "PRESET DSP SELECT %i", data); break;
				case 6: sprintf (parameter_text, "USER DSP SELECT %i", data); break;
				case 7: sprintf (parameter_text, "STYLE %03i %s", data, style_name); break;
				default: sprintf (parameter_text, "UNKNOWN MODE %i", data); break;
				}
				break;
			case 4:
				if (data > 127) sprintf (parameter_text, "DEVICE ID=OFF");
				if (data == 127) sprintf (parameter_text, "DEVICE ID=BROADCAST");
				if (data < 127) sprintf (parameter_text, "DEVICE ID=%i", data);
				break;
			case 5: sprintf (parameter_text, "MAIN TUNE=%+7.3f", dbp); break;
			case 6: sprintf (parameter_text, "ARRANGER ASSIGN=%i", data); break;
			case 7: sprintf (parameter_text, "RESERVED NOTES=%i", data); break;
			default: sprintf (parameter_text, "MAIN MODE"); break;
			}
			break;
		case 4: case nrpn_tune_key: sprintf (parameter_text, "INDIVIDUAL KEY TUNE %3i=%7.3f", tune_key, dbp); break;
		case 6:
			switch (nrpn_lsb) {
				case 1: sprintf (parameter_text, "VECTOR AUTOMATION %s", data != 0 ? "ON" : "OFF"); break;
				case 2: sprintf (parameter_text, "INITIAL X=%+7.3f", dbp); break;
				case 3: sprintf (parameter_text, "INITIAL Y=%+7.3f", dbp); break;
				case 4: sprintf (parameter_text, "FRAME COUNT=%i", data); break;
				case 5: sprintf (parameter_text, "FRAME SELECTOR=%i", data); break;
				case 6: sprintf (parameter_text, "FRAME DELAY=%i", data); break;
				case 7: sprintf (parameter_text, "FRAME X=%+7.3f", dbp); break;
				case 8: sprintf (parameter_text, "FRAME Y=%+7.3f", dbp); break;
				case 9: sprintf (parameter_text, "RESOLUTION=%i", data); break;
				case 10: sprintf (parameter_text, "RESOLUTION SECONDS=%7.3f", dbp); break;
				case 11: switch (data) {
					case 0: sprintf (parameter_text, "TRIGGER FIRST"); break;
					case 1: sprintf (parameter_text, "TRIGGER ALL"); break;
					case 2: sprintf (parameter_text, "TRIGGER RECORD [FIRST]"); break;
					case 3: sprintf (parameter_text, "TRIGGER RECORD [ALL]"); break;
					default: break;} break;
				case 12: sprintf (parameter_text, "ATTACK =%7.3f [%.02f sec]", dbp, data == 0 ? 0.0 : pow (2.0, (double) (data - 8192) / 768.0)); break;
				case 13: sprintf (parameter_text, "RELEASE=%7.3f [%.02f sec]", dbp, data == 0 ? 0.0 : pow (2.0, (double) (data - 8192) / 768.0)); break;
				case 14: sprintf (parameter_text, "LOOP TYPE=%s", data == 0 ? "OFF" : data == 1 ? "LOOP" : "ROUND"); break;
				case 15: sprintf (parameter_text, "LOOP START=%i", data); break;
				case 16: sprintf (parameter_text, "LOOP SIZE=%i", data); break;
				case 17: sprintf (parameter_text, "LOOP DELAY=%7.3f", dbp); break;
				default: sprintf (parameter_text, "UNKNOWN VECTOR PARAMETER=%7.3f", dbp); break;
			}
			break;
		case 7: sprintf (parameter_text, "ALGO [%03i]=%7.3f [%s]", nrpn_lsb, dbp, algo_parameter_name); break;
		case 8: case 9: case 10: case 11: case 12: case 13: case 14: calculate_dsp_display (); break;
		case 15: sprintf (parameter_text, "DSP %i [%02i]=%7.3f [%s]", 1 + (nrpn_lsb >> 4), nrpn_lsb & 0xf, dbp, dsp_parameter_name); break;
		case 16: sprintf (parameter_text, "LFO RETRIGGER %s", data == 0 ? "OFF" : "ON"); break;
		case 17: sprintf (parameter_text, "LFO SYNC TO = %s %i", data < 16 ? "STRIPE" : "PAN", data < 16 ? data + 1 : data - 15); break;
		case 18: sprintf (parameter_text, "LFO SPEED = %7.3f [%.2f Hz]", dbp, pow (2.0, (double) (data - 8192) / 768.0)); break;
		case 19: sprintf (parameter_text, "LFO PHASE SHIFT = %+7.3f [%+3i deg]", dbp, (int) ((double) data * 90.0 / 8191.0)); break;
		case 20: sprintf (parameter_text, "LFO PULSE WIDTH = %+7.3f [%+4i %c]", dbp, (int) ((double) data * 100.0 / 8191.0), '%'); break;
		case 21: switch (data) {
			case 0: sprintf (parameter_text, "LFO WAVE = SINE"); break;
			case 1: sprintf (parameter_text, "LFO WAVE = TRIANGLE"); break;
			case 2: sprintf (parameter_text, "LFO WAVE = SQUARE"); break;
			case 3: sprintf (parameter_text, "LFO WAVE = RANDOM"); break;
			case 4: sprintf (parameter_text, "LFO WAVE = S/H"); break;
			default: break;
			} break;
		case 24: case 25: case 26: case 27: calculate_lfo_sensitivity_block_display (dbp); break;
		case 31: switch (nrpn_lsb) {
			case 1: sprintf (parameter_text, "TEMPO BEATS/SEC=%5i", data); break;
			case 2: sprintf (parameter_text, "TEMPO SECONDS=%3i", data); break;
			case 3: sprintf (parameter_text, "BEATS PER BAR=%3i", data); break;
			case 4: sprintf (parameter_text, "TICKS PER BEAT=%3i", data); break;
			case 5: sprintf (parameter_text, "ARPEGGIATOR HOLD %s", data == 0 ? "OFF" : "ON"); break;
			case 6: sprintf (parameter_text, "ARPEGGIATOR %s", data == 0 ? "OFF" : "ON"); break;
			case 7: sprintf (parameter_text, "VARIATION =%7.3f", dbp); break;
			default: sprintf (parameter_text, "UNKNOWN=%i", data); break;
			} break;
		case 32: sprintf (parameter_text, "INDEX = %7.3f", dbp); break;
		case 33: sprintf (parameter_text, "INDEX LFO = %7.3f", dbp); break;
		case 34: sprintf (parameter_text, "INDEX EG LEVEL %i = %+7.3f", (nrpn_lsb >> 4) + 1, dbp); break;
		case 35: sprintf (parameter_text, "INDEX EG TIME  %i = %7.3f [%.2f sec]", (nrpn_lsb >> 4) + 1, dbp, data == 0 ? 0.0 : pow (2.0, (double) (data - 8192) / 768.0)); break;
		case 36: sprintf (parameter_text, "INDEX RND = %7.3f", dbp); break;
		case 37: sprintf (parameter_text, "INDEX LFO SOURCE = %s %i", data < 16 ? "STRIPE" : "PAN", data < 16 ? data + 1 : data - 15); break;
		case 40: sprintf (parameter_text, "FREQ = %+7.3f [%.2f Hz]", dbp, 330.0 * pow (2.0, (double) data / 1536.0)); break;
		case 41: sprintf (parameter_text, "FREQ LFO = %7.3f", dbp); break;
		case 42: sprintf (parameter_text, "FREQ EG LEVEL %i = %+7.3f", (nrpn_lsb >> 4) + 1, dbp); break;
		case 43: sprintf (parameter_text, "FREQ EG TIME  %i = %7.3f [%.2f sec]", (nrpn_lsb >> 4) + 1, dbp, data == 0 ? 0.0 : pow (2.0, (double) (data - 8192) / 768.0)); break;
		case 44: sprintf (parameter_text, "FREQ RND = %7.3f", dbp); break;
		case 45: sprintf (parameter_text, "FREQ LFO SOURCE = %s %i", data < 16 ? "STRIPE" : "PAN", data < 16 ? data + 1 : data - 15); break;
		case 46: sprintf (parameter_text, "FREQ COARSE = %+3i", data); break;
		case 47: sprintf (parameter_text, "FREQ RETRIGGER %s", data == 0 ? "OFF" : "ON"); break;
		case 48: sprintf (parameter_text, "AMP = %+8.3f [%.2f dB]", dbp, 20.0 * log10 (pow (2.0, (double) data / 1536.0))); break;
		case 49: sprintf (parameter_text, "AMP LFO = %7.3f", dbp); break;
		case 50: sprintf (parameter_text, "AMP EG LEVEL %i = %+8.3f", (nrpn_lsb >> 4) + 1, dbp); break;
		case 51: sprintf (parameter_text, "AMP EG TIME  %i = %7.3f [%.02f sec]", (nrpn_lsb >> 4) + 1, dbp, data == 0 ? 0.0 : pow (2.0, (double) (data - 8192) / 768.0)); break;
		case 52: sprintf (parameter_text, "AMP RND = %7.3f", dbp); break;
		case 53: sprintf (parameter_text, "AMP LFO SOURCE = %s %i", data < 16 ? "STRIPE" : "PAN", data < 16 ? data + 1 : data - 15); break;
		case 56: sprintf (parameter_text, "PAN = %+7.3f [%8s %4i %c]", dbp, data == 0 ? "CENTRE" : data < 0 ? "LEFT" : "RIGHT", (int) ((double) (data >= 0 ? data : - data) * 100.0 / 8191.0), '%'); break;
		case 57: sprintf (parameter_text, "PAN LFO = %7.3f", dbp); break;
		case 58: sprintf (parameter_text, "PAN EG LEVEL %i = %+7.3f", (nrpn_lsb >> 4) + 1, dbp); break;
		case 59: sprintf (parameter_text, "PAN EG TIME  %i = %7.3f [%.2f sec]", (nrpn_lsb >> 4) + 1, dbp, data == 0 ? 0.0 : pow (2.0, (double) (data - 8192) / 768.0)); break;
		case 60: sprintf (parameter_text, "PAN RND = %7.3f", dbp); break;
		case 61: sprintf (parameter_text, "PAN LFO SOURCE = %s %i", data < 16 ? "STRIPE" : "PAN", data < 16 ? data + 1 : data - 15); break;
		case nrpn_midi:
			ind = get_reception_channel ();
			if (ind > 16) sprintf (parameter_text, "MIDI CHANNEL = OFF");
			if (ind == 16) sprintf (parameter_text, "MIDI CHANNEL = ALL");
			if (ind < 16) sprintf (parameter_text, "MIDI CHANNEL = %i", ind + 1);
			break;
		case nrpn_midi_extension:
			ind = get_channel_extension ();
			if (ind < 127) sprintf (parameter_text, "MIDI CHANNEL EXTENSION = %i", ind);
			else sprintf (parameter_text, "MIDI CHANNEL EXTENSION = BROADCAST");
			break;
		case nrpn_name: case nrpn_name_character:
			for (int ind = 0; ind <= name_character; ind++) {if (program_name [name_character] == '\0') {program_name [name_character] = ' '; program_name [name_character + 1] = '\0';}}
			strncpy (command, program_name, name_character); command [name_character] = '\0';
			sprintf (parameter_text, "NAME=%s[%c]%s", command, program_name [name_character], & program_name [name_character + 1]);
			break;
		default: sprintf (parameter_text, "msb = %i; lsb = %i; min = %i; max = %i;", nrpn_msb, nrpn_lsb, range_min, range_max); break;
	}
}

void hercs_front_panel :: calculate_status (void) {
	int ind;
	char midi_text [32];
	ind = get_reception_channel ();
	if (ind > 16) sprintf (midi_text, "OFF");
	if (ind == 16) sprintf (midi_text, "ALL");
	if (ind < 16) sprintf (midi_text, "%2i", ind + 1);
	ind = get_channel_extension ();
	char extension_text [32];
	if (ind > 126) sprintf (extension_text, "BROADCAST");
	else sprintf (extension_text, "%3i", ind);
	switch (main_mode) {
		case mode_button_program:
		case mode_button_control:
		case mode_button_system:
		case mode_button_midi:
		case mode_button_arpeggiator:
		case mode_button_vector:
			sprintf (status_text, "DEVICE =%3i    MIDI CHANNEL=%s [EXTENSION=%s]", 1 + part_ids [part_id], midi_text, extension_text);
			break;
		case mode_button_stripe:
		case mode_button_lfo:
		case mode_button_sensitivity:
			if (type == 3) sprintf (status_text, "DEVICE =%3i     PAN =%i", 1 + part_ids [part_id], stereo_stripe + 1);
			else sprintf (status_text, "DEVICE =%3i     STRIPE =%i [%s]", 1 + part_ids [part_id], stripe + 1, algo_parameter_names [stripe]);
			break;
		case mode_button_dsp:
		case mode_button_dsp_parameter:
			sprintf (status_text, "DEVICE =%3i     DSP =%i [%s]", 1 + part_ids [part_id], dsp_id + 1, dsp_algo_parameter_names [dsp_id]);
			break;
		case mode_button_algo_parameter:
			sprintf (status_text, "DEVICE =%3i", 1 + part_ids [part_id]);
			break;
	}
}

void hercs_front_panel :: press_parameter_button (int button) {
	switch (main_mode) {
		case mode_button_program: program_parameter = button; break;
		case mode_button_control: control_parameter = button; break;
		case mode_button_stripe: stripe_parameter = button; break;
		case mode_button_lfo: lfo_parameter = button; break;
		case mode_button_sensitivity: sensitivity_parameter = button; break;
		case mode_button_arpeggiator: arpeggiator_parameter = button; break;
		case mode_button_vector: vector_parameter = button; break;
		case mode_button_dsp: dsp_parameter = button; break;
		case mode_button_dsp_parameter: dsp_p_parameter = button; break;
		case mode_button_algo_parameter: algo_parameter = button; break;
		case mode_button_system: system_parameter = button; break;
		case mode_button_midi: midi_parameter = button; break;
	}
	change_parameter (button);
}

void send_string_request (midi_stream * line, int channel, int ind) {
	line -> open_system_exclusive ();
	line -> insert (channel);
	line -> insert (ind);
	line -> close_system_exclusive ();
}

void hercs_front_panel :: press_mode_button (int button) {
	if (main_mode == mode_button_algo_parameter) {
		for (int ind = 0; ind < 16; ind++) send_string_request (line, get_transmission_channel (), ind);
	}
	main_mode = button;
	change_main_mode (button);
	if (control_store_required) send_control_store_request ();
	control_store_required = false;
}

void hercs_front_panel :: press_type_button (int button) {
	type = button;
	change_type (type);
}

void hercs_front_panel :: press_sensitivity_button (int button) {
	sensitivity_type = button;
	change_sensitivity_type (sensitivity_type);
}

void hercs_front_panel :: press_dsp_sensitivity_button (int button) {
	dsp_sensitivity_controller = button;
	change_dsp_sensitivity_controller (dsp_sensitivity_controller);
}

void hercs_front_panel :: press_part_button (int button) {
	switch (main_mode) {
		case mode_button_program:
		case mode_button_control:
		case mode_button_system:
		case mode_button_midi:
		case mode_button_arpeggiator:
		case mode_button_vector:
			part_id = button;
			send_device_request (part_ids [part_id]);
			change_device (button);
			break;
		case mode_button_stripe:
		case mode_button_lfo:
		case mode_button_sensitivity:
			if (type == 3) {stereo_stripe = button; change_stereo_stripe (button);}
			else {stripe = button; change_stripe (button);}
			break;
		case mode_button_dsp:
		case mode_button_dsp_parameter:
			if (button >= 0 && button < 8) {
				dsp_id = button;
				change_dsp (button);
			}
			break;
		case mode_button_algo_parameter:
			change_algo_parameter_base (button);
			break;
	}
}

void hercs_front_panel :: press_bank_type_button (int button) {
	bank_change_mode = button;
	switch (bank_change_mode) {
		case 0: bank_msb = & preset_bank_msb; bank_lsb = & preset_bank_lsb; break;
		case 1: bank_msb = & user_bank_msb; bank_lsb = & user_bank_lsb; break;
		case 2: bank_msb = & algo_bank_msb; bank_lsb = & algo_bank_lsb; break;
		case 3: bank_msb = & arp_bank_msb; bank_lsb = & arp_bank_lsb; break;
		case 4: bank_msb = & dsp_algo_bank_msb; bank_lsb = & dsp_algo_bank_lsb; break;
		case 5: bank_msb = & preset_dsp_bank_msb; bank_lsb = & preset_dsp_bank_lsb; break;
		case 6: bank_msb = & user_dsp_bank_msb; bank_lsb = & user_dsp_bank_lsb; break;
		case 7: bank_msb = & style_bank_msb; bank_lsb = & style_bank_lsb; break;
		default: break;
	}
	int channel = get_transmission_channel (get_channel_extension ());
	line -> insert_nrpn (channel, 2, 1, * bank_msb);
	line -> insert_nrpn (channel, 2, 2, * bank_lsb);
	change_bank_type_selector (bank_change_mode);
}

void hercs_front_panel :: press_command_button (int button) {
	switch (button) {
		case 0: switch (command_mode) {
			case 0: command_mode = 1; break;
			case 1: command_mode = 0; break;
			case 2: command_mode = 1; break;
			default: break;
			} break;
		case 1: switch (command_mode) {
			case 0:
				if (nrpn_msb == nrpn_name) {
					if (name_character >= 23) break;
					name_character++;
					for (int ind = 0; ind <= name_character; ind++) {if (program_name [name_character] == '\0') {program_name [name_character] = ' '; program_name [name_character + 1] = '\0';}}
					line -> open_system_exclusive ();
					line -> insert (0x10 | get_transmission_channel ());
					line -> insert (0x11);
					line -> insert (program_name);
					line -> close_system_exclusive ();
				}
				break;
			case 1: reset_command (); break;
			case 2: store_command (store_location); break;
			default: break;
			} command_mode = 0; break;
		case 2: switch (command_mode) {
			case 0: command_mode = 2; break;
			case 1: command_mode = 2; break;
			case 2: command_mode = 0; break;
			default: break;
			} break;
		default: break;
	}
	change_command_mode (command_mode);
	calculate_display ();
	change_parameter_text ();
}


void hercs_front_panel :: change_main_mode (int mode) {}
void hercs_front_panel :: change_type (int type) {}
void hercs_front_panel :: change_sensitivity_type (int type) {}
void hercs_front_panel :: change_dsp_sensitivity_controller (int type) {}
void hercs_front_panel :: change_parameter (int parameter) {}
void hercs_front_panel :: change_parameter_name (int parameter, char * text) {}
void hercs_front_panel :: change_device (int device) {}
void hercs_front_panel :: change_stripe (int stripe) {}
void hercs_front_panel :: change_stereo_stripe (int stripe) {}
void hercs_front_panel :: change_dsp (int dsp) {}
void hercs_front_panel :: change_algo_parameter_base (int parameter) {}
void hercs_front_panel :: change_bank_type_selector (int type) {}
void hercs_front_panel :: change_command_mode (int mode) {}
void hercs_front_panel :: change_parameter_text (void) {}
void hercs_front_panel :: change_status_text (void) {}
void hercs_front_panel :: open_file_for_input (char * file_name) {}
void hercs_front_panel :: open_file_for_record (int seconds, char * file_name) {}
void hercs_front_panel :: update_controllers (int nrpn_lsb, int data_msb, int data_lsb, bool separate_vector_coordinates) {}

void hercs_front_panel :: reset_command (void) {
	line -> open_system_exclusive ();
	line -> insert (0x40 | get_transmission_channel ());
	line -> insert (0x40);
	line -> close_system_exclusive ();
	send_device_request (part_ids [part_id]);
}
void hercs_front_panel :: store_command (int location) {
	location &= 0x7f;
	int channel = get_transmission_channel (get_channel_extension ());
	line -> insert_nrpn (channel, 2, 2, 127);
	line -> insert_nrpn (channel, 2, 3, location);
	line -> insert_nrpn (channel, 2, 2, 0);
}
void hercs_front_panel :: send_control_store_request (void) {
	line -> open_system_exclusive ();
	line -> insert (0x40 | get_transmission_channel ());
	line -> insert (0x03);
	line -> close_system_exclusive ();
}

void hercs_front_panel :: console_print (char * text) {}
void hercs_front_panel :: console_operations (int a, int b, int c) {}
void hercs_front_panel :: text_operations (int selector, char * text) {}
void hercs_front_panel :: identity_operations (int * identity) {}
void hercs_front_panel :: error_operations (int channel, int code, int msb, int lsb, char * error) {}

void hercs_front_panel :: midi_system_exclusive (midi_stream * line) {
	char * cp;
	int ind, sub;
	char command [256];
	line -> mark ();
	ind = line -> get ();
	if (ind == 0x7e) {
		int identity [256];
		int ip = 0;
		identity [ip++] = 0xf0;
		while (ind != 0xf7 && line -> message_waiting ()) {
			if (ip < 200) identity [ip++] = ind;
			ind = line -> get ();
		}
		identity [ip++] = 0xf7;
		identity_operations (identity);
		return;
	}
	line -> restore ();
	if (! line -> check_system_exclusive ()) {
		line -> restore ();
		line -> get_f7 ();
		return;
	}
	int selector = line -> get ();
	int channel = selector & 0xf;
	selector >>= 4;
	int sub_selector;
	int msb, lsb;
	switch (selector) {
	case 1:
		sub_selector = line -> get ();
		if (sub_selector < 0x10) {line -> get (algo_parameter_names [sub_selector]); break;}
		if (sub_selector >= 0x20) {line -> get (command); text_operations (sub_selector, command); break;}
		switch (sub_selector) {
		case 0x10: line -> get (algo_name); break;
		case 0x11: line -> get (program_name); if (nrpn_msb == nrpn_name || nrpn_msb == nrpn_name_character) {calculate_display (); change_parameter_text ();} break;
		case 0x12: line -> get (bank_msb_name); break;
		case 0x13: line -> get (bank_lsb_name); break;
		case 0x14: line -> get (category_name); break;
		case 0x15: line -> get (pattern_name); break;
		case 0x16: line -> get (style_name); break;
		case 0x18: case 0x19: case 0x1a: case 0x1b:
		case 0x1c: case 0x1d: case 0x1e: case 0x1f: line -> get (dsp_algo_name); strcpy (dsp_algo_parameter_names [sub_selector - 0x18], dsp_algo_name); break;
		default: line -> get_f7 (); break;

		}
		break;
	case 2:
		switch (channel) {
		case 0x6: line -> get (command); open_file_for_input (command); line -> get_f7 (); break;
		case 0x7: ind = line -> get_int (); line -> get (command); open_file_for_record (ind, command); line -> get_f7 (); break;
		case 0xa: line -> get (command); console_print (command); break;
		case 0xb: line -> get (command); cp = command; while (* cp != '\0') {* cp = 128 + * cp; cp++;} console_print (command); line -> get_f7 (); break;
		case 0x9: ind = line -> get (); sub = line -> get (); console_operations (ind, sub, line -> get ()); line -> get_f7 (); break;
		default: line -> get_f7 (); break;
		}
		break;
	case 3:
		switch (channel) {
		case 0:
			line -> get (); // device
			sub_selector = line -> get ();
			set_midi_channel (sub_selector);
			line -> get_f7 ();
			if (main_mode == mode_button_midi && nrpn_msb != 2 && nrpn_lsb != 4) {
				calculate_display ();
				change_parameter_text ();
				calculate_status ();
				change_status_text ();
			} else {
				send_panel_controllers_request ();
				send_parameter_request ();
			}
			break;
		case 1:
			line -> get (); // device
			sub_selector = line -> get ();
			line -> get_f7 ();
			set_midi_channel_extension (sub_selector);
			this -> line -> insert_channel_extension (sub_selector);
			if (main_mode == mode_button_midi) {
				calculate_display ();
				change_parameter_text ();
				calculate_status ();
				change_status_text ();
			}
			break;
		default: line -> get_f7 (); break;
		}
		break;
	case 5:
		sub_selector = line -> get ();
		switch (sub_selector) {
			case 0x22: if (line -> get () != 0xf7) line -> get (algo_parameter_name); break;
			case 0x23: if (line -> get () != 0xf7) line -> get (dsp_parameter_name); break;
			default: line -> get_f7 (); break;
		}
		break;
	case 7:
		sub_selector = line -> get ();
		if (sub_selector == 0xf7) return;
		msb = line -> get ();
		if (msb == 0xf7) return;
		lsb = line -> get ();
		if (lsb == 0xf7) return;
		line -> get (command);
		error_operations (channel, sub_selector, msb, lsb, command);
		break;
	default: line -> get_f7 (); break;
	}
}

void hercs_front_panel :: midi_nrpn (int nrpn_msb, int nrpn_lsb, int data_msb, int data_lsb) {
	if (panel_controllers_skip > 0) {
		if (nrpn_msb == 0) {if (nrpn_lsb < 23) controller_ids [nrpn_lsb] = data_msb;}
		if (nrpn_msb == 1) update_controllers (nrpn_lsb, data_msb, data_lsb, false);
		panel_controllers_skip--;
		return;
	}
	if (this -> nrpn_msb == nrpn_tune_key) nrpn_msb = nrpn_tune_key;
	this -> nrpn_msb = nrpn_msb; this -> nrpn_lsb = nrpn_lsb;
	if (nrpn_msb == nrpn_tune_key) {
		data = (data_msb << 7) + data_lsb;
		tune_key = nrpn_lsb;
	} else {
		if (msb_only) data = data_msb;
		else data = (data_msb << 7) + data_lsb;
		if (data > range_max) {
			if (msb_only) data -= 128;
			else data -= 16384;
		}
	}
	if (nrpn_msb == 2) {
		if (nrpn_lsb == 1) * bank_msb = data;
		if (nrpn_lsb == 2) * bank_lsb = data;
	}
//	wxMessageBox (wxString :: Format (_T ("nrpn [%i %i %i]"), nrpn_msb, nrpn_lsb, panel_controllers_skip), _T ("INFO"), wxOK, NULL);
	calculate_display ();
	change_parameter_text ();
//	sprintf (status_text, "%02X %02X %02X %02X", nrpn_msb, nrpn_lsb, data_msb, data_lsb);
	calculate_status ();
	change_status_text ();
}

void hercs_front_panel :: send_extra_requests (int extended_channel) {
	int channel = extended_channel & 0xf;
	if (nrpn_msb == nrpn_name || nrpn_msb == nrpn_name_character) {
		send_string_request (line, channel, 0x11);
		return;
	}
	if (nrpn_msb == 2) {
//		wxMessageBox (wxString :: Format (_T ("request [%i %i]"), channel, nrpn_lsb), _T ("INFO"), wxOK, NULL);
		switch (nrpn_lsb) {
		case 0: send_string_request (line, channel, 0x14); break;
		case 1: send_string_request (line, channel, 0x12); break;
		case 2: send_string_request (line, channel, 0x13); break;
		case 3:
			for (int ind = 0; ind < 16; ind++) send_string_request (line, channel, ind);
			for (int ind = 0; ind < total_dsp; ind++) send_string_request (line, channel, 0x18 + ind);
			switch (bank_change_mode) {
			case 0: case 1: send_string_request (line, channel, 0x14); send_string_request (line, channel, 0x11); send_panel_controllers_request (); break;
			case 2: send_string_request (line, channel, 0x10); break;
			case 3: send_string_request (line, channel, 0x15); break;
			case 4: send_string_request (line, channel, 0x18 + (dsp_algo_bank_lsb & 0x7)); break;
			case 5: case 6: break;
			case 7: send_string_request (line, channel, 0x16); break;
			default: break;
			}
			send_string_request (line, channel, 0x11);
			break;
		default: break;
		}
		return;
	}
	if (nrpn_msb == 15) {
//		wxMessageBox (_T ("dsp parameter name request"), _T ("INFO"), wxOK, NULL);
		line -> open_system_exclusive ();
		line -> insert (0x40 + channel);
		line -> insert (0x23);
		line -> insert (nrpn_lsb);
		line -> close_system_exclusive ();
		return;
	}
	if (nrpn_msb == 7) {
		line -> open_system_exclusive ();
		line -> insert (0x40 + channel);
		line -> insert (0x22);
		line -> insert (nrpn_lsb);
		line -> close_system_exclusive ();
		return;
	}
}

void hercs_front_panel :: send_parameter_request (void) {
//	if (nrpn_msb == 2 && nrpn_lsb == 4) return;
	if (nrpn_msb == nrpn_midi) {
		line -> open_system_exclusive ();
		line -> insert (0x20);
		line -> insert (part_ids [part_id]);
		line -> close_system_exclusive ();
		return;
	}
	if (nrpn_msb == nrpn_midi_extension) {
		line -> open_system_exclusive ();
		line -> insert (0x21);
		line -> insert (part_ids [part_id]);
		line -> close_system_exclusive ();
		return;
	}
	int extended_channel = get_transmission_channel (get_channel_extension ());
	int channel = get_transmission_channel ();
	if (nrpn_msb == nrpn_tune_key) {
		send_parameter_request (channel, extended_channel, 4, tune_key);
		return;
	}
	send_extra_requests (extended_channel);
	send_parameter_request (channel, extended_channel, nrpn_msb, nrpn_lsb);
}
//	wxMessageBox (wxString :: Format (_T ("channel [%06X %06X]"), channel, line -> get_channel_extension ()), _T ("INFO"), wxOK, NULL);
//	line -> insert_control (channel, 99, nrpn_msb);
//	line -> insert_control (channel, 98, nrpn_lsb);
//	channel = get_transmission_channel ();
//	line -> open_system_exclusive ();
//	line -> insert (0x40 | channel);
//	line -> insert (0x10);
//	line -> close_system_exclusive ();
//}

void hercs_front_panel :: send_parameter_request (int channel, int extended_channel, int msb, int lsb) {
	line -> insert_control (channel, 99, msb);
	line -> insert_control (channel, 98, lsb);
	line -> open_system_exclusive ();
	line -> insert (0x40 | channel);
	line -> insert (0x10);
	line -> close_system_exclusive ();
}

void hercs_front_panel :: send_panel_controllers_request (void) {
	int extended_channel = get_transmission_channel (get_channel_extension ());
	int channel = get_transmission_channel ();
	panel_controllers_skip = 45;
	for (int ind = 0; ind < 22; ind++) send_parameter_request (channel, extended_channel, 0, ind);
	for (int ind = 0; ind < 22; ind++) send_parameter_request (channel, extended_channel, 1, ind);
	send_parameter_request (channel, extended_channel, 1, 23);
}

void hercs_front_panel :: send_device_request (int device) {
	line -> open_system_exclusive ();
	line -> insert (0x21);
	line -> insert (device);
	line -> close_system_exclusive ();
	line -> open_system_exclusive ();
	line -> insert (0x20);
	line -> insert (device);
	line -> close_system_exclusive ();
}

void hercs_front_panel :: encoder_change (midi_stream * line, int delta) {
	if (command_mode == 2) {
		store_location += delta;
		if (store_location < 0) store_location = 0; if (store_location > 127) store_location = 127;
		calculate_display ();
		change_parameter_text ();
		return;
	}
	if (nrpn_msb == nrpn_name_character) {
		if (delta > 1) delta = 1; if (delta < -1) delta = -1;
		name_character += delta;
		if (name_character < 0) name_character = 0; if (name_character > 23) name_character = 23;
		for (int ind = 0; ind <= name_character; ind++) {if (program_name [name_character] == '\0') {program_name [name_character] = ' '; program_name [name_character + 1] = '\0';}}
		line -> open_system_exclusive ();
		line -> insert (0x10 | get_transmission_channel ());
		line -> insert (0x11);
		line -> insert (program_name);
		line -> close_system_exclusive ();
		calculate_display ();
		change_parameter_text ();
		return;
	}
	if (nrpn_msb == nrpn_name) {
		int ch = program_name [name_character];
		ch += delta * encoder_sensitivity;
		if (ch < ' ') ch = ' '; if (ch > 127) ch = 127;
		program_name [name_character] = ch;
		for (int ind = 0; ind <= name_character; ind++) {if (program_name [name_character] == '\0') {program_name [name_character] = ' '; program_name [name_character + 1] = '\0';}}
		line -> open_system_exclusive ();
		line -> insert (0x10 | get_transmission_channel ());
		line -> insert (0x11);
		line -> insert (program_name);
		line -> close_system_exclusive ();
		calculate_display ();
		change_parameter_text ();
		return;
	}
	if (nrpn_msb == nrpn_tune_key) {
		tune_key += delta;
		if (tune_key < 0) tune_key = 0; if (tune_key > 127) tune_key = 127;
		send_parameter_request ();
		return;
	}
	if (range_max - range_min > 128) delta *= encoder_sensitivity;
	if (delta < -1 && data == range_max && range_max > 0) delta++;
	if (delta > 1 && data == range_min && range_max == 0) delta--;
	data += delta;
	if (data > range_max) data = range_max;
	if (data < range_min) data = range_min;
	switch (nrpn_msb) {
	case 17:
		if (data > stripe && data < 16) {
			if (type == 3) data = delta > 0 ? 16 : hercules_number - 1;
			else data = stripe;
		}
		break;
	case 37: case 45: case 53: case 61:
		if (data >= hercules_number && data < 16) data = delta > 0 ? 16 : hercules_number - 1;
		break;
	default: break;
	}
	int channel;
	if (nrpn_msb == nrpn_midi) {
		channel = get_reception_channel () + delta;
		if (channel < 0) channel = 0;
		if (channel == 0x7e) channel = 16;
		if (channel > 16) channel = 0x7f;
		set_midi_channel (channel);
		line -> open_system_exclusive ();
		line -> insert (0x30);
		line -> insert (part_ids [part_id]);
		line -> insert (channel);
		line -> close_system_exclusive ();
		calculate_display (); change_parameter_text ();
		control_store_required = true;
		return;
	}
	if (nrpn_msb == nrpn_midi_extension) {
		int extension = get_channel_extension () + delta;
		if (extension > 127) extension = 0; if (extension < 0) extension = 127;
		set_midi_channel_extension (extension);
		line -> open_system_exclusive ();
		line -> insert (0x31);
		line -> insert (part_ids [part_id]);
		line -> insert (extension);
		line -> close_system_exclusive ();
		line -> insert_channel_extension (extension);
		calculate_display (); change_parameter_text ();
		control_store_required = true;
		return;
	}
	channel = get_transmission_channel (get_channel_extension ());
	if (nrpn_msb == 2) {
		if (nrpn_lsb >= 4 && nrpn_lsb <= 7) control_store_required = true;
	}
	if (nrpn_msb == 2 && nrpn_lsb == 4) {
		line -> insert_nrpn_14 (channel, nrpn_msb, nrpn_lsb, data > 127 ? 0x3fff : data);
		if (data < 128) part_ids [part_id] = data;
	} else {
		if (msb_only) line -> insert_nrpn (channel, nrpn_msb, nrpn_lsb, data & 0x7f);
		else line -> insert_nrpn_14 (channel, nrpn_msb, nrpn_lsb, data & 0x3fff);
	}
	if (nrpn_msb == 2 && nrpn_lsb == 3 && bank_msb == & user_bank_msb) {store_location = data & 0x7f;}
	if (nrpn_msb == 0) {control_store_required = true;}
	if (nrpn_msb == 2 && nrpn_lsb != 4) {send_parameter_request (); return;}
	if (nrpn_msb == 1) update_controllers (nrpn_lsb, data >> 7, data & 0x7f, true);
	calculate_display ();
	change_parameter_text ();
}

void hercs_front_panel :: controller_change (int controller_id, double value) {
	if (controller_id < 0 || controller_id > 23) return;
	int channel = get_transmission_channel (get_channel_extension ());
	int data = value > 0.5 ? (int) (64.0 + (value - 0.5) * 126.0) : (int) (value * 128.0);
//	int data = (int) (value * 127.0);
//	if (data > 127) data = 127; if (data < 0) data = 0;
	switch (controller_ids [controller_id]) {
	case 127: break;
	case 126: line -> insert_pat (channel, 64, data); break;
	case 125: line -> insert_cat (channel, data); break;
	case 124: line -> insert_pitchbend (channel, data);
	default: line -> insert_control (channel, controller_id != 23 ? controller_ids [controller_id] : data == 0.0 ? 126 : 127, data); break;
	}
//	if (controller_ids [controller_id] == 124) line -> insert_pitchbend (channel, data);
//	else line -> insert_control (channel, controller_id != 23 ? controller_ids [controller_id] : data == 0.0 ? 126 : 127, data);
	if (controller_id == 13 || controller_id == 14) {
		if (nrpn_msb == 1 && controller_id == nrpn_lsb) {
			this -> data = data << 7;
			calculate_display ();
			change_parameter_text ();
		}
		return;
	}
	range_min = 0; range_max = 16383; msb_only = false;
	nrpn_msb = 1; nrpn_lsb = controller_id; this -> data = data << 7;
	switch (controller_id) {
	case 17: case 18: case 20: case 23: range_max = 1; msb_only = true; break;
	default: break;
	}
	calculate_display ();
	change_parameter_text ();
	main_mode = mode_button_control;
	switch (controller_id) {
	case 0: control_parameter = 0; break;
	case 1: control_parameter = 1; type = 0; break;
	case 2: control_parameter = 1; type = 1; break;
	case 3: control_parameter = 1; type = 2; break;
	case 4: control_parameter = 3; break;
	case 5: control_parameter = 4; break;
	case 6: control_parameter = 5; break;
	case 7: control_parameter = 6; break;
	case 8: control_parameter = 9; break;
	case 9: control_parameter = 2; type = 0; break;
	case 10: control_parameter = 2; type = 1; break;
	case 11: control_parameter = 2; type = 2; break;
	case 12: control_parameter = 2; type = 3; break;
	case 13: control_parameter = 7; break;
	case 14: control_parameter = 8; break;
	case 15: control_parameter = 10; break;
	case 16: control_parameter = 1; type = 3; break;
	case 17: control_parameter = 11; break;
	case 18: control_parameter = 12; break;
	case 19: control_parameter = 13; break;
	case 20: control_parameter = 14; break;
	case 21: control_parameter = 15; break;
	case 22: control_parameter = 16; break;
	case 23: control_parameter = 17; break;
	default: break;
	}
	change_main_mode (main_mode);
//	wxMessageBox (wxString :: Format (_T ("control change [%i %i %i]"), channel, panel . controller_ids [controller_id], data), _T ("INFO"), wxOK, NULL);
}


void hercs_front_panel :: set_midi_out (midi_stream * line) {this -> line = line;}

hercs_dx_type_panel :: hercs_dx_type_panel (void) {
	algo_parameter_base = 0;
	previous_algo_parameter_base = -1;
	previous_main_mode = -1;
	previous_dsp_id = -1;
}


void hercs_dx_type_panel :: set_dsp_parameter_names (void) {
	change_parameter_name (1, dsp_id == 0 ? (char *) "OUT" : (char *) "");
	change_parameter_name (2, dsp_id == 1 ? (char *) "OUT" : dsp_id < 1 ? (char *) "=>2" : (char *) "");
	change_parameter_name (3, dsp_id == 2 ? (char *) "OUT" : dsp_id < 2 ? (char *) "=>3" : (char *) "");
	change_parameter_name (4, dsp_id == 3 ? (char *) "OUT" : dsp_id < 3 ? (char *) "=>4" : (char *) "");
	change_parameter_name (5, dsp_id == 4 ? (char *) "OUT" : dsp_id < 4 ? (char *) "=>5" : (char *) "");
	change_parameter_name (6, dsp_id == 5 ? (char *) "OUT" : dsp_id < 5 ? (char *) "=>6" : (char *) "");
	change_parameter_name (7, dsp_id == 6 ? (char *) "OUT" : dsp_id < 6 ? (char *) "=>7" : (char *) "");
	change_parameter_name (8, dsp_id == 7 ? (char *) "OUT" : dsp_id < 7 ? (char *) "=>8" : (char *) "");
}

void hercs_dx_type_panel :: set_indexed_parameter_names (int shift) {
	int ind = 0;
	int parameter = 0;
	char command [32];
	while (ind < 16) {
		sprintf (command, "%i", ind + shift);
		change_parameter_name (parameter, command);
		ind++;
		parameter++;
		if (parameter == 8) parameter++;
	}
	change_parameter_name (8, "");
	change_parameter_name (17, "");
}

void hercs_dx_type_panel :: change_main_mode (int mode) {set_all_buttons ();}
void hercs_dx_type_panel :: change_type (int type) {set_all_buttons ();}
void hercs_dx_type_panel :: change_sensitivity_type (int type) {set_all_buttons ();}
void hercs_dx_type_panel :: change_dsp_sensitivity_controller (int type) {set_all_buttons ();}
void hercs_dx_type_panel :: change_parameter (int parameter) {set_all_buttons ();}
void hercs_dx_type_panel :: change_device (int device) {set_all_buttons ();}
void hercs_dx_type_panel :: change_stripe (int stripe) {set_all_buttons ();}
void hercs_dx_type_panel :: change_stereo_stripe (int stripe) {set_all_buttons ();}
void hercs_dx_type_panel :: change_dsp (int dsp) {set_all_buttons ();}
void hercs_dx_type_panel :: change_bank_type_selector (int type) {set_all_buttons ();}
void hercs_dx_type_panel :: change_command_mode (int mode) {set_all_buttons ();}
//	int ind;
//	for (ind = 0; ind < 8; ind++) {set_button (ind + part_button_0, ind == dsp ? 1.0 : 0.0);}
//	if (main_mode == mode_button_dsp) set_dsp_parameter_names ();
//}

void hercs_dx_type_panel :: change_algo_parameter_base (int parameter) {
	algo_parameter_base = parameter << 4;
	for (int ind = 0; ind < 10; ind++) {set_button (ind + part_button_0, ind == parameter ? 1.0 : 0.0);}
//	set_indexed_parameter_names (algo_parameter_base);
	set_all_buttons ();
}


void hercs_dx_type_panel :: set_sensitivity_buttons (void) {
	int type = sensitivity_type;
	switch (main_mode) {
		case mode_button_program:
		case mode_button_control:
		case mode_button_stripe:
		case mode_button_arpeggiator:
		case mode_button_vector:
		case mode_button_dsp_parameter:
		case mode_button_algo_parameter:
		case mode_button_system:
		case mode_button_midi:
			type = -1;
			break;
		case mode_button_lfo: if (lfo_parameter < 14) type = -1; break;
		case mode_button_dsp: if (dsp_parameter == 0 || dsp_sensitivity_controller == 0) type = -1; break;
		default: break;
	}
	set_button (sensitivity_button_lc, type == 0 ? 1.0 : 0.0);
	set_button (sensitivity_button_bp, type == 1 ? 1.0 : 0.0);
	set_button (sensitivity_button_rc, type == 2 ? 1.0 : 0.0);
}

void hercs_dx_type_panel :: set_parameter_buttons (void) {
	int parameter = 0;
	switch (main_mode) {
	case mode_button_program: parameter = program_parameter; break;
	case mode_button_control: parameter = control_parameter; break;
	case mode_button_stripe: parameter = stripe_parameter; break;
	case mode_button_lfo: parameter = lfo_parameter; break;
	case mode_button_sensitivity: parameter = sensitivity_parameter; break;
	case mode_button_arpeggiator: parameter = arpeggiator_parameter; break;
	case mode_button_vector: parameter = vector_parameter; break;
	case mode_button_dsp: parameter = dsp_parameter; break;
	case mode_button_dsp_parameter: parameter = dsp_p_parameter; if (parameter > 7) parameter++; break;
	case mode_button_algo_parameter: parameter = algo_parameter - algo_parameter_base; if (parameter > 7) parameter++; break;
	case mode_button_system: parameter = system_parameter; break;
	case mode_button_midi: parameter = midi_parameter; break;
	default: break;
	}
	set_parameter_buttons (parameter);
}

void hercs_dx_type_panel :: set_parameter_buttons (int parameter) {
	if (main_mode == mode_button_program) {
		for (int ind = 0; ind <= 8; ind++) {
			set_button (ind + pb0, ind == parameter ? 1.0 : 0.0);
			set_button (ind + pb9, ind == bank_change_mode ? 1.0 : 0.0);
		}
		return;
	}
	for (int ind = 0; ind < 18; ind++) set_button (ind + pb0, ind == parameter ? 1.0 : 0.0);
	if (main_mode != mode_button_dsp || dsp_parameter == 0) {
		for (int ind = 0; ind < 18; ind++) set_button (ind + pb0, ind == parameter ? 1.0 : 0.0);
	} else {
		for (int ind = 0; ind < 14; ind++) set_button (ind + pb0, ind == parameter ? 1.0 : 0.0);
		for (int ind = 0; ind < 4; ind++) set_button (ind + pb14, ind == dsp_sensitivity_controller ? 1.0 : 0.0);
	}
}

void hercs_dx_type_panel :: set_button (int button, double value) {}

void hercs_dx_type_panel :: press_part_button_dx (int button) {press_part_button (button);}

void hercs_dx_type_panel :: press_parameter_button_dx (int button) {
	if (main_mode == mode_button_program) {
		if (button <= 8) press_parameter_button (button);
		else press_bank_type_button (button < 17 ? button - 9 : 7);
		return;
	}
	if (main_mode == mode_button_dsp && button >= 14) {press_dsp_sensitivity_button (button - 14); return;}
	if (main_mode == mode_button_algo_parameter) {
		if (button > 7) button--; if (button > 15) button--;
		press_parameter_button (button + algo_parameter_base);
		return;
	}
	if (main_mode == mode_button_dsp_parameter) {if (button > 7) button--; if (button > 15) button--;}
	press_parameter_button (button);
}

void hercs_dx_type_panel :: press_zoom_button (int button) {
	switch (button) {
	case 0: encoder_sensitivity = 1; break;
	case 1: encoder_sensitivity = 8; break;
	case 2: encoder_sensitivity = 128; break;
	default: break;
	}
	set_zoom_buttons ();
}


void hercs_dx_type_panel :: set_zoom_buttons (void) {
	set_button (extra_1_button, encoder_sensitivity == 1 ? 1.0 : 0.0);
	set_button (extra_8_button, encoder_sensitivity == 8 ? 1.0 : 0.0);
	set_button (extra_128_button, encoder_sensitivity == 128 ? 1.0 : 0.0);
}

void hercs_dx_type_panel :: set_command_buttons (void) {
	set_button (extra_reset_button, command_mode == 1 ? 1.0 : 0.0);
	set_button (extra_enter_button, command_mode != 0 ? 1.0 : 0.0);
	set_button (extra_store_button, command_mode == 2 ? 1.0 : 0.0);
}

void hercs_dx_type_panel :: set_all_buttons (void) {
	set_main_buttons ();
	set_part_buttons ();
	set_type_buttons ();
	set_sensitivity_buttons ();
	set_parameter_buttons ();
	set_button_names ();
	set_zoom_buttons ();
	set_command_buttons ();
}
void hercs_dx_type_panel :: set_main_buttons (void) {
	for (int ind = 0; ind <= mode_button_midi; ind++) set_button (ind, ind == main_mode ? 1.0 : 0.0);
}

void hercs_dx_type_panel :: set_part_buttons (int part) {
	part += part_button_0;
	for (int ind = part_button_0; ind <= part_button_9; ind++) set_button (ind, ind == part ? 1.0 : 0.0);
}
void hercs_dx_type_panel :: set_part_buttons (void) {
	switch (main_mode) {
	case mode_button_program:
	case mode_button_control:
	case mode_button_arpeggiator:
	case mode_button_vector:
	case mode_button_midi:
	case mode_button_system: set_part_buttons (part_id); break;
	case mode_button_stripe:
	case mode_button_lfo:
	case mode_button_sensitivity: set_part_buttons (type == 3 ? stereo_stripe : stripe); break;
	case mode_button_dsp:
	case mode_button_dsp_parameter: set_part_buttons (dsp_id); break;
	case mode_button_algo_parameter: set_part_buttons (algo_parameter_base >> 4); break;
	default: break;
	}
}

void hercs_dx_type_panel :: set_type_buttons (void) {
	int mode = 0;
	switch (main_mode) {
	case mode_button_program: break;
	case mode_button_control: if (control_parameter == 1 || control_parameter == 2) mode = 1; break;
	case mode_button_stripe: mode = 1; break;
	case mode_button_lfo: mode = 2; break;
	case mode_button_sensitivity: mode = 1; break;
	case mode_button_arpeggiator: break;
	case mode_button_vector: break;
	case mode_button_dsp: break;
	case mode_button_dsp_parameter: break;
	case mode_button_algo_parameter: break;
	case mode_button_midi: break;
	case mode_button_system: if (system_parameter == 1 || system_parameter == 2) mode = 1; break;
	default: break;
	}
	set_type_buttons (mode);
}

void hercs_dx_type_panel :: set_type_buttons (int mode) {
	int ind;
	switch (mode) {
	case 0: for (ind = 0; ind < 4; ind++) set_button (ind + type_button_freq, 0.0); break;
	case 1: for (ind = 0; ind < 4; ind++) set_button (ind + type_button_freq, ind == type ? 1.0 : 0.0); break;
	case 2: for (ind = 0; ind < 4; ind++) set_button (ind + type_button_freq, (type < 3 && ind < 3) || (type == 3 && ind == 3) ? 1.0 : 0.0); break;
	default: break;
	}
}

void hercs_dx_type_panel :: set_button_names (void) {
	if (previous_main_mode == main_mode && previous_dsp_id == dsp_id && previous_algo_parameter_base == algo_parameter_base) return;
	previous_main_mode = main_mode;
	previous_dsp_id = dsp_id;
	previous_algo_parameter_base = algo_parameter_base;
	switch (main_mode) {
		case mode_button_program:
			change_parameter_name (0, "PROG");
			change_parameter_name (1, "MSB");
			change_parameter_name (2, "LSB");
			change_parameter_name (3, "CTGR");
			change_parameter_name (4, "CHAR");
			change_parameter_name (5, "NAME");
			change_parameter_name (6, "TUNK");
			change_parameter_name (7, "TUNE");
			change_parameter_name (8, "TUNM");
			change_parameter_name (9, "PRES");
			change_parameter_name (10, "USER");
			change_parameter_name (11, "ALGO");
			change_parameter_name (12, "ARP");
			change_parameter_name (13, "DSPA");
			change_parameter_name (14, "DSPP");
			change_parameter_name (15, "DSPU");
			change_parameter_name (16, "STYLE");
			change_parameter_name (17, "");
//			set_parameter_buttons (program_parameter);
			break;
		case mode_button_control:
			change_parameter_name (0, "PITCH");
			change_parameter_name (1, "SHIFT");
			change_parameter_name (2, "LFO");
			change_parameter_name (3, "ATK");
			change_parameter_name (4, "DECAY");
			change_parameter_name (5, "SUST");
			change_parameter_name (6, "REL");
			change_parameter_name (7, "V/X");
			change_parameter_name (8, "V/Y");
			change_parameter_name (9, "SPEED");
			change_parameter_name (10, "VOL");
			change_parameter_name (11, "HOLD");
			change_parameter_name (12, "PSW");
			change_parameter_name (13, "PTM");
			change_parameter_name (14, "PCTRL");
			change_parameter_name (15, "D/W");
			change_parameter_name (16, "G/VOL");
			change_parameter_name (17, "POLY");
//			set_parameter_buttons (control_parameter);
			break;
		case mode_button_midi:
			change_parameter_name (0, "CHAN");
			change_parameter_name (1, "EXT");
			change_parameter_name (2, "DEV");
			change_parameter_name (3, ""); change_parameter_name (4, ""); change_parameter_name (5, ""); change_parameter_name (6, "");
			change_parameter_name (7, "ARR"); change_parameter_name (8, "RSVD");
			for (int ind = 9; ind <= 17; ind++) change_parameter_name (ind, "");
			break;
		case mode_button_system:
			change_parameter_name (0, "PITCH");
			change_parameter_name (1, "SHIFT");
			change_parameter_name (2, "LFO");
			change_parameter_name (3, "ATK");
			change_parameter_name (4, "DECAY");
			change_parameter_name (5, "SUST");
			change_parameter_name (6, "REL");
			change_parameter_name (7, "V/X");
			change_parameter_name (8, "V/Y");
			change_parameter_name (9, "SPEED");
			change_parameter_name (10, "VOL");
			change_parameter_name (11, "HOLD");
			change_parameter_name (12, "PSW");
			change_parameter_name (13, "PTM");
			change_parameter_name (14, "PCTRL");
			change_parameter_name (15, "D/W");
			change_parameter_name (16, "G/VOL");
			change_parameter_name (17, "");
//			set_parameter_buttons (system_parameter);
			break;
		case mode_button_sensitivity:
			change_parameter_name (0, "PITCH");
			change_parameter_name (1, "SHIFT");
			change_parameter_name (2, "LFO");
			change_parameter_name (3, "ATK");
			change_parameter_name (4, "DECAY");
			change_parameter_name (5, "SUST");
			change_parameter_name (6, "REL");
			change_parameter_name (7, "V/X");
			change_parameter_name (8, "V/Y");
			change_parameter_name (9, "KEY");
			change_parameter_name (10, "VELOC");
			change_parameter_name (11, "AEGK");
			change_parameter_name (12, "AEGV");
			change_parameter_name (13, "EGKEY");
			change_parameter_name (14, "EGVEL");
			change_parameter_name (15, "");
			change_parameter_name (16, "LFOX");
			change_parameter_name (17, "LFOY");
//			set_parameter_buttons (sensitivity_parameter);
			break;
		case mode_button_dsp:
			change_parameter_name (0, "SW");
			set_dsp_parameter_names ();
			change_parameter_name (9, "SEND");
			change_parameter_name (10, "LOC");
			change_parameter_name (11, "GLOB");
			change_parameter_name (12, "");
			change_parameter_name (13, "");
			change_parameter_name (14, "VALUE");
			change_parameter_name (15, "DWS");
			change_parameter_name (16, "X/S");
			change_parameter_name (17, "Y/S");
//			set_parameter_buttons (dsp_parameter);
			break;
		case mode_button_dsp_parameter: set_indexed_parameter_names (0); break; //set_parameter_buttons (dsp_p_parameter); break;
		case mode_button_arpeggiator:
			change_parameter_name (0, "SW");
			change_parameter_name (1, "HOLD");
			change_parameter_name (2, "TEMPO");
			change_parameter_name (3, "T/SEC");
			change_parameter_name (4, "VARI");
			change_parameter_name (5, "DIV");
			change_parameter_name (6, "SUBD");
			for (int ind = 7; ind <= 17; ind++) change_parameter_name (ind, "");
//			set_parameter_buttons (arpeggiator_parameter);
			break;
		case mode_button_vector:
			change_parameter_name (0, "SW");
			change_parameter_name (1, "RESO");
			change_parameter_name (2, "RSEC");
			change_parameter_name (3, "TRIG");
			change_parameter_name (4, "ATK");
			change_parameter_name (5, "REL");
			change_parameter_name (6, "");
			change_parameter_name (7, "INITX");
			change_parameter_name (8, "INITY");
			change_parameter_name (9, "LTYPE");
			change_parameter_name (10, "LSTRT");
			change_parameter_name (11, "LSIZE");
			change_parameter_name (12, "LDEL");
			change_parameter_name (13, "COUNT");
			change_parameter_name (14, "FSEL");
			change_parameter_name (15, "FDEL");
			change_parameter_name (16, "F/X");
			change_parameter_name (17, "F/Y");
//			set_parameter_buttons (vector_parameter);
			break;
		case mode_button_stripe:
			change_parameter_name (0, "SHIFT");
			change_parameter_name (1, "LFO");
			change_parameter_name (2, "RND");
			change_parameter_name (3, "");
			change_parameter_name (4, "LV1");
			change_parameter_name (5, "LV2");
			change_parameter_name (6, "LV3");
			change_parameter_name (7, "LV4");
			change_parameter_name (8, "");
			change_parameter_name (9, "RATIO");
			change_parameter_name (10, "RETR");
			change_parameter_name (11, "LFO/S");
			change_parameter_name (12, "");
			change_parameter_name (13, "TIME1");
			change_parameter_name (14, "TIME2");
			change_parameter_name (15, "TIME3");
			change_parameter_name (16, "TIME4");
			change_parameter_name (17, "");
//			set_parameter_buttons (stripe_parameter);
			break;
		case mode_button_lfo:
			change_parameter_name (0, "SPEED");
			change_parameter_name (1, "WAVE");
			change_parameter_name (2, "PULSE");
			change_parameter_name (3, "PHASE");
			change_parameter_name (4, ""); change_parameter_name (5, ""); change_parameter_name (6, ""); change_parameter_name (7, ""); change_parameter_name (8, "");
			change_parameter_name (9, "RETR");
			change_parameter_name (10, "SYNC");
			change_parameter_name (11, ""); change_parameter_name (12, ""); change_parameter_name (13, "");
			change_parameter_name (14, "SPEED");
			change_parameter_name (15, "PICH");
			change_parameter_name (16, "V/X");
			change_parameter_name (17, "V/Y");
//			set_parameter_buttons (lfo_parameter);
			break;
		case mode_button_algo_parameter: set_indexed_parameter_names (algo_parameter_base); break;//set_parameter_buttons (algo_parameter); break;
		default: break;
	}
}

int hercs_dx_type_panel :: get_system_parameter (void) {return get_system_parameter (system_parameter);}

int hercs_dx_type_panel :: get_system_parameter (int parameter) {
	switch (parameter) {
	case 0: return 0;
	case 1: return type < 3 ? 1 + type : 16;
	case 2: return 9 + type;
	case 3: case 4: case 5: case 6: return parameter + 1;
	case 7: case 8: return parameter + 6;
	case 9: return 8;
	case 10: return 15;
	case 11: return 17;
	case 12: case 13: case 14: case 15: case 16: return parameter + 6;
	default: return -1;
	}
}

int hercs_dx_type_panel :: get_control_parameter (void) {return control_parameter == 17 ? 23 : get_system_parameter (control_parameter);}
int hercs_dx_type_panel :: get_type_parameter (void) {
	switch (type) {
	case 0: return 1;
	case 1: return 2;
	case 2: return 0;
	case 3: return 3;
	default: return -1;
	}
}

#define STRIPE_RANGE range_min = type == 1 ? -16383 : -8192; range_max = type == 1 ? 0 : 8191
void hercs_dx_type_panel :: calculate_nrpn_from_mode (void) {
	range_min = 0; range_max = 16383; msb_only = false;
	nrpn_msb = nrpn_lsb = 127;
	switch (main_mode) {
	case mode_button_system:
		nrpn_msb = 0; nrpn_lsb = get_system_parameter ();
		range_min = 0; range_max = 127; msb_only = true;
		if (nrpn_lsb < 0) {nrpn_msb = nrpn_lsb = 127;}
		break;
	case mode_button_control:
		nrpn_msb = 1; nrpn_lsb = get_control_parameter ();
		if (nrpn_lsb < 0) {nrpn_msb = nrpn_lsb = 127;}
		switch (nrpn_lsb) {
			case 17: case 18: case 20: case 23: range_max = 1; msb_only = true; break;
			default: break;
		}
		break;
	case mode_button_stripe:
		nrpn_msb = 32 + get_type_parameter () * 8;
		nrpn_lsb = type < 3 ? stripe : stereo_stripe;
		switch (stripe_parameter) {
		case 0:
			switch (type) {
			case 0: case 3: range_min = -8192; range_max = 8191; break;
			case 1: range_min = -16383; range_max = 0; break;
			default: break;
			}
			break;
		case 1: nrpn_msb++; break;
		case 2: nrpn_msb += 4; break;
		case 4: nrpn_msb += 2; STRIPE_RANGE; break;
		case 5: nrpn_msb += 2; nrpn_lsb += 16; STRIPE_RANGE; break;
		case 6: nrpn_msb += 2; nrpn_lsb += 32; STRIPE_RANGE; break;
		case 7: nrpn_msb += 2; nrpn_lsb += 48; STRIPE_RANGE; break;
		case 9:
			if (type != 0) {nrpn_msb = nrpn_lsb = 127; break;}
			nrpn_msb += 6; range_min = -63; range_max = 63; msb_only = true;
			break;
		case 10:
			if (type != 0) {nrpn_msb = nrpn_lsb = 127; break;}
			nrpn_msb += 7; range_max = 1; msb_only = true;
			break;
		case 11: nrpn_msb += 5; range_max = 15 + hercules_stereo_number; msb_only = true; break;
		case 13: nrpn_msb += 3; break;
		case 14: nrpn_msb += 3; nrpn_lsb += 16; break;
		case 15: nrpn_msb += 3; nrpn_lsb += 32; break;
		case 16: nrpn_msb += 3; nrpn_lsb += 48; break;
		default: nrpn_msb = nrpn_lsb = 127; break;
		}
		break;
	case mode_button_sensitivity:
		nrpn_msb = 64 + get_type_parameter () * 16;
		nrpn_lsb = type < 3 ? stripe : stereo_stripe;
		switch (sensitivity_type) {
		case 0: nrpn_lsb += 16; range_min = -8192; range_max = 8191; break;
		case 1: break;
		case 2: nrpn_lsb += 32; range_min = -8192; range_max = 8191; break;
		default: break;
		}
		switch (sensitivity_parameter) {
		case 0: nrpn_msb += 14; break;
		case 1: case 2: case 3: case 4: case 5: case 6: nrpn_msb += sensitivity_parameter - 1; break;
		case 7: case 8: nrpn_msb += sensitivity_parameter + 1; break;
		case 9: case 10: nrpn_msb += sensitivity_parameter - 3; break;
		case 11: case 12: nrpn_msb += sensitivity_parameter + 1; nrpn_lsb += 64; break;
		case 13: case 14: nrpn_msb += sensitivity_parameter - 1; break;
		case 16: case 17: nrpn_msb += sensitivity_parameter - 6; break;
		default: nrpn_msb = nrpn_lsb = 127; break;
		}
		break;
	case mode_button_lfo:
		nrpn_lsb = type < 3 ? stripe : stereo_stripe + 64;
		if (lfo_parameter >= 14 && lfo_parameter <= 17) {
			switch (sensitivity_type) {
			case 0: nrpn_lsb += 16; range_min = -8192; range_max = 8191; break;
			case 1: break;
			case 2: nrpn_lsb += 32; range_min = -8192; range_max = 8191; break;
			default: break;
			}
		}
		switch (lfo_parameter) {
		case 0: nrpn_msb = 18; break;
		case 1: nrpn_msb = 21; range_max = 4; msb_only = true; break;
		case 2: nrpn_msb = 20; range_min = -8192; range_max = 8191; break;
		case 3: nrpn_msb = 19; range_min = -8192; range_max = 8191; break;
		case 9: nrpn_msb = 16; range_max = 1; msb_only = true; break;
		case 10: nrpn_msb = 17; range_max = 15 + hercules_stereo_number; msb_only = true; break;
		case 14: nrpn_msb = 24; break;
		case 15: nrpn_msb = 25; break;
		case 16: nrpn_msb = 26; break;
		case 17: nrpn_msb = 27; break;
		default: nrpn_msb = nrpn_lsb = 127; break;
		}
		break;
	case mode_button_algo_parameter:
		nrpn_msb = 7;
		nrpn_lsb = algo_parameter;
		break;
	case mode_button_dsp_parameter:
		nrpn_msb = 15; nrpn_lsb = (dsp_id << 4) + dsp_p_parameter;
		break;
	case mode_button_vector:
		nrpn_msb = 6;
		switch (vector_parameter) {
		case 0: nrpn_lsb = 1; range_max = 1; msb_only = true; break;
		case 1: nrpn_lsb = 9; break;
		case 2: nrpn_lsb = 10; break;
		case 3: nrpn_lsb = 11; range_max = 3; msb_only = true; break;
		case 4: nrpn_lsb = 12; break;
		case 5: nrpn_lsb = 13; break;
		case 7: nrpn_lsb = 2; range_min = -8192; range_max = 8191; break;
		case 8: nrpn_lsb = 3; range_min = -8192; range_max = 8191; break;
		case 9: nrpn_lsb = 14; range_max = 2; msb_only = true; break;
		case 10: nrpn_lsb = 15; break;
		case 11: nrpn_lsb = 16; break;
		case 12: nrpn_lsb = 17; break;
		case 13: nrpn_lsb = 4; break;
		case 14: nrpn_lsb = 5; break;
		case 15: nrpn_lsb = 6; break;
		case 16: nrpn_lsb = 7; range_min = -8192; range_max = 8191; break;
		case 17: nrpn_lsb = 8; range_min = -8192; range_max = 8191; break;
		default: nrpn_msb = nrpn_lsb = 127; break;
		}
		break;
	case mode_button_arpeggiator:
		nrpn_msb = 31;

		switch (arpeggiator_parameter) {
		case 0: nrpn_lsb = 6; range_max = 1; msb_only = true; break;
		case 1: nrpn_lsb = 5; range_max = 1; msb_only = true; break;
		case 2: nrpn_lsb = 1; break;
		case 3: nrpn_lsb = 2; break;
		case 4: nrpn_lsb = 7; break;
		case 5: nrpn_lsb = 3; break;
		case 6: nrpn_lsb = 4; break;
		default: nrpn_msb = nrpn_lsb = 127; break;
		}
		break;
	case mode_button_dsp:
		switch (dsp_parameter) {
		case 0: nrpn_msb = 14; nrpn_lsb = dsp_id; range_max = 1; msb_only = true; break;
		case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
			nrpn_lsb = (dsp_id << 4) + dsp_parameter - 1;
			switch (dsp_sensitivity_controller) {
			case 0: nrpn_msb = 8; break;
			case 1:
				switch (sensitivity_type) {
				case 0: nrpn_msb = 9; break;
				case 1: nrpn_msb = 8; nrpn_lsb += 8; break;
				case 2: nrpn_msb = 9; nrpn_lsb += 8; break;
				default: break;
				}
				break;
			case 2:
				switch (sensitivity_type) {
				case 0: nrpn_msb = 11; break;
				case 1: nrpn_msb = 10; break;
				case 2: nrpn_msb = 11; nrpn_lsb += 8; break;
				default: break;
				}
				break;
			case 3:
				switch (sensitivity_type) {
				case 0: nrpn_msb = 12; break;
				case 1: nrpn_msb = 10; nrpn_lsb += 8; break;
				case 2: nrpn_msb = 12; nrpn_lsb += 8; break;
				default: break;
				}
				break;
			default: break;
			}
			if (dsp_parameter > 0 && dsp_sensitivity_controller > 0 && sensitivity_type != 1) {range_min = -8192; range_max = 8191;}
			break;
		case 9: nrpn_msb = 13; nrpn_lsb = dsp_sensitivity_controller * 4;
			if (nrpn_lsb > 0) nrpn_lsb += sensitivity_type == 1 ? 0 : sensitivity_type == 0 ? 1 : 2;
			nrpn_lsb += dsp_id << 4;
			if (dsp_sensitivity_controller > 0 && sensitivity_type != 1) {range_min = -8192; range_max = 8191;}
			break;
		case 10: nrpn_msb = 14; nrpn_lsb = 16 + dsp_sensitivity_controller * 4;
			if (nrpn_lsb > 16) nrpn_lsb += sensitivity_type == 1 ? 0 : sensitivity_type == 0 ? 1 : 2;
			if (dsp_sensitivity_controller > 0 && sensitivity_type != 1) {range_min = -8192; range_max = 8191;}
			break;
		case 11: nrpn_msb = 14; nrpn_lsb = 32 + dsp_sensitivity_controller * 4;
			if (nrpn_lsb > 32) nrpn_lsb += sensitivity_type == 1 ? 0 : sensitivity_type == 0 ? 1 : 2;
			if (dsp_sensitivity_controller > 0 && sensitivity_type != 1) {range_min = -8192; range_max = 8191;}
			break;
		default: nrpn_msb = nrpn_lsb = 127; break;
		}
		break;
	case mode_button_program:
			switch (program_parameter) {
				case 0: nrpn_msb = 2; nrpn_lsb = 3; range_max = 127; msb_only = true; break;
				case 1: nrpn_msb = 2; nrpn_lsb = 1; range_max = 127; msb_only = true; break;
				case 2: nrpn_msb = 2; nrpn_lsb = 2; range_max = 127; msb_only = true; break;
				case 3: nrpn_msb = 2; nrpn_lsb = 0; range_max = 127; msb_only = true; break;
				case 4: nrpn_msb = nrpn_name_character; range_max = 127; msb_only = true; break;
				case 5: nrpn_msb = nrpn_name; range_max = 127; msb_only = true; break;
				case 6: nrpn_msb = nrpn_tune_key; nrpn_lsb = tune_key; range_max = 127; msb_only = true; break;
				case 7: nrpn_msb = 4; nrpn_lsb = tune_key; break;
				case 8: nrpn_msb = 2; nrpn_lsb = 5; range_min = -8192; range_max = 8191; break;
//				case 4: nrpn_msb = nrpn_name; range_max = 127; msb_only = true; break;
//				case 5: nrpn_msb = 4; nrpn_lsb = tune_key; break;
//				case 8: nrpn_msb = 2; nrpn_lsb = 5; range_min = -8192; range_max = 8191; break;
//				case 9: nrpn_msb = nrpn_midi; range_max = 127; msb_only = true; break;
//				case 10: nrpn_msb = nrpn_midi_extension; range_max = 127; msb_only = true; break;
//				case 11: nrpn_msb = nrpn_device; break;
//				case 13: nrpn_msb = nrpn_name_character; range_max = 127; msb_only = true; break;
//				case 14: nrpn_msb = nrpn_tune_key; range_max = 127; msb_only = true; break;
//				case 16: nrpn_msb = 2; nrpn_lsb = 6; range_max = 127; msb_only = true; break;
//				case 17: nrpn_msb = 2; nrpn_lsb = 7; range_max = 127; msb_only = true; break;
				default: nrpn_msb = nrpn_lsb = 127; break;
			}
		break;
	case mode_button_midi:
			switch (midi_parameter) {
				case 0: nrpn_msb = nrpn_midi; range_max = 127; msb_only = true; break;
				case 1: nrpn_msb = nrpn_midi_extension; range_max = 127; msb_only = true; break;
				case 2: nrpn_msb = 2; nrpn_lsb = 4; range_max = 128; break;
				case 7: nrpn_msb = 2; nrpn_lsb = 6; range_max = 127; msb_only = true; break;
				case 8: nrpn_msb = 2; nrpn_lsb = 7; range_max = 127; msb_only = true; break;
				default: nrpn_msb = nrpn_lsb = 127; break;
			}
//			wxMessageBox (wxString :: Format (_T ("MIDI [%i %i]"), midi_parameter, nrpn_msb), _T ("INFO"), wxOK, NULL);
			break;
	default: break;
	}
}

