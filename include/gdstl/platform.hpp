#ifndef _GDSTL_PLATFORM_HPP
#define _GDSTL_PLATFORM_HPP

#include <cstdlib>
#include <cstdint>

// Check if we're using the MS STL.
#if defined(_VCRUNTIME_H)
#define GDSTL_USES_MSVC

// We only need to enable under debug mode.
#if defined(_DEBUG)
#define GDSTL_ENABLED
#endif
#else
// Always enable for other target.
#define GDSTL_ENABLED
#endif

#endif /* _GDSTL_PLATFORM_HPP */