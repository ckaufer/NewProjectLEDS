OUTDIR = Output
EMBUILDER ?=
SCHEMAFILE = $(APPNAME).ems
MAIN = $(APPNAME)-Prog
BINFILE = $(OUTDIR)/$(MAIN).bin
HEXFILE = $(OUTDIR)/$(MAIN).hex
OUTFILE = $(OUTDIR)/$(MAIN).out
OBJECTS = $(OUTDIR)/$(MAIN).obj $(OUTDIR)/$(APPNAME).obj $(OUTDIR)/Hal.obj 

CC = $(TOOLS)/$(GCCARCH)-gcc
LD = $(TOOLS)/$(GCCARCH)-ld
OBJCOPY = $(TOOLS)/$(GCCARCH)-objcopy
SIZE = $(TOOLS)/$(GCCARCH)-size

CFLAGS = -std=gnu99 -O2 -w -ffunction-sections -fdata-sections -fpack-struct=1 -fno-strict-aliasing -fomit-frame-pointer -c -g -I$(PLATFORM)/Hal -IEm $(COPTS)

load: out-check
	$(EXEC)

build: $(OUTDIR) out-remove $(OUTFILE)

$(OUTDIR):
ifeq (,$(findstring Windows,$(OS)))
	mkdir $(OUTDIR)
else
	cmd /c mkdir $(OUTDIR)
endif

$(OUTDIR)/$(MAIN).obj: $(MAIN).c Em/$(APPNAME).c
	$(CC) $< -o $@ $(CFLAGS) 

$(OUTDIR)/$(APPNAME).obj: Em/$(APPNAME).c
	$(CC) $< -o $@ $(CFLAGS) 

$(OUTDIR)/Hal.obj: $(PLATFORM)/Hal/Hal.c
	$(CC) $< -o $@ $(CFLAGS) 

Em/$(APPNAME).c: $(SCHEMAFILE)
ifneq (,$(EMBUILDER))
	$(EMBUILDER) -v --root=$(<D) --outdir=Em --jsondir=Em $<
else
	@echo terminating because of prior schema errors 1>&2
	@exit 1
endif

local-clean:
ifeq (,$(findstring Windows,$(OS)))
	rm -rf $(OUTDIR)
else
ifneq (,$(wildcard $(OUTDIR)))
	cmd /c rmdir /q /s $(subst /,\,$(OUTDIR))
endif
endif

clean: local-clean
ifeq (,$(findstring Windows,$(OS)))
	rm -rf $(EM)
else
ifneq (,$(wildcard Em))
	cmd /c rmdir /q /s $(subst /,\,Em)
endif
endif

out-check:
ifeq (,$(wildcard $(OUTFILE)))
	@echo error: $(OUTFILE): No such file or directory 1>&2
	@exit 1
endif

out-remove:
ifeq (,$(findstring Windows,$(OS)))
	rm -f $(OUTFILE)
else
ifneq (,$(wildcard $(OUTFILE)))
	cmd /c del /q $(subst /,\,$(OUTFILE))
endif
endif

.PHONY: all load clean local-clean out-check
