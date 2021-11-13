
#ifndef __NW_EXCEPTION_HPP__
# define __NW_EXCEPTION_HPP__

/*!
@file nw_exception.hpp
@brief ...
*/

# include <ostream>
# include <string>
# include <exception>

namespace nw {
	class exception : public std::exception {
		public:
			virtual const char *	what(void) const noexcept = 0;
	};
};

std::ostream &	operator<<(std::ostream &o, const nw::exception &C);

#endif
