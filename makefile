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

#TARGET TO COMPILE ONLY TYPICAL HOME NETWORK (THN) SIMULATOR

#TARGET TO COMPILE EVERYTHING (THN SIMULATOR + TESTS TOGETHER)

#CLEAN COMMANDS
clean:
	rm -f bin/* build/* simulation_results/*