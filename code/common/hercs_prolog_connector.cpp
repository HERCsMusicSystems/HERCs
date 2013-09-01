///////////////////////////////////////
// Copyright (C) 2005 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "data.h"
#include "prolog.h"

void synthesizer :: connect_prolog_root (PrologRoot * root) {this -> root = root;}

void synthesizer :: disconnect_prolog_root (void) {this -> root = NULL;}

void synthesizer :: insert_prolog_command (int sub_selector, midi_stream * line) {
	if (root == NULL) return;
	AREA command;
	command [0] = sub_selector;
	line -> get (& command [1]);
	root -> insertCommand (command);
}
