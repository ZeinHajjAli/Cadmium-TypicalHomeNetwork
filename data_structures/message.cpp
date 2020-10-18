#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "message.hpp"

// Output stream
ostream& operator<<(ostream& os, const Message_t& msg) {
    os << msg.dest << " " << msg.packetType;
    return os;
}

// Input stream
istream& operator>>(istream& is, Message_t& msg) {
    is >> msg.dest;
    is >> msg.packetType;
    return is;
}