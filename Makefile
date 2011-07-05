#############################################################
CC      = gcc
NAME    = vvtbi
OBJDIR	= src
CFLAGS	= -Wall -Werror -O2 -Wextra -pedantic -ansi

#############################################################
#### DO NOT EDIT BELOW THIS LINE ############################

VERSION = 2.0
SOURCES = io.c tokenizer.c vvtbi.c
OBJS    = $(SOURCES:%.c=$(OBJDIR)/%.o)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) src/main.c
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

PHONY: clean

clean :
	@rm -f $(NAME)*

#############################################################
