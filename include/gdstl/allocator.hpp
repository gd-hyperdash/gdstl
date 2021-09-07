#ifndef _GDSTL_ALLOCATOR_HPP
#define _GDSTL_ALLOCATOR_HPP

#include "platform.hpp"

#include <type_traits>

namespace gdstd
{
	// Must be a power of 2!
	static auto constexpr ALLOC_SIZE = 0x1000;

	extern "C" ::std::uintptr_t gdstd_allocate_raw(::std::size_t const size);
	extern "C" void gdstd_free_raw(::std::uintptr_t p);

	inline ::std::size_t round_size(::std::size_t const size)
	{
		return ALLOC_SIZE + (size & ~(ALLOC_SIZE - 1u));
	}

	template <typename T>
	auto allocate(::std::size_t const size)
		-> typename ::std::enable_if_t<
		::std::is_pointer_v<T>,
		T>
	{
		if (!(size & (ALLOC_SIZE - 1u)))
		{
			return reinterpret_cast<T>(
				gdstd_allocate_raw(size));
		}
		
		return nullptr;
	}

	template <typename T>
	auto free(T p)
		-> typename ::std::enable_if_t<
		::std::is_pointer_v<T>>
	{
		gdstd_free_raw(reinterpret_cast<::std::uintptr_t>(p));
	}
}

#endif /* _GDSTL_ALLOCATOR_HPP */