#ifndef _GDSTL_CONTAINER_BASE_H
#define _GDSTL_CONTAINER_BASE_H

#include "allocator.h"

namespace gdstd
{
    template <typename T>
	struct VecNonTrivialImpl
		: ::std::false_type {};
}

#endif /* _GDSTL_CONTAINER_BASE_H */ 