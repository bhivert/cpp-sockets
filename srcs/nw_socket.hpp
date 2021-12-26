
#ifndef __NW_SOCKET_HPP__
# define __NW_SOCKET_HPP__

/*!
@file nw_socket.hpp
@brief ...
*/

# include <ostream>
# include <string>

# include "nw_typedef.hpp"
# include "nw_buffer.hpp"

namespace nw {
	template <size_type SIZE>
	class socket {
		public:
			virtual	~socket(void);

			virtual const std::string	to_string(void) const = 0;

			void	open(void);
			void	close(void);

		protected:
			;
			fd_type		_fd;

		private:
			socket(void) = delete;
			socket(const socket &src) = delete;
			socket(socket &&src) = delete;

			socket &	operator=(const socket &src) = delete;
			socket &	operator=(socket &&src) = delete;
	};

//setsockopt

//	template <sock_use USE, sa_family FAMILY>
//	class socket {
//		public:
//			virtual	~socket(void);
//
//			const std::string	to_string(void) const;
//
//		protected:
//		private:
//			socket(void) = delete;
//			socket(const socket &src) = delete;
//			socket(socket &&src) = delete;
//
//			socket &	operator=(const socket &src) = delete;
//			socket &	operator=(socket &&src) = delete;
//	};

};

//template <nw::sock_use USE, nw::sa_family FAMILY>
//std::ostream &	operator<<(std::ostream &o, const nw::socket<USE, FAMILY> &C);

#endif
