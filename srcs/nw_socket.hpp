
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

static const std::function<int(int, int, int)>								_s_socket = &socket;
static const std::function<int(int, const struct sockaddr *, socklen_t)>	_s_bind = &bind;
static const std::function<int(int, int)>									_s_listen = &listen;
static const std::function<int(int, const struct sockaddr *, socklen_t)>	_s_connect = &connect;
static const std::function<int(int, struct sockaddr *, socklen_t *)>		_s_accept = &accept;

# include "nw_typedef.hpp"
# include "nw_protoent.hpp"
# include "nw_addr.hpp"
//# include "nw_addrinfo.hpp"
//# include "nw_sockopt.hpp"
//# include "nw_buffer.hpp"

namespace nw {
	template <sa_family FAMILY>
	class socket_interface {
		public:
			virtual const std::string	to_string(void) const {
				std::string	str;

				str = "{ \"family\": \"" + sa_family_str(FAMILY) + "\", ";
				str += "\"type\": \"" + sock_type_str(this->_type) + "\", ";
				str += "\"protocol\": " + this->_proto.to_string() + ", ";
				str += "\"address\": " + this->_addr.to_string() + "}";

				return str;
			};

			void	listen(int backlog) {
				if (_s_listen(this->_fd, backlog) == -1)
					throw system_error(errno, std::generic_category(), "listen");
			}

			void	bind(const addr<FAMILY> &addr) {
				this->_addr = addr;
				if (_s_bind(this->_fd, reinterpret_cast<const sockaddr *>(&this->_addr._struct), this->_addr._sizeof) == -1)
					throw system_error(errno, std::generic_category(), "bind");
			}

			void	connect(const addr<FAMILY> &addr) {
				this->_addr = addr;
				if (_s_connect(this->_fd, reinterpret_cast<const sockaddr *>(&this->_addr._struct), this->_addr._sizeof) == -1)
					throw system_error(errno, std::generic_category(), "connect");
			}

		protected:
			const sock_type		_type;
			const protoent		_proto;
			const sockfd_type	_fd;
			addr<FAMILY>		_addr;

			socket_interface(const sock_type &type, const protoent &proto, const sockfd_type &fd) \
				: _type(type), _proto(proto), _fd(fd) {}
			socket_interface(const sock_type &type, const protoent &proto, const sockfd_type &fd, const addr<FAMILY> &a) \
				: _type(type), _proto(proto), _fd(fd), _addr(a) {}

			virtual	~socket_interface(void) {}

			template <sa_family, sock_type>
			friend class socket;

		private:
			socket_interface(const socket_interface &src) = delete;
			socket_interface(socket_interface &&src) = delete;

			socket_interface &	operator=(const socket_interface &src) = delete;
			socket_interface &	operator=(socket_interface &&src) = delete;
	};

	template <sa_family FAMILY, sock_type TYPE>
	class socket : public socket_interface<FAMILY> {
		public:
			socket(const int &proto_id) : socket(nw::protoent(proto_id)) {}
			socket(const std::string &proto_name) : socket(nw::protoent(proto_name)) {}
			socket(const protoent &proto) \
				: socket_interface<FAMILY>(TYPE, proto, _s_socket(static_cast<int32_t>(FAMILY), static_cast<int32_t>(TYPE), proto._struct->p_proto)) {
				if (this->_fd == -1)
					throw system_error(errno, std::generic_category(), "socket");
			}

			socket(const socket<FAMILY, TYPE> &src) \
				: socket_interface<FAMILY>(src._type, src._proto, src._fd, src._addr) {}
			socket(socket<FAMILY, TYPE> &&src) \
				: socket_interface<FAMILY>(src._type, src._proto, src._fd, src._addr) {}

			socket(const socket<sa_family::UNSPEC, TYPE> &src) \
				: socket_interface<FAMILY>(src._type, src._proto, src._fd, src._addr) {}
			socket(socket<sa_family::UNSPEC, TYPE> &&src) \
				: socket_interface<FAMILY>(src._type, src._proto, src._fd, src._addr) {}

			socket<sa_family::UNSPEC, TYPE>	accept(void) {
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
						throw logic_error("accept: invalid address type");
				}
			}

			virtual	~socket(void) {}

		protected:
			socket(const protoent &proto, const sockfd_type &fd, const addr<FAMILY> &a) \
				: socket_interface<FAMILY>(TYPE, proto, fd, a) {}

			template <sa_family, sock_type>
			friend class socket;

		private:
			socket(void) = delete;

			socket &	operator=(const socket &src) = delete;
			socket &	operator=(socket &&src) = delete;
	};

	template <sock_type TYPE>
	class socket<sa_family::UNSPEC, TYPE> : public socket_interface<sa_family::UNSPEC> {
		public:
			socket(const socket<sa_family::INET, TYPE> &src) \
				: socket_interface<sa_family::UNSPEC>(src._type, src._proto, src._fd, src._addr) {}
			socket(const socket<sa_family::INET6, TYPE> &src) \
				: socket_interface<sa_family::UNSPEC>(src._type, src._proto, src._fd, src._addr) {}

			socket(const socket &src) \
				: socket_interface<sa_family::UNSPEC>(src._type, src._proto, src._fd, src._addr) {}
			socket(socket &&src) \
				: socket_interface<sa_family::UNSPEC>(src._type, src._proto, src._fd, src._addr) {}


			virtual	~socket(void) {}

		protected:


		private:
			socket &	operator=(const socket &src) = delete;
			socket &	operator=(socket &&src) = delete;
	};

	template <sock_type TYPE>
	class socket<sa_family::INET6V4M, TYPE> {
		public:
			virtual	~socket(void) {}

		protected:
		private:
			socket(const socket &src) = delete;
			socket(socket &&src) = delete;

			socket(const int &proto_id = 0) = delete;
			socket(const std::string &proto_name) = delete;
			socket(const protoent &proto) = delete;

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
