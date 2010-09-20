ARCH = $(shell uname -m)

ROOT = /share/puhe/online-demo

DECODER_PATH = $(ROOT)/decoder/src
AKU_PATH = $(ROOT)/aku

OPT = -O2
WARNINGS = -Wall -Wno-deprecated
AUX_CXXFLAGS ?= -Wall -fno-strict-aliasing -Wno-sign-compare

ifeq ($(ARCH),i686)
INCLUDES = -I$(AKU_PATH) -I$(DECODER_PATH) \
	-I$(DECODER_PATH)/fsalm -I$(DECODER_PATH)/misc \
	$(shell paragui-config --cflags) \
	-I/share/puhe/linux/include \
	-I/share/puhe/linux/include/lapackpp
LDFLAGS = -L$(AKU_PATH) -L$(DECODER_PATH) -L$(DECODER_PATH)/fsalm -L$(DECODER_PATH)/misc -L/share/puhe/linux/lib
endif

ifeq ($(ARCH),x86_64)
LAPACKPP_PATH = /share/puhe/x86_64/stow/lapackpp-2.5.0
INCLUDES = -I$(AKU_PATH) -I$(DECODER_PATH) \
	-I$(DECODER_PATH)/fsalm -I$(DECODER_PATH)/misc \
	$(shell paragui-config --cflags) \
	-I$(LAPACKPP_PATH)/include/lapackpp \
	-I/share/puhe/x86_64/include \
	-I/share/puhe/x86_64/include/hcld \
	-I/share/puhe/linux/include \
	-I/share/puhe/linux/include/lapackpp
LDFLAGS = -L$(AKU_PATH) -L$(DECODER_PATH) -L$(DECODER_PATH)/fsalm -L$(DECODER_PATH)/misc
endif

CXXFLAGS ?= $(AUX_CXXFLAGS) $(INCLUDES) $(OPT) $(WARNINGS)

##################################################

progs = jaakko decoder recognizer 

default: $(progs)

all: $(progs)

decoder_srcs = decoder.cc Decoder.cc conf.cc msg.cc endian.cc
decoder_libs = -ldecoder -lfsalm -lmisc
decoder: $(decoder_srcs:%.cc=%.o) $(DECODER_PATH)/libdecoder.a

recognizer_srcs = recognizer.cc conf.cc msg.cc \
	Recognizer.cc Process.cc Adapter.cc
recognizer_libs = -lpthread -laku -lfftw3 -lsndfile -llapackpp -llapack -lhcld
recognizer: $(recognizer_srcs:%.cc=%.o) $(AKU_PATH)/libaku.a

jaakko_srcs = jaakko.cc AudioStream.cc Buffer.cc \
	Process.cc msg.cc endian.cc \
	AudioInputController.cc \
	Application.cc Window.cc \
	WidgetWave.cc \
	WindowRecognizer.cc \
	WidgetRecognitionArea.cc WidgetRecognitionText.cc \
	WidgetScrollArea.cc WindowFile.cc WindowMessageBox.cc \
	WidgetSpectrogram.cc WindowChild.cc WindowSettings.cc \
	RecognizerListener.cc WindowStartProcess.cc \
	WidgetScrollBar.cc WindowSaveFile.cc WindowOpenFile.cc \
	WidgetTimeAxis.cc WidgetComparisonArea.cc \
	WidgetMultiLineEdit.cc conf.cc RecognizerProcess.cc WidgetAudioView.cc \
	comparison.cc WindowComparison.cc WidgetContainer.cc WindowTextEdit.cc \
	scrap.cc RecognizerStatus.cc WidgetStatus.cc

jaakko_libs = -lportaudio -lsndfile -lfftw3 $(shell paragui-config --libs) -lX11
#jaakko_libs = -lportaudio -lsndfile -lfftw3 $(shell paragui-config --libs) --static -L/usr/X11R6/lib -lphysfs -lz -laa -lgpm -lX11 -lXext -lslang-utf8 -ldl -ljack
jaakko: $(jaakko_srcs:%.cc=%.o)

srcs = $(decoder_srcs) $(recognizer_srcs) $(gui_srcs) $(jaakko_srcs)
objs = $(srcs:%.cc=%.o)

##################################################

default: $(progs)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<
#	$(CXX) $(CXXFLAGS) -c $< 2>&1 | ./c++filter.pl

$(progs): %: %.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $($@_srcs:%.cc=%.o) $($@_libs)

.PHONY: clean
clean:
	rm -f $(progs) $(objs) $(extra_clean) 

.PHONY: cleaner
cleaner: clean
	rm -f *~ dep

dep:
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $(sort $(srcs)) > dep

include dep

