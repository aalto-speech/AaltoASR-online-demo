arch = $(shell uname -m)

DECODER_PATH = /home/thirsima/Work/online-demo-libs/$(arch)/decoder
#DECODER_PATH = /home/thirsima/share/decoder-online-demo/src
AKU_PATH = /home/thirsima/Work/online-demo-libs/$(arch)/akumod
#AKU_PATH = /home/thirsima/Work/akumod

OPT = -g -O2
AUX_CXXFLAGS ?= -Wall
INCLUDES = -I$(AKU_PATH) -I$(DECODER_PATH) \
	$(shell paragui-config --cflags) \
	-I/share/puhe/linux/include \
	-I/share/puhe/linux/include/lapackpp
LDFLAGS = -L$(AKU_PATH) -L$(DECODER_PATH) # -static
CXXFLAGS ?= $(AUX_CXXFLAGS) $(INCLUDES) $(OPT)

##################################################

progs = jaakko gui decoder recognizer

default: $(progs)

all: $(progs)

decoder_srcs = decoder.cc Decoder.cc conf.cc msg.cc endian.cc
decoder_libs = -ldecoder
decoder: $(decoder_srcs:%.cc=%.o) $(DECODER_PATH)/libdecoder.a

recognizer_srcs = recognizer.cc conf.cc msg.cc \
	Recognizer.cc Process.cc Adapter.cc
recognizer_libs = -L/share/puhe/x86_64/lib/ \
	-lpthread -lakumod -lfftw3 -lsndfile -llapackpp -llapack
recognizer: $(recognizer_srcs:%.cc=%.o) $(AKU_PATH)/libakumod.a

gui_srcs = gui.cc conf.cc msg.cc Process.cc io.cc endian.cc
gui_libs = -lpthread -lakumod -lsndfile
gui: $(gui_srcs:%.cc=%.o)

jaakko_srcs = jaakko.cc AudioStream.cc Buffer.cc \
	Process.cc msg.cc endian.cc \
	AudioInputController.cc \
	Application.cc Window.cc \
	WindowMain.cc WidgetWave.cc \
	WindowReset.cc WindowRecognizer.cc \
	WidgetRecognitionArea.cc str.cc WidgetRecognitionText.cc \
	RecognitionParser.cc WidgetScrollArea.cc WindowFile.cc WindowMessageBox.cc \
	WidgetSpectrogram.cc WindowChild.cc WindowSettings.cc \
	RecognizerListener.cc WindowStartProcess.cc WindowWaitRecognizer.cc \
	WidgetScrollBar.cc WindowSaveFile.cc WindowOpenFile.cc \
	WidgetTimeAxis.cc WidgetComparisonArea.cc WindowOpenAudioFile.cc \
	WindowSaveAudioFile.cc WindowOpenTextFile.cc WindowSaveTextFile.cc \
	WidgetMultiLineEdit.cc conf.cc RecognizerProcess.cc WidgetAudioView.cc \
	comparison.cc WindowComparison.cc WidgetContainer.cc WindowTextEdit.cc \
	scrap.cc

jaakko_libs = -lportaudio -lsndfile -lparagui -lfreetype -lfftw3
jaakko: $(jaakko_srcs:%.cc=%.o)

srcs = $(decoder_srcs) $(recognizer_srcs) $(gui_srcs) $(jaakko_srcs)
objs = $(srcs:%.cc=%.o)

##################################################

default: $(progs)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< 2>&1 | ./c++filter.pl

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

