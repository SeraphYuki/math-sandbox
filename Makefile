CC=gcc

LFLAGS = -pg
CFLAGS =  $(shell pkg-config --cflags sdl3) $(shell pkg-config --cflags freetype2) -pg
GLEWLIBS = $(shell pkg-config  --libs glew)
SDLLIBS = $(shell pkg-config --libs sdl3)
LIBS = -lm  $(GLEWLIBS) $(SDLLIBS) -pg

SOURCES=main.c math.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) $(LIBS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o
