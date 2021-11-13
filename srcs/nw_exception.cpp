
/*!
@file nw_exception.cpp
@brief ...
*/

#include "nw_exception.hpp"

std::ostream &	operator<<(std::ostream &o, const nw::exception &C) {
	o << C.what();
	return (o);
}
