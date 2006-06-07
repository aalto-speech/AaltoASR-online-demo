### TÄNNE PITI KORJATA -m EIKÄ -p
ARCH = $(shell uname -m)

#############################
# Opteron cluster
ifeq ($(ARCH),x86_64)
CXX = /usr/bin/g++
OPT = -g
INCLUDES = -I/share/puhe/x86_64/include -I/share/puhe/linux/include
LDFLAGS = 
WARNINGS = -Wall
DEPFLAG = -MM
endif

##############################
# Linux
ifeq ($(ARCH),i686)
CXX = /usr/bin/g++
OPT = -g
##############################
# HUOM!
# JAAKKO TEHNYT LISÄYKSIÄ INCLUDESIIN
##############################3
INCLUDES = -I/usr/include/SDL -I/usr/include/paragui -I/usr/include/freetype2
LDFLAGS =
WARNINGS = -Wall
DEPFLAG = -MM
endif

##################################################

PROGS = testi slave

PROGS_SRCS = $(PROGS:=.cc)

CLASS_SRCS = AudioStream.cc AudioInput.cc FileOutput.cc AudioInputController.cc Process.cc

CLASS_OBJS = $(CLASS_SRCS:.cc=.o)

LIBS = -lportaudio -lSDL -lpthread

ALL_SRCS = $(CLASS_SRCS) $(PROGS_SRCS)
ALL_OBJS = $(ALL_SRCS:.cc=.o)

CXXFLAGS += $(OPT) $(WARNINGS) $(INCLUDES)

##################################################

all: $(PROGS)

objs: $(ALL_OBJS)

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(PROGS) : %: %.o $(CLASS_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@ \
		$(CLASS_OBJS) $(LIBS)

.PHONY: dep
dep:
	rm .depend
	make .depend

.depend:
	$(CXX) -MM $(CXXFLAGS) $(DEPFLAG) $(ALL_SRCS) > .depend
include .depend

.PHONY: clean
clean:
	rm -f $(ALL_OBJS) $(PROGS) .depend *~
	rm -rf html
	cd tests && $(MAKE) clean
