CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
message.o: data_structures/message.cpp
	$(CC) -g  -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/message.cpp -o build/message.o

main_personalDevice_test.o: test/main_personalDevice_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_personalDevice_test.cpp -o build/main_personalDevice_test.o

main_router_test.o: test/main_router_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_router_test.cpp -o build/main_router_test.o

main_modem_test.o: test/main_modem_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_modem_test.cpp -o build/main_modem_test.o

tests: main_personalDevice_test.o main_router_test.o main_modem_test.o message.o
	$(CC) -g -o bin/PERSONALDEVICE_TEST build/main_personalDevice_test.o build/message.o
	$(CC) -g -o bin/ROUTER_TEST build/main_router_test.o build/message.o
	$(CC) -g -o bin/MODEM_TEST build/main_modem_test.o build/message.o

#TARGET TO COMPILE ONLY TYPICAL HOME NETWORK (THN) SIMULATOR

#TARGET TO COMPILE EVERYTHING (THN SIMULATOR + TESTS TOGETHER)
all: tests

#CLEAN COMMANDS
clean:
	rm -f bin/* build/* simulation_results/*