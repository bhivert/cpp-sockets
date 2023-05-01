/*!
@file nw_ibuffer.cpp
@brief ...
*/

#include "nw_ibuffer.hpp"

nw_ibuffer::nw_ibuffer(void) {
}

nw_ibuffer::~nw_ibuffer(void) {
}

const std::string		nw_ibuffer::to_string(void) const {
}

std::ostream &	operator<<(std::ostream &o, const nw_ibuffer &C) {
	o << C.to_string();
	return (o);
}
