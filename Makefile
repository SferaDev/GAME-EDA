# Add here any extra .o player files you want to link to the executable
EXTRA_OBJS = *.player

# Config
OPTIMIZE = 2 # Optimization level (0 to 3)
DEBUG    = 0 # Compile for debugging (0 or 1)
PROFILE  = 1 # Compile for profile (0 or 1)
32BITS   = 0 # Produce 32 bits objects on 64 bits systems (0 or 1)


# Do not edit past this line
############################

# The following two lines will detect all your players (files matching "AI*.cc")

PLAYERS_SRC = $(wildcard AI*.cc)
PLAYERS_OBJ = $(patsubst %.cc, %.o, $(PLAYERS_SRC)) $(EXTRA_OBJS)

# Flags

ifeq ($(strip $(PROFILE)),1)
	PROFILEFLAGS=-pg
endif
ifeq ($(strip $(DEBUG)),1)
	DEBUGFLAGS=-DDEBUG -g -rdynamic
endif
ifeq ($(strip $(32BITS)),1)
	ARCHFLAGS=-m32 -L/usr/lib32
endif

CXXFLAGS = -std=c++11 -Wall -Wno-unused-variable $(ARCHFLAGS) $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE)) -fPIC

LDFLAGS  = -std=c++11 -lm $(ARCHFLAGS) $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE)) -lrt

# Rules

all: Game 

clean:
	rm -rf Game SecGame *.o *.exe Makefile.deps

# Order of objects is important here to deactivate standard sleep function.

Game: Structs.o Settings.o State.o Info.o Random.o Board.o Action.o Player.o Registry.o Game.o Main.o $(PLAYERS_OBJ) Utils.o 
	$(CXX) $^ -o $@ $(LDFLAGS)

SecGame: Structs.o Settings.o State.o Info.o Random.o Board.o Action.o Player.o Registry.o SecGame.o SecMain.o Utils.o 
	$(CXX) $^ -o $@ $(LDFLAGS)

%.exe: %.o Structs.o Settings.o State.o Info.o Random.o Board.o Action.o Player.o Registry.o SecGame.o SecMain.o Utils.o
	$(CXX) $^ -o $@ $(LDFLAGS)

Makefile.deps: *.cc
	$(CXX) $(CXXFLAGS) -MM *.cc > Makefile.deps

include Makefile.deps
