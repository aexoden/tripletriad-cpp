CXXFLAGS += -std=gnu++0x -pedantic -Wall -Wextra -Wwrite-strings -ggdb3

SDL_CXXFLAGS := `sdl-config --cflags`
SDL_LDFLAGS := `sdl-config --libs` -lSDL_gfx

ifndef VERBOSE
	QUIET_CXX = @echo '  CXX '$@;
	QUIET_LD  = @echo '  LD  '$@;
endif

BINARY := src/tripletriad

OBJECTS := \
	src/card.o \
	src/game_board.o \
	src/move.o \
	src/player.o \
	src/square.o \
	src/transposition_table.o \
	src/tripletriad.o

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(QUIET_LD)$(CXX) $(LDFLAGS) $(SDL_LDFLAGS) $(OBJECTS) -o $@

src/%.o : src/%.cc
	$(QUIET_CXX)$(CXX) $(CXXFLAGS) $(SDL_CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJECTS) $(BINARY)
