.PHONY: all build run clean

all: build

build:
	@mkdir -p build
	@cd build && cmake .. && cmake --build . -j$$(nproc)

run: build
	@echo "Iniciando ST-Tactical..."
	@./build/ST_Tactical

clean:
	@echo "Limpando diretório de build..."
	@rm -rf build
