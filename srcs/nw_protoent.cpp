
/*!
@file nw_protoent.cpp
@brief ...
*/

#include <cstring>

#include "nw_typedef.hpp"
#include "nw_protoent.hpp"

static char	*_str_dup(const char *s) {
	nw::size_type	len = std::strlen(s);
	char		*n = new char [len + 1];
	std::strcpy(n, s);
	return n;
}

static nw::protoent::type *	_protoent_dup(const nw::protoent::type *protoent_ptr) {
	if (!protoent_ptr)
		return nullptr;

	nw::size_type			aliases_count = 0;
	nw::protoent::type	*p = new nw::protoent::type;

	for (char *aliase = *protoent_ptr->p_aliases; aliase; aliase = protoent_ptr->p_aliases[aliases_count]) {
		++aliases_count;
	}
	p->p_name = _str_dup(protoent_ptr->p_name);
	p->p_aliases = new char *[aliases_count + 1];
	for (nw::size_type i = 0; i != aliases_count; ++i) {
		p->p_aliases[i] = _str_dup(protoent_ptr->p_aliases[i]);
	}
	p->p_aliases[aliases_count] = nullptr;
	p->p_proto = protoent_ptr->p_proto;
	return p;
}

static void					_protoent_delete(nw::protoent::type *protoent_ptr) {
	if (!protoent_ptr)
		return ;
	delete [] protoent_ptr->p_name;

	char *alias = *protoent_ptr->p_aliases;
	for (nw::size_type i = 0; alias; alias = protoent_ptr->p_aliases[++i]) {
		delete [] alias;
	}
	delete [] protoent_ptr->p_aliases;
	delete protoent_ptr;
}

nw::protoent::protoent(const std::string &proto_name) : _struct(_protoent_dup(getprotobyname(proto_name.c_str())), &_protoent_delete) {
	endprotoent();
	if (!this->_struct)
		throw logic_error("getprotobyname: protocol not found");
}

nw::protoent::protoent(const proto_id &proto_number) : _struct(_protoent_dup(getprotobynumber(proto_number)), &_protoent_delete) {
	endprotoent();
	if (!this->_struct)
		throw logic_error("getprotobynumber: protocol not found");
}

nw::protoent::protoent(const protoent &src) : _struct(src._struct) {
}

nw::protoent::~protoent(void) {
}

const std::string			nw::protoent::to_string(void) const {
	std::string	str;

	str = "{ \"name\": \"" + std::string(this->_struct->p_name) + "\", ";
	str += "\"number\": " + std::to_string(this->_struct->p_proto) + ", ";
	str += "\"aliases\": [ ";

	char *alias = *this->_struct->p_aliases;
	for (size_type i = 0; alias; alias = this->_struct->p_aliases[++i]) {
		str += "\"" + std::string(alias) + "\"";
		if (this->_struct->p_aliases[i + 1])
			str += ", ";
	}

	str += " ] }";

	return str;
}

std::ostream &				operator<<(std::ostream &o, const nw::protoent &C) {
	o << C.to_string();
	return (o);
}
