///////////////////////////////////////
// Copyright (C) 2004 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "osc_cpp.h"
#include "operating_system.h"

#ifdef LINUX_OPERATING_SYSTEM
//#include <pthread.h>
#include <dlfcn.h>
//#include <unistd.h>
//#include <sys/times.h>
#define THREAD void *
#define DLL_OPEN(name) dlopen (name, RTLD_LAZY)
#define GET_SYMBOL(dll, name) dlsym (dll, name)
#define DLL_CLOSE(dll) dlclose (dll)
#endif

#ifdef WINDOWS_OPERATING_SYSTEM
#include <windows.h>
#define THREAD HINSTANCE
#define DLL_OPEN(name) LoadLibrary (name)
#define GET_SYMBOL(dll, name) GetProcAddress (dll, name)
#define DLL_CLOSE(dll) FreeLibrary (dll)
#endif

typedef bank * (* creator) (char * parameter, synthesiser * sth, oscillator_pool_cpp * pool);

bank * oscillator_pool_cpp :: create_plugin_algo_bank (char * plugin, char * parameter, synthesiser * sth) {
	char command [256];
	sprintf (command, "%s%s", sth -> root_directory, plugin);
	THREAD dll = DLL_OPEN (command);
	if (dll == NULL) {
		sprintf (command, "DLL %s not found!", plugin);
		message (command);
		return NULL;
	}
	creator create_algo_bank = (creator) GET_SYMBOL (dll, "create_algo_bank");
	if (! create_algo_bank) {
		DLL_CLOSE (dll);
		return NULL;
	}
	return create_algo_bank (parameter, sth, this);
}

#ifdef WINDOWS_OPERATING_SYSTEM

void message (char * head) {
	MessageBox (GetActiveWindow (), head, "INFO", MB_OK);
}

void message (char * head, int ind) {
	char command [256];
	sprintf (command, "%s [%i]", head, ind);
	MessageBox (GetActiveWindow (), command, "INFO", MB_OK);
}

void message (char * head, int ind, int sub) {
	char command [256];
	sprintf (command, "%s [%i %i]", head, ind, sub);
	MessageBox (GetActiveWindow (), command, "INFO", MB_OK);
}

void message (char * head, int ind, int sub, int id) {
	char command [256];
	sprintf (command, "%s [%i %i %i]", head, ind, sub, id);
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
	string_copy (value, "");
	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, TEXT (root), 0, KEY_QUERY_VALUE, & hkey) == ERROR_SUCCESS) {
		if (RegQueryValueEx (hkey, TEXT (key), NULL, NULL, (LPBYTE) command, & buffer_size) == ERROR_SUCCESS) {
			int ind = 0;
			while (command [ind] != '\0') {value [ind] = command [ind]; ind++;} value [ind] = '\0';
		}
		RegCloseKey (hkey);
	}
}

unsigned long int get_volume_id (char * path) {
	char volume_name [500];
	char name_buffer [500];
	DWORD l1, l2;
	DWORD volume_serial = 0;
	GetVolumeInformation (path, volume_name, 256, & volume_serial, & l1, & l2, name_buffer, 200);
	return volume_serial;
}

#endif

#ifdef LINUX_OPERATING_SYSTEM

void message (char * head) {printf ("INFO [%s]\n", head);}
void message (char * head, int ind) {printf ("%s [%i]\n", head, ind);}
void message (char * head, int ind, int sub) {printf ("%s [%i %i]\n", head, ind, sub);}
void message (char * head, int ind, int sub, int id) {printf ("%s [%i %i %i]\n", head, ind, sub, id);}

void get_registry_key (char * root, char * key, char * value) {sprintf (value, "NOT IMPLEMENTED");}
unsigned long int get_volume_id (char * path) {return 0L;}

#endif
