PROGRAM = multiproductrewrite
CPP_FILES = ElectroPlatingLines/multi_product_lib.cpp ElectroPlatingLines/$(PROGRAM).cpp
HEADER_FILES = ElectroPlatingLines/ElectroPlatingLines.h ElectroPlatingLines/multi_product_lib.h ElectroPlatingLines/tuple_hash.h ../Hadamard_product_counters/etvo.h
ETVO_OBJECT = ../Hadamard_product_counters/etvo.o

CC = g++
CFLAGS = -I ../Hadamard_product_counters/ -std=c++17

$(ETVO_OBJECT):
	@if [ ! -d "../Hadamard_product_counters" ]; then \
		echo "Warning: Hadamard_product_counters directory not found."; \
        else \
                make -C ../Hadamard_product_counters etvo.o; \
        fi

all: $(CPP_FILES) $(ETVO_OBJECT) $(HEADER_FILES)
	$(CC) $(CFLAGS) $(CPP_FILE) $(CPP_FILES) $(ETVO_OBJECT) -o $(PROGRAM)

clean:
	rm -f $(PROGRAM)

cleanhadamard:
	@if [ ! -d "../Hadamard_product_counters" ]; then \
		echo "Warning: Hadamard_product_counters directory not found."; \
	else \
		make -C ../Hadamard_product_counters clean; \
	fi
