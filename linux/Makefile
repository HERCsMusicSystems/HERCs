
#include ../../prolog/linux/Makefile

pc = ../../prolog/code/prolog/
po = ../../prolog/linux/

cc = ../code/common/
hc = ../code/hardware/
oc = ../code/orakle/

prolog_prc_objects = \
	$(pc)studio.prc\
	$(pc)http.prc\
	$(pc)store.prc\
	$(pc)f1.prc\
	$(pc)help.prc\
	$(pc)record.prc\
	$(pc)neural.prc\
	$(pc)keyboard.prc\
	$(pc)sql.prc

prolog_objects = \
	$(po)prolog_dll_loader.o\
	$(po)midi_stream.o $(po)object_tracker.o $(po)prolog_atom.o $(po)prolog_command.o $(po)prolog_directory.o \
	$(po)prolog_element.o $(po)prolog_loader.o $(po)prolog_match.o $(po)prolog_midi_command.o \
	$(po)prolog_native_code.o $(po)prolog_query.o $(po)prolog_reader.o $(po)prolog_resolution.o \
	$(po)prolog_root.o $(po)prolog_service_class.o \
	$(po)prolog_studio.o $(po)prolog_http.o \
	$(po)prolog_transport.o \
	$(po)prolog_variable.o \
	$(po)prolog_linux_console.o


common_objects = \
	algos_classic.o\
	algos_ffm_2_3.o\
	algos_fm2_3_4.o\
	algos_input.o\
	algos_test.o\
	banks.o\
	config.o\
	core.o\
	data.o\
	dsp.o\
	file_system.o\
	file_stream_system.o\
	hercs_prolog_connector.o\
	lemat.o\
	midi_back.o\
	midi_processor.o\
	osc.o\
	osc_cpp.o\
	preset_algo_bank_cpp.o\
	preset_dsp_algo_bank.o\
	preset_pattern_bank.o\
	preset_dsp_algos.o\
	preset_patterns.o\
	rom_player_bank_cpp.o\
	scripted_pattern_bank_cpp.o\
	setup_file_reader.o\
	stream_system.o\
	transport.o\
	wavetable_bank_cpp.o\
	plugin_loader.o

algos_classic.o: $(cc)algos_classic.cpp $(cc)data.h $(cc)algo_building_blocks.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)algos_classic.cpp -c -o algos_classic.o
algos_ffm_2_3.o: $(cc)algos_ffm_2_3.cpp $(cc)data.h $(cc)algo_building_blocks.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)algos_ffm_2_3.cpp -c -o algos_ffm_2_3.o
algos_fm2_3_4.o: $(cc)algos_fm2_3_4.cpp $(cc)data.h $(cc)algo_building_blocks.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)algos_fm2_3_4.cpp -c -o algos_fm2_3_4.o
algos_input.o: $(cc)algos_input.cpp $(cc)data.h $(cc)algo_building_blocks.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)algos_input.cpp -c -o algos_input.o
algos_test.o: $(cc)algos_test.cpp $(cc)data.h $(cc)algo_building_blocks.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)algos_test.cpp -c -o algos_test.o
banks.o: $(cc)banks.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)banks.cpp -c -o banks.o
config.o: $(cc)config.h $(cc)config.cpp
	g++ -Wno-write-strings -I. -I$(pc) $(cc)config.cpp -c -o config.o
data.o: $(cc)data.h $(cc)data.cpp $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)data.cpp -c -o data.o
dsp.o: $(cc)dsp.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)dsp.cpp -c -o dsp.o
file_system.o: $(cc)file_system.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)file_system.cpp -c -o file_system.o
file_stream_system.o: $(cc)file_stream_system.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)file_stream_system.cpp -c -o file_stream_system.o
hercs_prolog_connector.o: $(cc)hercs_prolog_connector.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)hercs_prolog_connector.cpp -c -o hercs_prolog_connector.o
lemat.o: $(cc)lemat.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)lemat.cpp -c -o lemat.o
midi_back.o: $(cc)midi_back.cpp $(cc)data.h $(cc)mnemonic.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)midi_back.cpp -c -o midi_back.o
midi_processor.o: $(cc)midi_processor.cpp $(cc)data.h $(cc)mnemonic.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)midi_processor.cpp -c -o midi_processor.o
osc.o: $(cc)osc.cpp $(cc)osc.h $(cc)data.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)osc.cpp -c -o osc.o
osc_cpp.o: $(cc)osc_cpp.cpp $(cc)osc.h $(cc)osc_cpp.h $(cc)data.h
	g++ -fopenmp -Wno-write-strings -I. -I$(pc) $(cc)osc_cpp.cpp -c -o osc_cpp.o
preset_algo_bank_cpp.o: $(cc)preset_algo_bank_cpp.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)preset_algo_bank_cpp.cpp -c -o preset_algo_bank_cpp.o
preset_dsp_algo_bank.o: $(cc)preset_dsp_algo_bank.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)preset_dsp_algo_bank.cpp -c -o preset_dsp_algo_bank.o
preset_pattern_bank.o: $(cc)preset_pattern_bank.cpp $(cc)data.h $(cc)transport.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)preset_pattern_bank.cpp -c -o preset_pattern_bank.o
preset_patterns.o: $(cc)preset_patterns.cpp $(cc)data.h $(cc)transport.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)preset_patterns.cpp -c -o preset_patterns.o
preset_dsp_algos.o: $(cc)preset_dsp_algos.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)preset_dsp_algos.cpp -c -o preset_dsp_algos.o
rom_player_bank_cpp.o: $(cc)rom_player_bank_cpp.cpp $(cc)data.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)rom_player_bank_cpp.cpp -c -o rom_player_bank_cpp.o
scripted_pattern_bank_cpp.o: $(cc)scripted_pattern_bank_cpp.cpp $(cc)data.h $(cc)scripted_pattern_bank_cpp.h $(cc)transport.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)scripted_pattern_bank_cpp.cpp -c -o scripted_pattern_bank_cpp.o
setup_file_reader.o: $(cc)setup_file_reader.h $(cc)setup_file_reader.cpp
	g++ -Wno-write-strings -I. -I$(pc) $(cc)setup_file_reader.cpp -c -o setup_file_reader.o
stream_system.o: $(cc)stream_system.cpp $(cc)data.h $(cc)mnemonic.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)stream_system.cpp -c -o stream_system.o
transport.o: $(cc)transport.cpp $(cc)data.h $(cc)transport.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)transport.cpp -c -o transport.o
wavetable_bank_cpp.o: $(cc)wavetable_bank_cpp.cpp $(cc)data.h $(cc)wavetable_bank_cpp.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)wavetable_bank_cpp.cpp -c -o wavetable_bank_cpp.o
plugin_loader.o: $(cc)plugin_loader.cpp $(cc)data.h $(cc)osc_cpp.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) $(cc)plugin_loader.cpp -c -o plugin_loader.o
core.o: $(cc)core.h $(cc)core.cpp $(cc)data.cpp $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(cc) -I$(pc) $(cc)core.cpp -c -o core.o
controls.o: $(hc)controls.h $(hc)controls.cpp $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings `wx-config --cxxflags` $(hc)controls.cpp -c -o controls.o

multiplatform_audio.o: $(hc)multiplatform_audio.h $(hc)multiplatform_audio.cpp operating_system.h
	g++ -Wno-write-strings -I. $(hc)multiplatform_audio.cpp -c -o multiplatform_audio.o
wx_audio_dialog.o: $(hc)wx_audio_dialog.h $(hc)wx_audio_dialog.cpp $(hc)multiplatform_audio.h
	g++ -Wno-write-strings -I. `wx-config --cxxflags` $(hc)wx_audio_dialog.cpp -c -o wx_audio_dialog.o

front_panel.o: $(hc)front_panel.h $(hc)front_panel.cpp $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(pc) -I$(cc) `wx-config --cxxflags` $(hc)front_panel.cpp -c -o front_panel.o
hardware_hercs.o: $(pc)linux_midi.h $(hc)hardware_hercs.cpp $(hc)controls.h $(hc)core_layout.h $(hc)front_panel.h $(cc)osc.h $(cc)osc_cpp.h
	g++ -Wno-write-strings -I. -I$(hc) -I$(cc) -I$(pc) `wx-config --cxxflags` $(hc)hardware_hercs.cpp -c -o hardware_hercs.o

~/bin/hrcs: $(po)encoder.o $(prolog_objects) $(po)prolog_resources.o $(prolog_prc_objects) $(common_objects) hardware_hercs.o $(po)neural.o front_panel.o controls.o multiplatform_audio.o wx_audio_dialog.o
	g++ -fopenmp -lpthread -lasound -ldl -Wl,--export-dynamic `wx-config --libs` $(prolog_objects) $(common_objects) $(po)encoder.o $(po)prolog_resources.o $(po)neural.o controls.o front_panel.o hardware_hercs.o multiplatform_audio.o wx_audio_dialog.o -o ~/bin/hrcs -lreadline


hrcs: ../../dock/hrcs

../../dock/hrcs: ~/bin/hrcs
	cp ~/bin/hrcs ../../dock/hrcs

