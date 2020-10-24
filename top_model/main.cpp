//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Messages structures
#include "../data_structures/message.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/modem.hpp"
#include "../atomics/personalDevice.hpp"
#include "../atomics/router.hpp"

//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>


using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct inp_request : public in_port<Message_t>{};
struct inp_userInput : public in_port<int>{};

/***** Define output ports for coupled model *****/
struct outp_response : public out_port<Message_t>{};
struct outp_inetSent : public out_port<Message_t>{};
struct outp_userResponse : public out_port<int>{};
struct outp_inetSent_top : public out_port<Message_t>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<int,T> {
public:
    InputReader_Int() = default;
    InputReader_Int(const char* file_path) : iestream_input<int,T>(file_path) {}
};

int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1; 
    }
    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader" , move(i_input));

    /****** PersonalDevice atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> personalDevice1 = dynamic::translate::make_dynamic_atomic_model<PersonalDevice, TIME>("personalDevice1");

    /****** Router atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> router1 = dynamic::translate::make_dynamic_atomic_model<Router, TIME>("router1");

    /****** Modem atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> modem1 = dynamic::translate::make_dynamic_atomic_model<Modem, TIME>("modem1");

    /*******ModemRouterCombo (MRC) COUPLED MODEL********/
    dynamic::modeling::Ports iports_MRC = {typeid(inp_request)};
    dynamic::modeling::Ports oports_MRC = {typeid(outp_response),typeid(outp_inetSent)};
    dynamic::modeling::Models submodels_MRC = {router1, modem1};
    dynamic::modeling::EICs eics_MRC = {
        dynamic::translate::make_EIC<inp_request, Router_defs::lanIn_in>("router1")
    };
    dynamic::modeling::EOCs eocs_MRC = {
        dynamic::translate::make_EOC<Router_defs::lanOut_out,outp_response>("router1"),
        dynamic::translate::make_EOC<Modem_defs::modemOut2_out,outp_inetSent>("modem1")
    };
    dynamic::modeling::ICs ics_MRC = {
        dynamic::translate::make_IC<Router_defs::routerOut_out, Modem_defs::modemIn_in>("router1","modem1"),
        dynamic::translate::make_IC<Modem_defs:: modemOut1_out, Router_defs::routerIn_in>("modem1","router1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> MRC;
    MRC = make_shared<dynamic::modeling::coupled<TIME>>(
        "MRC", submodels_MRC, iports_MRC, oports_MRC, eics_MRC, eocs_MRC, ics_MRC 
    );

    /*******TypicalHomeNetwork (THN) SIMULATOR COUPLED MODEL********/
    dynamic::modeling::Ports iports_THN = {typeid(inp_userInput)};
    dynamic::modeling::Ports oports_THN = {typeid(outp_userResponse),typeid(outp_inetSent_top)};
    dynamic::modeling::Models submodels_THN = {personalDevice1, MRC};
    dynamic::modeling::EICs eics_THN = {
        cadmium::dynamic::translate::make_EIC<inp_userInput, PersonalDevice_defs::userInput_in>("personalDevice1")
    };
    dynamic::modeling::EOCs eocs_THN = {
        dynamic::translate::make_EOC<PersonalDevice_defs::userResponse_out,outp_userResponse>("personalDevice1"),
        dynamic::translate::make_EOC<outp_inetSent,outp_inetSent_top>("MRC")
    };
    dynamic::modeling::ICs ics_THN = {
        dynamic::translate::make_IC<PersonalDevice_defs::request_out, inp_request>("personalDevice1","MRC"),
        dynamic::translate::make_IC<outp_response, PersonalDevice_defs::response_in>("MRC","personalDevice1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> THN;
    THN = make_shared<dynamic::modeling::coupled<TIME>>(
        "THN", submodels_THN, iports_THN, oports_THN, eics_THN, eocs_THN, ics_THN 
    );


    /*******TOP COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(outp_inetSent_top),typeid(outp_userResponse)};
    dynamic::modeling::Models submodels_TOP = {input_reader, THN};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<outp_inetSent_top,outp_inetSent_top>("THN"),
        dynamic::translate::make_EOC<outp_userResponse,outp_userResponse>("THN")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<int>::out, inp_userInput>("input_reader","THN")
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/THN_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/THN_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/ 
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until_passivate();
    return 0;
}