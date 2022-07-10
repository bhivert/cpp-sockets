/*!
@file nw_sockopt.cpp
@brief ...
*/

#include "nw_sockopt.hpp"

nw::sockopt::sockopt(void) {
}

nw::sockopt::~sockopt(void) {
}

const std::string		nw::sockopt::to_string(void) const {
}

std::ostream &	operator<<(std::ostream &o, const nw::sockopt &C) {
	o << C.to_string();
	return (o);
}
