#ifndef _GDSTL_VECTOR_HPP
#define _GDSTL_VECTOR_HPP

#include "allocator.hpp"

#include <cmath>
#include <vector>

namespace gdstd
{
	template <typename T>
	class vector
	{
		template <typename R>
		friend ::std::vector<R> to_vector(vector<R> const&);

		struct VectorBase
		{
			union
			{
				::std::uintptr_t m_Start;
				T* m_P;
			};

			::std::uintptr_t m_Finish;
			::std::uintptr_t m_StorageEnd;
		};

	protected:
		VectorBase m_Data;

		void init_data(
			void const* p,
			::std::size_t const size)
		{
			m_Data.m_Finish = size;
			m_Data.m_StorageEnd = m_Data.m_Finish;

			m_Data.m_P = ::gdstd::allocate<T*>(m_Data.m_StorageEnd);

			::std::memset(
				reinterpret_cast<void*>(m_Data.m_P),
				0x00,
				m_Data.m_StorageEnd);

			::std::memcpy(
				reinterpret_cast<void*>(m_Data.m_P),
				p,
				m_Data.m_Finish);

			m_Data.m_Finish += m_Data.m_Start;
			m_Data.m_StorageEnd += m_Data.m_Start;
		}

		void clear_data()
		{
			::gdstd::free(m_Data.m_P);
			m_Data = {};
		}

		bool is_valid() const
		{
			return m_Data.m_P != nullptr;
		}

		vector()
			: m_Data({}) {}

	public:
		vector(vector<T>&&) = delete;

		~vector()
		{
			clear_data();
		}

		vector(vector<T> const& rhs)
			: vector()
		{
			assign(rhs);
		}

		vector(::std::vector<T> const& rhs)
			: vector()
		{
			assign(rhs);
		}

		vector<T>& operator=(vector<T> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		vector<T>& operator=(::std::vector<T> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		void assign(vector<T> const& rhs)
		{
			clear_data();
			init_data(
				reinterpret_cast<void const*>(rhs.m_Data.m_P),
				rhs.m_Data.m_Finish - rhs.m_Data.m_Start);
		}

		void assign(::std::vector<T> const& rhs)
		{
			clear_data();
			init_data(
				reinterpret_cast<void const*>(rhs.data()),
				rhs.size());
		}
	};

	template <typename T>
	::std::vector<T> to_vector(::gdstd::vector<T> const& v)
	{
		if (v.is_valid())
		{
			return ::std::vector<T>(
				v.m_Data.m_P,
				reinterpret_cast<T*>(v.m_Data.m_Finish));
		}

		return ::std::vector<T>();
	}
#if 0 && defined(GDSTL_USES_MSVC)
	template<>
	class vector<bool>
	{
		friend ::std::vector<bool> to_vector(vector<bool> const&);

	protected:
		std::vector<::std::uint32_t> m_Vec;
		::std::size_t m_Size;

		::std::uint8_t const* internal_data() const
		{
			return reinterpret_cast<::std::uint8_t const*>(m_Vec.data());
		}

		::std::size_t const item_count() const
		{
			return m_Size;
		}

		vector()
			: m_Size(0u) {}

	public:
		vector(vector<bool>&&) = delete;
		~vector() = default;

		vector(vector<bool> const& rhs)
			: vector()
		{
			assign(rhs);
		}

		vector(::std::vector<bool> const& rhs)
			: vector()
		{
			assign(rhs);
		}

		vector<bool>& operator=(vector<bool> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		vector<bool>& operator=(::std::vector<bool> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		void assign(vector<bool> const& rhs)
		{
			m_Vec = rhs.m_Vec;
			m_Size = rhs.m_Size;
		}

		void assign(::std::vector<bool> const& rhs)
		{
			::std::uint32_t buffer = 0u;

			for (auto const b : rhs)
			{
				auto const pos = m_Size++ & 31u;

				buffer |= (b << pos);

				if (!(m_Size & 31u))
				{
					m_Vec.push_back(buffer);
					buffer = 0;
				}
			}

			if (buffer)
				m_Vec.push_back(buffer);
		}
	};
#else
	template <>
	class vector<bool>
	{
		friend ::std::vector<bool> to_vector(vector<bool> const&);

		using BitBuffer = ::std::size_t;

		static auto constexpr ITEM_BITS = sizeof(BitBuffer) * 8u;

		struct BitIterator
		{
			union
			{
				BitBuffer* m_P;
				::std::uintptr_t m_Base;
			};
			::std::size_t m_Offset;
		};

	protected:
		BitIterator m_Start;
		BitIterator m_Finish;
		::std::uintptr_t m_StorageEnd;

		void clear_data()
		{
			::gdstd::free(m_Start.m_P);
			m_Start = {};
			m_Finish = {};
			m_StorageEnd = 0u;
		}

		::std::uint8_t const* internal_data() const
		{
			return reinterpret_cast<::std::uint8_t const*>(m_Start.m_Base);
		}

		::std::size_t const item_diff() const
		{
			return m_Finish.m_Base - m_Start.m_Base;
		}

		::std::size_t const item_count() const
		{
			return (item_diff() * ITEM_BITS) + m_Finish.m_Offset;
		}

		vector()
			: m_Start({}),
			m_Finish({}),
			m_StorageEnd({}) {}

	public:
		vector(vector<bool>&&) = delete;

		~vector()
		{
			clear_data();
		}

		vector(vector<bool> const& rhs)
			: vector()
		{
			assign(rhs);
		}

		vector(::std::vector<bool> const& rhs)
			: vector()
		{
			assign(rhs);
		}

		vector<bool>& operator=(vector<bool> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		vector<bool>& operator=(::std::vector<bool> const& rhs)
		{
			assign(rhs);
			return *this;
		}

		void assign(vector<bool> const& rhs)
		{
			clear_data();

			auto const bufSize = rhs.m_StorageEnd - rhs.m_Start.m_Base;

			m_Start.m_P = ::gdstd::allocate<BitBuffer*>(bufSize);
			m_Start.m_Offset = 0u;
			m_Finish.m_Base = m_Start.m_Base + rhs.item_diff();
			m_Finish.m_Offset = rhs.m_Finish.m_Offset;
			m_StorageEnd = m_Start.m_Base + bufSize;

			::std::memcpy(
				reinterpret_cast<void*>(m_Start.m_P),
				reinterpret_cast<void const*>(rhs.internal_data()),
				bufSize);
		}

		void assign(::std::vector<bool> const& rhs)
		{
			clear_data();

			auto const bufferSize =
				static_cast<::std::size_t>(
					::std::ceil(rhs.size() / 8.0f));

			m_Start.m_P = ::gdstd::allocate<BitBuffer*>(bufferSize);
			m_Start.m_Offset = 0u;
			m_Finish.m_P = m_Start.m_P;
			m_Finish.m_Offset = 0u;
			m_StorageEnd = m_Start.m_Base + bufferSize;

			for (auto const b : rhs)
			{
				*(m_Finish.m_P) |= (b << m_Finish.m_Offset++);

				if (!(m_Finish.m_Offset & (ITEM_BITS - 1u)))
				{
					++m_Finish.m_P;
					m_Finish.m_Offset = 0u;
				}
			}
		}
	};
#endif // GDSTL_USES_MSVC
	inline ::std::vector<bool> to_vector(::gdstd::vector<bool> const& v)
	{
		::std::vector<bool> ret;
		auto buffer = v.internal_data();

		for (auto i = 0; i < v.item_count(); ++i)
		{
			auto const byte = i / 8u;
			auto const pos = i & 7u;
			ret.push_back((buffer[byte] >> pos) & 1);
		}

		return ret;
	}
}

#endif /* _GDSTL_VECTOR_HPP */