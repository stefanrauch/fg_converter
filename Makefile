CXX = $(shell root-config --cxx)
LD = $(shell root-config --ld)

OS_NAME:=$(shell uname -s | tr A-Z a-z)
ifeq ($(OS_NAME),darwin)
STDINCDIR := -I/opt/local/include
STDLIBDIR := -L/opt/local/lib
else
STDINCDIR := 
STDLIBDIR := 
endif

#ROOFITINCDIR1 := -I$(ROOTSYS)/roofit/roofitcore/inc
#ROOFITINCDIR2 := -I$(ROOTSYS)/roofit/roofit/inc
#$(ROOFITINCDIR1) $(ROOFITINCDIR2)

CPPFLAGS := $(shell root-config --cflags) $(STDINCDIR) -I/Users/winckler/subversion/FairRoot/FairSoft/include
LDFLAGS := $(shell root-config --glibs) $(STDLIBDIR) -lRooFit -lRooFitCore -lMinuit -lFoam -lRooStats -L/Users/winckler/subversion/FairRoot/FairSoft/lib

CPPFLAGS += -g

TARGET = Analysis

#SRC = Myfunction.cpp osc2012_tools.cpp Analysis.cpp main.cpp
#SRC = Interpolation.cpp PrepareLSAinput.cpp TXTDATA.cpp Accumulator64B.cpp main.cpp
SRC = Interpolation.cpp LSAParameter.cpp LSAtoFPGA.cpp TXTDATA.cpp Accumulator64B.cpp main.cpp
#SRC = LSAParameter.cpp main.cpp

OBJ = $(SRC:.cpp=.o)

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(LD) $(CPPFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $<

clean :
	rm -f *.o $(TARGET) *~
