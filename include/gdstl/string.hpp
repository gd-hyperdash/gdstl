#ifndef _GDSTL_STRING_HPP
#define _GDSTL_STRING_HPP

#include "allocator.hpp"

#include <string>

namespace gdstd
{
#if defined(GDSTL_ENABLED)
#if defined(GDSTL_USES_MSVC)
	template <typename T>
	class basic_string
	{
		template <typename T>
		friend ::std::basic_string<T> to_basic_string(basic_string<T> const&);

		static auto constexpr BUF_SIZE = 16u;
		static auto constexpr MAX_CHAR_SIZE = BUF_SIZE / sizeof(T);

		union StringBase
		{
			T* m_P;
			::std::uint8_t m_Buffer[BUF_SIZE];
		};

	protected:
		StringBase m_Data;
		::std::size_t m_Length;
		::std::size_t m_Capacity;

		void init_data(
			void const* p,
			::std::size_t const length)
		{
			m_Length = length;
			::std::memset(&m_Data, 0x00, sizeof(StringBase));

			if (m_Length <= MAX_CHAR_SIZE)
			{
				m_Capacity = MAX_CHAR_SIZE;

				::std::memcpy(
					reinterpret_cast<void*>(&m_Data),
					p,
					m_Length * sizeof(T));
			}
			else
			{
				m_Capacity = gdstd::round_size(m_Length);
				m_Data.m_P = ::gdstd::allocate<T*>(m_Capacity);

				::std::memcpy(
					reinterpret_cast<void*>(m_Data.m_P),
					p,
					m_Length * sizeof(T));
			}
		}

		basic_string(basic_string<T> const& rhs)
		{
			auto length = rhs.m_Length;

			if (length <= MAX_CHAR_SIZE)
			{
				init_data(
					reinterpret_cast<void const*>(&rhs.m_Data),
					length);
			}
			else
			{
				init_data(
					reinterpret_cast<void const*>(rhs.m_Data.m_P),
					length);
			}
		}

	public:
		basic_string() = delete;
		basic_string(basic_string<T>&&) = delete;

		~basic_string()
		{
			if (m_Length > MAX_CHAR_SIZE)
			{
				::gdstd::free(m_Data.m_P);
				m_Data.m_P = nullptr;
			}
		}

		basic_string<T>& operator=(basic_string<T> const&) = delete;

		basic_string(::std::basic_string<T> const& rhs)
		{
			init_data(
				reinterpret_cast<void const*>(rhs.data()),
				rhs.length());
		}
	};

	template <typename T>
	::std::basic_string<T> to_basic_string(basic_string<T> const& v)
	{
		if (v.m_Length <= v.MAX_CHAR_SIZE)
		{
			return ::std::basic_string<T>(
				reinterpret_cast<T const*>(&v.m_Data.m_Buffer),
				v.m_Length);
		}
		return ::std::basic_string<T>(v.m_Data.m_P, v.m_Length);
	}
#else
	template <typename T>
	class basic_string
	{
		template <typename T>
		friend ::std::basic_string<T> to_basic_string(basic_string<T> const&);
		
		struct RepBase
		{
			::std::size_t m_Length;
			::std::size_t m_Capacity;
			::std::size_t m_RefCount;
		};
	protected:
		T* m_P;

		RepBase const* data() const
		{
			return reinterpret_cast<RepBase*>(
				reinterpret_cast<::std::uintptr_t>(m_P) -
				sizeof(RepBase));
		}

		RepBase* data()
		{
			return reinterpret_cast<RepBase*>(
				reinterpret_cast<::std::uintptr_t>(m_P) -
				sizeof(RepBase));
		}

		basic_string(basic_string<T> const& rhs)
		{
			if (this == &rhs)
				return;

			m_P = rhs.m_P;
			++data()->m_RefCount;
		}
	public:
		basic_string() = delete;
		basic_string(basic_string<T>&&) = delete;

		~basic_string()
		{
			--data()->m_RefCount;

			if (data()->m_RefCount <= 0u)
			{
				::gdstd::free(m_P);
			}
		}

		basic_string<T>& operator=(basic_string<T> const&) = delete;

		basic_string(::std::basic_string<T> const& rhs)
			: m_P(nullptr)
		{
			RepBase rep = {};

			rep.m_Length = rhs.size();
			rep.m_Capacity = rhs.size();
			rep.m_RefCount = 0u;

			auto const allocSize = ::gdstd::round_size(
				sizeof(RepBase) + ((rep.m_Capacity + 1u) * sizeof(T)));

			m_P = ::gdstd::allocate<T*>(allocSize);

			::std::memset(reinterpret_cast<void*>(m_P), 0x00, allocSize);
			::std::memcpy(m_P, &rep, sizeof(RepBase));

			m_P = reinterpret_cast<T*>(
				reinterpret_cast<::std::uintptr_t>(m_P) + sizeof(RepBase));
		}
	};

	template <typename T>
	::std::basic_string<T> to_basic_string(basic_string<T> const& v)
	{
		return ::std::basic_string<T>(v.m_P, v.data()->m_Length);
	}
#endif // GDSTL_USES_MSVC
#else
	template <typename T>
	struct basic_string
		: private ::std::basic_string<T>
	{
		template <typename T>
		friend ::std::basic_string<T> to_basic_string(basic_string<T> const&);
	};

	template <typename T>
	::std::basic_string<T> to_basic_string(basic_string<T> const& v)
	{
		return v;
	}
#endif // GDSTL_ENABLED

	using string = ::gdstd::basic_string<char>;
	using wstring = ::gdstd::basic_string<wchar_t>;

	static auto constexpr& to_string = to_basic_string<char>;
	static auto constexpr& to_wstring = to_basic_string<wchar_t>;
}

#endif /* _GDSTL_STRING_HPP */