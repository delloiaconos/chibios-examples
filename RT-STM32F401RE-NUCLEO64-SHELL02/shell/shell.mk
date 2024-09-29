# RT Shell files.
SHELLSRC = ./shell/shell.c \
           ./shell/shell_cmd.c

SHELLINC = ./shell

# Shared variables
ALLCSRC += $(SHELLSRC)
ALLINC  += $(SHELLINC)
