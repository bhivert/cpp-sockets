
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
	//! @enum sa_family
	enum class	sa_family : sa_family_t {
		UNSPEC		= AF_UNSPEC,					//!< Unspecified Internet family
		INET		= AF_INET,						//!< IPv4 Internet family
		INET6		= AF_INET6,						//!< IPv6 Internet family
		INET6V4M	= static_cast<sa_family_t>(~0)	//!< IPv6 with IPv4 mapped Internet family
	};

	//! @enum sock_type
	enum class	sock_type : int32_t {
		UNSPEC		= 0,				//!< Unspecified socket type
		STREAM		= SOCK_STREAM,		//!< Provides sequenced, reliable, two-way, connection-based byte streams. An out-of-band data transmission mechanism may be supported.
		DGRAM		= SOCK_DGRAM,		//!< Supports datagrams (connectionless, unreliable messages of a fixed maximum length).
		SEQPACKET	= SOCK_SEQPACKET,	//!< Provides a sequenced, reliable, two-way connection-based data transmission path for datagrams of fixed maximum length; a consumer is required to read an entire packet with each input system call.
		RAW			= SOCK_RAW,			//!< Provides raw network protocol access.
		RDM			= SOCK_RDM			//!< Provides a reliable datagram layer that does not guarantee ordering.
	};

	typedef socklen_t	socklen_type;
	typedef in_port_t	port_type;
	typedef int32_t		proto_id;
	typedef int32_t		sockfd_type;

	typedef size_t		size_type;
	typedef size_type	pos_type;

	typedef std::exception		exception;
	typedef std::bad_alloc		bad_alloc;
	typedef std::logic_error	logic_error;
	typedef std::system_error	system_error;

	const std::string &	sa_family_str(const sa_family &family);
	const std::string &	sock_type_str(const sock_type &type);
};

#endif
