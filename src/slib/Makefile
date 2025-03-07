LIBS = debug

SLIB = slib

all: always
	@for dir in $(LIBS); do \
		make -C $$dir DPATH=$(abspath $(BUILD_DIR)/$(SLIB)); \
		done

always:
	mkdir -p $(BUILD_DIR)/$(SLIB)

clean:
	rm -rf $(BUILD_DIR)/$(SLIB)

.PHONY: all always clean
