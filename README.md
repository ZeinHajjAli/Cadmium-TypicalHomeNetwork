# Typical Home Network

This is a model of a Typical Home Network implemented in Cadmium

## File Organization

- /README.md
- /TypicalHomeNetwork.pdf
- /ModelDescription-TypicalHomeNetwork.xml
- /makefile
- /atomics/ [*This folder contains atomic models implemented in Cadmium*]
    - modem.hpp
	- personalDevice.hpp
	- router.hpp
- /bin/ [*This folder will be created automatically the first time you compile the project. It will contain all the executables*]
- /build/ [*This folder will be created automatically the first time you compile the project. It will contain all the build files (.o) generated during compilation*]
- /data_structures/ [*This folder contains message data structure used in the model*]
    - message.cpp
	- message.hpp
- input_data [*This folder contains all the input data to run the model and the tests*]
	- input_thn_0.txt
	- input_thn_1.txt
	- modem_input_test_modemIn_in.txt
	- MRC_input_test_request_In.txt
	- personalDevice_input_test_response_In.txt
	- personalDevice_input_test_userInput_In.txt
	- router_input_test_lan_In.txt
	- router_input_test_router_In.txt
- simulation_results [*This folder will be created automatically the first time you compile the poject. It will store the outputs from your simulations and tests*]
- test [*This folder contains the unit tests of the atomic models and the MRC coupled model*]
	- main_modem_test.cpp
	- main_MRC_test.cpp
	- main_personalDevice_test.cpp
	- main_router_test.cpp
- top_model [*This folder contains the Typical Home Network top model*]	
	- main.cpp

## Steps to Compile and Run

0. TypicalHomeNetwork.pdf contains the explanation of this model.

1. Update include path in the makefile in this folder. You need to update the following lines:

	`INCLUDECADMIUM=-I ../../cadmium/include
	INCLUDEDESTIMES=-I ../../DESTimes/include`

	Update the relative path to cadmium/include from the folder where the makefile is. You need to take into account where you copied the folder during the installation process.

	Example: 

	`INCLUDECADMIUM=-I ../../cadmium/include`

	Do the same for the DESTimes library.

2. Compile the project and the tests.
	1. Open a terminal in the 'TypicalHomeNetwork' folder.
	2. To compile the project and the tests type:

		`make clean; make all`

3. Run the tests.
	1. Open a terminal in the 'TypicalHomeNetwork/bin' folder.
	2. To run a test, type:
		
		`./NAME_OF_THE_COMPILED_FILE` (For windows, `./NAME_OF_THE_COMPILED_FILE.exe`).
	
		For example, to run the router test, type:

		`./ROUTER_TEST`
	3. To check the output of the test, go to the 'simulation_results' folder and open 'router_test_output_messages.txt' and 'router_test_output_state.txt'.

4. Run the top model.
	1. Open a terminal in the 'bin' folder.
	2. To run the model, type:
	
		`./NAME_OF_THE_COMPILED_FILE PATH_OF_THE_INPUT_FILE`
		
		For this test you need to type:

		`./THN ../input_data/input_thn_0.txt`
	3. To check the output of the model, go to the 'simulation_results' folder and open 'thn_output_messages.txt' and thn_output_state.txt'.
	4. To run the model with different inputs:
		1. Create new '.txt' files with the same structure as input_thn_0.txt in the folder input_data.
		2. Follow the instructions from step 2
		3. If you want to keep the output, rename the output '.txt' files to prevent them from being overwritten.