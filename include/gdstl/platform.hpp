#ifndef _GDSTL_PLATFORM_HPP
#define _GDSTL_PLATFORM_HPP

#include <cstdlib>
#include <cstdint>

// Check if we're using the MS STL.
#if defined(_VCRUNTIME_H)
#define GDSTL_USES_MSVC
#endif

#endif /* _GDSTL_PLATFORM_HPP */