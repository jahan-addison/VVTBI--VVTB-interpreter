#############################################################
CC      = gcc
NAME    = vvtbi
VERSION = 2.0
BINDIR	= bin
OBJDIR	= src
CFLAGS	= -Wall -Werror -O2 -Wextra -pedantic -ansi

SOURCES = io.c tokenizer.c vvtbi.c
OBJS    = $(SOURCES:%.c=$(OBJDIR)/%.o)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(BINDIR)/$(NAME) src/main.c
	@rm -f $(OBJS);
	@echo ""
	@echo "**************************************************"
	@echo " Vvtbi: \"Very, Very, Tiny, Basic\" Interpreter"
	@echo " Version: "$(VERSION)
	@echo " Vvtbi (the \"software\") is distributed under"
	@echo " the terms of the Apache License, Version 2.0."
	@echo " For more information, see README and CHANGELOG."
	@echo "   <jahan.addison[at]jacata[dot]me>"
	@echo "**************************************************"

$(OBJS): $(OBJDIR)/%.o : src/%.c $(BINDIR) $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@

#############################################################
