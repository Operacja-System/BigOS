NAME := debug

TPATH := $(DPATH)/$(NAME)
BPATH := $(DPATH)/$(NAME)/obj

LDFLAGS := -r -o $(TPATH)/$(NAME).o

SRC_C := $(shell find . -name '*.c')
SRC_S := $(shell find . -name '*.s')
OBJ_C := $(patsubst %.c, $(BPATH)/%_C.o, $(SRC_C))
OBJ_S := $(patsubst %.s, $(BPATH)/%_S.o, $(SRC_S))

all: $(NAME)

always:
	mkdir -p $(BPATH)
	mkdir -p $(TPATH)

clean:
	rm -rf $(BPATH)
	rm -rf $(TPATH)

$(NAME): $(OBJ_C) $(OBJ_S)
	$(LD) $(LDFLAGS) $(OBJ_S) $(OBJ_C)

$(BPATH)/%_C.o : %.c always
	$(CC) $(CFLAGS) -c $< -o $@

$(BPATH)/%_S.o : %.s always
	$(AS) $(AFLAGS) -c $< -o $@

.PHONY: all clean always
