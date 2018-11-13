# makefile for ctomat examples.

EXAMPLES := simple

all:
	@for d in $(EXAMPLES); do \
	    make -C $$d;          \
	done

clean:
	@for d in $(EXAMPLES); do \
	    make -C $$d clean;    \
	done
