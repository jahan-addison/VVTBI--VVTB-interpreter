CC      = gcc
NAME    = vvtbi
VERSION = 1.2
BINDIR	= release
OBJDIR	= src
CFLAGS	= -Wall -Werror -O2 -Wextra -pedantic -ansi

SOURCES = io.c tokenizer.c vvtbi.c
OBJS    = $(SOURCES:%.c=$(OBJDIR)/%.o)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BINDIR)/$(NAME) src/main.c
	@rm -rf $(OBJS);
	@echo ""
	@echo "**************************************************"
	@echo " Vvtbi: Very, Very, Tiny, Basic Interpreter"
	@echo " Version: "$(VERSION)
	@echo " This \"Work\" is distributed under the terms of"
	@echo " the Apache License, Version 2.0."
	@echo " For more information, check README and CHANGELOG"
	@echo "    <jahan.addison@jacata.me>"
	@echo "**************************************************"

$(OBJS): $(OBJDIR)/%.o : src/%.c $(BINDIR) $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean :
	@rm -rf $(BINDIR)/vvtbi*
