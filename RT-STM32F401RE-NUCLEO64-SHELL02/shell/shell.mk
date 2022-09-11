# RT Shell files.
SHELLDIR = ./shell
SHELLSRC = $(SHELLDIR)/shell.c \
           $(SHELLDIR)/shell_cmd.c

SHELLINC = $(SHELLDIR)

# Shared variables
ALLCSRC += $(SHELLSRC)
ALLINC  += $(SHELLINC)
