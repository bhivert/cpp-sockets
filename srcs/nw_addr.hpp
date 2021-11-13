
#ifndef __NW_ADDR_HPP__
# define __NW_ADDR_HPP__

/*!
@file nw_addr.hpp
@brief ...
*/

# include <ostream>
# include <string>
# include <cstring>

# include "nw_typedef.hpp"
# include "nw_exception.hpp"

# include <arpa/inet.h>

namespace nw {
	class addr_storage {
		public:
			typedef struct sockaddr_storage	type;

			virtual const std::string	to_string(void) const = 0;

			class invalid_address : public nw::exception {
				public:
					const char	*what(void) const noexcept {
						return "invalid address";
					}
			};

			virtual const sa_family &	get_family(void) const {
				return *reinterpret_cast<const sa_family *>(&this->_struct.ss_family);
			}

		protected:
			const type			_struct;

			addr_storage(void) : _struct{static_cast<sa_family_t>(sa_family::UNSPEC), {0},0} {}

			virtual	~addr_storage(void) {};

			const type &	get_ref(void) const {
				return this->_struct;
			}

		private:
			addr_storage(const type &storage) = delete;
			addr_storage(const addr_storage &src) = delete;
			addr_storage(addr_storage &&src) = delete;

			virtual addr_storage &	operator=(const addr_storage &src) = delete;
			virtual addr_storage &	operator=(addr_storage &&src) = delete;
	};

	template<sa_family FAMILY>
	class addr : public addr_storage {
		private:
			addr(void) = delete;
			addr(const addr &src) = delete;
			addr(addr &&src) = delete;

			virtual const std::string	to_string(void) const = delete;

			virtual addr &	operator=(const addr &src) = delete;
			virtual addr &	operator=(addr &&src) = delete;
	};

	template <>
	class addr<sa_family::INET> : public addr_storage {
		public:
			typedef struct sockaddr_in				type;
			typedef addr_storage::invalid_address	invalid_adress;

			class invalid_conversion : public nw::exception {
				public:
					const char	*what(void) const noexcept {
						return "invalid conversion to inet family";
					}
			};

			addr(void) : _struct(reinterpret_cast<const type &>(get_ref())) {
				const_cast<type &>(this->_struct).sin_family = AF_INET;
			}

			addr(const port_type &port, const std::string &addr = "0.0.0.0") : addr::addr() {
				type &ref = const_cast<type &>(this->_struct) = {
					.sin_family	= AF_INET,
					.sin_port	= htons(port),
					.sin_addr	= {0},
					.sin_zero	= {0}
				};
				if (!inet_aton(addr.c_str(), &ref.sin_addr))
					throw invalid_address();
			}

			addr(const type &addr) : addr::addr() {
				const_cast<type &>(this->_struct) = addr;
			}

			addr(const addr &src) : addr(src._struct) {}
			addr(addr &&src) : addr(src._struct) {}

			addr(const addr_storage &src) : addr(static_cast<const addr<sa_family::INET> &>(src)._struct) {
				if (this->_struct.sin_family != static_cast<sa_family_t>(sa_family::INET))
					throw invalid_conversion();
			}

			virtual	~addr(void) {}

			addr &	operator=(const addr &src) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			addr &	operator=(addr &&src) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			const std::string	to_string(void) const {
				std::string	str;

				str = "{\n\t\"family\": \"" + sa_family_str(static_cast<sa_family>(this->_struct.sin_family)) + "\",\n";
				str += "\t\"port\": " + std::to_string(ntohs(this->_struct.sin_port)) + ",\n";
				str += "\t\"address\": \"" + std::string(inet_ntoa(this->_struct.sin_addr)) + "\"\n\t}";

				return str;
			}

		protected:
			const type	&_struct;

			friend addr<sa_family::UNSPEC>;

		private:
	};

	template <>
	class addr<sa_family::INET6> : public addr_storage {
		public:
			typedef struct sockaddr_in6				type;
			typedef addr_storage::invalid_address	invalid_adress;

			class family_not_supported : public nw::exception {
				public:
					const char	*what(void) const noexcept {
						return "inet6 family not supported";
					}
			};

			class invalid_conversion : public nw::exception {
				public:
					const char	*what(void) const noexcept {
						return "invalid conversion to inet6 family";
					}
			};

			addr(void) : _struct(reinterpret_cast<const type &>(get_ref())) {
				const_cast<type &>(this->_struct).sin6_family = AF_INET6;
			}

			addr(const port_type &port, const std::string &addr = "::", uint32_t flowinfo = 0, uint32_t scope_id = 0) : addr::addr() {
				type	&ref = const_cast<type &>(this->_struct) = {
					.sin6_family	= AF_INET6,
					.sin6_port		= htons(port),
					.sin6_flowinfo	= htonl(flowinfo),
					.sin6_addr		= {{{0}}},
					.sin6_scope_id	= htonl(scope_id)
				};
				int8_t	ret;

				if (!(ret = inet_pton(AF_INET6, addr.c_str(), &ref.sin6_addr)))
					throw invalid_address();
				else if (ret == -1)
					throw family_not_supported();
			}

			addr(const type &addr) : addr::addr() {
				const_cast<type &>(this->_struct) = addr;
			}

			addr(const addr &src) : addr(src._struct) {}
			addr(addr &&src) : addr(src._struct) {}

			addr(const addr_storage &src) : addr(static_cast<const addr<sa_family::INET6> &>(src)._struct) {
				if (this->_struct.sin6_family != static_cast<sa_family_t>(sa_family::INET6))
					throw invalid_conversion();
			}

			virtual	~addr(void) {}

			addr &	operator=(const addr &src) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			addr &	operator=(addr &&src) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			const std::string	to_string(void) const {
				std::string	str;
				char		addr[INET6_ADDRSTRLEN];

				str = "{\n\t\"family\": \"" + sa_family_str(static_cast<sa_family>(this->_struct.sin6_family)) + "\",\n";
				str += "\t\"port\": " + std::to_string(ntohs(this->_struct.sin6_port)) + ",\n";
				str += "\t\"flowinfo\": " + std::to_string(ntohl(this->_struct.sin6_flowinfo)) + ",\n";
				str += "\t\"address\": \"" + std::string(inet_ntop(AF_INET6, &this->_struct.sin6_addr, reinterpret_cast<char *>(&addr), sizeof(type))) + "\",\n";
				str += "\t\"scope_id\": " + std::to_string(ntohl(this->_struct.sin6_scope_id)) + "\n\t}";

				return str;
			}

		protected:
			const type	&_struct;

			friend addr<sa_family::UNSPEC>;

		private:
	};

	template <>
	class addr<sa_family::INET6V4M> : public addr<sa_family::INET6> {
		public:
			typedef addr<sa_family::INET6>::type		type;

			addr(void) : addr<sa_family::INET6>::addr() {}
			addr(const type &addr) : nw::addr<sa_family::INET6>::addr(addr) {}
			addr(const addr &src) : addr<sa_family::INET6>::addr(src._struct) {}
			addr(addr &&src) : addr<sa_family::INET6>::addr(src._struct) {}

			addr(const addr_storage &src) : addr<sa_family::INET6>::addr(src) {}

			virtual	~addr(void) {}

			addr &	operator=(const addr &src) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			addr &	operator=(addr &&src) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

		protected:
		private:
	};

	template <>
	class addr<sa_family::UNSPEC> : public addr_storage {
		public:
			typedef addr_storage::type	type;

			addr(void) : _struct(reinterpret_cast<const type &>(get_ref())) {
				const_cast<type &>(this->_struct).ss_family = AF_UNSPEC;
			}

			addr(const type &addr) : addr::addr() {
				switch (static_cast<sa_family>(addr.ss_family)) {
					default:
						const_cast<type &>(this->_struct) = addr;
						break ;
					case sa_family::INET:
						reinterpret_cast<nw::addr<sa_family::INET>::type &>(const_cast<type &>(this->_struct)) = reinterpret_cast<const nw::addr<sa_family::INET>::type &>(addr);
						break ;
					case sa_family::INET6:
						reinterpret_cast<nw::addr<sa_family::INET6>::type &>(const_cast<type &>(this->_struct)) = reinterpret_cast<const nw::addr<sa_family::INET6>::type &>(addr);
						break ;
				}
			}

			addr(const addr &src) : addr(src._struct) {}
			addr(const addr<sa_family::INET> &src) : addr(reinterpret_cast<const type &>(src._struct)) {}
			addr(const addr<sa_family::INET6> &src) : addr(reinterpret_cast<const type &>(src._struct)) {}
			addr(const addr<sa_family::INET6V4M> &src) : addr(reinterpret_cast<const type &>(src._struct)) {}
			addr(addr &&src) : addr(src._struct) {}

			virtual	~addr(void) {}

			addr &	operator=(const addr &src) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			addr &	operator=(addr &&src) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			virtual const std::string	to_string(void) const {
				std::string	str;

				switch (this->get_family()) {
					default:
						str = "{\n\t\"family\" :\"" + sa_family_str(sa_family::UNSPEC) + "\n\t}";
						break ;
					case sa_family::INET:
						str = addr<sa_family::INET>(*this).to_string();
						break ;
					case sa_family::INET6:
						str = addr<sa_family::INET6>(*this).to_string();
						break ;
				}

				return str;
			}

		protected:
			const type	&_struct;

		private:
	};
};

template <nw::sa_family FAMILY>
std::ostream &	operator<<(std::ostream &o, const nw::addr<FAMILY> &C) {
	o << C.to_string();
	return o;
}

#endif
