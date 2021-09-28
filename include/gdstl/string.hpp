#ifndef _GDSTL_STRING_HPP
#define _GDSTL_STRING_HPP

#include "allocator.hpp"

#include <string>

namespace gdstd
{
#if defined(GDSTL_USES_MSVC)
	template <typename T>
	class basic_string
	{
		template <typename R>
		friend ::std::basic_string<R> to_basic_string(basic_string<R> const&);

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
			m_Capacity = length;
			::std::memset(&m_Data, 0x00, sizeof(StringBase));

			if (m_Length < MAX_CHAR_SIZE)
			{
				::std::memcpy(
					reinterpret_cast<void*>(&m_Data),
					p,
					m_Length * sizeof(T));
			}
			else
			{
				m_Data.m_P = ::gdstd::allocate<T*>(m_Length * sizeof(T));

				::std::memset(
					reinterpret_cast<void*>(m_Data.m_P),
					0x00,
					m_Length * sizeof(T));

				::std::memcpy(
					reinterpret_cast<void*>(m_Data.m_P),
					p,
					m_Length * sizeof(T));
			}
		}

		void clear_data()
		{
			if (m_Length >= MAX_CHAR_SIZE)
				::gdstd::free(m_Data.m_P);

			m_Data = {};
			m_Length = 0u;
			m_Capacity = 0u;
		}

		bool is_valid() const
		{
			return m_Capacity != 0u;
		}

		basic_string()
			: m_Data({}), m_Length(0u), m_Capacity(0u) {}

	public:
		basic_string(basic_string<T>&&) = delete;

		~basic_string()
		{
			clear_data();
		}

		basic_string(basic_string<T> const& rhs)
			: basic_string()
		{
			assign(rhs);
		}

		basic_string(::std::basic_string<T> const& rhs)
			: basic_string()
		{
			assign(rhs);
		}

		basic_string(T const* p)
			: basic_string(::std::basic_string<T>(p)) {}

		basic_string<T>& operator=(basic_string<T> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		basic_string<T>& operator=(::std::basic_string<T> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		basic_string<T>& operator=(T const* p)
		{
			assign(p);
			return *this;
		}

		void assign(basic_string<T> const& rhs)
		{
			if (this == &rhs)
				return;

			auto length = rhs.m_Length;

			if (length < MAX_CHAR_SIZE)
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

		void assign(::std::basic_string<T> const& rhs)
		{
			clear_data();
			init_data(
				reinterpret_cast<void const*>(rhs.data()),
				rhs.length());
		}

		void assign(T const* p)
		{
			assign(::std::basic_string<T>(p));
		}
	};

	template <typename T>
	::std::basic_string<T> to_basic_string(::gdstd::basic_string<T> const& v)
	{
		if (v.is_valid())
		{
			if (v.m_Length < v.MAX_CHAR_SIZE)
			{
				return ::std::basic_string<T>(
					reinterpret_cast<T const*>(&v.m_Data.m_Buffer),
					v.m_Length);
			}
			return ::std::basic_string<T>(v.m_Data.m_P, v.m_Length);
		}

		return ::std::basic_string<T>();
	}
#else
	template <typename T>
	class basic_string
	{
		template <typename R>
		friend ::std::basic_string<R> to_basic_string(basic_string<R> const&);

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

		::std::size_t increment_refc()
		{
			return ++data()->m_RefCount;
		}

		::std::size_t decrement_refc()
		{
			return --data()->m_RefCount;
		}

		void init_data(
			void const* p,
			::std::size_t const length)
		{
			RepBase rep = {};

			rep.m_Length = length;
			rep.m_Capacity = rep.m_Length + 1u;
			rep.m_RefCount = 0u;

			auto const allocSize = sizeof(RepBase) + (rep.m_Capacity * sizeof(T));

			m_P = ::gdstd::allocate<T*>(allocSize);

			::std::memset(reinterpret_cast<void*>(m_P), 0x00, allocSize);

			::std::memcpy(
				reinterpret_cast<void*>(m_P),
				&rep,
				sizeof(RepBase));

			m_P = reinterpret_cast<T*>(
				reinterpret_cast<::std::uintptr_t>(m_P) + sizeof(RepBase));

			::std::memcpy(
				reinterpret_cast<void*>(m_P),
				p,
				rep.m_Length * sizeof(T));
		}

		void clear_data()
		{
			auto refc = decrement_refc();

			if (refc <= 0u)
				::gdstd::free(data());

			m_P = nullptr;
		}

		bool is_valid() const
		{
			return m_P != nullptr;
		}

		basic_string()
			: m_P(nullptr) {}
	public:
		basic_string(basic_string<T>&&) = delete;

		~basic_string()
		{
			clear_data();
		}

		basic_string(basic_string<T> const& rhs)
			: basic_string()
		{
			assign(rhs);
		}

		basic_string(::std::basic_string<T> const& rhs)
			: basic_string()
		{
			assign(rhs);
		}

		basic_string(T const* p)
			: basic_string(::std::basic_string<T>(p)) {}

		basic_string<T>& operator=(basic_string<T> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		basic_string<T>& operator=(::std::basic_string<T> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		basic_string<T>& operator=(T const* p)
		{
			assign(p);
			return *this;
		}

		void assign(basic_string<T> const& rhs)
		{
			if (this == &rhs)
				return;

			clear_data();
			m_P = rhs.m_P;
			increment_refc();
		}

		void assign(::std::basic_string<T> const& rhs)
		{
			clear_data();
			init_data(
				reinterpret_cast<void const*>(rhs.data()),
				rhs.length());
		}

		void assign(T const* p)
		{
			assign(::std::basic_string<T>(p));
		}
	};

	template <typename T>
	::std::basic_string<T> to_basic_string(::gdstd::basic_string<T> const& v)
	{
		if (v.is_valid())
			return ::std::basic_string<T>(v.m_P, v.data()->m_Length);

		return ::std::basic_string<T>();
	}
#endif // GDSTL_USES_MSVC

	using string = ::gdstd::basic_string<char>;
	using wstring = ::gdstd::basic_string<wchar_t>;

	inline ::std::string to_string(gdstd::string const& s)
	{
		return to_basic_string(s);
	}

	inline ::std::wstring to_wstring(gdstd::wstring const& s)
	{
		return to_basic_string(s);
	}
}

#endif /* _GDSTL_STRING_HPP */