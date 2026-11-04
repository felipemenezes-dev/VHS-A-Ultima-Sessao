COMPILADOR = gcc
OPCOES_C = -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -O2
RAYLIB ?= $(HOME)/.local
INCLUSOES = -Isrc -I$(RAYLIB)/include
BIBLIOTECAS = -L$(RAYLIB)/lib -Wl,-rpath,'$(abspath $(RAYLIB))/lib' -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
FONTES = $(wildcard src/*.c)
CABECALHOS = $(wildcard src/*.h)

.PHONY: compilar executar run limpar

compilar: build/vhs

build/vhs: $(FONTES) $(CABECALHOS)
	mkdir -p build
	$(COMPILADOR) $(OPCOES_C) $(INCLUSOES) $(FONTES) $(BIBLIOTECAS) -o $@

executar run: build/vhs
	./build/vhs

limpar:
	rm -f build/vhs
