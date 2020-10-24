#ifndef _PERSONALDEVICE_HPP_
#define _PERSONALDEVICE_HPP_

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
struct PersonalDevice_defs {
    struct userInput_in : public in_port<int> {};
    struct response_in : public in_port<Message_t> {};
    struct request_out : public out_port<Message_t> {};
    struct userResponse_out : public out_port<int> {};
};

template<typename TIME> class PersonalDevice{
    public:
        TIME   preparationTime;
        TIME   timeout;

        // Default constructor
        PersonalDevice() noexcept{
          preparationTime = TIME("00:00:05");
          timeout = TIME("00:01:00");
          state.data = -1;
          state.next_internal = std::numeric_limits<TIME>::infinity();
          state.model_active = false;
          state.sending = false;
        }
        
        // State definition
        struct state_type{
            bool ack;
            int data;
            bool sending;
            bool model_active;
            TIME next_internal;
        }; 
        state_type state;

        // Ports definition
        using input_ports = std::tuple<typename PersonalDevice_defs::response_in, typename PersonalDevice_defs::userInput_in>;
        using output_ports = std::tuple<typename PersonalDevice_defs::request_out, typename PersonalDevice_defs::userResponse_out>;

        // Internal transition
        void internal_transition() {
            if (state.ack) {
                state.model_active = false;
                state.next_internal = std::numeric_limits<TIME>::infinity();
            } else if (state.sending) {
                state.sending = false;
                state.model_active = true;
                state.next_internal = timeout;
            } else {
                state.sending = true;
                state.model_active = true;
                state.next_internal = preparationTime;
            }
        }

        // External transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
                if ((get_messages<typename PersonalDevice_defs::userInput_in>(mbs).size() + get_messages<typename PersonalDevice_defs::response_in>(mbs).size()) > 1)
                    assert(false && "One message per time unit");

                for (const auto &x : get_messages<typename PersonalDevice_defs::userInput_in>(mbs)) {
                    if (!state.model_active) {
                        state.data = x;
                        if (state.data >= 0) {
                            state.ack = false;
                            state.sending = true;
                            state.model_active = true;
                            state.next_internal = preparationTime;
                        } else if (state.next_internal != std::numeric_limits<TIME>::infinity()) {
                            state.next_internal = state.next_internal - e;
                        }
                    } else if (state.next_internal != std::numeric_limits<TIME>::infinity()) {
                        state.next_internal = state.next_internal - e;
                    }
                }

                for (const auto &x : get_messages<typename PersonalDevice_defs::response_in>(mbs)) {
                    if (state.model_active) {
                        if (x.packetType == 0 && state.data == x.data) {
                            state.ack = true;
                            state.sending = false;
                            state.next_internal = TIME("00:00:00");
                        } else if (state.next_internal != std::numeric_limits<TIME>::infinity()) {
                            state.next_internal = state.next_internal - e;
                        }
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
                out.packetType = 1;
                out.data = state.data;
                get_messages<typename PersonalDevice_defs::request_out>(bags).push_back(out);
            } else if (state.ack) {
                get_messages<typename PersonalDevice_defs::userResponse_out>(bags).push_back(state.data);
            }

            return bags;
        }

        // Time advance function
        TIME time_advance() const {  
            return state.next_internal;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename PersonalDevice<TIME>::state_type& i) {
            os << "sending: " << i.sending << " & data: " << i.data << " & ack: " << i.ack;
            return os;
        }
};

#endif