CC=cc

INCLUDEDIR=include
BUILDDIR=build

CFLAGS=-std=c99 -Wall -Werror -ggdb -O2 -I$(INCLUDEDIR) -D_DEFAULT_SOURCE

LIBS=-lexif

_OBJ = main.o args.o organize.o
OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

.PHONY: clean

$(BUILDDIR)/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: createbuilddir photo-organizer

photo-organizer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

createbuilddir:
	@mkdir -p $(BUILDDIR)

clean:
	rm -f $(BUILDDIR)/*.o *~ core
