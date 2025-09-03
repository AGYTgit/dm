CC := gcc
SRCS := main.c utils/log.c parse/flags.c parse/commands.c parse/yaml/module.c parse/yaml/config.c dispatcher/dis.c
OBJS := $(patsubst %.c,%.o,$(SRCS))

LDFLAGS := -lyaml
CFLAGS := -g -Wall -Wextra

APP_NAME := dm
INSTALL_DIR := /usr/bin

.PHONY: all
all: $(APP_NAME) install clean

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_NAME): $(OBJS)
	$(CC) $(OBJS) -o $(APP_NAME) $(LDFLAGS)

.PHONY: install
install:
	sudo mv $(APP_NAME) $(INSTALL_DIR)/

.PHONY: clean
clean:
	rm -f $(OBJS) $(APP_NAME)

.PHONY: uninstall
uninstall:
	sudo rm -f $(INSTALL_DIR)/$(APP_NAME)
