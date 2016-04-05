IDIR=../include
CC=g++
CFLAGS=-std=c++11 -I$(IDIR)

#Directory to store object files to compile quickly
ODIR=../obj
#Library directory for project (non-system libs)
LDIR=../lib
#Include libraries e.g. -lncurses
LIBS=

#Header files that need to be tracked//////////////////////////////////////////
_DEPS = generator.h instrument.h interpreter.h pattern.h song.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#Object files that need to be made, one per each cpp///////////////////////////
_OBJ = generator.o instrument.o interpreter.o pattern.o song.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#Creates a .o file for each .cpp
$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

#Creates the final executable, linking the .o files
plebitp: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
