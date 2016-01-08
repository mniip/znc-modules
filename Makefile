TARGET= ..
MODULES= $(patsubst %/,%,$(wildcard */))

all: $(MODULES)

$(MODULES):
	for file in $@/*; do [ "$${f##*.}" = md ] && continue; cp -r "$$file" $(TARGET)/modules/; done

.PHONY: all $(MODULES)
