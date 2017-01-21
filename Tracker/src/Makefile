IDIR=../include
CC=g++
CFLAGS=-std=c++11 -I$(IDIR)

#Directory to store object files to compile quickly
ODIR=../obj
#Library directory for project (non-system libs)
LDIR=../lib
#Include libraries e.g. -lncurses
LIBS=-lncurses

#Header files that need to be tracked//////////////////////////////////////////
_DEPS = cmdbar.h instedtr.h instrument.h pattern.h patternedtr.h song.h tables.h daemoncomm.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#Object files that need to be made, one per each cpp///////////////////////////
_OBJ = cmdbar.o instedtr.o instrument.o meta.o pattern.o patternedtr.o song.o daemoncomm.o externinit.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#Creates a .o file for each .cpp
$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
$(ODIR)/%.o: tests/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


#Creates the final executable, linking the .o files
plebtrkraw: $(OBJ) ../obj/main.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

transposeTest: $(OBJ) ../obj/transposeBugFix.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
