#ifndef __NW_ADDRINFO_HPP__
# define __NW_ADDRINFO_HPP__

/*!
@file nw_addrinfo.hpp
@brief ...
*/

# include <ostream>
# include <string>
# include <list>
# include <cstring>
# include <map>

# include <netdb.h>

# include "nw_typedef.hpp"
# include "nw_addr.hpp"
# include "nw_protoent.hpp"

typedef struct addrinfo		addrinfo_struct;

namespace nw {
	template<sa_family FAMILY>
	class addrinfo {
		public:
			addrinfo(const std::string &service, std::nullptr_t) \
				: addrinfo<FAMILY>(sock_type::UNSPEC, nullptr, service.c_str()) {}

			addrinfo(const std::string &service, std::nullptr_t, const sock_type &type) \
				: addrinfo<FAMILY>(type, nullptr, service.c_str(), 0) {}

			addrinfo(const std::string &service, std::nullptr_t, const std::string &proto_name, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo<FAMILY>(type, nullptr, service.c_str(), proto_name) {}

			addrinfo(const std::string &service, std::nullptr_t, const proto_id &proto_number, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo<FAMILY>(type, nullptr, service.c_str(), proto_number) {}

			addrinfo(const std::string &service, std::nullptr_t, const protoent &proto, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo<FAMILY>(type, nullptr, service.c_str(), proto) {}

			addrinfo(const std::string &service, const std::string &node) \
				: addrinfo<FAMILY>(sock_type::UNSPEC, node.c_str(), service.c_str()) {}

			addrinfo(const std::string &service, const std::string &node, const sock_type &type) \
				: addrinfo<FAMILY>(type, node.c_str(), service.c_str(), 0) {}

			addrinfo(const std::string &service, const std::string &node, const std::string &proto_name, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo<FAMILY>(type, node.c_str(), service.c_str(), proto_name) {}

			addrinfo(const std::string &service, const std::string &node, const proto_id &proto_number, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo<FAMILY>(type, node.c_str(), service.c_str(), proto_number) {}

			addrinfo(const std::string &service, const std::string &node, const protoent &proto, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo<FAMILY>(type, node.c_str(), service.c_str(), proto) {}

			virtual	~addrinfo(void) {}

			const std::string	to_string(void) const {
				std::string str;

				str = '[';
				for (typename std::list<addrinfo<FAMILY>::data>::const_iterator it = this->_addrinfo_list.begin(); it != this->_addrinfo_list.end() ; ++it) {
					str += it->to_string();
					if (std::next(it) != this->_addrinfo_list.end())
						str += ", ";
				}
				str += ']';

				return str;
			}

		protected:
			class data {
				public:
					data(const int32_t &flags, const sa_family &family, const sock_type &type, const protoent &proto, const addr<FAMILY> &addr, const std::string &canonname) \
						: _flags(flags), _family(family), _type(type), _proto(proto), _addr(addr), _canonname(canonname) {
					}

					data(const data &src) : _flags(src._flags), _family(src._family), _type(src._type), _proto(src._proto), _addr(src._addr), _canonname(src._canonname) {}
					data(data &&src) : _flags(src._flags), _family(src._family), _type(src._type), _proto(src._proto), _addr(src._addr), _canonname(src._canonname) {}

					const std::string	to_string(void) const {
						std::string str;
						char		flags[5] = {0};

						snprintf(flags, sizeof(flags), "%0#4X", this->_flags);
						str = "{ \"flags\": \"" + std::string(flags) + "\", ";
						str += "\"family\": \"" + sa_family_str(this->_family) + "\", ";
						str += "\"type\": \"" + sock_type_str(this->_type) + "\", ";
						str += "\"protocol\": " + this->_proto.to_string() + ", ";
						str += "\"sockaddr\": " + this->_addr.to_string() + ", ";
						str += "\"canonname\": " + ((this->_canonname == "null") ? this->_canonname : '\"' + this->_canonname + '\"') + " }";

						return str;
					}

					~data(void) {}

				protected:
					const int32_t		_flags;
					const sa_family		_family;
					const sock_type		_type;
					const protoent		_proto;
					const addr<FAMILY>	_addr;
					const std::string	_canonname;

					template <sa_family F>
					friend class addrinfo;

				private:
					data(void) = delete;

					data &	operator=(const data &src) = delete;
					data &	operator=(data &&src) = delete;
			};

			typedef addrinfo_struct		type;

			const std::list<data>		_addrinfo_list;

			addrinfo(const sock_type &type, const char *node = nullptr, const char *service = nullptr, const protoent &proto = 0) {
				uint8_t	flags;

				flags = (node) ? AI_CANONNAME : AI_PASSIVE;
				flags |= (FAMILY == sa_family::INET6V4M) ? AI_V4MAPPED | AI_ALL : 0;

				addrinfo::type	hints = {
					.ai_flags		= flags,
					.ai_family		= static_cast<sa_family_t>((FAMILY == sa_family::INET6V4M) ? sa_family::INET6 : FAMILY),
					.ai_socktype	= static_cast<int32_t>(type),
					.ai_protocol	= proto._struct->p_proto,
					.ai_addrlen		= 0,
					.ai_addr		= nullptr,
					.ai_canonname	= nullptr,
					.ai_next		= nullptr
				};
				addrinfo::type	*res;

				int32_t error_code = getaddrinfo(node, service, &hints, &res);
				switch (error_code) {
					case 0:
						break;
					case EAI_MEMORY:
						throw bad_alloc();
					case EAI_SYSTEM:
						throw system_error(errno, std::generic_category(), "addrinfo");
					default:
						throw logic_error(gai_strerror(static_cast<int32_t>(error_code)));
				}

				addrinfo::type	*tmp = res;

				for (; tmp; tmp = tmp->ai_next) {
					const_cast<std::list<data> *>(&this->_addrinfo_list)->push_back({
						tmp->ai_flags,
						static_cast<sa_family>(tmp->ai_family),
						static_cast<sock_type>(tmp->ai_socktype),
						tmp->ai_protocol,
						*reinterpret_cast<typename addr<FAMILY>::type *>(tmp->ai_addr),
						(tmp->ai_canonname) ? std::string(tmp->ai_canonname) : "null"
					});
				}
				freeaddrinfo(res);
			}

		private:
			addrinfo(void) = delete;
			addrinfo(const addrinfo &src) = delete;
			addrinfo(addrinfo &&src) = delete;

			addrinfo &	operator=(const addrinfo &src) = delete;
			addrinfo &	operator=(addrinfo &&src) = delete;
	};
};

template <nw::sa_family FAMILY>
std::ostream &	operator<<(std::ostream &o, const nw::addrinfo<FAMILY> &C) {
	o << C.to_string();
	return o;
}

#endif
