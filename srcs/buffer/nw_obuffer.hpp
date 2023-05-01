#ifndef __NW_OBUFFER_HPP__
# define __NW_OBUFFER_HPP__

/*!
@file nw_obuffer.hpp
@brief ...
*/

# include <ostream>
# include <string>

# include "nw_buffer.hpp"

namespace nw {
	template <size_type SIZE>
	class obuffer : public buffer<SIZE> {
		public:
			obuffer(void) : buffer<SIZE>() {};
			virtual	~obuffer(void) {};

			const std::string	to_string(void) const;

			virtual int	sync(const typename nw::buffer<SIZE>::sync_fct_t fct) {
				if (this->is_empty())
					return 0;
				ssize_t ret = fct(&this->_buf[this->_off.get], (this->_off.get < this->_off.put) ? this->_off.put - this->_off.get : this->size() - this->_off.get);
				if (!ret) {
					this->_stats.eof = true;
					return 0;
				}
				if (!(ret > 0))
					return -1;
				this->_off.get = (this->_off.get + ret) % this->size();
				if (this->_off.get == this->_off.put)
					this->_off = {0, 0};
				return 0;
			}

		protected:
		private:
			obuffer(const obuffer &src) = delete;
			obuffer(obuffer &&src) = delete;

			obuffer &	operator=(const obuffer &src) = delete;
			obuffer &	operator=(obuffer &&src) = delete;
	};
};

template <nw::size_type SIZE>
std::ostream &	operator<<(std::ostream &o, const nw::obuffer<SIZE> &C) {
}

#endif
