
#ifndef __NW_SOCKET_HPP__
# define __NW_SOCKET_HPP__

/*!
@file nw_socket.hpp
@brief ...
*/

# include <ostream>
# include <string>
# include <functional>

# include <sys/socket.h>
# include <unistd.h>

static const std::function<int(int, int, int)>								_s_socket = &socket;
static const std::function<int(int, const struct sockaddr *, socklen_t)>	_s_bind = &bind;
static const std::function<int(int, int)>									_s_listen = &listen;
static const std::function<int(int, const struct sockaddr *, socklen_t)>	_s_connect = &connect;
static const std::function<int(int, struct sockaddr *, socklen_t *)>		_s_accept = &accept;
static const std::function<int(int)>										_s_close = &close;

# include "nw_typedef.hpp"
# include "nw_protoent.hpp"
# include "nw_addr.hpp"
//# include "nw_addrinfo.hpp"
//# include "nw_sockopt.hpp"
# include "nw_buffer.hpp"

namespace nw {
	//! @tparam FAMILY nw::sa_family
	template <sa_family FAMILY>
	//! @brief Protected socket storage class
	class socket_storage {
		public:
		protected:
			const sock_type		_type;
			const protoent		_proto;
			const sockfd_type	_fd;
			addr<FAMILY>		_addr;

			socket_storage(socket_storage &&src) \
				: _type(src._type), _proto(src._proto), _fd(src._fd), _addr(src._addr) {
				*const_cast<sockfd_type *>(&src._fd) = -1;
			}

			socket_storage(const sock_type &type, const protoent &proto, const sockfd_type &fd) \
				: _type(type), _proto(proto), _fd(fd) {}

			socket_storage(const sock_type &type, const protoent &proto, const sockfd_type &fd, const addr<FAMILY> &a) \
				: _type(type), _proto(proto), _fd(fd), _addr(a) {}

			void	close(void) {
				if (this->_fd == -1)
					return ;
				if (_s_close(this->_fd) == -1)
					throw system_error(errno, std::generic_category(), "close");
				*const_cast<sockfd_type *>(&this->_fd) = -1;
			}

			void	close(std::nothrow_t) {
				if (this->_fd == -1)
					return ;
				_s_close(this->_fd);
				*const_cast<sockfd_type *>(&this->_fd) = -1;
			}

			virtual const std::string	to_string(void) const {
				std::string	str;

				str = "{ \"family\": \"" + sa_family_str(FAMILY) + "\", ";
				str += "\"type\": \"" + sock_type_str(this->_type) + "\", ";
				str += "\"protocol\": " + this->_proto.to_string() + ", ";
				str += "\"address\": " + this->_addr.to_string() + "}";

				return str;
			};

			virtual	~socket_storage(void) {}

			template <sa_family>
			friend class socket_storage;

			template <sa_family, sock_type>
			friend class socket;

		private:
			socket_storage(const socket_storage &src) = delete;

			socket_storage &	operator=(const socket_storage &src) = delete;
			socket_storage &	operator=(socket_storage &&src) = delete;
	};

	//! @tparam FAMILY nw::sa_family
	//! @tparam TYPE nw::sock_type
	template <sa_family FAMILY, sock_type TYPE>
	//! @brief socket template
	class socket : protected socket_storage<FAMILY> {
		public:
			//! @brief Construct form protocol number
			//!
			//! @throw nw::system_error if socket(2) function fail's
			//! @throw nw::logic_error if getprotobynumber do not find protocol
			socket(
				const int &proto_id	//!< protocol number required by getprotobynumber
			) : socket(nw::protoent(proto_id)) {}

			//! @brief Construct from protocol name
			//!
			//! @throw nw::system_error if socket(2) function fail's
			//! @throw nw::logic_error if getprotobyname do not find protocol
			socket(
				const std::string &proto_name	//!< protocol name required by getprotobyname
			) : socket(nw::protoent(proto_name)) {}

			//! @brief Construct form protoent class
			//!
			//! @throw nw::system_error if socket(2) function fail's
			socket(
				const protoent &proto	//!< nw::protoent
			) : socket_storage<FAMILY>(TYPE, proto, _s_socket(static_cast<int32_t>(FAMILY), static_cast<int32_t>(TYPE), proto._struct->p_proto)) {
				if (this->_fd == -1)
					throw system_error(errno, std::generic_category(), "socket");
			}

			//! @brief Move constructor
			socket(
				socket<FAMILY, TYPE> &&src	//!< nw::socket
			) : socket_storage<FAMILY>(std::move(src)) {}

			//! @brief Unspecified address family socket move constructor
			socket(
				socket<sa_family::UNSPEC, TYPE> &&src	//!< nw::sa_family::UNSPEC specialized nw::socket
			) : socket_storage<FAMILY>(src._type, src._proto, src._fd, src._addr) {
				*const_cast<sockfd_type *>(&src._fd) = -1;
			}

			//! Destructor
			//! @details
			//! If socket is valid close it with no throw behavior
			virtual	~socket(void) {
				this->close(std::nothrow);
			}

			//! @brief Marks the socket as a passive socket, that is, as a socket that will be used to accept incoming connection requests using nw::socket::accept.
			//!
			//! @throw nw::system_error if listen(2) function fail's
			void	listen(
				int backlog	//!< defines the maximum length to which the queue of pending connections for socket may grow
			) {
				if (_s_listen(this->_fd, backlog) == -1)
					throw system_error(errno, std::generic_category(), "listen");
			}

			//! @brief Assigns the address specified by addr to the socket.
			//! @details
			//! It is necessary to assign a local address before nw::sock_type::STREAM type socket may receive connections
			//!
			//! @throw nw::system_error if bind(2) function fail's
			void	bind(
				const addr<FAMILY> &addr	//!< nw::addr
			) {
				if (_s_bind(this->_fd, reinterpret_cast<const sockaddr *>(&addr._struct), addr._sizeof) == -1)
					throw system_error(errno, std::generic_category(), "bind");
				this->_addr = addr;
			}

			//! @brief Connects the socket to the address specified by addr.
			//! @details
			//! If socket's type is nw::sock_type::DGRAM, then addr is the address to which datagrams are sent by default, and the only address from which data‐grams are received.
			//!
			//! If the socket's type is nw::sock_type::STREAM or nw::sock_type::SEQPACKET, this call attempts to make a connection to the socket that is bound to the address specified by addr.
			//!
			//! @throw nw::system_error if connect(2) function fail's
			void	connect(
				const addr<FAMILY> &addr	//!< nw::addr
			) {
				if (_s_connect(this->_fd, reinterpret_cast<const sockaddr *>(&addr._struct), addr._sizeof) == -1)
					throw system_error(errno, std::generic_category(), "connect");
				this->_addr = addr;
			}

			//! @brief Accept incoming connection from connection-based socket types (nw::sock_type::STREAM, nw::sock_type::SEQPACKET).
			//! @details
			//! It extracts the first connection request on the pending connections queue for the listening socket and creates a new connected socket.
			//!
			//! Socket have to be bounded to a local address with nw::socket::bind(), and listening for connections after a nw::socket::listen().
			//!
			//! @return nw::sa_family::UNSPEC specialized nw::socket
			//! @throw nw::system_error if accept(2) function fail's
			//! @throw nw::logic_error if connected socket come from unsupported address family
			socket<sa_family::UNSPEC, TYPE> accept(void) {
				sockfd_type			fd;
				addr_storage::type	addr_struct;
				socklen_type		addr_len	= sizeof(addr_struct);

				if ((fd = _s_accept(this->_fd, reinterpret_cast<struct sockaddr *>(&addr_struct), &addr_len)) == -1)
					throw system_error(errno, std::generic_category(), "accept");
				switch (static_cast<sa_family>(addr_struct.ss_family)) {
					case sa_family::INET:
						return socket<sa_family::INET, TYPE>(this->_proto, fd, *reinterpret_cast<addr<sa_family::INET>::type *>(&addr_struct));
					case sa_family::INET6:
						return socket<sa_family::INET6, TYPE>(this->_proto, fd, *reinterpret_cast<addr<sa_family::INET6>::type *>(&addr_struct));
					default:
						throw logic_error("accept: invalid address family");
				}
			}

			//! @brief Close the socket.
			//! @throw nw::system_error if close(2) function fail's
			void	close(void) {
				socket_storage<FAMILY>::close();
			}

			//! @brief Close the socket with no throw behavior.
			void	close(std::nothrow_t) {
				socket_storage<FAMILY>::close(std::nothrow);
			}

			//! @brief Return a json formated std::string containing socket data
			//! @return json formated std::string
			const std::string	to_string(void) const {
				return socket_storage<FAMILY>::to_string();
			}

			template <nw::size_type SIZE>
			void		send(nw::buffer<SIZE> &buf, int flags) {
				static_cast<void>(buf);
				static_cast<void>(flags);
			}

			template <nw::size_type SIZE>
			void		send(nw::buffer<SIZE> &buf, int flags, nw::addr<FAMILY> &addr) {
				static_cast<void>(buf);
				static_cast<void>(flags);
				static_cast<void>(addr);
			}

			nw::size_type	send(struct msghdr msg, int flags) {
				static_cast<void>(msg);
				static_cast<void>(flags);
			}


		protected:
			socket(const protoent &proto, const sockfd_type &fd, const addr<FAMILY> &a) \
				: socket_storage<FAMILY>(TYPE, proto, fd, a) {}

			template <sa_family, sock_type>
			friend class socket;

		private:
			socket(void) = delete;
			socket(const socket &src) = delete;

			socket &	operator=(const socket &src) = delete;
			socket &	operator=(socket &&src) = delete;
	};

	//! @tparam TYPE nw::sock_type
	template <sock_type TYPE>
	//! @brief Unspecified address family socket template
	//! @details Socket placeholder for nw::sa_family::INET and nw::sa_family::INET6 address family
	class socket<sa_family::UNSPEC, TYPE> : protected socket_storage<sa_family::UNSPEC> {
		public:
			//! @brief Move construct from IPv4 socket
			socket(
				socket<sa_family::INET, TYPE> &&src	//!< nw::sa_family::INET nw::socket
			) : socket_storage<sa_family::UNSPEC>(src._type, src._proto, src._fd, src._addr) {
				*const_cast<sockfd_type *>(&src._fd) = -1;
			}

			//! @brief Move construct from IPv6 socket
			socket(
				socket<sa_family::INET6, TYPE> &&src	//!< nw::sa_family::INET6 nw::socket
			) : socket_storage<sa_family::UNSPEC>(src._type, src._proto, src._fd, src._addr) {
				*const_cast<sockfd_type *>(&src._fd) = -1;
			}

			//! @brief Move construct from unspecified address family socket
			socket(
				socket &&src	//!< nw::sa_family::UNSPEC specialized nw::socket
			) : socket_storage<sa_family::UNSPEC>(std::move(src)) {}

			//! @brief return a json formated std::string containing socket data
			//! @return json formated std::string
			virtual const std::string	to_string(void) const {
				return socket_storage<sa_family::UNSPEC>::to_string();
			}

			//! @brief Destructor
			//! @details
			//! If socket is valid close it with no throw behavior
			virtual	~socket(void) {
				socket_storage<sa_family::UNSPEC>::close(std::nothrow);
			}

		protected:
			template <sa_family, sock_type>
			friend class socket;

		private:
			socket(void) = delete;
			socket(const socket &src) = delete;

			socket &	operator=(const socket &src) = delete;
			socket &	operator=(socket &&src) = delete;
	};

	//! @tparam TYPE nw::sock_type
	template <sock_type TYPE>
	//! Deleted IPv6 with IPv4 mapped address socket template specialization
	class socket<sa_family::INET6V4M, TYPE> : protected socket_storage<sa_family::INET6V4M> {
		public:
		protected:
		private:
			socket(void) = delete;
			socket(const socket &src) = delete;
			socket(socket &&src) = delete;

			socket(const int &proto_id = 0) = delete;
			socket(const std::string &proto_name) = delete;
			socket(const protoent &proto) = delete;

			virtual	~socket(void) {}

			const std::string	to_string(void) const = delete;

			socket &	operator=(const socket &src) = delete;
			socket &	operator=(socket &&src) = delete;
	};
};

template <nw::sa_family FAMILY, nw::sock_type TYPE>
std::ostream &	operator<<(std::ostream &o, const nw::socket<FAMILY, TYPE> &C) {
	o << C.to_string();
	return o;
}

#endif
