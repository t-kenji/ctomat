# configure for ctomat
export

# project informations.
NAME := ctomat
MAJOR_VERSION := 0
MINOR_VERSION := 0
REVISION := 1
VERSION := $(MAJOR_VERSION).$(MINOR_VERSION).$(REVISION)
CROSS_COMPILE ?=

# dependencies.
CATCH2_DIR ?=

# debug options.
NODEBUG ?= 0
ENABLE_SANITIZER ?= 1
DISABLE_CCACHE ?= 0

# compile & link options.
EXTRA_CPPFLAGS ?=
EXTRA_CFLAGS ?=
EXTRA_CXXFLAGS ?=
EXTRA_LDFLAGS ?=
EXTRA_LIBS ?=

# verbose options.
V ?= 0
