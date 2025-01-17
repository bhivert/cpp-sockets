
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
static const std::function<ssize_t(int, void *, size_t, int)>				_s_send = &send;
static const std::function<ssize_t(int, void *, size_t, int, \
		const struct sockaddr *dest_addr, socklen_t addrlen)>				_s_sendto = &sendto;
static const std::function<ssize_t(int, const struct msghdr *, int)>		_s_sendmsg = &sendmsg;
static const std::function<ssize_t(int, void *, size_t, int)>				_s_recv = &recv;
static const std::function<ssize_t(int, void *, size_t, int, \
		struct sockaddr *dest_addr, socklen_t *addrlen)>					_s_recvfrom = &recvfrom;
static const std::function<ssize_t(int, struct msghdr *, int)>				_s_recvmsg = &recvmsg;

# include "nw_typedef.hpp"
# include "nw_protoent.hpp"
# include "nw_addr.hpp"
//# include "nw_addrinfo.hpp"
//# include "nw_sockopt.hpp"

# include "buffer/nw_ibuffer.hpp"
# include "buffer/nw_obuffer.hpp"

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
				str += "\"fd\": \"" + std::to_string(this->_fd) + "\", ";
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

			//! @brief Destructor
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
			//! Generally, connection-based protocol sockets may successfully connect() only once; connectionless protocol sockets may use connect() multiple times to change their association.
			//!
			//! @throw nw::system_error if connect(2) function fail's
			void	connect(
				const addr<FAMILY> &addr	//!< nw::addr
			) {
				if (_s_connect(this->_fd, reinterpret_cast<const sockaddr *>(&addr._struct), addr._sizeof) == -1)
					throw system_error(errno, std::generic_category(), "connect");
				this->_addr = addr;
			}

			//! @brief Connects the socket to the address specified by addr.
			//! @details
			//! Call to nw::socket<FAMILY, TYPE>::connect(const addr<FAMILY> &addr)
			//!
			//! @throw nw::system_error if connect(2) function fail's
			socket<FAMILY, TYPE> &	operator<<(
				const addr<FAMILY> &addr	//!< nw::addr
			) {
				this->connect(addr);
				return *this;
			}

			//! @briefon Dissolve the association with address
			//! @details
			//!
			//! @throw nw::system_error if connect(2) function fail's
			void	connect(
				const addr<sa_family::UNSPEC>	//!< nw::addr
			) {
				nw::addr<sa_family::UNSPEC>::type	ss = {AF_UNSPEC, {0}, 0};

				if (_s_connect(this->_fd, reinterpret_cast<const sockaddr *>(&ss), sizeof(ss)) == -1)
					throw system_error(errno, std::generic_category(), "connect");
				this->_addr = nw::addr<FAMILY>();
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
			socket<FAMILY, TYPE> accept(void) {
				sockfd_type					fd;
				typename addr<FAMILY>::type	addr_struct;
				socklen_type				addr_len	= sizeof(addr_struct);

				if ((fd = _s_accept(this->_fd, reinterpret_cast<struct sockaddr *>(&addr_struct), &addr_len)) == -1)
					throw system_error(errno, std::generic_category(), "accept");
				return socket<FAMILY, TYPE>(this->_proto, fd, *reinterpret_cast<typename addr<FAMILY>::type *>(&addr_struct));
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

			//! @tparam TYPE nw::size_type
			template <size_type SIZE>
			//! @brief Transmit a message to another socket.
			//! @details
			//! The send() call may be used only with an connected socket.
			//!
			//! @throw nw::system_error if send(2) function fail's
			size_type	send(
				obuffer<SIZE> &buf,	//!< nw::obuffer<SIZE>
				int flags = 0		//!< The flags argument is the bitwise OR of zero or more of flags defined in man 2 send.
			) {
				const sockfd_type	fd = this->_fd;

				return buf.sync([fd, flags](void *buf, size_type size){
					ssize_t	ret = _s_send(fd, buf, size, flags);
					if (ret == -1)
						throw system_error(errno, std::generic_category(), "send");
					return ret;
				});
			}

			//! @tparam TYPE nw::size_type
			template <size_type SIZE>
			//! @brief Transmit a message to another socket.
			//! @details
			//! Call to nw::socket<FAMILY, TYPE>::send(obuffer<SIZE> &buf, int flags = 0)
			//!
			//! @throw nw::system_error if send(2) function fail's
			socket<FAMILY, TYPE> &	operator<<(
				obuffer<SIZE> &buf	//!< nw::obuffer
			) {
				this->send(buf);
				return *this;
			}

			//! @tparam TYPE nw::size_type
			template <size_type SIZE>
			//! @brief Transmit a message to another socket.
			//! @details
			//! The sendto() call is used on a connection-mode (nw::socktype::STREAM, nw::socktype::SEQPACKET) socket, the address of the target is given by addr parameter.
			//!
			//! @throw nw::system_error if sendto(2) function fail's
			size_type	send(
				obuffer<SIZE> &buf,			//!< nw::obuffer<SIZE>
				const addr<FAMILY> &addr,	//!< nw::addr<FAMILY>
				int flags = 0				//!< The flags argument is the bitwise OR of zero or more of flags defined in man 2 send.
			) {
				const sockfd_type	fd = this->_fd;

				return buf.sync([fd, flags, &addr](void *buf, size_type size){
					size_t	ret = _s_sendto(fd, buf, size, flags, &addr._struct, sizeof(nw::addr<FAMILY>::type));
					if (ret == -1)
						throw system_error(errno, std::generic_category(), "sendto");
					return ret;
				});
			}

			size_type	send(
				struct msghdr msg,
				int flags
			) {
				static_cast<void>(msg);
				static_cast<void>(flags);
				//sendmsg
			}

			//! @tparam TYPE nw::size_type
			template <size_type SIZE>
			//! @brief Receive a message from another socket.
			//! @details
			//! The recv() call is used on both connectionless and connection-oriented sockets.
			//!
			//! @throw nw::system_error if recv(2) function fail's
			size_type	recv(
				ibuffer<SIZE> &buf,		//!< nw::ibuffer<SIZE>
				int flags = 0			//!< The flags argument is the bitwise OR of zero or more of flags defined in man 2 recv.
			) {
				const sockfd_type	fd = this->_fd;

				return buf.sync([fd, flags](void *buf, size_type size){
					ssize_t	ret = _s_recv(fd, buf, size, flags);
					if (ret == -1)
						throw system_error(errno, std::generic_category(), "recv");
					return ret;
				});
			}

			//! @tparam TYPE nw::size_type
			template <size_type SIZE>
			//! @brief Receive a message from another socket.
			//! @details
			//! Call to nw::socket<FAMILY, TYPE>::recv(obuffer<SIZE> &buf, int flags = 0)
			//!
			//! @throw nw::system_error if recv(2) function fail's
			socket<FAMILY, TYPE> &	operator>>(
				ibuffer<SIZE> &buf		//!< nw::ibuffer<SIZE>
			) {
				this->recv(buf);
				return *this;
			}

			//! @tparam TYPE nw::size_type
			template <size_type SIZE>
			//! @brief Transmit a message to another socket.
			//! @details
			//! The recvfrom() call is used on on both connectionless and connection-oriented sockets.
			//! The source address is placed in addr if the underlying protocol provides the source address of the message.
			//!
			//! @throw nw::system_error if sendto(2) function fail's
			size_type	recv(
				ibuffer<SIZE> &buf,			//!< nw::ibuffer<SIZE>
				const addr<FAMILY> &addr,	//!< nw::addr<FAMILY>
				int flags = 0				//!< The flags argument is the bitwise OR of zero or more of flags defined in man 2 recv.
			) {
				typename nw::addr<FAMILY>::type	sa;
				socklen_type					sa_len = sizeof(nw::addr<FAMILY>::type);

				size_type ret = buf.sync([this, flags, &sa, &sa_len](void *buf, size_type size){
					ssize_t	ret = _s_recvfrom(this->_fd, buf, size, flags, &sa, &sa_len);
					if (ret == -1)
						throw system_error(errno, std::generic_category(), "recvfrom");
					return ret;
				});
				addr = nw::addr<FAMILY>(sa);
				return ret;
			}

			size_type recv(
				struct msghdr msg,
				int flags
			) {
				static_cast<void>(msg);
				static_cast<void>(flags);
				//recvmsg
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
				socket<sa_family::INET, TYPE> &&src		//!< nw::sa_family::INET nw::socket
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
