CC = gcc
CFLAGS = -g3 -Wall -std=c17 $(DEPFLAGS)
DEPFLAGS = -MMD -MP
LDFLAGS = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
LDDEBUGFLAGS = -Wl,--verbose

SRCDIR = src
BASEDIR = $(SRCDIR)/base
BASELIB = $(BASEDIR)/libbase.a

CFILES = $(wildcard $(SRCDIR)/*.c)
OBJFILES = $(CFILES:.c=.o)
DEPFILES = $(OBJFILES:.o=.d)
EXE = program

$(EXE): $(OBJFILES) $(BASELIB)
	$(CC) -o $@ $^ $(EXTRALDFLAGS) $(LDFLAGS) 

$(BASELIB):
	$(MAKE) -C $(BASEDIR)

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRAFLAGS) -c -o $@ $<

memtest: $(EXE)
	valgrind ./program

run: $(EXE)
	./program

clean:
	rm -f $(EXE) $(SRCDIR)/*.o  $(SRCDIR)/*.d
	$(MAKE) -C $(BASEDIR) clean

-include $(DEPFILES)

.PHONY: memtest tests clean $(BASELIB)

