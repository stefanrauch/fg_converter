CXX = $(shell root-config --cxx)
LD = $(shell root-config --ld)

OS_NAME:=$(shell uname -s | tr A-Z a-z)
ifeq ($(OS_NAME),darwin)
STDINCDIR := -I/opt/local/include
STDLIBDIR := -L/opt/local/lib
# for boost (temporary)
BOOSTINCDIR := -I/Users/winckler/subversion/FairRoot/FairSoft/include
BOOSTLIBDIR := -L/Users/winckler/subversion/FairRoot/FairSoft/lib	
else
STDINCDIR := 
STDLIBDIR := 
BOOSTINCDIR :=
BOOSTLIBDIR :=
endif

CPPFLAGS := $(shell root-config --cflags) $(STDINCDIR) $(BOOSTINCDIR)
LDFLAGS := $(shell root-config --glibs) $(STDLIBDIR) $(BOOSTLIBDIR)

CPPFLAGS += -g

TARGET = Convert

SRC = Interpolation.cpp LSAParameter.cpp LSAtoFPGA.cpp TXTDATA.cpp Accumulator64B.cpp main.cpp

OBJ = $(SRC:.cpp=.o)

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(LD) $(CPPFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $<

clean :
	rm -f *.o $(TARGET) *~
