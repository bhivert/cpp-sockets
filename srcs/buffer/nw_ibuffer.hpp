#ifndef __NW_IBUFFER_HPP__
# define __NW_IBUFFER_HPP__

/*!
@file nw_ibuffer.hpp
@brief ...
*/

# include <ostream>
# include <string>

# include "nw_buffer.hpp"

namespace nw {
	template <size_type SIZE>
	class ibuffer : public buffer<SIZE> {
		public:
			ibuffer(void) : buffer<SIZE>() {};
			virtual	~ibuffer(void) {}

			virtual int	sync(const typename nw::buffer<SIZE>::sync_fct_t fct) {
				if (this->is_full())
					return 0;
				ssize_t ret = fct(&this->_buf[this->_off.put], (this->_off.put < this->_off.get) ? this->_off.get - this->_off.put : this->size() - this->_off.put);
				if (!ret) {
					this->_stats.eof = true;
					return 0;
				}
				if (!(ret > 0))
					return -1;
				this->_off.put = (this->_off.put + ret) % this->size();
				if (this->_off.get == this->_off.put)
					this->_stats.is_full = true;
				return 0;
			}

		protected:
		private:
			ibuffer(const ibuffer &src) = delete;
			ibuffer(ibuffer &&src) = delete;

			ibuffer &	operator=(const ibuffer &src) = delete;
			ibuffer &	operator=(ibuffer &&src) = delete;
	};
};

template <nw::size_type SIZE>
std::ostream &	operator<<(std::ostream &o, const nw::ibuffer<SIZE> &C) {
}

#endif
