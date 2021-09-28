#ifndef _GDSTL_ALLOCATOR_HPP
#define _GDSTL_ALLOCATOR_HPP

#include "platform.hpp"

#include <type_traits>

namespace gdstd
{
	extern "C" ::std::uintptr_t gdstd_allocate_raw(::std::size_t const size);
	extern "C" void gdstd_free_raw(::std::uintptr_t p);

	template <typename T>
	typename ::std::enable_if<
		::std::is_pointer<T>::value,
		T>::type allocate(::std::size_t const size)
	{
		return reinterpret_cast<T>(
			gdstd_allocate_raw(size));
	}

	template <typename T>
	typename ::std::enable_if<
		::std::is_pointer<T>::value
		>::type free(T p)
	{
		gdstd_free_raw(reinterpret_cast<::std::uintptr_t>(p));
	}
}

#endif /* _GDSTL_ALLOCATOR_HPP */