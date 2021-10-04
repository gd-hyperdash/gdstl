#ifndef _GDSTL_CONTAINER_BASE_HPP
#define _GDSTL_CONTAINER_BASE_HPP

#include "allocator.hpp"

namespace gdstd
{
    template <typename T>
	struct VecNonTrivialImpl
		: ::std::false_type {};
}

#endif /* _GDSTL_CONTAINER_BASE_HPP */ 