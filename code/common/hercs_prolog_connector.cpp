///////////////////////////////////////
// Copyright (C) 2005 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "prolog.h"

void synthesiser :: connect_prolog_root (PrologRoot * root) {this -> root = root;}

void synthesiser :: disconnect_prolog_root (void) {this -> root = NULL;}

void synthesiser :: insert_prolog_command (int sub_selector, midi_stream * line) {
	if (root == NULL) return;
	AREA command;
	command [0] = sub_selector;
	line -> get (& command [1]);
	root -> insertCommand (command);
}
