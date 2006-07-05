arch = $(shell uname -m)

DECODER_PATH = /home/thirsima/Work/online-demo-libs/$(arch)/decoder
AKU_PATH = /home/thirsima/Work/online-demo-libs/$(arch)/akumod

OPT = -g -O2
AUX_CXXFLAGS ?= -Wall
INCLUDES = -I$(AKU_PATH) -I$(DECODER_PATH) -I/usr/include/SDL \
	-I/usr/include/paragui -I/usr/include/freetype2 \
	-I/opt/gnome/lib/sigc++-1.2/include -I/opt/gnome/include/sigc++-1.2 \
	-I/share/puhe/linux/include
LDFLAGS = -L$(AKU_PATH) -L$(DECODER_PATH)
CXXFLAGS ?= $(AUX_CXXFLAGS) $(INCLUDES) $(OPT)

##################################################

progs = jaakko gui ## decoder recognizer

default: $(progs)

all: $(progs)

decoder_srcs = decoder.cc Decoder.cc conf.cc msg.cc endian.cc
decoder_libs = -ldecoder
decoder: $(decoder_srcs:%.cc=%.o) $(DECODER_PATH)/libdecoder.a

recognizer_srcs = recognizer.cc conf.cc msg.cc Recognizer.cc Process.cc
recognizer_libs = -lpthread -lakumod -lfftw3 -lsndfile
recognizer: $(recognizer_srcs:%.cc=%.o) $(AKU_PATH)/libakumod.a

gui_srcs = gui.cc conf.cc msg.cc Process.cc io.cc endian.cc
gui_libs = -lpthread -lakumod
gui: $(gui_srcs:%.cc=%.o)

jaakko_srcs = jaakko.cc AudioStream.cc AudioInputStream.cc Buffer.cc \
	Process.cc msg.cc endian.cc AudioFileInputController.cc \
	AudioOutputStream.cc AudioInputController.cc AudioLineInputController.cc \
	OutQueueController.cc Application.cc Window.cc WindowInit.cc \
	WindowMain.cc WindowFileRecognizer.cc WidgetSpectrum.cc \
	WindowReset.cc WindowRecognizer.cc WindowMicrophoneRecognizer.cc \
	WidgetRecognitionArea.cc str.cc WidgetRecognitionTexts.cc Recognition.cc
jaakko_libs = -lportaudio -lsndfile -lparagui -lfreetype
jaakko: $(jaakko_srcs:%.cc=%.o)

srcs = $(decoder_srcs) $(recognizer_srcs) $(gui_srcs) $(jaakko_srcs)
objs = $(srcs:%.cc=%.o)

##################################################

default: $(progs)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

$(progs): %: %.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $($@_srcs:%.cc=%.o) $($@_libs)

.PHONY: clean
clean:
	rm -f $(progs) $(objs) $(extra_clean) dep

.PHONY: cleaner
cleaner: clean
	rm -f *~ Makefile

dep:
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $(sort $(srcs)) > dep

include dep

