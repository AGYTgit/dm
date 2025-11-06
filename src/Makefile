CC := gcc
SRCS := main.c log/util.c file/util.c command/core.c command/parse.c flag/core.c flag/parse.c dmConfig/core.c dmConfig/parseYaml.c dmConfig/emitYaml.c dungeonModule/core.c dungeonModule/util.c dungeonModule/parseYaml.c dungeonConf/core.c dungeonConf/parseYaml.c exPaths/core.c funcDispatcher/core.c funcDispatcher/util.c
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
