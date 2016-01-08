TARGET= ..
MODULES= $(patsubst %/,%,$(wildcard */))

all: $(MODULES)

$(MODULES):
	cp -r $@/* $(TARGET)/modules/

.PHONY: all $(MODULES)
