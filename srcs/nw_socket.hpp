
#ifndef __NW_SOCKET_HPP__
# define __NW_SOCKET_HPP__

/*!
@file socket.cpp
@brief ...
*/

# include <ostream>
# include <string>

# include "nw_typedef.hpp"

namespace nw {
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

std::ostream &	operator<<(std::ostream &o, const nw::socket &C);

#endif
