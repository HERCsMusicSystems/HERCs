///////////////////////////////////////
// Copyright (C) 2004 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "config.h"
#include "va_global_switches.h"
#include "resource.h"

#define STRING_SIZE 256

bool validate_alarm = true;
bool validate_micro = true;
bool validate_poly = true;
bool validate_classic = true;
bool validate_core = true;
bool accept_distribution_id = true;
char distributor [64] = "";

HINSTANCE hinstance;
HWND main_console;
HFONT display_font;
HFONT fixed_font;
int focus_target = 1;

char root_directory [STRING_SIZE] = "";
char command_validation_key [STRING_SIZE] = "";
char command_distribution_id [STRING_SIZE] = "";


void message (char * head, int ind) {
	char command [256];
	sprintf (command, "%s [%i]", head, ind);
	MessageBox (GetActiveWindow (), command, "INFO", MB_OK);
}

void get_registry_key (char * root, char * key, char * value) {
	HKEY hkey;
	TCHAR command [STRING_SIZE - 1];
	TCHAR t_key [STRING_SIZE - 1];
	DWORD buffer_size = STRING_SIZE - 1;
	int ind;
	ind = 0;
	while (root [ind] != '\0') {command [ind] = root [ind]; ind++;} command [ind] = '\0';
	ind = 0;
	while (key [ind] != '\0') {t_key [ind] = key [ind]; ind++;} t_key [ind] = '\0';
	strcpy (value, "");
	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, TEXT (root), 0, KEY_QUERY_VALUE, & hkey) == ERROR_SUCCESS) {
		if (RegQueryValueEx (hkey, TEXT (key), NULL, NULL, (LPBYTE) command, & buffer_size) == ERROR_SUCCESS) {
			int ind = 0;
			while (command [ind] != '\0') {value [ind] = command [ind]; ind++;} value [ind] = '\0';
		}
		RegCloseKey (hkey);
	}
}

int check_char_to_int (char ch) {
	if (ch == '-') return 0;
	if (ch == '/') return 1;
	if (ch >= '0' && ch <= '9') return 2 + ch - '0';
	if (ch >= 'A' && ch <= 'Z') return 12 + ch - 'A';
	if (ch >= 'a' && ch <= 'z') return 38 + ch - 'a';
	return ch;
}

int check_character (char * code) {
	int sub = 0;
	int sentinel = (int) strlen (code);
	for (int ind = 0; ind < sentinel; ind++) sub += check_char_to_int (code [ind]);
	sub &= 63;
	if (sub != 0) sub = 64 - sub;
	return sub;
}

unsigned long int extract_serial (char * serial) {
	unsigned long int ret = 0;
	int ch;
	for (int ind = 3; ind < 9; ind++) {
		ret <<= 4;
		ch = serial [ind];
		if (ch >= 'A') ch -= 'A' - 10;
		else ch -= '0';
		ret += ch;
	}
	return ret;
}

void create_serial_number (char * out, char * header, unsigned long int serial) {
	serial &= 0xffffff;
	unsigned long int copy = serial;
	sprintf (out, "%s-%06X-", header, serial);
	int checksum = 0;
	char * ch = out;
	while (* ch != '\0') checksum += * (ch++);
	checksum &= 0xff;
	sprintf (out, "%s-%06X-%02X", header, serial, checksum);
}

void calculate_serial_number (char * root_directory, char * distribution_id) {
	DWORD l1, l2, volume_serial;
	char volume_name [500];
	char name_buffer [500];
	GetVolumeInformation (root_directory, volume_name, 256, & volume_serial, & l1, & l2, name_buffer, 200);
	char header [3];
	header [0] = distribution_id [0];
	header [1] = distribution_id [1];
	header [2] = '\0';
	volume_serial ^= extract_serial (distribution_id);
	create_serial_number (distribution_id, header, volume_serial);
}

bool set_key (char * distribution_id, char * validation_key, char * root_directory) {
	int check = 0;
	int ind = 0;
	for (ind = 0; ind < 10; ind++) check += distribution_id [ind];
	check &= 0xff;
	int msb = distribution_id [10];
	if (msb > 'A') msb -= 'A' - 10;
	else msb -= '0';
	int lsb = distribution_id [11];
	if (lsb > 'A') lsb -= 'A' - 10;
	else lsb -= '0';
	ind = (msb << 4) + lsb;
	if (ind != check) {
		MessageBox (GetActiveWindow (), "Misspelled serial number.", "INFO", MB_OK);
		return false;
	}
	if (check_character (validation_key) != 0) {
		MessageBox (GetActiveWindow (), "Misspelled validation key.", "INFO", MB_OK);
		return false;
	}

	calculate_serial_number (root_directory, distribution_id);
	int length = 0;
	config * cfg;
	get_registry_key ("SOFTWARE\\HERCS\\classic", "root_directory", root_directory);
	if (validate_classic && strlen (root_directory) > 0) {
		strcat (root_directory, "config.txt");
		cfg = new config (root_directory);
		strcpy (cfg -> serial_number, distribution_id);
		strcpy (cfg -> key, validation_key);
		cfg -> store ();
		delete cfg;
		length++;
	} else {MessageBox (GetActiveWindow (), "HERCs Classic not installed.", "INFO", MB_OK);}
	get_registry_key ("SOFTWARE\\HERCS\\micro", "root_directory", root_directory);
	if (validate_micro && strlen (root_directory) > 0) {
		strcat (root_directory, "config.txt");
		cfg = new config (root_directory);
		strcpy (cfg -> serial_number, distribution_id);
		strcpy (cfg -> key, validation_key);
		cfg -> store ();
		delete cfg;
		length++;
	} else {MessageBox (GetActiveWindow (), "HERCs Micro not installed.", "INFO", MB_OK);}
	get_registry_key ("SOFTWARE\\HERCS\\alarm", "root_directory", root_directory);
	if (validate_alarm && strlen (root_directory) > 0) {
		strcat (root_directory, "config.txt");
		cfg = new config (root_directory);
		strcpy (cfg -> serial_number, distribution_id);
		strcpy (cfg -> key, validation_key);
		cfg -> store ();
		delete cfg;
		length++;
	} else {MessageBox (GetActiveWindow (), "HERCs Alarm not installed", "INFO", MB_OK);}
	get_registry_key ("SOFTWARE\\HERCS\\poly", "root_directory", root_directory);
	if (validate_poly && strlen (root_directory) > 0) {
		strcat (root_directory, "config.txt");
		cfg = new config (root_directory);
		strcpy (cfg -> serial_number, distribution_id);
		strcpy (cfg -> key, validation_key);
		cfg -> store ();
		delete cfg;
		length++;
	} else {MessageBox (GetActiveWindow (), "HERCs Poly not installed", "INFO", MB_OK);}
	get_registry_key ("SOFTWARE\\HERCS\\virtual_analog", "root_directory", root_directory);
	if (validate_core && strlen (root_directory) > 0) {
		strcat (root_directory, "config.txt");
		cfg = new config (root_directory);
		strcpy (cfg -> serial_number, distribution_id);
		strcpy (cfg -> key, validation_key);
		cfg -> store ();
		delete cfg;
	}
	int sentinel = 0;
	if (accept_distribution_id) sentinel = 3;
	if (length > sentinel) MessageBox (GetActiveWindow (), "Operation successfull", "INFO", MB_OK);
	if (length < sentinel) MessageBox (GetActiveWindow (), "Operation partially successfull", "INFO", MB_OK);
	if (length == 0) MessageBox (GetActiveWindow (), "Operation failed", "INFO", MB_OK);
	return true;
}

int CALLBACK main_console_window_proc (HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam) {
	char validation_key [64] = "";
	char distribution_id [64] = "";
	char command [64] = "";
	switch (msg) {
	case WM_INITDIALOG:
		if (! accept_distribution_id) {
			focus_target = 4;
			SetFocus (GetDlgItem (hwnd, IDC_VALIDATION_KEY));
			strcpy (distribution_id, distributor);
			calculate_serial_number (root_directory, distribution_id);
//			SendMessage (GetDlgItem (hwnd, IDC_SERIAL_NUMBER), WM_SETFONT, (WPARAM) display_font, 0);
			SendMessage (GetDlgItem (hwnd, IDC_SERIAL_NUMBER), WM_SETTEXT, (WPARAM) 0, (LPARAM) distribution_id);
		} else {
			SetFocus (GetDlgItem (hwnd, IDC_DISTRIBUTION_HEADER));
			SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_HEADER), EM_LIMITTEXT, (WPARAM) 2, 0);
			SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_NUMBER), EM_LIMITTEXT, (WPARAM) 6, 0);
			SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_CHECKSUM), EM_LIMITTEXT, (WPARAM) 2, 0);
			SendMessage (GetDlgItem (hwnd, IDC_M1_STATIC), WM_SETFONT, (WPARAM) display_font, 0);
			SendMessage (GetDlgItem (hwnd, IDC_M2_STATIC), WM_SETFONT, (WPARAM) display_font, 0);
			SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_HEADER), WM_SETFONT, (WPARAM) fixed_font, 0);
			SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_NUMBER), WM_SETFONT, (WPARAM) fixed_font, 0);
			SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_CHECKSUM), WM_SETFONT, (WPARAM) fixed_font, 0);
		}
		SendMessage (GetDlgItem (hwnd, IDC_VALIDATION_KEY), EM_LIMITTEXT, (WPARAM) 12, 0);
		SendMessage (hwnd, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon (hinstance, MAKEINTRESOURCE (IDI_H)));
		SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_STATIC), WM_SETFONT, (WPARAM) display_font, 0);
		SendMessage (GetDlgItem (hwnd, IDC_VALIDATION_STATIC), WM_SETFONT, (WPARAM) display_font, 0);
		SendMessage (GetDlgItem (hwnd, IDC_VALIDATION_KEY), WM_SETFONT, (WPARAM) fixed_font, 0);
		SendMessage (GetDlgItem (hwnd, IDC_VALIDATION_KEY), WM_SETTEXT, 0, (LPARAM) command_validation_key);
		return true;
	case WM_COMMAND:
		switch (LOWORD (wparam)) {
		case IDC_DISTRIBUTION_HEADER:
			SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_HEADER), WM_GETTEXT, (WPARAM) 64, (LPARAM) command);
			if (strlen (command) > 1 && focus_target == 1) {
				SetFocus (GetDlgItem (hwnd, IDC_DISTRIBUTION_NUMBER));
				focus_target = 2;
			}
			return true;
		case IDC_DISTRIBUTION_NUMBER:
			SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_NUMBER), WM_GETTEXT, (WPARAM) 64, (LPARAM) command);
			if (strlen (command) > 5 && focus_target == 2) {
				SetFocus (GetDlgItem (hwnd, IDC_DISTRIBUTION_CHECKSUM));
				focus_target = 3;
			}
			return true;
		case IDC_DISTRIBUTION_CHECKSUM:
			SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_CHECKSUM), WM_GETTEXT, (WPARAM) 64, (LPARAM) command);
			if (strlen (command) > 1 && focus_target == 3) {
				SetFocus (GetDlgItem (hwnd, IDC_VALIDATION_KEY));
				focus_target = 4;
			}
			return true;
		case IDC_VALIDATION_KEY:
			SendMessage (GetDlgItem (hwnd, IDC_VALIDATION_KEY), WM_GETTEXT, (WPARAM) 64, (LPARAM) command);
			if (strlen (command) > 11 && focus_target == 4) {
				EnableWindow (GetDlgItem (hwnd, IDOK), TRUE);
				SetFocus (GetDlgItem (hwnd, IDOK));
				focus_target = 1;
			}
			return true;
		case IDOK:
			if (accept_distribution_id) {
				SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_HEADER), WM_GETTEXT, (WPARAM) 64, (LPARAM) command);
				strcpy (distribution_id, command);
				strcat (distribution_id, "-");
				SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_NUMBER), WM_GETTEXT, (WPARAM) 64, (LPARAM) command);
				strcat (distribution_id, command);
				strcat (distribution_id, "-");
				SendMessage (GetDlgItem (hwnd, IDC_DISTRIBUTION_CHECKSUM), WM_GETTEXT, (WPARAM) 64, (LPARAM) command);
				strcat (distribution_id, command);
			} else strcpy (distribution_id, distributor);
			SendMessage (GetDlgItem (hwnd, IDC_VALIDATION_KEY), WM_GETTEXT, (WPARAM) 64, (LPARAM) validation_key);
			if (! set_key (distribution_id, validation_key, root_directory)) return true;
		case IDCANCEL:
			PostQuitMessage (0);
			EndDialog (hwnd, IDCANCEL);
			return true;
		default: return false;
		}
		return false;
	default: return false;
	}
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE previous, LPSTR lpCmdLine, int nShowCmd) {

	get_registry_key ("SOFTWARE\\HERCS\\classic", "root_directory", root_directory);
	if (strlen (root_directory) < 1) {
		MessageBox (GetActiveWindow (), "HERCs not found.\nPlease install the software\nbefore entering the validation key.", "INFO", MB_OK);
		return 0;
	}
	root_directory [3] = '\0';

	strcpy (distributor, RETAIL_DISTRIBUTION_ID);
	if (strlen (lpCmdLine) > 0) {
		char * ch = lpCmdLine;
		char * to = command_distribution_id;
		while (* ch > 32) * (to++) = * (ch++);
		* to = '\0';
		if (strcmp (command_distribution_id, "va_pack") == 0) {
			validate_alarm = true;
			validate_micro = true;
			validate_poly = true;
			validate_classic = true;
			validate_core = false;
			accept_distribution_id = false;
			strcpy (distributor, VA_PACK_DISTRIBUTION_ID);
		} else if (strcmp (command_distribution_id, "alarm") == 0) {
			validate_alarm = true;
			validate_micro = false;
			validate_poly = false;
			validate_classic = false;
			validate_core = false;
			accept_distribution_id = false;
			strcpy (distributor, ALARM_DISTRIBUTION_ID);
		} else if (strcmp (command_distribution_id, "micro") == 0) {
			validate_alarm = false;
			validate_micro = true;
			validate_poly = false;
			validate_classic = false;
			validate_core = false;
			accept_distribution_id = false;
			strcpy (distributor, MICRO_DISTRIBUTION_ID);
		} else if (strcmp (command_distribution_id, "poly") == 0) {
			validate_alarm = false;
			validate_micro = false;
			validate_poly = true;
			validate_classic = false;
			validate_core = false;
			accept_distribution_id = false;
			strcpy (distributor, POLY_DISTRIBUTION_ID);
		} else if (strcmp (command_distribution_id, "classic") == 0) {
			validate_alarm = false;
			validate_micro = false;
			validate_poly = false;
			validate_classic = true;
			validate_core = false;
			accept_distribution_id = false;
			strcpy (distributor, CLASSIC_DISTRIBUTION_ID);
		} else {
			to = command_validation_key;
			while (* ch == ' ') ch++;
			while (* ch > 32) * (to++) = * (ch++);
			* to = '\0';
//			MessageBox (GetActiveWindow (), command_validation_key, command_distribution_id, MB_OK);
			set_key (command_distribution_id, command_validation_key, root_directory);
			return 1;
		}
	}

	hinstance = hInstance;
	display_font = CreateFont (16, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE | DEFAULT_PITCH, NULL);
	fixed_font = CreateFont (14, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "fixedsys");

	WNDCLASS wn;
	wn . lpszClassName = TEXT ("HERCs");
	wn . lpfnWndProc = DefWindowProc;
	wn . style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wn . hInstance = hInstance;
	wn . hIcon = LoadIcon (NULL, MAKEINTRESOURCE (IDI_H));
	wn . hCursor = LoadCursor (NULL, IDC_ARROW);
	wn . hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
	wn . lpszMenuName = NULL;
	wn . cbClsExtra = 0;
	wn . cbWndExtra = 0;
	if (! RegisterClass (& wn)) return 0;

	if (accept_distribution_id) main_console = CreateDialog (hinstance, MAKEINTRESOURCE (IDD_MAIN_DIALOG), NULL, main_console_window_proc);
	else main_console = CreateDialog (hinstance, MAKEINTRESOURCE (IDD_DOWNLOAD_DIALOG), NULL, main_console_window_proc);


	MSG msg;
	while (GetMessage (& msg, NULL, 0, 0)) {
		TranslateMessage (& msg);
		DispatchMessage (& msg);
	}

	DeleteObject (display_font);
	DeleteObject (fixed_font);
	return (int) msg . wParam;
}
