SRCS := main.c util.c
OBJS := $(patsubst %.c,%.o,$(SRCS))
SRC_DIR := src
LDFLAGS := -ltoml
APP_NAME := dotmod

INSTALL_DIR := /usr/bin

.PHONY: all
all: $(APP_NAME) install clean

$(APP_NAME): $(SRCS)
	gcc -c $(SRCS)
	gcc $(OBJS) -o $(APP_NAME) $(LDFLAGS)

.PHONY: install
install:
	sudo mv $(APP_NAME) $(INSTALL_DIR)/

.PHONY: clean
clean:
	rm -f $(OBJS)

.PHONY: uninstall
uninstall:
	sudo rm -f $(INSTALL_DIR)/$(APP_NAME)
