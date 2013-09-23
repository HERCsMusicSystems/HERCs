
#define DEC_LOCATION 1135, 216
#define INC_LOCATION 1175, 216
#define T1_LOCATION 1114, 236
#define T2_LOCATION 1154, 236
#define T3_LOCATION 1194, 236
#define RESET_LOCATION 860, 118
#define ENTER_LOCATION 900, 118
#define STORE_LOCATION 940, 118
#define PART_BUTTONS_ROW 236
#define PART_BUTTONS_START 621
#define PART_BUTTONS_DISTANCE 40
#define PARAMETER_BUTTONS_START 661
#define PARAMETER_BUTTONS_ROW_1 146
#define PARAMETER_BUTTONS_ROW_2 191
#define PARAMETER_BUTTONS_DISTANCE 40
#define PARAMETER_NAMES_START 658
#define PARAMETER_NAMES_ROW_1 161
#define PARAMETER_NAMES_ROW_2 206
#define MODE_BUTTONS_START 495
#define MODE_BUTTONS_ROW 120
#define MODE_BUTTONS_ROW_DISTANCE 25
#define MODE_BUTTONS_DISTANCE 40
#define TYPE_BUTTONS_START 495
#define TYPE_BUTTONS_ROW 195
#define TYPE_BUTTONS_DISTANCE 40
#define SENSITIVITY_BUTTONS_START 495
#define SENSITIVITY_BUTTONS_ROW 220
#define SENSITIVITY_BUTTONS_DISTANCE 40
#define PARAMETER_TEXT_LOCATION 560, 46
#define STATUS_TEXT_LOCATION 560, 71
#define ENCODER_LOCATION 1046, 180

class main_panel : public wxPanel {
public:
	wxBitmap green_button;
	wxBitmap red_button;
	wxBitmap gray_knob;
	wxBitmap encoder_knob;
	wxBitmap encoder_track;
	wxBitmap encoder_handle;
	wxBitmap picture_keyboard_small;
	wxBitmap picture_keyboard;
	wxBitmap picture_keyboard_big;
	wxBitmap front_panel;
	wxBitmap vector_handle;
	wxBitmap pitch_handle;
	wxBitmap pitch_track;
	wxBitmap controller_knob_track;
	wxBitmap controller_knob_handle;
	wxBitmap png_vector_slider;
	wxBitmap png_vector_surface;
	wxBitmap picture_controls;
	wxBitmap oscilloscope_background;
	void initialise_bitmaps (void) {
		green_button . LoadFile (_T ("hercs.app/Contents/Resources/green_button_big.png"), wxBITMAP_TYPE_PNG);
		red_button . LoadFile (_T ("hercs.app/Contents/Resources/red_button.bmp"), wxBITMAP_TYPE_BMP);
		gray_knob . LoadFile (_T ("hercs.app/Contents/Resources/knob.bmp"), wxBITMAP_TYPE_BMP);
		encoder_knob . LoadFile (_T ("hercs.app/Contents/Resources/encoder.png"), wxBITMAP_TYPE_PNG);
		encoder_track . LoadFile (_T ("hercs.app/Contents/Resources/encoder_track.png"), wxBITMAP_TYPE_PNG);
		encoder_handle . LoadFile (_T ("hercs.app/Contents/Resources/encoder_handle.png"), wxBITMAP_TYPE_PNG);
		picture_keyboard_small . LoadFile (_T ("hercs.app/Contents/Resources/picture_keyboard_small.bmp"), wxBITMAP_TYPE_BMP);
		picture_keyboard . LoadFile (_T ("hercs.app/Contents/Resources/picture_keyboard.bmp"), wxBITMAP_TYPE_BMP);
		picture_keyboard_big . LoadFile (_T ("hercs.app/Contents/Resources/picture_keyboard_big.png"), wxBITMAP_TYPE_PNG);
		if (cfg . layout_small) front_panel . LoadFile (_T ("hercs.app/Contents/Resources/front_panel_small.png"), wxBITMAP_TYPE_PNG);
		else front_panel . LoadFile (_T ("hercs.app/Contents/Resources/front_panel.png"), wxBITMAP_TYPE_PNG);
		//front_panel . LoadFile (_T ("hercs.app/Contents/Resources/front_panel.png"), wxBITMAP_TYPE_PNG);
		vector_handle . LoadFile (_T ("hercs.app/Contents/Resources/vectorhandle.bmp"), wxBITMAP_TYPE_BMP);
		picture_controls . LoadFile (_T ("hercs.app/Contents/Resources/control_background.bmp"), wxBITMAP_TYPE_BMP);
		pitch_handle . LoadFile (_T ("hercs.app/Contents/Resources/pitch_handle.bmp"), wxBITMAP_TYPE_BMP);
		pitch_track . LoadFile (_T ("hercs.app/Contents/Resources/pitch_track.bmp"), wxBITMAP_TYPE_BMP);
		controller_knob_track . LoadFile (_T ("hercs.app/Contents/Resources/knob_track.bmp"), wxBITMAP_TYPE_BMP);
		controller_knob_handle . LoadFile (_T ("hercs.app/Contents/Resources/knob_handle.png"), wxBITMAP_TYPE_PNG);
		png_vector_slider . LoadFile (_T ("hercs.app/Contents/Resources/vector_red_handle.png"), wxBITMAP_TYPE_PNG);
		png_vector_surface . LoadFile (_T ("hercs.app/Contents/Resources/xy_surface.png"), wxBITMAP_TYPE_PNG);
		oscilloscope_background . LoadFile (_T ("hercs.app/Contents/Resources/oscilloscope_background.png"), wxBITMAP_TYPE_PNG);
	}
	main_panel (wxWindow * parent) : wxPanel (parent) {
		initialise_bitmaps ();
		int ind = 0;
		int sub = 0;
		wxFont font (cfg . layout_small ? 6 : 7, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
		wxFont parameter_font (cfg . layout_small ? 8 : 12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);
//		parameter_font . SetFaceName (_T ("fixedsys"));
		wxFont status_font (cfg . layout_small ? 7 : 8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
		ind = ROW_START;
		ind = ROW_START;
		(panel . parameter_display = new wxStaticText (this, -1, _T ("PROGRAM NAME"), wxPoint (PARAMETER_TEXT_LOCATION))) -> SetFont (parameter_font);
		panel . parameter_display -> SetBackgroundColour (wxColour (0, 118, 158));
		panel . parameter_display -> SetForegroundColour (wxColour (255, 255, 0));
		(panel . status_display = new wxStaticText (this, -1, _T ("PART = 0"), wxPoint (STATUS_TEXT_LOCATION))) -> SetFont (status_font);
		panel . status_display -> SetBackgroundColour (wxColour (0, 118, 158));
		panel . status_display -> SetForegroundColour (wxColour (255, 255, 0));
		for (ind = 0; ind < part_button_0; ind++) {
			panel . mode_buttons [ind] = new wxAnimatedButton (this, sub++, & green_button);
			panel . mode_buttons [ind] -> setCallback (button_callback) -> locateOnBackground ((ind & 0x3) * MODE_BUTTONS_DISTANCE + MODE_BUTTONS_START, (ind >> 2) * MODE_BUTTONS_ROW_DISTANCE + MODE_BUTTONS_ROW, & front_panel);
		}
		for (ind = 0; ind < 10; ind++) {
			panel . part_buttons [ind] = new wxAnimatedButton (this, sub++, & green_button);
			panel . part_buttons [ind] -> setCallback (button_callback) -> locateOnBackground (ind * PART_BUTTONS_DISTANCE + PART_BUTTONS_START, PART_BUTTONS_ROW, & front_panel);
		}
		for (ind = 0; ind < 4; ind++) {
			panel . type_buttons [ind] = new wxAnimatedButton (this, sub++, & green_button);
			panel . type_buttons [ind] -> setCallback (button_callback) -> locateOnBackground (ind * TYPE_BUTTONS_DISTANCE + TYPE_BUTTONS_START, TYPE_BUTTONS_ROW, & front_panel);
		}
		for (ind = 0; ind < 3; ind++) {
			panel . sensitivity_buttons [ind] = new wxAnimatedButton (this, sub++, & green_button);
			panel . sensitivity_buttons [ind] -> setCallback (button_callback) -> locateOnBackground (ind * SENSITIVITY_BUTTONS_DISTANCE + SENSITIVITY_BUTTONS_START, SENSITIVITY_BUTTONS_ROW, & front_panel);
		}
		for (ind = 0; ind < 9; ind++) {
			panel . parameter_buttons [ind] = new wxAnimatedButton (this, sub++, & green_button);
			panel . parameter_buttons [ind] -> setCallback (button_callback) -> locateOnBackground (ind * PARAMETER_BUTTONS_DISTANCE + PARAMETER_BUTTONS_START, PARAMETER_BUTTONS_ROW_1, & front_panel);
			(panel . parameter_names [ind] = new wxStaticText (this, -1, _T ("PARAM"), wxPoint (ind * PARAMETER_BUTTONS_DISTANCE + PARAMETER_NAMES_START, PARAMETER_NAMES_ROW_1), wxDefaultSize, wxTRANSPARENT_WINDOW)) -> SetFont (font);
		}
		for (ind = 0; ind < 9; ind++) {
			panel . parameter_buttons [9 + ind] = new wxAnimatedButton (this, sub++, & green_button);
			panel . parameter_buttons [9 + ind] -> setCallback (button_callback) -> locateOnBackground (ind * PARAMETER_BUTTONS_DISTANCE + PARAMETER_BUTTONS_START, PARAMETER_BUTTONS_ROW_2, & front_panel);
			(panel . parameter_names [ind + 9] = new wxStaticText (this, -1, _T ("PARAM"), wxPoint (ind * PARAMETER_BUTTONS_DISTANCE + PARAMETER_NAMES_START, PARAMETER_NAMES_ROW_2))) -> SetFont (font);
		}
		for (ind = 0; ind < 18; ind++) {
			panel . parameter_names [ind] -> SetBackgroundColour (wxColour (0, 118, 158));
			panel . parameter_names [ind] -> SetForegroundColour (wxColour (255, 255, 255));
		}
		(new wxAnimatedKnob (this, -1, & encoder_knob, 128, true)) -> locateOnBackground (ENCODER_LOCATION, & front_panel) -> setCallback (encoder_callback);// -> SetSize (ENCODER_LOCATION, -1, -1);
//		(new wxTravelingBitmapKnob (this, -1, & encoder_handle, & encoder_track, 18, true)) -> setCallback (encoder_callback) -> SetSize (ENCODER_LOCATION, -1, -1);
//		(new wxTravelingBitmapKnob (this, -1, & controller_knob_handle, & controller_knob_track, 10, true)) -> setCallback (encoder_callback) -> SetSize (ENCODER_LOCATION, -1, -1);
		(panel . dec_button = new wxAnimatedButton (this, sub++, & green_button, false)) -> setCallback (button_callback) -> locateOnBackground (DEC_LOCATION, & front_panel);
		(panel . inc_button = new wxAnimatedButton (this, sub++, & green_button, false)) -> setCallback (button_callback) -> locateOnBackground (INC_LOCATION, & front_panel);
		(panel . times_buttons [0] = new wxAnimatedButton (this, sub++, & green_button)) -> setCallback (button_callback) -> locateOnBackground (T1_LOCATION, & front_panel);
		(panel . times_buttons [1] = new wxAnimatedButton (this, sub++, & green_button)) -> setCallback (button_callback) -> locateOnBackground (T2_LOCATION, & front_panel);
		(panel . times_buttons [2] = new wxAnimatedButton (this, sub++, & green_button)) -> setCallback (button_callback) -> locateOnBackground (T3_LOCATION, & front_panel);
		(panel . reset_button = new wxAnimatedButton (this, sub++, & green_button)) -> setCallback (button_callback) -> locateOnBackground (RESET_LOCATION, & front_panel);
		(panel . enter_button = new wxAnimatedButton (this, sub++, & green_button)) -> setCallback (button_callback) -> locateOnBackground (ENTER_LOCATION, & front_panel);
		(panel . store_button = new wxAnimatedButton (this, sub++, & green_button)) -> setCallback (button_callback) -> locateOnBackground (STORE_LOCATION, & front_panel);
		(oscilloscope_controller = new wxOscilloscopeController (this, -1, oscilloscope_monitor_wave)) -> locateOnBackground (1034, 41, & front_panel);
		if (cfg . layout_small) oscilloscope_controller -> setScalingFactor (4);
//		oscilloscope_controller -> background = & oscilloscope_background;
		oscilloscope_visible = true;
		if (cfg . layout_small) (new keyboard_controller (this, -1, & picture_keyboard_small, 1)) -> SetSize (108, 267, -1, -1);
		else (new keyboard_controller (this, -1, & picture_keyboard_big, 3)) -> SetSize (108, 267, -1, -1);
		(panel . vector = new wxAnimatedVector (this, 13, 14, & png_vector_slider, & png_vector_surface)) -> setCallback (controller_callback) -> setMotionRatio (0.2) -> set (0.5, 0.5) -> SetSize (31, 125, -1, -1);
		(panel . pitch_wheel = new wxAnimatedVector (this, -1, 0, & pitch_handle, & pitch_track)) -> setCallback (controller_callback) -> set (0.0, 0.5) -> loadSpring () -> SetSize (39, 306, -1, -1);
		(panel . modulation_wheel = new wxAnimatedVector (this, -1, cfg . modulation_wheel_id, & pitch_handle, & pitch_track)) -> setCallback (controller_callback) -> set (0.0, 0.0) -> SetSize (69, 306, -1, -1);
		(panel . mono_poly_switch = new wxAnimatedButton (this, 23, & green_button)) -> setCallback (controller_callback) -> locateOnBackground (29, 255, & front_panel);
		(panel . porta_switch = new wxAnimatedButton (this, 18, & green_button)) -> setCallback (controller_callback) -> locateOnBackground (69, 255, & front_panel);
		(panel . volume_knob = new wxTravelingBitmapKnob (this, 15, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (35, 57, -1, -1);
		(panel . pan_knob = new wxTravelingBitmapKnob (this, 16, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (105, 57, -1, -1);
		(panel . dry_wet_knob = new wxTravelingBitmapKnob (this, 21, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (175, 57, -1, -1);
		(panel . attack_knob = new wxTravelingBitmapKnob (this, 4, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (258, 57, -1, -1);
		(panel . decay_knob = new wxTravelingBitmapKnob (this, 5, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (328, 57, -1, -1);
		(panel . sustain_knob = new wxTravelingBitmapKnob (this, 6, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (398, 57, -1, -1);
		(panel . release_knob = new wxTravelingBitmapKnob (this, 7, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (468, 57, -1, -1);
		(panel . freq_knob = new wxTravelingBitmapKnob (this, 1, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (180, 127, -1, -1);
		(panel . amp_knob = new wxTravelingBitmapKnob (this, 2, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (250, 127, -1, -1);
		(panel . index_knob = new wxTravelingBitmapKnob (this, 3, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (320, 127, -1, -1);
		(panel . porta_time_knob = new wxTravelingBitmapKnob (this, 19, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (424, 127, -1, -1);
		(panel . speed_knob = new wxTravelingBitmapKnob (this, 8, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (155, 198, -1, -1);
		(panel . vibrato_knob = new wxTravelingBitmapKnob (this, 9, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (225, 198, -1, -1);
		(panel . tremolo_knob = new wxTravelingBitmapKnob (this, 10, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (295, 198, -1, -1);
		(panel . wah_wah_knob = new wxTravelingBitmapKnob (this, 11, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (365, 198, -1, -1);
		(panel . lfo_pan_knob = new wxTravelingBitmapKnob (this, 12, & controller_knob_handle, & controller_knob_track, 13.0)) -> setCallback (controller_callback) -> SetSize (435, 198, -1, -1);
		//panel . set_all_buttons ();
	}
	void OnEraseBackground (wxEraseEvent & event) {}
	void OnPaint (wxPaintEvent & event) {wxPaintDC dc (this); dc . DrawBitmap (front_panel, 0, 0, true);}
	void OnLeftUp (wxMouseEvent & event) {
		wxPoint position = event . GetPosition ();
		if (position . x < 81 && position . y < 31) {
			if (wxYES == wxMessageBox (_T ("CONFITM EXIT"), _T ("INFO"), wxYES_NO | wxICON_EXCLAMATION, this)) general_quit ();
			return;
		}
		if (position . y > 24 && position . y < 39) {
			if (position . x > 158 && position . x < 199) {
				wxAudioDialog dialog (this, audio);
				dialog . ShowModal ();
				return;
			}
//			if (position . x > 107 && position . x < 152) {
//				MidiHardwareDialogClass dialog (this);
//				dialog . ShowModal ();
//				return;
//			}
		}
	}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(main_panel, wxPanel)
EVT_ERASE_BACKGROUND(main_panel :: OnEraseBackground)
EVT_PAINT(main_panel :: OnPaint)
EVT_LEFT_UP(main_panel :: OnLeftUp)
END_EVENT_TABLE()


