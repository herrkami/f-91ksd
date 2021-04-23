#!/usr/bin/env python3
from gi.repository import Gtk, Gdk, GObject, Pango, PangoCairo
import cairo
import sys
import json
import zmq
import time

LED_ON_TIME_MIN = 20e-3

class Display(Gtk.DrawingArea) :

	__gsignals__ = {
		"key":(GObject.SIGNAL_RUN_FIRST , GObject.TYPE_NONE, (GObject.TYPE_CHAR, )),
	}

	S_ON =    1<<0
	S_BLINK = 1<<1

	def __init__(self, definition) :
		Gtk.DrawingArea.__init__(self)
		self.definition = definition
		self.backlight = False
		self.rightled = False
		self.caseled = False
		self.backlight_tlast = 0
		self.rightled_tlast = 0
		self.caseled_tlast = 0
		self.blink_state = True
		self.last_frame_time = 0
		self.show_names = False
		self.segment_states = {s["name"]:0 for s in definition["segments"]}
		self.connect("draw", self.paint)
		self.connect("scroll-event", self._scroll)
		self.connect("button-press-event", self._bpress)
		self.add_events(Gdk.EventMask.SCROLL_MASK|Gdk.EventMask.BUTTON_PRESS_MASK)
		self.add_tick_callback(self._tick)

	def _tick(self, wi, clock) :
		ti = clock.get_frame_time()
		if ti-self.last_frame_time > 200000 :
			#print("tock", self.segment_states)
			self.blink_state = not self.blink_state
			self.last_frame_time = ti
			self.queue_draw()
		return True

	def _scroll(self, wi, ev) :
		if ev.direction == Gdk.ScrollDirection.UP :
			self.emit("key", "U")
		elif ev.direction == Gdk.ScrollDirection.DOWN :
			self.emit("key", "D")

	def _bpress(self, wi, ev) :
		self.emit("key", "E")

	def set_backlight(self, v) :
		if v:
			self.backlight_tlast = time.time()
		else:
			while time.time() - self.backlight_tlast < LED_ON_TIME_MIN:
				pass
		self.backlight = bool(v)
		self.redraw()

	def set_rightled(self, v) :
		if v:
			self.rightled_tlast = time.time()
		else:
			while time.time() - self.rightled_tlast < LED_ON_TIME_MIN:
				pass
		self.rightled = bool(v)
		self.redraw()

	def set_caseled(self, v) :
		if v:
			self.caseled_tlast = time.time()
		else:
			while time.time() - self.caseled_tlast < LED_ON_TIME_MIN:
				pass
		self.caseled = bool(v)
		self.redraw()

	def clear(self) :
		for k in self.segment_states.keys() :
			self.segment_states[k] = 0
		self.redraw()

	def _set_segment(self, seg, v, m) :
		if seg not in self.segment_states.keys() :
			try :
				seg = next(s for s in self.segment_states.keys() if s.startswith(seg))
			except StopIteration :
				return
		if v :
			self.segment_states[seg] |= m
		else :
			self.segment_states[seg] &= ~m
		self.redraw()

	def set_segment(self, seg, v) :
		self._set_segment(seg, v, self.S_ON)

	def set_segment_blink(self, seg, v) :
		self._set_segment(seg, v, self.S_BLINK)

	def draw_background(self) :

		linpat = cairo.LinearGradient(0,0,0,self.height)
		linpat.add_color_stop_rgb(0, 128/255,166/255,128/255)
		linpat.add_color_stop_rgb(1, 104/255,135/255,104/255)

		self.cr.set_source(linpat)

		self.cr.rectangle(0, 0, self.width, self.height)
		self.cr.fill()


	def draw_backlight(self) :
		linpat = cairo.LinearGradient(0,0,self.width,0)
		linpat.add_color_stop_rgba(0, 1, 1, .9, .7)
		linpat.add_color_stop_rgba(1, 1, 1, .9, 0)
		self.cr.set_source(linpat)
		self.cr.rectangle(0, 0, self.width, self.height)
		self.cr.fill()

	def draw_rightled(self) :
		linpat = cairo.LinearGradient(0,0,self.width,0)
		linpat.add_color_stop_rgba(0, 1, 1, 0, 0)
		linpat.add_color_stop_rgba(1, 1, 1, 0, .7)
		self.cr.set_source(linpat)
		self.cr.rectangle(0, 0, self.width, self.height)
		self.cr.fill()

	def draw_caseled(self) :
		linpat = cairo.LinearGradient(self.width/2,0,self.width/2,4*self.height/5)
		linpat.add_color_stop_rgba(0, .9, 1, 1, 0)
		linpat.add_color_stop_rgba(1, .9, 1, 1, 1)
		self.cr.set_source(linpat)
		self.cr.rectangle(0, 0, self.width, self.height)
		self.cr.fill()

	def draw_display(self) :
		width = self.definition["width"]
		height = self.definition["height"]

		scale = min((self.width/width, self.height/height))
		self.cr.translate((self.width-width*scale)/2, (self.height-height*scale)/2)

		self.cr.scale(scale, scale)
		self.cr.set_line_cap(cairo.LINE_CAP_ROUND)
		self.cr.set_line_join(cairo.LINE_JOIN_ROUND)
		layout = PangoCairo.create_layout (self.cr)
		layout.set_alignment(Pango.Alignment.CENTER)

		desc = Pango.font_description_from_string ("Cantarell 3")
		layout.set_font_description( desc)
		for segment in self.definition["segments"] :
			if (self.segment_states[segment["name"]] & self.S_ON) and (self.segment_states[segment["name"]] & self.S_BLINK == 0):
				self.cr.set_source_rgb(0,0,0)
			elif (self.segment_states[segment["name"]] & self.S_ON) and (self.segment_states[segment["name"]] & self.S_BLINK) and self.blink_state:
				self.cr.set_source_rgb(0,0,0)
			else :
				self.cr.set_source_rgba(0,0,0, .05)
			self.cr.set_line_width(segment["width"])
			self.cr.move_to(*segment["coords"][0])
			for c in segment["coords"][1:] :
				self.cr.rel_line_to(*c)
			self.cr.stroke()
			if self.show_names :
				layout.set_text(segment["name"], -1)
				c1 = segment["coords"][0]
				c2 = c1.copy()
				for c in segment["coords"][1:] :
					c2[0] += c[0]
					c2[1] += c[1]
				self.cr.move_to((c1[0]+c2[0])/2, (c1[1]+c2[1])/2)

				self.cr.set_source_rgb(1,0,0)
				PangoCairo.show_layout(self.cr, layout)


	def paint(self, wi, cr) :
		self.cr = cr
		self.width = self.get_allocated_width()
		self.height = self.get_allocated_height()

		self.draw_background()
		if self.backlight :
			self.draw_backlight()
		if self.rightled :
			self.draw_rightled()
		if self.caseled :
			self.draw_caseled()

		self.draw_display()

	def redraw(self) :
		self.queue_draw()



builder = Gtk.Builder()
builder.add_from_file("ui.glade")

window = builder.get_object("window1")




class Handler :
	def button_mode_clicked(self, widget) :
		outsock.send(b"KD")

	def button_light_clicked(self, widget) :
		outsock.send(b"KU")

	def button_alarm_clicked(self, widget) :
		outsock.send(b"KE")

	def button_mode_clicked_long(self, widget) :
		outsock.send(b"LD")

	def button_light_clicked_long(self, widget) :
		outsock.send(b"LU")

	def button_alarm_clicked_long(self, widget) :
		outsock.send(b"LE")



	def quit(self, widget) :
		Gtk.main_quit()


def key_press(widget, event):
	key = Gdk.keyval_name(event.keyval)
	if key == 'I':
			outsock.send(b"LU")
	if key == 'i':
			outsock.send(b"KU")
	if key == 'K':
			outsock.send(b"LD")
	if key == 'k':
			outsock.send(b"KD")
	if key == 'L':
			outsock.send(b"LE")
	if key == 'l':
			outsock.send(b"KE")

with open("display.json", "r") as fi :
	display = Display(json.load(fi))
	builder.get_object("display_box").pack_start(display, True, True, 0)
	# display.connect("key", lambda w,z: outsock.send_string("K"+z))
	window.connect("key-press-event", key_press)
builder.connect_signals(Handler())
window.set_default_size(500, 200)
window.show_all()

ctx = zmq.Context()

insock = ctx.socket(zmq.SUB)
insock.setsockopt(zmq.SUBSCRIBE, b"")
insock.bind("ipc://aswemu-in")

outsock = ctx.socket(zmq.PUSH)
outsock.connect("ipc://aswemu-out")

def zmq_callback(queue, condition, display):
	while insock.getsockopt(zmq.EVENTS) & zmq.POLLIN:
		rx = insock.recv().decode()
		cmd = rx[0]
		if cmd == "L" :
			display.set_backlight(rx[1]=="1")
		elif cmd == "r" :
			display.set_rightled(rx[1]=="1")
		elif cmd == "c" :
			display.set_caseled(rx[1]=="1")
		elif cmd == "S" :
			display.set_segment(rx[2:], int(rx[1]))
		elif cmd == "s" :
			display.set_segment_blink(rx[2:], int(rx[1]))
		elif cmd == "C" :
			display.clear()


	return True

def wdt_callback() :
	outsock.send(b"W")
	return True

GObject.io_add_watch(insock.getsockopt(zmq.FD), GObject.IO_IN|GObject.IO_ERR|GObject.IO_HUP	, zmq_callback, display)
GObject.timeout_add(250, wdt_callback)

Gtk.main()
