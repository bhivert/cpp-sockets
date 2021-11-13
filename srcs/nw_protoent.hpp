
#ifndef __NW_PROTOENT_HPP__
# define __NW_PROTOENT_HPP__

/*!
@file nw_proto.hpp
@brief ...
*/

# include <ostream>
# include <string>
# include <memory>

# include <netdb.h>

# include "nw_typedef.hpp"
# include "nw_exception.hpp"

typedef struct protoent		protoent_struct;

namespace nw {
	template <sa_family FAMILY>
		class addrinfo_interface;

	class protoent {
		public:
			typedef protoent_struct		type;

			class proto_not_found : public nw::exception {
				virtual const char	*what(void) const noexcept;
			};

			protoent(const std::string &proto_name);
			protoent(const proto_id &proto_id);

			protoent(const protoent &src);

			virtual	~protoent(void);

			const std::string	to_string(void) const;

		protected:
			std::shared_ptr<const type>	_struct;

			template <sa_family FAMILY>
			friend class addrinfo_interface;

		private:
			protoent(void) = delete;
			protoent(protoent &&src) = delete;

			protoent &	operator=(const protoent &src) = delete;
			protoent &	operator=(protoent &&src) = delete;
	};
};

std::ostream &	operator<<(std::ostream &o, const nw::protoent &C);

#endif
