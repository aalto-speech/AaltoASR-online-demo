arch = $(shell uname -m)

DECODER_PATH = /home/thirsima/Work/online-demo-libs/$(arch)/decoder
AKU_PATH = /home/thirsima/Work/online-demo-libs/$(arch)/akumod

OPT = -g -O2
AUX_CXXFLAGS ?= \
\
-Wno-redundant-decls \
-fsyntax-only \
-Wno-aggregate-return \
-Wno-cast-align \
-Wno-cast-qual \
-Wno-char-subscripts \
-Wno-comment \
-Wno-conversion \
-Wno-deprecated-declarations \
-Wno-disabled-optimization \
-Wno-error \
-Wno-float-equal \
-Wno-format \
-Wno-format=2 \
-Wno-format-nonliteral \
-Wno-format-security \
-Wno-implicit \
-Wno-import \
-Wno-inline \
-Wno-endif-labels \
-Wno-long-long \
-Wno-missing-braces \
-Wno-missing-format-attribute \
-Wno-missing-noreturn \
-Wno-multichar \
-Wno-format-extra-args \
-Wno-format-y2k \
-Wno-import \
-Wno-packed \
-Wno-padded \
-Wno-parentheses \
-Wno-pointer-arith \
-Wno-return-type \
-Wno-shadow \
-Wno-sign-compare \
-Wno-strict-aliasing \
-Wno-switch \
-Wno-switch-default \
-Wno-switch-enum \
-Wno-system-headers \
-Wno-trigraphs \
-Wno-undef \
-Wno-uninitialized \
-Wno-unknown-pragmas \
-Wno-unreachable-code \
-Wno-unused \
-Wno-unused-function \
-Wno-unused-label \
-Wno-unused-parameter \
-Wno-unused-value \
-Wno-unused-variable \
-Wno-write-strings \
\
\
-Wno-aggregate-return \
-Wno-cast-align \
-Wno-cast-qual \
-Wno-char-subscripts \
-Wno-comment \
-Wno-conversion \
-Wno-deprecated-declarations \
-Wdisabled-optimization \
-Wno-float-equal \
-Wno-format \
-Wno-format=2 \
-Wno-format-nonliteral \
-Wno-format-security \
-Wno-implicit \
-Wno-import \
-Wno-inline \
-Wno-endif-labels \
-Wno-missing-braces  \
-Wno-missing-noreturn \
-Wno-multichar \
-Wno-format-extra-args \
-Wno-format-y2k \
-Wno-import \
-Wno-packed \
-Wno-padded \
-Wno-parentheses \
-Wno-pointer-arith   \
-Wno-redundant-decls \
-Wno-return-type \
-Wno-shadow \
-Wno-sign-compare \
-Wno-strict-aliasing \
-Wno-switch \
-Wno-switch-default \
-Wno-switch-enum \
-Wno-system-headers \
-Wno-trigraphs \
-Wno-undef \
-Wno-uninitialized \
-Wno-unknown-pragmas \
-Wno-unreachable-code \
-Wno-unused \
-Wno-unused-function \
-Wno-unused-label \
-Wno-unused-parameter \
-Wno-unused-value \
-Wno-unused-variable \
-Wno-write-strings \
\
-Wno-char-subscripts \
-Wno-comment \
-Wno-format \
-Wno-implicit \
-Wno-missing-braces \
-Wno-parentheses \
-Wno-return-type \
-Wno-switch \
-Wno-switch-default \
-Wno-switch-enum \
-Wno-trigraphs \
-Wno-unused-function \
-Wno-unused-label \
-Wno-unused-parameter \
-Wno-unused-variable \
-Wno-unused-value \
-Wno-unused \
-Wno-uninitialized \
-Wno-unknown-pragmas \
-Wno-strict-aliasing \
-Wno-system-headers \
-Wno-float-equal \
-Wno-undef \
-Wno-endif-labels \
-Wno-shadow \
-Wno-pointer-arith \
-Wno-cast-qual \
-Wno-cast-align \
-Wno-write-strings \
-Wno-conversion \
-Wno-sign-compare \
-Wno-aggregate-return \
-Wno-missing-prototypes \
-Wno-missing-noreturn \
-Wno-missing-format-attribute \
-Wno-deprecated-declarations \
-Wno-packed \
-Wno-padded \
-Wno-redundant-decls \
-Wno-unreachable-code \
-Wno-inline \
-Wno-long-long


#-Wall -Wno-system-headers #-Wno-redundant-decls
INCLUDES = -I$(AKU_PATH) -I$(DECODER_PATH) -I/usr/include/SDL \
	-I/usr/include/paragui -I/usr/include/freetype2 \
	-I/opt/gnome/lib/sigc++-1.2/include -I/opt/gnome/include/sigc++-1.2 \
	-I/share/puhe/linux/include
LDFLAGS = -L$(AKU_PATH) -L$(DECODER_PATH)
CXXFLAGS ?= $(AUX_CXXFLAGS) $(INCLUDES) $(OPT)

##################################################

progs = jaakko gui decoder recognizer

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

jaakko_srcs = jaakko.cc AudioStream.cc Buffer.cc \
	Process.cc msg.cc endian.cc AudioFileInputController.cc \
	AudioInputController.cc AudioLineInputController.cc \
	Application.cc Window.cc \
	WindowMain.cc WindowFileRecognizer.cc WidgetWave.cc \
	WindowReset.cc WindowRecognizer.cc WindowMicrophoneRecognizer.cc \
	WidgetRecognitionArea.cc str.cc WidgetRecognitionTexts.cc Recognition.cc \
	WidgetScrollArea.cc WindowFile.cc WindowMessageBox.cc \
	WidgetSpectrogram.cc WindowChild.cc WindowSettings.cc Settings.cc \
	QueueController.cc WindowStartProcess.cc WindowWaitRecognizer.cc \
	ScrollBar.cc WindowSaveFile.cc
jaakko_libs = -lportaudio -lsndfile -lparagui -lfreetype -lfftw3
jaakko: $(jaakko_srcs:%.cc=%.o)

srcs = $(decoder_srcs) $(recognizer_srcs) $(gui_srcs) $(jaakko_srcs)
objs = $(srcs:%.cc=%.o)

##################################################

default: $(progs)

%.o: %.cc
	$(CXX) -U PACKAGE $(CXXFLAGS) -c  -U PACKAGE $<

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

