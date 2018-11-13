# makefile for ctomat project.

export TOP_DIR := $(shell pwd)

include $(TOP_DIR)/config.mk

.PHONY: all example test test-build clean

all:
	make -C src

example: all
	make -C example

test: test-build
	./test/unit_test -r compact -s $(TAGS)

test-build: all
	make -C test

clean:
	make -C src clean
	make -C example clean
	make -C test clean
