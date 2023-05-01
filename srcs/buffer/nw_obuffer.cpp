/*!
@file nw_obuffer.cpp
@brief ...
*/

#include "nw_obuffer.hpp"

nw_obuffer::nw_obuffer(void) {
}

nw_obuffer::~nw_obuffer(void) {
}

const std::string		nw_obuffer::to_string(void) const {
}

std::ostream &	operator<<(std::ostream &o, const nw_obuffer &C) {
	o << C.to_string();
	return (o);
}
