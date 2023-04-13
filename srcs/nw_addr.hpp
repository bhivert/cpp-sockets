
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

# include <arpa/inet.h>

namespace nw {
	//! Protected address storage class
	class addr_storage {
		public:
		protected:
			using type = struct sockaddr_storage;

			const type			_struct;
			const socklen_type	_sizeof;

			addr_storage(void) : _struct{static_cast<sa_family_t>(sa_family::UNSPEC), {0},0}, _sizeof(sizeof(type)) {}

			virtual	~addr_storage(void) {}

			virtual const std::string	to_string(void) const = 0;

			virtual const sa_family &	get_family(void) const {
				return *reinterpret_cast<const sa_family *>(&this->_struct.ss_family);
			}

			const type &				get_ref(void) const {
				return this->_struct;
			}

			template <sa_family, sock_type>
			friend class socket;

		private:
			addr_storage(const type &storage) = delete;
			addr_storage(const addr_storage &src) = delete;
			addr_storage(addr_storage &&src) = delete;

			virtual addr_storage &	operator=(const addr_storage &src) = delete;
			virtual addr_storage &	operator=(addr_storage &&src) = delete;
	};

	//! @tparam FAMILY ::sa_family
	template<sa_family FAMILY>
	//! addr template
	class addr : protected addr_storage {
		private:
			addr(void) = delete;
			addr(const addr &src) = delete;
			addr(addr &&src) = delete;

			virtual	~addr(void) {}

			virtual const std::string	to_string(void) const = delete;

			virtual addr &	operator=(const addr &src) = delete;
			virtual addr &	operator=(addr &&src) = delete;
	};

	template <>
	//! IPv4 addr template specialization
	class addr<sa_family::INET> : protected addr_storage {
		public:
			//! @brief type is struct sockaddr_in
			using type = struct sockaddr_in;

			//! @brief Default constructor
			addr(void) : _struct(reinterpret_cast<const type &>(get_ref())) {
				const_cast<type &>(this->_struct).sin_family = AF_INET;
				const_cast<socklen_type &>(this->_sizeof) = sizeof(type);
			}

			//! @brief Construct from port and address
			//! @throw nw::logic_error if inet_aton(3) function fail's
			addr(
				const port_type		&port,					//!< port
				const std::string	&ipv4_addr = "0.0.0.0"	//!< IPv4 address
			) : addr::addr() {
				type &ref = const_cast<type &>(this->_struct) = {
					.sin_family	= AF_INET,
					.sin_port	= htons(port),
					.sin_addr	= {0},
					.sin_zero	= {0}
				};
				if (!inet_aton(ipv4_addr.c_str(), &ref.sin_addr))
					throw logic_error("inet_aton: invalid address");
			}

			//! @brief Construct from nw::addr::type
			addr(
				const type &sa_in	//!< struct sockaddr_in
			) : addr::addr() {
				const_cast<type &>(this->_struct) = sa_in;
			}

			//! @brief Copy constructor
			addr(
				const addr &src		//!< nw::sa_family::INET nw::addr
			) : addr(src._struct) {}

			//! @brief Move constructor
			addr(
				addr &&src			//!< nw::sa_family::INET nw::addr
			) : addr(src._struct) {}

			//! @brief Destructor
			virtual	~addr(void) {}

			//! @brief Assignment operator
			//! @return nw::sa_family::INET nw::addr
			addr &	operator=(
				const addr &src		//!< nw::sa_family::INET nw::addr
			) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			//! @brief Move operator
			//! @return nw::sa_family::INET nw::addr
			addr &	operator=(
				addr &&src			//!< nw::sa_family::INET nw::addr
			) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			//! @brief Return a json formated std::string containing addr data
			//! @return json formated std::string
			const std::string	to_string(void) const {
				std::string	str;

				str = "{ \"family\": \"" + sa_family_str(static_cast<sa_family>(this->_struct.sin_family)) + "\", ";
				str += "\"port\": " + std::to_string(ntohs(this->_struct.sin_port)) + ", ";
				str += "\"ip\": \"" + std::string(inet_ntoa(this->_struct.sin_addr)) + "\" }";

				return str;
			}

		protected:
			const type	&_struct;

			addr(const addr_storage &src) : addr(static_cast<const addr<sa_family::INET> &>(src)._struct) {
				if (this->_struct.sin_family != static_cast<sa_family_t>(sa_family::INET))
					throw logic_error("addr_storage: invalid conversion");
			}

			friend addr<sa_family::UNSPEC>;

			template <sa_family, sock_type>
			friend class socket;

		private:
	};

	template <>
	//! IPv6 addr template specialization
	class addr<sa_family::INET6> : protected addr_storage {
		public:
			//! @brief type is sockaddr_in6
			using type = struct sockaddr_in6;

			//! @brief Default constructor
			addr(void) : _struct(reinterpret_cast<const type &>(get_ref())) {
				const_cast<type &>(this->_struct).sin6_family = AF_INET6;
				const_cast<socklen_type &>(this->_sizeof) = sizeof(type);
			}

			//! @brief Construct from port, address, flowinfo and scope_id
			//! @throw nw::system_error if inet_pton(3) function fail's with -1
			//! @throw nw::logic_error if inet_pton(3) function fail's with 0
			addr(
				const port_type &port,			//!< port
				const std::string &ipv6_addr = "::",	//!< IPv6 address
				uint32_t flowinfo = 0,			//!< IPv6 flow info
				uint32_t scope_id = 0			//!< IPv6 scope id
			) : addr::addr() {
				type	&ref = const_cast<type &>(this->_struct) = {
					.sin6_family	= AF_INET6,
					.sin6_port		= htons(port),
					.sin6_flowinfo	= htonl(flowinfo),
					.sin6_addr		= {{{0}}},
					.sin6_scope_id	= htonl(scope_id)
				};
				int8_t	ret;

				if (!(ret = inet_pton(AF_INET6, ipv6_addr.c_str(), &ref.sin6_addr)))
					throw logic_error("inet_pton: invalid address");
				else if (ret == -1)
					throw system_error(errno, std::generic_category(), "inet_pton");
			}

			//! @brief Construct from nw::addr::type
			addr(
				const type &sa_in6	//!< struct sockaddr_in6
			) : addr::addr() {
				const_cast<type &>(this->_struct) = sa_in6;
			}

			//! @brief Copy constructor
			addr(
				const addr &src		//!< nw::sa_family::INET6 nw::addr
			) : addr(src._struct) {}

			//! @brief Move constructor
			addr(
				addr &&src			//!< nw::sa_family::INET6 nw::addr
			) : addr(src._struct) {}

			//! @brief Destructor
			virtual	~addr(void) {}

			//! @brief Assignment operator
			//! @return nw::sa_family::INET6 nw::addr
			addr &	operator=(
				const addr &src		//!< nw::sa_family::INET6 nw::addr
			) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			//! @brief Move operator
			//! @return nw::sa_family::INET6 nw::addr
			addr &	operator=(
				addr &&src			//!< nw::sa_family::INET6 nw::addr
			) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			//! @brief Return a json formated std::string containing addr data
			//! @return json formated std::string
			const std::string	to_string(void) const {
				std::string	str;
				char		addr[INET6_ADDRSTRLEN];

				str = "{ \"family\": \"" + sa_family_str(static_cast<sa_family>(this->_struct.sin6_family)) + "\", ";
				str += "\"port\": " + std::to_string(ntohs(this->_struct.sin6_port)) + ", ";
				str += "\"flowinfo\": " + std::to_string(ntohl(this->_struct.sin6_flowinfo)) + ", ";
				str += "\"ipv6\": \"" + std::string(inet_ntop(AF_INET6, &this->_struct.sin6_addr, reinterpret_cast<char *>(&addr), sizeof(type))) + "\", ";
				str += "\"scope_id\": " + std::to_string(ntohl(this->_struct.sin6_scope_id)) + " }";

				return str;
			}

		protected:
			const type	&_struct;

			addr(const addr_storage &src) : addr(static_cast<const addr<sa_family::INET6> &>(src)._struct) {
				if (this->_struct.sin6_family != static_cast<sa_family_t>(sa_family::INET6))
					throw logic_error("addr_storage: invalid conversion");
			}

			friend addr<sa_family::UNSPEC>;

			template <sa_family, sock_type>
			friend class socket;

		private:
	};

	template <>
	//! IPv6 with IPv4 mapped addr template specialization
	class addr<sa_family::INET6V4M> : protected addr<sa_family::INET6> {
		public:
			//! @brief type is sockaddr_in6
			using type = addr<sa_family::INET6>::type;

			//! @brief Default constructor
			addr(void) : addr<sa_family::INET6>::addr() {}

			//! @brief Construct from nw::addr::type
			addr(
				const type &sa_in6	//!< struct sockaddr_in6
			) : nw::addr<sa_family::INET6>::addr(sa_in6) {}

			//! @brief Copy constructor
			addr(
				const addr &src		//!< nw::sa_family::INET6V4M nw::addr
			) : addr<sa_family::INET6>::addr(src._struct) {}

			//! @brief Move constructor
			addr(
				addr &&src			//!< nw::sa_family::INET6V4M nw::addr
			) : addr<sa_family::INET6>::addr(src._struct) {}

			//! @brief Destructor
			virtual	~addr(void) {}

			//! @brief Assignment operator
			//! @return nw::sa_family::INET6V4M nw::addr
			addr &	operator=(
				const addr &src		//!< nw::sa_family::INET6V4M nw::addr
			) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			//! @brief Move operator
			//! @return nw::sa_family::INET6V4M nw::addr
			addr &	operator=(
				addr &&src			//!< nw::sa_family::INET6V4M nw::addr
			) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			//! @brief Return a json formated std::string containing addr data
			//! @return json formated std::string
			const std::string	to_string(void) const {
				return addr<sa_family::INET6>::to_string();
			}

		protected:
			addr(const addr_storage &src) : addr<sa_family::INET6>::addr(src) {}

			friend addr<sa_family::UNSPEC>;

		private:
	};

	template <>
	//! Unspecified IP addr template specialization
	class addr<sa_family::UNSPEC> : protected addr_storage {
		public:
			//! @brief type is sockaddr_storage
			using type = addr_storage::type;

			//! @brief Default constructor
			addr(void) : _struct(reinterpret_cast<const type &>(get_ref())) {
				const_cast<type &>(this->_struct).ss_family = AF_UNSPEC;
				const_cast<socklen_type &>(this->_sizeof) = sizeof(type);
			}

			//! @brief Construct from nw::addr::type
			//! @details
			//! Cast to original type (sockadd_in or sockadd_in6) from sockaddr_storage.ss_family value and construct
			addr(
				const type &addr	//!< struct sockaddr_storage
			) : addr::addr() {
				switch (static_cast<sa_family>(addr.ss_family)) {
					default:
						const_cast<type &>(this->_struct) = addr;
						const_cast<socklen_type &>(this->_sizeof) = sizeof(type);
						break ;
					case sa_family::INET:
						reinterpret_cast<nw::addr<sa_family::INET>::type &>(const_cast<type &>(this->_struct)) = reinterpret_cast<const nw::addr<sa_family::INET>::type &>(addr);
						const_cast<socklen_type &>(this->_sizeof) = sizeof(nw::addr<sa_family::INET>::type);
						break ;
					case sa_family::INET6:
						reinterpret_cast<nw::addr<sa_family::INET6>::type &>(const_cast<type &>(this->_struct)) = reinterpret_cast<const nw::addr<sa_family::INET6>::type &>(addr);
						const_cast<socklen_type &>(this->_sizeof) = sizeof(nw::addr<sa_family::INET6>::type);
						break ;
				}
			}

			//! @brief Copy constructor
			addr(
				const addr &src		//!< nw::sa_family::UNSPEC nw::addr
			) : addr(src._struct) {}

			//! @brief Move constructor
			addr(
				addr &&src			//!< nw::sa_family::UNSPEC nw::addr
			) : addr(src._struct) {}

			//! @brief Construct from nw::sa_family::INET nw::addr
			addr(
				const addr<sa_family::INET> &src		//!< nw::sa_family::UNSPEC nw::addr
			) : addr(reinterpret_cast<const type &>(src._struct)) {}

			//! @brief Construct from nw::sa_family::INET6 nw::addr
			addr(
				const addr<sa_family::INET6> &src		//!< nw::sa_family::INET6 nw::addr
			) : addr(reinterpret_cast<const type &>(src._struct)) {}

			//! @brief Construct from nw::sa_family::INET6V4M nw::addr
			addr(
				const addr<sa_family::INET6V4M> &src	//!< nw::sa_family::INET6V4M nw::addr
			) : addr(reinterpret_cast<const type &>(src._struct)) {}

			//! @brief Destructor
			virtual	~addr(void) {}

			//! @brief Assignment operator
			//! @return nw::sa_family::UNSPEC nw::addr
			addr &	operator=(
				const addr &src	//!< nw::sa_family::UNSPEC nw::addr
			) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			//! @brief Move operator
			//! @return nw::sa_family::UNSPEC nw::addr
			addr &	operator=(
				addr &&src		//!< nw::sa_family::UNSPEC nw::addr
			) {
				const_cast<type &>(this->_struct) = src._struct;
				return *this;
			}

			//! @brief return a json formated std::string containing addr data
			//! @return json formated std::string
			virtual const std::string	to_string(void) const {
				std::string	str;

				switch (this->get_family()) {
					default:
						str = "{ \"family\" :\"" + sa_family_str(sa_family::UNSPEC) + "\" }";
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

			template <sa_family, sock_type>
			friend class socket;

		private:
	};
};

template <nw::sa_family FAMILY>
std::ostream &	operator<<(std::ostream &o, const nw::addr<FAMILY> &C) {
	o << C.to_string();
	return o;
}

#endif
