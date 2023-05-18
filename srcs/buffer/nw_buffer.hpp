#ifndef __NW_BUFFER_HPP__
# define __NW_BUFFER_HPP__

/*!
@file nw_buffer.hpp
@brief ...
*/

# include <ostream>
# include <string>
# include <functional>
# include <cstring>
# include <cctype>

# include "../nw_typedef.hpp"

namespace nw {
	template <size_type SIZE>
	class buffer {
		public:
			typedef std::function<ssize_t(void *, size_type)>	sync_fct_t;

			buffer(void)\
				: _buf{0}, \
				_stats{false, false}, \
				_off{0, 0} {
			}

			virtual	~buffer(void) {}

			const std::string	to_string(void) const {
				std::string str;

				str = "{\"get_off\" : " + std::to_string(this->_off.get) + ", ";
				str += "\"put_off\" : " + std::to_string(this->_off.put) + ", ";
				str += "\"in_avail\" : " + std::to_string(this->in_avail()) + ", ";
				str += "\"full\" : " + std::string((this->is_full()) ? "true" : "false") + ", ";
				str += "\"data\" : [ ";
				for (nw::size_type i = 0; i != this->size(); ++i) {
					str += std::to_string(static_cast<uint8_t>(this->_buf[i]));
					if (i + 1 != this->size())
						str += ", ";
				}
				str += " ]}";

				return str;
			}

			inline size_type	size(void) const {
				return SIZE;
			}

			inline bool		is_full(void) const {
				return this->_stats.is_full;
			}

			inline bool		is_empty(void) const {
				return !this->_stats.is_full && this->_off.get == this->_off.put;
			}

			inline bool		eof(void) const {
				return this->_stats.eof;
			}

			void		clear(void) {
				this->_off = {0, 0};
				this->_stats = {false, false};
			}

			size_type	in_avail(void) const {
				if (this->is_empty())
					return 0;
				if (this->_off.get < this->_off.put)
					return this->_off.put - this->_off.get;
				return (this->size() - this->_off.get) + (this->_off.put);
			}

			virtual int	sync(const sync_fct_t fct = [](void *, size_type){ return 0; }) {
				return fct(nullptr, 0);
			}

			size_type	getn(void *b, size_type n) {
				if (!n || this->is_empty())
					return 0;
				this->_stats.is_full = false;
				if (this->_off.get < this->_off.put) {
					size_type	get_size = std::min(n, this->_off.put - this->_off.get);

					std::memcpy(b, this->_buf + this->_off.get, get_size);
					this->_off.get += get_size;
					if (this->_off.get == this->_off.put)
						this->_off = {0, 0};
					return get_size;
				}

				size_type	gb_size = std::min(n, this->size() - this->_off.get);
				size_type	gf_size = std::min(n - gb_size, this->_off.put);

				if (gb_size) {
					std::memcpy(b, this->_buf + this->_off.get, gb_size);
					this->_off.get = (this->_off.get + gb_size) % this->size();
				}
				if (gf_size) {
					std::memcpy(static_cast<int8_t *>(b) + gb_size, this->_buf, gf_size);
					this->_off.get = (this->_off.get + gf_size) % this->size();
				}
				if (this->_off.get == this->_off.put)
					this->_off = {0, 0};
				return gb_size + gf_size;
			}

			size_type	putn(const void *b, size_type n) {
				if (!n || this->is_full())
					return 0;
				if (this->_off.put < this->_off.get) {
					size_type	put_size = std::min(n, this->_off.get - this->_off.put);

					std::memcpy(this->_buf + this->_off.put, b, put_size);
					this->_off.put += put_size;
					if (this->_off.get == this->_off.put)
						this->_stats.is_full = true;
					return put_size;
				}

				size_type	pb_size = std::min(n, this->size() - this->_off.put);
				size_type	pf_size = std::min(n - pb_size, this->_off.get);

				if (pb_size) {
					std::memcpy(this->_buf + this->_off.put, b, pb_size);
					this->_off.put = (this->_off.put + pb_size) % this->size();
				}
				if (pf_size) {
					std::memcpy(this->_buf, static_cast<const int8_t *>(b) + pb_size, pf_size);
					this->_off.put = (this->_off.put + pf_size) % this->size();
				}
				if (this->_off.get == this->_off.put)
					this->_stats.is_full = true;
				return pb_size + pf_size;
			}

		protected:
			int8_t			_buf[SIZE];
			struct	{
				uint8_t		is_full	: 1;
				uint8_t		eof		: 1;
				uint8_t				: 6;
			}				_stats;
			struct	{
				pos_type	get;
				pos_type	put;
			}				_off;

		private:
			buffer(const buffer &src) = delete;
			buffer(buffer &&src) = delete;

			buffer &	operator=(const buffer &src) = delete;
			buffer &	operator=(buffer &&src) = delete;
	};
};

template <nw::size_type SIZE>
std::ostream &	operator<<(std::ostream &o, const nw::buffer<SIZE> &C) {
	o << C.to_string();
	return o;
}

#endif
