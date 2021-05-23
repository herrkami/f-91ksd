import os
import sys
import rtttl
import time

import numpy as np

import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GObject, GLib


def play_ndarray_gst(y, sample_rate):
    """
    y: ndarray, values between -1 and 1
    sample_rate: sample rate in Hz
    """
    Gst.init(None)

    pipeline = Gst.Pipeline()

    appsrc = Gst.ElementFactory.make('appsrc', 'appsrc')
    queueaudio = Gst.ElementFactory.make('queue', 'queueaudio')
    audioconvert = Gst.ElementFactory.make('audioconvert', 'audioconvert')
    sink = Gst.ElementFactory.make('autoaudiosink', 'sink')

    appsrc.set_property("format", Gst.Format.TIME)
    appsrc.set_property("block", True)
    appsrc.set_property("is-live", True)
    appsrc.set_property("caps", Gst.Caps.from_string("audio/x-raw,format={},rate={},channels=1,layout=interleaved".format("S16LE" if sys.byteorder=="little" else "S16BE", sample_rate)))

    pipeline.add(appsrc)
    pipeline.add(queueaudio)
    pipeline.add(audioconvert)
    pipeline.add(sink)

    appsrc.link(queueaudio)
    queueaudio.link(audioconvert)
    audioconvert.link(sink)

    pipeline.set_state(Gst.State.PLAYING)

    y = (y*32767).astype(np.int16)
    appsrc.emit('push-buffer', Gst.Buffer.new_wrapped(y.tobytes()))
    appsrc.emit("end-of-stream")

    mainloop = GLib.MainLoop()

    bus = pipeline.get_bus()
    bus.add_signal_watch()

    def eos_callback(*args):
        pipeline.set_state(Gst.State.NULL)
        mainloop.quit()

    bus.connect("message::eos", eos_callback)

    mainloop.run()

def play_rtttl(melody):
    sample_rate = 44100
    waveform = np.sin
    waveform = lambda x: np.clip(np.inf*np.sin(x), -1, 1)

    seq = rtttl.parse_rtttl(melody)['notes']
    yy = np.array([])
    for note in seq:
        f = note['frequency']
        d = note['duration']/1000
        # print(f, d)
        t = np.arange(0, d, 1/sample_rate)
        y = waveform(np.mod(2*np.pi*t*f, 2*np.pi))
        yy = np.append(yy, y)
    yy *= .1
    play_ndarray_gst(yy, sample_rate)



k1 = "K1:d=32,o=5,b=900:\
e7,p,e7,p,e7,p,e7,p,e6,p,e6,p,e6,p,e6,p,e5,p,e5,p,e5,p,e5,p,e4,p,e4,p,e4,p,e4,p"

k2 = "K2:d=32,o=5,b=900:\
a7,a4,a7,a4,a7,a4,a7,a4, 4p, g7,g4,f7,f4,d7,d4,c7,c4"

k3 = "K3:d=32,o=5,b=900:\
a7,d7,a7,d7, a7,p,a7,p,a7,p,a7,p, a7,4p,a7,4p, a7,8p, a7,16p, a7,32p, a7"

k4 = "K4:d=32,o=5,b=900:\
a7,8p, a7,8p, a7,4p,a7,4p, a7,d7,a7,d7,a7,d7,a7,d7, a7,p,a7,p,a7,p,a7,p, a7,4p,a7,4p, a7,8p, a7,8p"

k5 = "K5:d=32,o=5,b=900:\
b7,32p,b7,32p,b7,16p,f7,8p,b7,4p,f7,2p,b7,1p,b7"

k6 = "K6:d=8,o=5,b=900:\
a#7,p,a7,p,f#6"

k7 = "K7:d=32,o=5,b=900:\
c7,d#7,c7,d#7,c7,d#7,c7,d#7, e7,a#7,e7,a#7,e7,a#7,e7,a#7"

k8 = "K8:d=32,o=7,b=900:\
g,a,g,a,g#,a#,g#,a#,a,h,a,h,a#,b,a#,b,  g,a,g,a,g#,a#,g#,a#,a,h,a,h,a#,b,a#,b"

g0 = "G0:d=32,o=7,b=900:\
b,8p,b,32p,b,8p,b,1p,b,4p,b,4p,b,2p,b,2p,b,4p,b,4p,b,32p,b,2p,b,1p,b,8p,b,16p,b"

g1 = "G1:d=32,o=7,b=900:\
b,8p,b,8p,b,1p,b,8p,b,32p,b,4p,b,8p,b,1p,b,1p,b,1p,b,1p,b,2p,b,4p,b,16p,b,4p,b,2p,b,1p,b,4p,b,16p,b,2p,b,2p,b,1p,b,8p,b,8p,b,4p,b,8p,b,2p,b,16p,b,16p,b,4p,b,32p,b"

g2 = "G2:d=32,o=7,b=900:\
b,8p,b,4p,b,32p,b,2p,b,2p,b,2p,b,8p,b,2p,b,1p,b,2p,b,8p,b,4p,b,1p,b,4p,b,32p,b,16p,b,16p,b,2p,b,32p,b,1p,b,32p,b,4p,b,4p,b,8p,b,8p,b,16p,b,2p,b,32p,b,1p,b,1p,b,8p,b,16p,b,1p,b,2p,b,2p,b,1p,b,32p,b,32p,b,1p,b,2p,b,16p,b,1p,b,1p,b,1p,b,8p,b,1p,b,2p,b,16p,b,1p,b,1p,b,16p,b,16p,b,16p,b,8p,b,4p,b,4p,b,1p,b,2p,b,16p,b,1p,b,8p,b,2p,b,8p,b"

g3 = "G3:d=32,o=7,b=900:\
b,32p,b,4p,b,4p,b,1p,b,1p,b,2p,b,16p,b,32p,b,2p,b,1p,b,32p,b,1p,b,4p,b,8p,b,32p,b,8p,b,1p,b,8p,b,2p,b,4p,b,32p,b,4p,b,1p,b,4p,b,16p,b,16p,b,2p,b,1p,b,1p,b,4p,b,2p,b,32p,b,16p,b,4p,b,32p,b,8p,b,2p,b,16p,b,1p,b,2p,b,4p,b,1p,b,16p,b,16p,b,8p,b,4p,b,32p,b,2p,b,1p,b,8p,b,1p,b,1p,b,8p,b,2p,b,32p,b,16p,b,32p,b,4p,b,4p,b,16p,b,4p,b,1p,b,4p,b,2p,b,1p,b,2p,b,32p,b,32p,b,4p,b,16p,b,8p,b,2p,b,2p,b,1p,b,16p,b,2p,b,32p,b,4p,b,16p,b,2p,b,1p,b,32p,b,8p,b,2p,b,16p,b,32p,b,1p,b,2p,b,1p,b,4p,b,8p,b,16p,b,8p,b,1p,b,2p,b,8p,b,32p,b,16p,b,8p,b,2p,b,8p,b,32p,b,2p,b,1p,b,2p,b,32p,b,2p,b,8p,b,1p,b,8p,b,1p,b,16p,b,1p,b,4p,b,8p,b,8p,b,16p,b,2p,b,4p,b,1p,b,2p,b,8p,b,4p,b,8p,b,1p,b,1p,b,32p,b"

r0 = "R0:d=32,o=7,b=900:\
b,4p,b"

r1 = "R1:d=32,o=7,b=900:\
b,4p,b,4p,b"

r2 = "R2:d=32,o=7,b=900:\
b,1p,c,1p,b"

r3 = "R3:d=32,o=7,b=900:\
4c,32p,8b"


# play_rtttl(k1)
# time.sleep(0.5)
# play_rtttl(k2)
# time.sleep(0.5)
# play_rtttl(k3)
# time.sleep(0.5)
# play_rtttl(k4)
# time.sleep(0.5)
# play_rtttl(k5)
# time.sleep(0.5)
# play_rtttl(k6)
# time.sleep(0.5)
# play_rtttl(k7)
# time.sleep(0.5)
# play_rtttl(k8)
# time.sleep(0.5)
# play_rtttl(g1)
# time.sleep(0.5)
# play_rtttl(g2)
# time.sleep(0.5)
play_rtttl(g3)
time.sleep(0.5)
# play_rtttl(r0)
# time.sleep(0.5)
# play_rtttl(r1)
# time.sleep(0.5)
# play_rtttl(r2)
# time.sleep(0.5)
# play_rtttl(r3)
# time.sleep(0.5)
