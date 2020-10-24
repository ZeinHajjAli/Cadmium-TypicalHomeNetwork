#ifndef _MODEM_HPP_
#define _MDOEM_HPP_

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

// Port definition
struct Modem_defs {
    struct modemIn_in : public in_port<Message_t> {};
    struct modemOut1_out : public out_port<Message_t> {};
    struct modemOut2_out : public out_port<Message_t> {};
};

template<typename TIME> class Modem{
    public:
        TIME timeout;

        // Default constructor
        Modem() noexcept{
          timeout = TIME("00:00:30");
          state.next_internal = std::numeric_limits<TIME>::infinity();
          state.model_active = false;
          state.sending = false;
        }
        
        // State definition
        struct state_type{
            bool sending;
            bool model_active;
            TIME next_internal;
            Message_t message;
        }; 
        state_type state;

        // Ports definition
        using input_ports = std::tuple<typename Modem_defs::modemIn_in>;
        using output_ports = std::tuple<typename Modem_defs::modemOut1_out, typename Modem_defs::modemOut2_out>;

        // Internal transition
        void internal_transition() {
            state.model_active = false;
            state.sending = false;
            state.next_internal = std::numeric_limits<TIME>::infinity();
        }

        // External transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            int randWaitTime;
            string timeString;
            TIME waitTime;

            if (get_messages<typename Modem_defs::modemIn_in>(mbs).size() > 1)
                assert(false && "One message per time unit");

            for (const auto &x : get_messages<typename Modem_defs::modemIn_in>(mbs)) {
                if (!state.model_active) {
                    state.message = x;
                    randWaitTime = rand() % 45 + 5;
                    string timeString = "00:00:" + to_string(randWaitTime);
                    waitTime = TIME(timeString);
                    state.model_active = true;

                    if (waitTime <= timeout) {
                        state.sending = true;
                        state.next_internal = waitTime;
                    } else {
                        state.sending = false;
                        state.next_internal = timeout;
                    }
                        
                } else if (state.next_internal != std::numeric_limits<TIME>::infinity()) {
                    state.next_internal = state.next_internal - e;
                }
            }
        }

        // Confluence transition
        void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
            internal_transition();
            external_transition(TIME(), std::move(mbs));
        }

        // Output function
        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bags;
            Message_t out;
            if (state.sending) {
                out.packetType = 0;
                out.data = state.message.data;
                get_messages<typename Modem_defs::modemOut1_out>(bags).push_back(out);
                out = state.message;
                get_messages<typename Modem_defs::modemOut2_out>(bags).push_back(out);
            }  

            return bags;
        }

        // Time advance function
        TIME time_advance() const {  
            return state.next_internal;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Modem<TIME>::state_type& i) {
            os << "model_active: " << i.model_active << " & sending: " << i.sending << " & message: " << i.message;
            return os;
        }
};

#endif