#ifndef _ROUTER_HPP_
#define _ROUTER_HPP_

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
struct Router_defs {
    struct lanIn_in : public in_port<Message_t> {};
    struct routerIn_in : public in_port<Message_t> {};
    struct lanOut_out : public out_port<Message_t> {};
    struct routerOut_out : public out_port<Message_t> {};
};

template<typename TIME> class Router{
    public:
        TIME   preparationTime;

        // Default constructor
        Router() noexcept{
          preparationTime = TIME("00:00:05");
          state.next_internal = std::numeric_limits<TIME>::infinity();
          state.sending = false;
        //   state.sendingLan = true;
        }
        
        // State definition
        struct state_type{
            bool sending;
            bool sendingLan;
            Message_t message;
            TIME next_internal;
        }; 
        state_type state;

        // Ports definition
        using input_ports = std::tuple<typename Router_defs::lanIn_in, typename Router_defs::routerIn_in>;
        using output_ports = std::tuple<typename Router_defs::lanOut_out, typename Router_defs::routerOut_out>;

        // Internal transition
        void internal_transition() {
            if (state.sending) {
                state.sending = false;
                state.next_internal = std::numeric_limits<TIME>::infinity();
            }
        }

        // External transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            if ((get_messages<typename Router_defs::lanIn_in>(mbs).size() + get_messages<typename Router_defs::routerIn_in>(mbs).size()) > 1)
                assert(false && "One message per time unit");
            if (!state.sending) {
                for (const auto &x : get_messages<typename Router_defs::lanIn_in>(mbs)) {
                
                    state.sendingLan = false;
                    state.message = x;
                    state.sending = true;
                    
                    state.next_internal = preparationTime;
                }

                for (const auto &x : get_messages<typename Router_defs::routerIn_in>(mbs)) {
                    state.sendingLan = true;
                    state.message = x;
                    state.sending = true;

                    state.next_internal = preparationTime;
                }
            } else if (state.next_internal != std::numeric_limits<TIME>::infinity()) {
                state.next_internal = state.next_internal - e;
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
                out = state.message;
                if (state.sendingLan) {
                    get_messages<typename Router_defs::lanOut_out>(bags).push_back(out);
                } else {
                    get_messages<typename Router_defs::routerOut_out>(bags).push_back(out);
                }
            }
            return bags;
        }

        // Time advance function
        TIME time_advance() const {  
            return state.next_internal;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Router<TIME>::state_type& i) {
            os << "sending: " << i.sending << " & sendingLan: " << i.sendingLan << " & data: " << i.message.data;
            return os;
        }
};

#endif