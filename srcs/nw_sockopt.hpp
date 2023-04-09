#ifndef __NW_SOCKOPT_HPP__
# define __NW_SOCKOPT_HPP__

/*!
@file nw_sockopt.hpp
@brief ...
*/

# include <ostream>
# include <string>

# include "nw_typedef.hpp"
# include "nw_protoent.hpp"

namespace nw {
	class sockopt {
		public:
			sockopt(void);
			virtual	~sockopt(void);

			const std::string	to_string(void) const;

			virtual void	set(void) {};
			virtual void	get(void) = 0;

		protected:
		private:
			sockopt(const sockopt &src) = delete;
			sockopt(sockopt &&src) = delete;

			sockopt &	operator=(const sockopt &src) = delete;
			sockopt &	operator=(sockopt &&src) = delete;
	};
};
std::ostream &	operator<<(std::ostream &o, const nw::sockopt &C);

#endif

//SO_ACCEPTCONN
//SO_ATTACH_FILTER (since Linux 2.2), SO_ATTACH_BPF (since Linux 3.19)
//SO_ATTACH_REUSEPORT_CBPF, SO_ATTACH_REUSEPORT_EBPF
//SO_BINDTODEVICE
//SO_BROADCAST
//SO_BSDCOMPAT
//SO_DEBUG
//SO_DETACH_FILTER (since Linux 2.2), SO_DETACH_BPF (since Linux 3.19)
//SO_DOMAIN (since Linux 2.6.32)
//SO_ERROR
//SO_DONTROUTE
//SO_INCOMING_CPU (gettable since Linux 3.19, settable since Linux 4.4)
//SO_KEEPALIVE
//SO_LINGER
//SO_LOCK_FILTER
//SO_MARK (since Linux 2.6.25)
//SO_OOBINLINE
//SO_PASSCRED
//SO_PEEK_OFF (since Linux 3.4)
//SO_PEERCRED
//SO_PRIORITY
//SO_PROTOCOL (since Linux 2.6.32)
//SO_RCVBUF
//SO_RCVBUFFORCE (since Linux 2.6.14)
//SO_RCVLOWAT and SO_SNDLOWAT
//SO_RCVTIMEO and SO_SNDTIMEO
//SO_REUSEADDR
//SO_REUSEPORT (since Linux 3.9)
//SO_RXQ_OVFL (since Linux 2.6.33)
//SO_SNDBUF
//SO_SNDBUFFORCE (since Linux 2.6.14)
//SO_TIMESTAMP
//SO_TYPE
//SO_BUSY_POLL (since Linux 3.11)

