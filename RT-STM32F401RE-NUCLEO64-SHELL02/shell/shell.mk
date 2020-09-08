# Shell PATH
SHELLPATH = ./shell

# RT Shell files.
SHELLSRC = $(SHELLPATH)/shell.c \
           $(SHELLPATH)/shell_cmd.c

SHELLINC = $(SHELLPATH)

# Shared variables
ALLCSRC += $(SHELLSRC)
ALLINC  += $(SHELLINC)
