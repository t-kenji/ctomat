# makefile for ctomat tests.

TEST = unit_test
OBJS = main.o collections.o utils.o

EXTRA_CXXFLAGS += -I$(TOP_DIR)/src
ifneq ($(CATCH2_DIR),)
  EXTRA_CXXFLAGS += -I$(CATCH2_DIR)
endif

include $(TOP_DIR)/rules.mk
