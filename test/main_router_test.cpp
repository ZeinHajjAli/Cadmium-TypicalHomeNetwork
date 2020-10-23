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
#include "../atomics/router.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/

/***** Define output ports for coupled model *****/
struct outp_lanOut : public cadmium::out_port<Message_t>{};
struct outp_routerOut : public cadmium::out_port<Message_t>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_lan : public iestream_input<Message_t, T> {
    public:
        InputReader_lan () = default;
        InputReader_lan (const char* file_path) : iestream_input<Message_t, T>(file_path) {}
};

template<typename T>
class InputReader_router : public iestream_input<Message_t,T> {
    public:
        InputReader_router () = default;
        InputReader_router (const char* file_path) : iestream_input<Message_t, T>(file_path) {}
};

int main(){


    /****** Input Reader atomic models instantiation *******************/
    const char * i_input_data_lanInput = "../input_data/router_input_test_lan_In.txt";
    shared_ptr<dynamic::modeling::model> input_reader_lanInput = dynamic::translate::make_dynamic_atomic_model<InputReader_lan, TIME, const char* >("input_reader_lanInput" , move(i_input_data_lanInput));

    const char * i_input_data_routerInput = "../input_data/router_input_test_router_In.txt";
    shared_ptr<dynamic::modeling::model> input_reader_routerInput = dynamic::translate::make_dynamic_atomic_model<InputReader_router , TIME, const char* >("input_reader_routerInput" , move(i_input_data_routerInput));


    /****** Router atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> router1 = dynamic::translate::make_dynamic_atomic_model<Router, TIME>("router1");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(outp_lanOut), typeid(outp_routerOut)};
    dynamic::modeling::Models submodels_TOP = {input_reader_lanInput, input_reader_routerInput, router1};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<Router_defs::lanOut_out, outp_lanOut>("router1"),
        dynamic::translate::make_EOC<Router_defs::routerOut_out, outp_routerOut>("router1"),
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out, Router_defs::lanIn_in>("input_reader_lanInput","router1"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out, Router_defs::routerIn_in>("input_reader_routerInput","router1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/router_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/router_test_output_state.txt");
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
    r.run_until(NDTime("04:00:00:000"));
    return 0;
}
