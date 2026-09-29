CC = gcc
CFLAGS = -g3 -Wall -std=c23 $(DEPFLAGS)
DEPFLAGS = -MMD -MP
LDFLAGS =
LDLIBS = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm 
SANFLAGS = -fsanitize=address,undefined
LDDEBUGFLAGS = -Wl,--verbose

EXEDEF = program
EXESAN = program_san

SRCDIR = src
BUILDDIR = build
BUILDDIRDEF = $(BUILDDIR)/default
BUILDDIRSAN = $(BUILDDIR)/sanitized
BASEDIR = $(SRCDIR)/base
BASELIBDEF = $(BASEDIR)/libbase.a
BASELIBSAN = $(BASEDIR)/libbasesan.a

CFILES = $(wildcard $(SRCDIR)/*.c)
OBJFILES = $(CFILES:$(SRCDIR)/%.c=%.o)
OBJFILESDEF = $(addprefix $(BUILDDIRDEF)/,$(OBJFILES))
OBJFILESSAN = $(addprefix $(BUILDDIRSAN)/,$(OBJFILES))
DEPFILESDEF = $(OBJFILESDEF:.o=.d)
DEPFILESSAN = $(OBJFILESSAN:.o=.d)

all: $(EXEDEF) $(EXESAN)

$(EXEDEF): $(OBJFILESDEF) $(BASELIBDEF)
	$(CC) $(LDFLAGS) $(EXTRALDFLAGS) $^ $(LDLIBS) -o $@

$(EXESAN): $(OBJFILESSAN) $(BASELIBSAN)
	$(CC) $(LDFLAGS) $(SANFLAGS) $(EXTRALDFLAGS) $^ $(LDLIBS) -o $@

$(BASELIBDEF) $(BASELIBSAN):
	$(MAKE) -C $(BASEDIR) $($@:$(BASEDIR)/%=%)

$(BUILDDIRDEF)/%.o: $(SRCDIR)/%.c | $(BUILDDIRDEF)
	$(CC) $(CFLAGS) $(EXTRAFLAGS) -c -o $@ $<

$(BUILDDIRSAN)/%.o: $(SRCDIR)/%.c | $(BUILDDIRSAN)
	$(CC) $(CFLAGS) $(SANFLAGS) $(EXTRAFLAGS) -c -o $@ $<

$(BUILDDIRDEF) $(BUILDDIRSAN):
	mkdir -p $@

run: $(EXEDEF)
	./$(EXEDEF)

run_san: $(EXESAN)
	./$(EXESAN)

clean:
	rm -rf $(EXEDEF) $(EXESAN) $(BUILDDIR)
	$(MAKE) -C $(BASEDIR) clean

-include $(DEPFILES)

.PHONY: all run run_san clean $(BASELIB)

