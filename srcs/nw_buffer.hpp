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

# include "nw_typedef.hpp"

namespace nw {
	template <size_type SIZE>
	class buffer {
		public:

			enum class	dir	: bool {
				out	= 0,
				in	= 1
			};

			typedef std::function<ssize_t(void *, size_type)>	sync_fct;

			buffer(const dir &d = dir::out, const sync_fct &fct = [](void *, size_type){ return 0; }) \
				: _sync_fct(fct), \
				_buf{0}, \
				_stats{false, false, static_cast<bool>(d)}, \
				_off{0, 0}, \
				_sync_off(static_cast<bool>(d) ? this->_off.put : this->_off.get) {
			}

			virtual	~buffer(void) {}

			const std::string	to_string(void) const {
				std::string str;

				str = "{\n\"get_off\" : " + std::to_string(this->_off.get) + ", \n";
				str += "\"put_off\" : " + std::to_string(this->_off.put) + ", \n";
				str += "\"in_avail\" : " + std::to_string(this->in_avail()) + ", \n";
				str += "\"full\" : " + std::string((this->is_full()) ? "true" : "false") + ", \n";
				str += "\"data\" : [ ";
				for (nw::size_type i = 0; i != this->size(); ++i) {
					str += std::to_string(this->_buf[i]);
					if (i + 1 != this->size())
						str += ", ";
				}
				str += " ]\n}";

				return str;
			}

			size_type	size(void) const {
				return SIZE;
			}

			bool		is_full(void) const {
				return this->_stats.is_full;
			}

			bool		is_empty(void) const {
				return !this->_stats.is_full && this->_off.get == this->_off.put;
			}

			void		clear(void) {
				this->_off = {0, 0};
				this->_stats = {false, false, this->_stats.dir};
			}

			bool		eof(void) const {
				return this->_stats.eof;
			}

			size_type	in_avail(void) const {
				if (this->is_empty())
					return 0;
				if (this->_off.get < this->_off.put)
					return this->_off.put - this->_off.get;
				return (this->size() - this->_off.get) + (this->_off.put);
			}

			int			sync(void) {
				if ((this->_stats.dir && this->is_full()) || (!this->_stats.dir && this->is_empty())) {
					return 0;
				}
				ssize_t ret = this->_sync_fct(&this->_buf[this->_sync_off], this->_sync_avail[this->_stats.dir]());
				if (!ret) {
					this->_stats.eof = true;
				} else if (ret > 0) {
					this->_sync_off = (this->_sync_off + ret) % this->size();
					if (this->_off.get == this->_off.put) {
						if (this->_stats.dir) {
							this->_stats.is_full = true;
						} else {
							this->_off = {0, 0};
						}
					}
				} else {
					return -1;
				}
				return 0;
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
					this->_off.get += gb_size;
				}
				if (gf_size) {
					std::memcpy(static_cast<int8_t *>(b) + gb_size, this->_buf, gf_size);
					this->_off.get = gf_size;
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
					this->_off.put += pb_size;
				}
				if (pf_size) {
					std::memcpy(this->_buf, static_cast<const int8_t *>(b) + pb_size, pf_size);
					this->_off.put = pf_size;
				}
				if (this->_off.get == this->_off.put)
					this->_stats.is_full = true;
				return pb_size + pf_size;
			}

		protected:
			const sync_fct	_sync_fct;
			int8_t			_buf[SIZE];
			struct	{
				uint8_t		is_full	: 1;
				uint8_t		eof		: 1;
				uint8_t				: 5;
				uint8_t		dir		: 1;
			}				_stats;
			struct	{
				size_type	get;
				size_type	put;
			}				_off;

			size_type		&_sync_off;

			std::function<size_type(void)>	_sync_avail[2] = {
				[this](){ return (this->_off.get < this->_off.put) ? this->_off.put - this->_off.get : this->size() - this->_off.get; },
				[this](){ return (this->_off.put < this->_off.get) ? this->_off.get - this->_off.put : this->size() - this->_off.put; }
			};

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
