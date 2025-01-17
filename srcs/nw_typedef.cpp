
/*!
@file nw_typedef.cpp
@brief ...
*/

#include <map>

#include "nw_typedef.hpp"

static const std::map<const nw::sa_family, const std::string>	s_family_str = {
	{nw::sa_family::UNSPEC,		"AF_UNSPEC"},
	{nw::sa_family::INET,		"AF_INET"},
	{nw::sa_family::INET6,		"AF_INET6"}
};

const std::string &	nw::sa_family_str(const nw::sa_family &family) {
	return s_family_str.at(family);
}

static const std::map<const nw::sock_type, const std::string>	s_type_str = {
	{nw::sock_type::UNSPEC,		"UNSPEC"},
	{nw::sock_type::STREAM,		"STREAM"},
	{nw::sock_type::DGRAM,		"DGRAM"},
	{nw::sock_type::SEQPACKET,	"SEQPACKET"},
	{nw::sock_type::RAW,		"RAW"},
	{nw::sock_type::RDM,		"RDM"}
};

const std::string &	nw::sock_type_str(const nw::sock_type &type) {
	return s_type_str.at(type);
}

