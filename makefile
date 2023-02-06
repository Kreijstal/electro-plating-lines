PROGRAM = multiproductrewrite
CPP_FILE = ElectroPlatingLines/$(PROGRAM).cpp
ETVO_OBJECT = ../Hadamard_product_counters/etvo.o

CC = g++
CFLAGS = -I ../Hadamard_product_counters/ -std=c++17

$(ETVO_OBJECT):
	@if [ ! -d "../Hadamard_product_counters" ]; then \
		echo "Warning: Hadamard_product_counters directory not found."; \
        else \
                make -C ../Hadamard_product_counters etvo.o; \
        fi

all: $(CPP_FILE) $(ETVO_OBJECT)
	$(CC) $(CFLAGS) $(CPP_FILE) $(ETVO_OBJECT) -o $(PROGRAM)

clean:
	rm -f $(PROGRAM)

cleanhadamard:
	@if [ ! -d "../Hadamard_product_counters" ]; then \
		echo "Warning: Hadamard_product_counters directory not found."; \
	else \
		make -C ../Hadamard_product_counters clean; \
	fi
