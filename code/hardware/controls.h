///////////////////////////////////////
// Copyright (C) 2008 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#ifndef _WX_CONTROLS_
#define _WX_CONTROLS_

#include "wx/wx.h"

typedef void (* callback) (int controller_id, double value);

class wxAnimatedKnob : public wxWindow {
protected:
	bool circular;
	double motion;
	int controller_id;
	double last_value;
	double value;
	int position;
	int captured_x, captured_y;
	int frames, frames_m1;
	int frame_height, frame_width;
	int centre_x, centre_y;
	double angle, back_angle;
	double shift;
	callback cb;
	wxBitmap * bitmap;
	bool rotation_overflow;
	bool not_encoder;
	int encoder_resolution;
	int last_encoder_value;
	wxBitmap background;
	bool with_background;
public:
	wxAnimatedKnob * setValue (double value);
	wxAnimatedKnob * redraw (void);
	double getValue (void);
	wxAnimatedKnob * setCallback (callback cb);
	wxAnimatedKnob * setEncoderResolution (int resolution = 128);
	wxAnimatedKnob * setCircular (bool circular = true);
	wxAnimatedKnob * setMotion (double motion);
	wxAnimatedKnob * locateOnBackground (int x, int y, wxBitmap * background);
	wxAnimatedKnob (wxWindow * parent, wxWindowID id, wxBitmap * bitmap, int frames, bool encoder = false);
	~ wxAnimatedKnob (void);
	void OnErase (wxEraseEvent & event);
	virtual void OnPaint (wxPaintEvent & event);
	void OnMouse (wxMouseEvent & event);
	void OnLeftDown (wxMouseEvent & event);
	void OnLeftUp (wxMouseEvent & event);
private:
	void circular_mouse_process (wxMouseEvent & event, bool capture_angle);
	void linear_mouse_process (wxMouseEvent & event);
	void callback_update (void);
	DECLARE_EVENT_TABLE()
};

class wxTravelingBitmapKnob : public wxAnimatedKnob {
private:
	wxBitmap * traveller;
	double radius;
	double traveller_half_width, traveller_half_height;
	double range, range_shift;
public:
	virtual void OnPaint (wxPaintEvent & event);
	wxTravelingBitmapKnob * setRange (double range);
	wxTravelingBitmapKnob (wxWindow * parent, wxWindowID id, wxBitmap * traveller, wxBitmap * track, double radius, bool encoder = false);
	~ wxTravelingBitmapKnob (void);
};

class wxAnimatedVector : public wxWindow {
protected:
	int controller_idx, controller_idy;
	int range_x, range_y;
	bool smaller_slider_x, smaller_slider_y;
	int frame_width, frame_height;
	wxBitmap * slider;
	wxBitmap * surface;
	callback cb;
	double captured_x, captured_y;
	double motion;
	double x, y;
	double reciprocal_x, reciprocal_y;
	bool spring_loaded;
	double spring_x, spring_y;
public:
	wxAnimatedVector * setX (double x);
	wxAnimatedVector * setY (double y);
	wxAnimatedVector * set (double x, double y);
	wxAnimatedVector * redraw (void);
	double getX (void);
	double getY (void);
	wxAnimatedVector * loadSpring (double x = 0.5, double y = 0.5);
	wxAnimatedVector * unloadSpring (void);
	wxAnimatedVector * setCallback (callback cb);
	wxAnimatedVector (wxWindow * parent, wxWindowID idx, wxWindowID idy, wxBitmap * slider, int width, int height, bool no_initial_size = false);
	wxAnimatedVector (wxWindow * parent, wxWindowID idx, wxWindowID idy, wxBitmap * slider, wxBitmap * surface, bool no_initial_size = false);
	~ wxAnimatedVector (void);
	void OnErase (wxEraseEvent & event);
	virtual void OnPaint (wxPaintEvent & event);
	void OnMouse (wxMouseEvent & event);
	void OnLeftDown (wxMouseEvent & event);
	void OnLeftUp (wxMouseEvent & event);
	wxAnimatedVector * setMotionRatio (double motion);
private:
	void mouse_process (wxMouseEvent & event);
private:
	DECLARE_EVENT_TABLE()
};

class wxAnimatedButton : public wxWindow {
private:
	int controller_id;
	bool two_states;
	bool state;
	int half_height, width;
	wxBitmap * bitmap;
	callback cb;
	wxBitmap background;
	bool with_background;
public:
	wxAnimatedButton * locateOnBackground (int x, int y, wxBitmap * background);
	wxAnimatedButton * setValue (double value);
	double GetValue (void);
	wxAnimatedButton * setCallback (callback cb);
	wxAnimatedButton (wxWindow * parent, wxWindowID id, wxBitmap * bitmap, bool two_states = true);
	~ wxAnimatedButton (void);
	void OnLeftDown (wxMouseEvent & event);
	void OnLeftUp (wxMouseEvent & event);
	void OnErase (wxEraseEvent & event);
	void OnPaint (wxPaintEvent & event);
private:
	DECLARE_EVENT_TABLE()
};

class wx_keyboard_calculator {
private:
	int first, whites, kx0, ky0, kx100, shift, first_c, white_length, black_length;
	int d_key, e_key, f_key, g_key, a_key, h_key, c_key, cis_key, des_key, dis_key, es_key, fis_key, ges_key, gis_key, as_key, ais_key, b_key;
public:
	wx_keyboard_calculator (int x, int y);
	wx_keyboard_calculator * set_keyboard_layout_y (int white, int black);
	// 66, 44
	wx_keyboard_calculator * set_keyboard_layout_x (int white, int b1, int b2, int b3, int b4, int b5);
	// 11, 1, 2, 3, 4, 5
	wx_keyboard_calculator * set_ambitus (int first, int whites);
	// first = 17, whites = 54
	int get (int x, int y);
};

class wxPictureKeyboard : public wxWindow {
private:
	int width, height;
	wxBitmap * bitmap;
	wx_keyboard_calculator kb;
public:
	wxPictureKeyboard (wxWindow * parent, wxWindowID id, wxBitmap * bitmap, int size_selector = 2);
	~ wxPictureKeyboard (void);
	void OnLeftDown (wxMouseEvent & event);
	void OnLeftUp (wxMouseEvent & event);
	void OnErase (wxEraseEvent & event);
	void OnPaint (wxPaintEvent & event);
	void MouseProcess (wxMouseEvent & event, bool up);
	virtual void keyon (int key);
	virtual void keyoff (int key);
private:
	DECLARE_EVENT_TABLE()
};

class wxOscilloscopeController : public wxWindow {
private:
	wxBitmap background;
	bool with_background;
	int scaling_factor;
public:
	int * wave;
	wxOscilloscopeController * setScalingFactor (int factor);
	wxOscilloscopeController * locateOnBackground (int x, int y, wxBitmap * background);
	wxOscilloscopeController (wxWindow * parent, wxWindowID id, int * wave);
	~ wxOscilloscopeController (void);
	void OnErase (wxEraseEvent & event);
	void OnPaint (wxPaintEvent & event);
private:
	DECLARE_EVENT_TABLE()
};


#endif
