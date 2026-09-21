CXX ?= g++
EMXX ?= em++
CXXFLAGS ?= -std=c++17 -Wall -Wextra
SRC := main.cpp

TARGET = game
WEB_DIR = ../web
WEB_TARGET = $(WEB_DIR)/index.html

RAYLIB_DIR ?=
RAYLIB_CFLAGS ?= $(if $(RAYLIB_DIR),-I$(RAYLIB_DIR)/include,$(shell pkg-config --cflags raylib 2>/dev/null))
RAYLIB_LIBS ?= $(if $(RAYLIB_DIR),-L$(RAYLIB_DIR)/lib -lraylib,$(shell pkg-config --libs raylib 2>/dev/null))
RAYLIB_BIN_DIR ?= /ucrt64/bin
RAYLIB_WEB_DIR ?= ../lib/web
RAYLIB_WEB_INCLUDE ?= $(RAYLIB_WEB_DIR)/include
WEB_RAYLIB_LIBS ?= -L$(RAYLIB_WEB_DIR) -lraylib

.PHONY: all desktop web clean

all: desktop

desktop: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(RAYLIB_CFLAGS) -o $@ $(SRC) $(RAYLIB_LIBS)
	@for dll in libraylib.dll glfw3.dll libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll; do \
		if [ -f "$(RAYLIB_BIN_DIR)/$$dll" ]; then cp "$(RAYLIB_BIN_DIR)/$$dll" .; fi; \
	done

web: $(WEB_TARGET)

$(WEB_TARGET): $(SRC) ../web/shell.html
	@mkdir -p $(WEB_DIR)
	$(EMXX) $(CXXFLAGS) -DPLATFORM_WEB -I$(RAYLIB_WEB_INCLUDE) $(SRC) -o $@ \
		$(WEB_RAYLIB_LIBS) \
		-sUSE_GLFW=3 \
		-sASYNCIFY \
		-sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
		-sALLOW_MEMORY_GROWTH=1 \
		--shell-file ../web/shell.html

clean:
	rm -f $(TARGET)
	rm -rf $(WEB_DIR)/index.html $(WEB_DIR)/index.js $(WEB_DIR)/index.wasm
