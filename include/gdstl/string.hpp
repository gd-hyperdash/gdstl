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
				m_Capacity = m_Length + 1u;
				m_Data.m_P = ::gdstd::allocate<T*>(m_Capacity);

				std::memset(
					reinterpret_cast<void*>(m_Data.m_P),
					0x00,
					m_Capacity);

				::std::memcpy(
					reinterpret_cast<void*>(m_Data.m_P),
					p,
					m_Length * sizeof(T));
			}
		}

		void clear_data()
		{
			if (m_Length > MAX_CHAR_SIZE)
				::gdstd::free(m_Data.m_P);

			m_Data = {};
			m_Length = 0u;
			m_Capacity = 0u;
		}

		bool is_valid() const
		{
			return m_Capacity != 0;
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
			if (v.m_Length <= v.MAX_CHAR_SIZE)
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

		void init_data(
			void const* p,
			::std::size_t const length)
		{
			RepBase rep = {};

			auto const allocSize = sizeof(RepBase) + ((length + 1u) * sizeof(T));

			rep.m_Length = length;
			rep.m_Capacity = allocSize - sizeof(RepBase);
			rep.m_RefCount = 0u;

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
			--data()->m_RefCount;

			if (data()->m_RefCount <= 0u)
				::gdstd::free(data());

			m_P = nullptr;
		}

		bool is_valid() const
		{
			return m_P != nullptr;
		}

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
			++data()->m_RefCount;
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

	static auto constexpr& to_string = to_basic_string<char>;
	static auto constexpr& to_wstring = to_basic_string<wchar_t>;
}

#endif /* _GDSTL_STRING_HPP */