#ifndef BOOST_SIMULATION_MESSAGE_HPP
#define BOOST_SIMULATION_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

struct Message_t{
    Message_t(){}

    Message_t(int i_packetType, int i_data) : packetType(i_packetType), data(i_data){}
    
    int packetType;
    int data;
};

istream& operator>>(istream& is, Message_t& msg);

ostream& operator<<(ostream& os, const Message_t& msg);

#endif