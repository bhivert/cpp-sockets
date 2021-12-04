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
	class addrinfo_interface {
		public:
			class data {
				public:
					data(const sock_use &flags, const sa_family &family, const sock_type &type, const protoent &proto, const addr<FAMILY> &addr, const std::string &canonname) \
						: _flags(flags), _family(family), _type(type), _proto(proto), _addr(addr), _canonname(canonname) {
					}

					data(const data &src) : _flags(src._flags), _family(src._family), _type(src._type), _proto(src._proto), _addr(src._addr), _canonname(src._canonname) {}
					data(data &&src) : _flags(src._flags), _family(src._family), _type(src._type), _proto(src._proto), _addr(src._addr), _canonname(src._canonname) {}

					const std::string	to_string(void) const {
						std::string str;

						str = "{ \"flags\": \"" + sock_use_str(static_cast<sock_use>(static_cast<uint8_t>(this->_flags) & static_cast<uint8_t>(sock_use::BIND))) + "\", ";
						str += "\"family\": \"" + sa_family_str(this->_family) + "\", ";
						str += "\"type\": \"" + sock_type_str(this->_type) + "\", ";
						str += "\"protocol\": " + this->_proto.to_string() + ", ";
						str += "\"sockaddr\": " + this->_addr.to_string() + ", ";
						str += "\"canonname\": " + ((this->_canonname == "null") ? this->_canonname : '\"' + this->_canonname + '\"') + " }";

						return str;
					}

					~data(void) {}

				protected:
					const sock_use		_flags;
					const sa_family		_family;
					const sock_type		_type;
					const protoent		_proto;
					const addr<FAMILY>	_addr;
					const std::string	_canonname;

					template <sa_family F>
					friend class addrinfo_interface;

				private:
					data(void) = delete;

					data &	operator=(const data &src) = delete;
					data &	operator=(data &&src) = delete;
			};

			enum class	error : int32_t {
				NO_ERROR		= 0,
				E_ADDRFAMILY	= EAI_ADDRFAMILY,
				E_AGAIN			= EAI_AGAIN,
				E_BADFLAGS		= EAI_BADFLAGS,
				E_FAIL			= EAI_FAIL,
				E_FAMILY		= EAI_FAMILY,
				E_MEMORY		= EAI_MEMORY,
				E_NODATA		= EAI_NODATA,
				E_NONAME		= EAI_NONAME,
				E_SERVICE		= EAI_SERVICE,
				E_SOCKTYPE		= EAI_SOCKTYPE,
				E_SYSTEM		= EAI_SYSTEM
			};

			virtual	~addrinfo_interface(void) {}

			const std::string	to_string(void) const {
				std::string str;

				str = '[';
				for (typename std::list<addrinfo_interface<FAMILY>::data>::const_iterator it = this->_addrinfo_list.begin(); it != this->_addrinfo_list.end() ; ++it) {
					str += it->to_string();
					if (std::next(it) != this->_addrinfo_list.end())
						str += ", ";
				}
				str += ']';

				return str;
			}

		protected:
			typedef addrinfo_struct		type;

			const std::list<data>		_addrinfo_list;

			addrinfo_interface(const sock_use &use, const char *node = nullptr, const char *service = nullptr, const protoent &proto = 0, const sock_type &type = sock_type::UNSPEC) {
				uint8_t	flags = (FAMILY == sa_family::INET6V4M) ? AI_V4MAPPED | AI_ALL: 0;
				addrinfo_interface::type	hints = {
					.ai_flags		= (node) ? static_cast<uint8_t>(use) | flags | AI_CANONNAME : static_cast<uint8_t>(use) | flags,
					.ai_family		= static_cast<sa_family_t>((FAMILY == sa_family::INET6V4M) ? sa_family::INET6 : FAMILY),
					.ai_socktype	= static_cast<int32_t>(type),
					.ai_protocol	= proto._struct->p_proto,
					.ai_addrlen		= 0,
					.ai_addr		= nullptr,
					.ai_canonname	= nullptr,
					.ai_next		= nullptr
				};
				addrinfo_interface::type	*res;

				error error_code = static_cast<error>(getaddrinfo(node, service, &hints, &res));
				switch (error_code) {
					case error::NO_ERROR:
						break;
					case error::E_MEMORY:
						throw bad_alloc();
					case error::E_SYSTEM:
						throw system_error(errno,std::generic_category(), "addrinfo");
					default:
						throw logic_error(gai_strerror(static_cast<int32_t>(error_code)));
				}

				addrinfo_interface::type	*tmp = res;

				for (; tmp; tmp = tmp->ai_next) {
					const_cast<std::list<data> *>(&this->_addrinfo_list)->push_back({
						static_cast<sock_use>(tmp->ai_flags),
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
			addrinfo_interface(void) = delete;
			addrinfo_interface(const addrinfo_interface &src) = delete;
			addrinfo_interface(addrinfo_interface &&src) = delete;

			addrinfo_interface &	operator=(const addrinfo_interface &src) = delete;
			addrinfo_interface &	operator=(addrinfo_interface &&src) = delete;
	};

	template <sock_use USE, sa_family FAMILY>
	class addrinfo : public addrinfo_interface<FAMILY> {
		public:
		protected:
		private:
			addrinfo(void) = delete;
			addrinfo(const addrinfo &src) = delete;
			addrinfo(addrinfo &&src) = delete;

			~addrinfo(void) = delete;

			addrinfo &	operator=(const addrinfo &src) = delete;
			addrinfo &	operator=(addrinfo &&src) = delete;
	};

	template <sa_family FAMILY>
	class addrinfo<sock_use::CONNECT, FAMILY> : public addrinfo_interface<FAMILY> {
		public:
			addrinfo(const std::string &node, const std::string &service) \
				: addrinfo_interface<FAMILY>(sock_use::CONNECT, node.c_str(), service.c_str()) {}

			addrinfo(const std::string &node, const std::string &service, const sock_type &type) \
				: addrinfo_interface<FAMILY>(sock_use::CONNECT, node.c_str(), service.c_str(), 0, type) {}

			addrinfo(const std::string &node, const std::string &service, const std::string &proto_name, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo_interface<FAMILY>(sock_use::CONNECT, node.c_str(), service.c_str(), proto_name, type) {}

			addrinfo(const std::string &node, const std::string &service, const proto_id &proto_number, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo_interface<FAMILY>(sock_use::CONNECT, node.c_str(), service.c_str(), proto_number, type) {}

			addrinfo(const std::string &node, const std::string &service, const protoent &proto, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo_interface<FAMILY>(sock_use::CONNECT, node.c_str(), service.c_str(), proto, type) {}

			~addrinfo(void) {};

		protected:
		private:
			addrinfo(void) = delete;
			addrinfo(const addrinfo &src) = delete;
			addrinfo(addrinfo &&src) = delete;

			addrinfo &	operator=(const addrinfo &src) = delete;
			addrinfo &	operator=(addrinfo &&src) = delete;
	};

	template <sa_family FAMILY>
	class addrinfo<sock_use::BIND, FAMILY> : public addrinfo_interface<FAMILY> {
		public:
			addrinfo(const std::string &service) \
				: addrinfo_interface<FAMILY>(sock_use::BIND, nullptr, service.c_str()) {}

			addrinfo(const std::string &service, const sock_type &type) \
				: addrinfo_interface<FAMILY>(sock_use::BIND, nullptr, service.c_str(), 0, type) {}

			addrinfo(const std::string &service, const std::string &proto_name, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo_interface<FAMILY>(sock_use::BIND, nullptr, service.c_str(), proto_name, type) {}

			addrinfo(const std::string &service, const proto_id &proto_number, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo_interface<FAMILY>(sock_use::BIND, nullptr, service.c_str(), proto_number, type) {}

			addrinfo(const std::string &service, const protoent &proto, const sock_type &type = sock_type::UNSPEC) \
				: addrinfo_interface<FAMILY>(sock_use::BIND, nullptr, service.c_str(), proto, type) {}

			~addrinfo(void) {};

		protected:
		private:
			addrinfo(void) = delete;
			addrinfo(const addrinfo &src) = delete;
			addrinfo(addrinfo &&src) = delete;

			addrinfo &	operator=(const addrinfo &src) = delete;
			addrinfo &	operator=(addrinfo &&src) = delete;
	};
};

template <nw::sa_family FAMILY>
std::ostream &	operator<<(std::ostream &o, const nw::addrinfo_interface<FAMILY> &C) {
	o << C.to_string();
	return o;
}

#endif
