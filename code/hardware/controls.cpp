///////////////////////////////////////
// Copyright (C) 2008 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "controls.h"
#include "wx/dcbuffer.h"

wxAnimatedButton :: wxAnimatedButton (wxWindow * parent, wxWindowID id, wxBitmap * bitmap, bool two_states) : wxWindow (parent, id) {
	controller_id = id;
	this -> cb = NULL;
	this -> bitmap = bitmap;
	this -> two_states = two_states;
	state = false;
	half_height = bitmap -> GetHeight () / 2;
	width = bitmap -> GetWidth ();
	SetInitialSize (wxSize (width, half_height));
	with_background = false;
}

wxAnimatedButton :: ~ wxAnimatedButton (void) {}

wxAnimatedButton * wxAnimatedButton :: locateOnBackground (int x, int y, wxBitmap * background) {
	this -> background = background -> GetSubBitmap (wxRect (x, y, bitmap -> GetWidth (), bitmap -> GetHeight ()));
	with_background = true;
	SetSize (x, y, -1, -1);
	return this;
}

wxAnimatedButton * wxAnimatedButton :: setValue (double value) {
	bool new_state = value != 0.0;
	if (new_state == state) return this;
	state = new_state;
	Refresh ();
	return this;
}
double wxAnimatedButton :: GetValue (void) {return state ? 1.0 : 0.0;}
wxAnimatedButton * wxAnimatedButton :: setCallback (callback cb) {this -> cb = cb; return this;}

void wxAnimatedButton :: OnLeftUp (wxMouseEvent & event) {
	if (two_states) return;
	state = false;
	if (cb) cb (controller_id, 0.0);
	Refresh ();
}
void wxAnimatedButton :: OnLeftDown (wxMouseEvent & event) {
	state = ! state;
	if (cb) cb (controller_id, state ? 1.0 : 0.0);
	Refresh ();
}
void wxAnimatedButton :: OnErase (wxEraseEvent & event) {}
void wxAnimatedButton :: OnPaint (wxPaintEvent & event) {
	wxBufferedPaintDC dc (this);
//	dc . Clear ();
	if (with_background) dc . DrawBitmap (background, 0, 0, true);
	dc . DrawBitmap (bitmap -> GetSubBitmap (wxRect (0, state ? half_height : 0, width, half_height)), 0, 0, true);
}

BEGIN_EVENT_TABLE(wxAnimatedButton, wxWindow)
EVT_ERASE_BACKGROUND(wxAnimatedButton :: OnErase)
EVT_PAINT(wxAnimatedButton :: OnPaint)
EVT_LEFT_DOWN(wxAnimatedButton :: OnLeftDown)
EVT_LEFT_UP(wxAnimatedButton :: OnLeftUp)
END_EVENT_TABLE()



wxAnimatedKnob :: wxAnimatedKnob (wxWindow * parent, wxWindowID id, wxBitmap * bitmap, int frames, bool encoder) : wxWindow (parent, id) {
	with_background = false;
	circular = false;
	motion = 1.0 / 128.0;
	captured_x = captured_y = 0;
	this -> cb = NULL;
	this -> controller_id = id;
	this -> bitmap = bitmap;
	this -> not_encoder = ! encoder;
	encoder_resolution = 128;
	last_encoder_value = 0;
	frame_height = bitmap -> GetHeight () / frames;
	frame_width = bitmap -> GetWidth ();
	centre_x = frame_width / 2; centre_y = frame_height / 2;
	shift = M_PI / 2.0;
	value = 0.0; last_value = 0.0;
	position = 0;
	angle = 0.0;
	back_angle = 0.0;
	rotation_overflow = false;
	this -> frames = frames;
	this -> frames_m1 = frames - 1;
	SetInitialSize (wxSize (frame_width, frame_height));
}
wxTravelingBitmapKnob :: wxTravelingBitmapKnob (wxWindow * parent, wxWindowID id, wxBitmap * traveller, wxBitmap * track, double radius, bool encoder)
: wxAnimatedKnob (parent, id, track, 1, encoder) {
	this -> traveller = traveller;
	traveller_half_width = 0.5 * (double) traveller -> GetWidth ();
	traveller_half_height = 0.5 * (double) traveller -> GetHeight ();
	this -> radius = radius;
	setRange (encoder ? 1.0 : 0.75);
}
wxTravelingBitmapKnob :: ~ wxTravelingBitmapKnob (void) {}
wxAnimatedKnob :: ~ wxAnimatedKnob (void) {}

BEGIN_EVENT_TABLE(wxAnimatedKnob, wxWindow)
EVT_ERASE_BACKGROUND(wxAnimatedKnob :: OnErase)
EVT_PAINT(wxAnimatedKnob :: OnPaint)
EVT_MOTION(wxAnimatedKnob :: OnMouse)
EVT_LEFT_DOWN(wxAnimatedKnob :: OnLeftDown)
EVT_LEFT_UP(wxAnimatedKnob :: OnLeftUp)
END_EVENT_TABLE()

wxAnimatedKnob * wxAnimatedKnob :: setEncoderResolution (int resolution) {encoder_resolution = resolution; last_encoder_value = (int) (value * (double) resolution); return this;}
wxAnimatedKnob * wxAnimatedKnob :: setCallback (callback cb) {this -> cb = cb; return this;}
wxAnimatedKnob * wxAnimatedKnob :: setCircular (bool circular) {this -> circular = circular; return this;}
wxAnimatedKnob * wxAnimatedKnob :: setMotion (double motion) {this -> motion = motion; return this;}

wxAnimatedKnob * wxAnimatedKnob :: locateOnBackground (int x, int y, wxBitmap * background) {
	this -> background = background -> GetSubBitmap (wxRect (x, y, frame_width, frame_height));
	with_background = true;
	SetSize (x, y, -1, -1);
	return this;
}

void wxAnimatedKnob :: OnLeftDown (wxMouseEvent & event) {
	rotation_overflow = false;
	CaptureMouse ();
	if (circular) {circular_mouse_process (event, true); return;}
	wxPoint px = event . GetPosition ();
	captured_x = (double) px . x;
	captured_y = (double) px . y;
}
void wxAnimatedKnob :: OnLeftUp (wxMouseEvent & event) {ReleaseMouse (); back_angle = angle;}
void wxAnimatedKnob :: OnMouse (wxMouseEvent & event) {
	if (! event . Dragging ()) return;
	if (circular) circular_mouse_process (event, false);
	else linear_mouse_process (event);
}

void wxAnimatedKnob :: OnErase (wxEraseEvent & event) {}
void wxAnimatedKnob :: OnPaint (wxPaintEvent & event) {
	wxBufferedPaintDC dc (this);
	if (with_background) dc . DrawBitmap (background, 0, 0, true);
	dc . DrawBitmap (bitmap -> GetSubBitmap (wxRect (0, frame_height * position, frame_width, frame_height)), 0, 0, true);
}

void wxTravelingBitmapKnob :: OnPaint (wxPaintEvent & event) {
	double angle = value * range;
	angle = range_shift - angle;
	angle *= M_PI * 2.0;
	int x = (int) (0.5 + (double) centre_x + cos (angle) * radius - traveller_half_width);
	int y = (int) (0.5 + (double) centre_y - sin (angle) * radius - traveller_half_height);
	wxBufferedPaintDC dc (this);
	dc . DrawBitmap (* bitmap, 0, 0, true);
	dc . DrawBitmap (* traveller, x, y, true);
}

wxTravelingBitmapKnob * wxTravelingBitmapKnob :: setRange (double range) {this -> range = range; this -> range_shift = 0.25 + 0.5 * this -> range; return this;}

void wxAnimatedKnob :: circular_mouse_process (wxMouseEvent & event, bool capture_angle) {
	if (rotation_overflow) return;
	wxPoint px = event . GetPosition ();
	double x = (double) (px . x - centre_x);
	double y = (double) (px . y - centre_y);
	double radius = sqrt (x * x + y * y);
	if (radius < 1.0) return;
	double sine = y / radius;
	double cosine = x / radius;
	double pi2 = M_PI * 2.0;
	if (sine >= 0.0) angle = acos (cosine);
	else angle = pi2 - acos (cosine);
	// shift goes here
	angle -= shift;
	while (angle < 0.0) angle += pi2;
	while (angle > pi2) angle -= pi2;
	// capture back angle and return
	if (capture_angle) {
		back_angle = angle - back_angle;
		return;
	}
	angle -= back_angle;
	while (angle < 0.0) angle += pi2;
	while (angle > pi2) angle -= pi2;
	value = angle / pi2;
//	value = angle * (double) frames / pi2;
	if (not_encoder) {
		if (value < 0.25 && last_value > 0.75) {value = 1.0; rotation_overflow = true; angle = 0.0;}// back_angle = angle;}
		if (value > 0.75 && last_value < 0.25) {value = 0.0; rotation_overflow = true; angle = 0.0;}// back_angle = angle;}
	}
	last_value = value;
	position = (int) (value * (double) frames);
	if (position < 0) position = 0;
	if (position > frames_m1) position = frames_m1;
	callback_update ();
	Refresh ();
}
void wxAnimatedKnob :: callback_update (void) {
	if (cb == NULL) return;
	if (not_encoder) {cb (controller_id, value); return;}
	int new_encoder_value = (int) (value * (double) encoder_resolution);
	int delta = new_encoder_value - last_encoder_value;
	int half_range = encoder_resolution >> 1;
	if (delta > half_range) delta -= encoder_resolution;
	if (delta < - half_range) delta += encoder_resolution;
	if (delta == 0) return;
	last_encoder_value = new_encoder_value;
	cb (controller_id, (double) delta);
}

void wxAnimatedKnob :: linear_mouse_process (wxMouseEvent & event) {
	wxPoint px = event . GetPosition ();
	double x = (double) (px . x - captured_x); captured_x = px . x;
	double y = (double) (px . y - captured_y); captured_y = px . y;
	bool negative = (x - y < 0.0);
	double delta = motion * sqrt (x * x + y * y);
	if (negative) value -= delta; else value += delta;
	if (not_encoder) {if (value > 1.0) value = 1.0; if (value < 0.0) value = 0.0;}
	else {while (value > 1.0) value -= 1.0; while (value < 0.0) value += 1.0;}
	position = (int) (value * (double) frames);
	callback_update ();
	Refresh ();
}

wxAnimatedKnob * wxAnimatedKnob :: setValue (double value) {
	this -> value = value;
	position = (int) (value * (double) frames);
	if (position < 0) position = 0;
	if (position > frames_m1) position = frames_m1;
	Refresh ();
	return this;
}


double wxAnimatedKnob :: getValue (void) {return value;}

wxAnimatedVector :: wxAnimatedVector (wxWindow * parent, wxWindowID idx, wxWindowID idy, wxBitmap * slider, int width, int height, bool no_initial_size) : wxWindow (parent, idx >= 0 ? idx : idy) {
	controller_idx = idx; controller_idy = idy;
	smaller_slider_x = smaller_slider_y = false;
	this -> surface = NULL;
	this -> slider = slider;
	this -> frame_width = width;
	this -> frame_height = height;
	reciprocal_x = width != 0 ? 1.0 / (double) width : 1.0;
	reciprocal_y = height != 0 ? 1.0 / (double) height : 1.0;
	captured_x = captured_y = 0.0;
	motion = 1.0;
	x = y = 0.0;
	range_x = slider -> GetWidth () - width;
	range_y = slider -> GetHeight () - height;
	if (range_x <= 0) {smaller_slider_x = true; range_x = - range_x;}
	if (range_y <= 0) {smaller_slider_y = true; range_y = - range_y;}
	spring_loaded = false;
	spring_x = spring_y = 0.5;
	if (no_initial_size) return;
	SetInitialSize (wxSize (width, height));
}

wxAnimatedVector :: wxAnimatedVector (wxWindow * parent, wxWindowID idx, wxWindowID idy, wxBitmap * slider, wxBitmap * surface, bool no_initial_size) : wxWindow (parent, idx >= 0 ? idx : idy) {
	controller_idx = idx; controller_idy = idy;
	smaller_slider_x = smaller_slider_y = false;
	this -> surface = surface;
	this -> slider = slider;
	this -> frame_width = surface -> GetWidth ();
	this -> frame_height = surface -> GetHeight ();
//	reciprocal_x = frame_width != 0 ? 1.0 / (double) frame_width : 1.0;
//	reciprocal_y = frame_height != 0 ? 1.0 / (double) frame_height : 1.0;
	captured_x = captured_y = 0.0;
	motion = 1.0;
	x = y = 0.0;
	range_x = slider -> GetWidth () - frame_width;
	range_y = slider -> GetHeight () - frame_height;
	if (range_x <= 0) {smaller_slider_x = true; range_x = - range_x;}
	if (range_y <= 0) {smaller_slider_y = true; range_y = - range_y;}
	reciprocal_x = range_x != 0 ? 1.0 / (double) range_x : 1.0;
	reciprocal_y = range_y != 0 ? 1.0 / (double) range_y : 1.0;
	spring_loaded = false;
	spring_x = spring_y = 0.5;
	if (no_initial_size) return;
	SetInitialSize (wxSize (frame_width, frame_height));
}

wxAnimatedVector :: ~ wxAnimatedVector (void) {}

wxAnimatedVector * wxAnimatedVector :: setX (double x) {this -> x = x * (double) range_x; Refresh (); return this;}
wxAnimatedVector * wxAnimatedVector :: setY (double y) {this -> y = (1.0 - y) * (double) range_y; Refresh (); return this;}
wxAnimatedVector * wxAnimatedVector :: set (double x, double y) {this -> x = x * (double) range_x; this -> y = (1.0 - y) * (double) range_y; Refresh (); return this;}
wxAnimatedVector * wxAnimatedVector :: redraw (void) {Refresh (); return this;}
double wxAnimatedVector :: getX (void) {return x * reciprocal_x;}
double wxAnimatedVector :: getY (void) {return 1.0 - y * reciprocal_y;}
wxAnimatedVector * wxAnimatedVector :: setMotionRatio (double motion) {this -> motion = motion; return this;}
wxAnimatedVector * wxAnimatedVector :: loadSpring (double x, double y) {spring_x = x; spring_y = y; spring_loaded = true; return this;}
wxAnimatedVector * wxAnimatedVector :: unloadSpring (void) {spring_loaded = false; return this;}
wxAnimatedVector * wxAnimatedVector :: setCallback (callback cb) {this -> cb = cb; return this;}

BEGIN_EVENT_TABLE(wxAnimatedVector, wxWindow)
EVT_ERASE_BACKGROUND(wxAnimatedVector :: OnErase)
EVT_PAINT(wxAnimatedVector :: OnPaint)
EVT_MOTION(wxAnimatedVector :: OnMouse)
EVT_LEFT_DOWN(wxAnimatedVector :: OnLeftDown)
EVT_LEFT_UP(wxAnimatedVector :: OnLeftUp)
END_EVENT_TABLE()

void wxAnimatedVector :: OnLeftDown (wxMouseEvent & event) {
	CaptureMouse ();
	wxPoint px = event . GetPosition ();
	captured_x = (double) px . x;
	captured_y = (double) px . y;
}
void wxAnimatedVector :: OnLeftUp (wxMouseEvent & event) {
	ReleaseMouse ();
	if (spring_loaded) {
		if (cb != NULL) {
			if (controller_idx >= 0 && spring_x != x) cb (controller_idx, spring_x);
			if (controller_idy >= 0 && spring_y != y) cb (controller_idy, spring_y);
		}
		set (spring_x, spring_y);
	}
}
void wxAnimatedVector :: OnMouse (wxMouseEvent & event) {if (! event . Dragging ()) return; mouse_process (event);}
void wxAnimatedVector :: OnErase (wxEraseEvent & event) {}
void wxAnimatedVector :: OnPaint (wxPaintEvent & event) {
	wxBufferedPaintDC dc (this);
	dc . Clear ();
	if (surface != NULL) dc . DrawBitmap (* surface, 0, 0, true);
	if (smaller_slider_x) {
		if (smaller_slider_y) dc . DrawBitmap (* slider, (int) (x + 0.499), (int) (y + 0.499), true);
		else dc . DrawBitmap (slider -> GetSubBitmap (wxRect (0, range_y - (int) (y + 0.499), slider -> GetWidth (), frame_height)), (int) (x + 0.499), 0, true);
	} else {
		if (smaller_slider_x) dc . DrawBitmap (slider -> GetSubBitmap (wxRect (range_x - (int) (x + 0.499), 0, frame_width, slider -> GetHeight ())), 0, (int) (y + 0.499), true);
		else dc . DrawBitmap (slider -> GetSubBitmap (wxRect (range_x - (int) (x + 0.499), range_y - (int) (y + 0.499), frame_width, frame_height)), 0, 0, true);
	}
}

void wxAnimatedVector :: mouse_process (wxMouseEvent & event) {
	wxPoint px = event . GetPosition ();
	x += motion * ((double) px . x - captured_x); if (x < 0.0) x = 0.0; if (x > (double) range_x) x = (double) range_x; captured_x = (double) px . x;
	y += motion * ((double) px . y - captured_y); if (y < 0.0) y = 0.0; if (y > (double) range_y) y = (double) range_y; captured_y = (double) px . y;
	if (cb != NULL) {
		if (controller_idx >= 0) cb (controller_idx, getX ());
		if (controller_idy >= 0) cb (controller_idy, getY ());
	}
	Refresh ();
}

wxPictureKeyboard :: wxPictureKeyboard (wxWindow * parent, wxWindowID id, wxBitmap * bitmap, int size_selector) : wxWindow (parent, id), kb (0, 0) {
	switch (size_selector) {
	case 1: kb . set_keyboard_layout_y (66, 44) -> set_keyboard_layout_x (11, 1, 2, 3, 4, 5) -> set_ambitus (17, 54); break;
	case 3: kb . set_keyboard_layout_y (132, 88) -> set_keyboard_layout_x (22, 2, 4, 6, 8, 10) -> set_ambitus (17, 54); break;
	default: kb . set_keyboard_layout_y (99, 66) -> set_keyboard_layout_x (16, 2, 3, 4, 5, 6) -> set_ambitus (17, 54); break;
	}
	this -> bitmap = bitmap;
	width = bitmap -> GetWidth ();
	height = bitmap -> GetHeight ();
	SetInitialSize (wxSize (width, height));
}

wxPictureKeyboard :: ~ wxPictureKeyboard (void) {
}

BEGIN_EVENT_TABLE(wxPictureKeyboard, wxWindow)
EVT_PAINT(wxPictureKeyboard :: OnPaint)
EVT_ERASE_BACKGROUND(wxPictureKeyboard :: OnErase)
EVT_LEFT_DOWN(wxPictureKeyboard :: OnLeftDown)
EVT_LEFT_UP(wxPictureKeyboard :: OnLeftUp)
EVT_RIGHT_DOWN(wxPictureKeyboard :: OnLeftUp)
END_EVENT_TABLE()

void wxPictureKeyboard :: OnErase (wxEraseEvent & event) {}
void wxPictureKeyboard :: OnPaint (wxPaintEvent & event) {wxBufferedPaintDC dc (this); dc . DrawBitmap (* bitmap, 0, 0, true);}

void wxPictureKeyboard :: OnLeftDown (wxMouseEvent & event) {MouseProcess (event, false);}
void wxPictureKeyboard :: OnLeftUp (wxMouseEvent & event) {MouseProcess (event, true);}

void wxPictureKeyboard :: MouseProcess (wxMouseEvent & event, bool up) {
	wxPoint point = event . GetPosition ();
	int key = kb . get (point . x, point . y);
	if (key < 0) return;
	if (up) keyoff (key);
	else keyon (key);
}

void wxPictureKeyboard :: keyon (int x) {}
void wxPictureKeyboard :: keyoff (int x) {}

wx_keyboard_calculator :: wx_keyboard_calculator (int x, int y) {
	kx0 = x;
	ky0 = y;
	first = 17;
	whites = 54;
	set_keyboard_layout_y (66, 44);
	set_keyboard_layout_x (11, 1, 2, 3, 4, 5);
}
int wx_keyboard_calculator :: get (int x, int y) {
	x -= kx0;
	y -= ky0;

	if (x < 0 || x > kx100) return -1;
	x += shift;
	int oct = x / c_key;
	oct *= 12;
	oct += first_c;
	x %= c_key;
	if (y <= white_length && y > black_length) {
		if (x < d_key) return 0 + oct;
		if (x < e_key) return 2 + oct;
		if (x < f_key) return 4 + oct;
		if (x < g_key) return 5 + oct;
		if (x < a_key) return 7 + oct;
		if (x < h_key) return 9 + oct;
		return 11 + oct;
	}
	if (y >= 0 && y <= black_length) {
		if (x < cis_key) return 0 + oct;
		if (x < des_key) return 1 + oct;
		if (x < dis_key) return 2 + oct;
		if (x < es_key) return 3 + oct;
		if (x < f_key) return 4 + oct;
		if (x < fis_key) return 5 + oct;
		if (x < ges_key) return 6 + oct;
		if (x < gis_key) return 7 + oct;
		if (x < as_key) return 8 + oct;
		if (x < ais_key) return 9 + oct;
		if (x < b_key) return 10 + oct;
		return 11 + oct;
	}
	return -1;
}

//void keyboard_calculator :: set_first_f (int f) {first_f = f * 12 + 5;}
wx_keyboard_calculator * wx_keyboard_calculator :: set_ambitus (int first, int whites) {
	this -> first = first;
	this -> whites = whites;
	kx100 = whites * d_key - 1;
	first_c = first / 12;
	first_c *= 12;
	first %= 12;
	switch (first) {
	case 0: case 1: shift = 0; break;
	case 2: case 3: shift = 1; break;
	case 4: shift = 2; break;
	case 5: case 6: shift = 3; break;
	case 7: case 8: shift = 4; break;
	case 9: case 10: shift = 5; break;
	default: shift = 6; break;
	}
	shift *= d_key;
	return this;
}
wx_keyboard_calculator * wx_keyboard_calculator :: set_keyboard_layout_x (int white, int b1, int b2, int b3, int b4, int b5) {
	d_key = white;
	e_key = d_key + white;
	f_key = e_key + white;
	g_key = f_key + white;
	a_key = g_key + white;
	h_key = a_key + white;
	c_key = h_key + white;

	cis_key = d_key - b4;
	des_key = d_key + b2;
	dis_key = e_key - b2;
	es_key = e_key + b4;
	fis_key = g_key - b5;
	ges_key = g_key + b1;
	gis_key = a_key - b3;
	as_key = a_key + b3;
	ais_key = h_key - b1;
	b_key = h_key + b5;

	set_ambitus (first, whites);
	return this;
}
wx_keyboard_calculator * wx_keyboard_calculator :: set_keyboard_layout_y (int white, int black) {
	white_length = white;
	black_length = black;
	return this;
}

#include "wx/dcbuffer.h"

wxOscilloscopeController :: wxOscilloscopeController (wxWindow * parent, wxWindowID id, int * wave) : wxWindow (parent, id) {
	this -> scaling_factor = 1;
	this -> with_background = false;
	this -> wave = wave;
	for (int ind = 0; ind < 256; ind++) {wave [ind] = (int) (sin (6.283185307179586476925286766559 * (double) ind / 256.0) * 52.0);}
	SetInitialSize (wxSize (256, 128));
}

wxOscilloscopeController :: ~ wxOscilloscopeController (void) {
}

wxOscilloscopeController * wxOscilloscopeController :: locateOnBackground (int x, int y, wxBitmap * background) {
	this -> background = background -> GetSubBitmap (wxRect (x, y, 256, 128));
	with_background = true;
	SetSize (x, y, -1, -1);
	return this;
}

wxOscilloscopeController * wxOscilloscopeController :: setScalingFactor (int factor) {this -> scaling_factor = factor; return this;}

void wxOscilloscopeController :: OnErase (wxEraseEvent & event) {}
void wxOscilloscopeController :: OnPaint (wxPaintEvent & event) {
	wxBufferedPaintDC dc (this);
	//dc . SetBackground (wxBrush (wxColour (128, 128, 128)));
	//dc . Clear ();
  // Create a memory DC
//  wxMemoryDC temp_dc;
//  temp_dc.SelectObject(test_bitmap);

  // We can now draw into the memory DC...
  // Copy from this DC to another DC.
//  old_dc.Blit(250, 50, BITMAP_WIDTH, BITMAP_HEIGHT, temp_dc, 0, 0);
	dc . SetBrush (wxBrush (wxColour (0, 0, 0)));
	if (with_background) dc . DrawBitmap (background, 0, 0, true);
	else dc . DrawRectangle (0, 0, 256, 128);
	dc . SetPen (wxPen (wxColour (0, 255, 0)));
	int * ipfrom = wave;
	int * ipto = ipfrom + 1;
	switch (scaling_factor) {
	case 1: for (int ind = 0; ind < 255; ind++) {dc . DrawLine (ind, 64 - * ipfrom++, ind + 1, 64 - * ipto++);} break;
	case 2: ipto++; for (int ind = 0; ind < 127; ind++) {dc . DrawLine (ind, 32 - (* ipfrom) / 2, ind + 1, 32 - (* ipto) / 2); ipfrom += 2; ipto += 2;} break;
	case 3: for (int ind = 0; ind < 255; ind++) {dc . DrawLine (ind, 32 - (* ipfrom++) / 2, ind + 1, 32 - (* ipto++) / 2);} break;
	case 4: ipto += 3; for (int ind = 0; ind < 63; ind++) {dc . DrawLine (ind, 16 - (* ipfrom) / 4, ind + 1, 16 - (* ipto) / 4); ipfrom += 4; ipto += 4;} break;
	case 5: ipto++; for (int ind = 0; ind < 127; ind++) {dc . DrawLine (ind, 16 - (* ipfrom) / 4, ind + 1, 16 - (* ipto) / 4); ipfrom += 2; ipto += 2;} break;
	}
}

BEGIN_EVENT_TABLE(wxOscilloscopeController, wxWindow)
EVT_PAINT(wxOscilloscopeController :: OnPaint)
EVT_ERASE_BACKGROUND(wxOscilloscopeController :: OnErase)
END_EVENT_TABLE()
