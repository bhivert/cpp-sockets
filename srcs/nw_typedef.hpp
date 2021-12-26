
#ifndef __NW_TYPEDEF_HPP__
# define __NW_TYPEDEF_HPP__

/*!
@file nw_typedef.hpp
@brief ...
*/

# include <string>
# include <cerrno>
# include <stdexcept>
# include <system_error>

# include <netdb.h>

namespace nw {
	enum class	sa_family : sa_family_t {
		UNSPEC		= AF_UNSPEC,
		INET		= AF_INET,
		INET6		= AF_INET6,
		INET6V4M	= static_cast<sa_family_t>(~0)
	};

//	enum class	sock_use	: uint8_t {
//		CONNECT		= 0,
//		BIND		= AI_PASSIVE
//	};

	enum class	sock_type : int32_t {
		UNSPEC		= 0,
		STREAM		= SOCK_STREAM,
		DGRAM		= SOCK_DGRAM,
		SEQPACKET	= SOCK_SEQPACKET,
		RAW			= SOCK_RAW,
		RDM			= SOCK_RDM
	};

	typedef socklen_t	socklen;
	typedef in_port_t	port_type;
	typedef int32_t		proto_id;
	typedef int32_t		fd_type;

	typedef size_t		size_type;
	typedef size_type	pos_type;

	typedef std::exception		exception;
	typedef std::bad_alloc		bad_alloc;
	typedef std::logic_error	logic_error;
	typedef std::system_error	system_error;

	const std::string &	sa_family_str(const sa_family &family);
//	const std::string &	sock_use_str(const sock_use &use);
	const std::string &	sock_type_str(const sock_type &type);
};

#endif
