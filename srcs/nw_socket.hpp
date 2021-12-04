
#ifndef __NW_SOCKET_HPP__
# define __NW_SOCKET_HPP__

/*!
@file nw_socket.hpp
@brief ...
*/

# include <ostream>
# include <string>

# include "nw_typedef.hpp"

namespace nw {
	class socket_interface {
		public:
			virtual	~socket_interface(void);

			virtual const std::string	to_string(void) const = 0;

			void	open(void);
			void	close(void);

		protected:
		private:
			socket_interface(void) = delete;
			socket_interface(const socket_interface &src) = delete;
			socket_interface(socket_interface &&src) = delete;

			socket_interface &	operator=(const socket_interface &src) = delete;
			socket_interface &	operator=(socket_interface &&src) = delete;
	};

	template <sock_use USE, sa_family FAMILY>
	class socket {
		public:
			virtual	~socket(void);

			const std::string	to_string(void) const;

		protected:
		private:
			socket(void) = delete;
			socket(const socket &src) = delete;
			socket(socket &&src) = delete;

			socket &	operator=(const socket &src) = delete;
			socket &	operator=(socket &&src) = delete;
	};

};

template <nw::sock_use USE, nw::sa_family FAMILY>
std::ostream &	operator<<(std::ostream &o, const nw::socket<USE, FAMILY> &C);

#endif
