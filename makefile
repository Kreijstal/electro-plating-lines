PROGRAM_MULTIMODE = multiproductrewrite
PROGRAM_MULTIROBOT = multi_robot
CPP_FILES = ElectroPlatingLines/multi_product_lib.cpp ElectroPlatingLines/$(PROGRAM_MULTIMODE).cpp
CPP_FILES_MULTIROBOT = ElectroPlatingLines/multi_product_lib.cpp ElectroPlatingLines/$(PROGRAM_MULTIROBOT).cpp
HEADER_FILES = ElectroPlatingLines/ElectroPlatingLines.h ElectroPlatingLines/multi_product_lib.h ElectroPlatingLines/tuple_hash.h ../Hadamard_product_counters/etvo.h
ETVO_OBJECT = ../Hadamard_product_counters/etvo.o

CC = g++
CFLAGS = -I ../Hadamard_product_counters/ -std=c++17

hadamard: $(ETVO_OBJECT)
	@if [ ! -d "../Hadamard_product_counters" ]; then \
		echo "Warning: Hadamard_product_counters directory not found."; \
        else \
                make -C ../Hadamard_product_counters etvo.o; \
        fi

$(PROGRAM_MULTIROBOT): $(CPP_FILES_MULTIROBOT) $(ETVO_OBJECT) $(HEADER_FILES)
	$(CC) $(CFLAGS) $(CPP_FILES_MULTIROBOT) $(ETVO_OBJECT) -o $(PROGRAM_MULTIROBOT)

$(PROGRAM_MULTIMODE): $(CPP_FILES) $(ETVO_OBJECT) $(HEADER_FILES)
	$(CC) $(CFLAGS) $(CPP_FILES) $(ETVO_OBJECT) -o $(PROGRAM_MULTIMODE)

all: $(PROGRAM_MULTIROBOT) $(PROGRAM_MULTIMODE)

clean:
	rm -f $(PROGRAM_MULTIROBOT) $(PROGRAM_MULTIMODE)

cleanhadamard:
	@if [ ! -d "../Hadamard_product_counters" ]; then \
		echo "Warning: Hadamard_product_counters directory not found."; \
	else \
		make -C ../Hadamard_product_counters clean; \
	fi
