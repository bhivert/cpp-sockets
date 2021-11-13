
/*!
@file socket.cpp
@brief ...
*/

#include "nw_socket.hpp"

nw::socket::socket(void) {
}

nw::socket::~socket(void) {
}

const std::string		nw::socket::to_string(void) const {
	return "";
}

std::ostream &	operator<<(std::ostream &o, const nw::socket &C) {
	o << C.to_string();
	return (o);
}
