CC := gcc
SRCS := main.c utils/log.c parse/flags.c parse/commands.c
# OBJS := $(SRCS:.c=.o)
OBJS := main.o log.o flags.o commands.o
SRC_DIR := src
LDFLAGS := -ltoml
APP_NAME := dm

INSTALL_DIR := /usr/bin

.PHONY: all
all: $(APP_NAME) install clean

$(APP_NAME): $(SRCS)
	$(CC) -c $(SRCS)
	$(CC) $(OBJS) -o $(APP_NAME) $(LDFLAGS)

.PHONY: install
install:
	sudo mv $(APP_NAME) $(INSTALL_DIR)/

.PHONY: clean
clean:
	rm -f $(OBJS)

.PHONY: uninstall
uninstall:
	sudo rm -f $(INSTALL_DIR)/$(APP_NAME)
