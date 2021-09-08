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
		template <typename T>
		friend ::std::vector<T> to_vector(vector<T> const&);

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
			m_Data.m_StorageEnd = ::gdstd::round_size(m_Data.m_Finish);

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

		vector(vector<T> const& rhs)
		{
			init_data(reinterpret_cast<void const*>(rhs.m_Data.m_P),
				rhs.m_Data.m_Finish - rhs.m_Data.m_Start);
		}
	public:
		vector() = delete;
		vector(vector<T>&&) = delete;

		~vector()
		{
			::gdstd::free(m_Data.m_P);
		}

		vector<T>& operator=(vector<T> const&) = delete;

		vector(::std::vector<T> const& rhs)
		{
			init_data(
				reinterpret_cast<void const*>(rhs.data()),
				rhs.size());
		}
	};

	template <typename T>
	::std::vector<T> to_vector(::gdstd::vector<T> const& v)
	{
		return ::std::vector<T>(
			v.m_Data.m_P,
			reinterpret_cast<T*>(v.m_Data.m_Finish));
	}
#if defined(GDSTL_USES_MSVC)
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

		vector(vector<bool> const& rhs)
		{
			m_Vec = rhs.m_Vec;
			m_Size = rhs.m_Size;
		}

	public:
		vector() = delete;
		vector(vector<bool>&&) = delete;
		~vector() = default;

		vector<bool>& operator=(vector<bool> const&) = delete;

		vector(::std::vector<bool> const& rhs)
			: m_Size(0u)
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
			{
				m_Vec.push_back(buffer);
			}
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

		vector(vector<bool> const& rhs)
		{
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

	public:
		vector() = delete;
		vector(vector<bool>&&) = delete;

		~vector()
		{
			::gdstd::free(m_Start.m_P);
		}

		vector<bool>& operator=(vector<bool> const&) = delete;

		vector(::std::vector<bool> const& rhs)
		{
			auto const bufferSize = ::gdstd::round_size(
				static_cast<::std::size_t>(
					::std::ceil(rhs.size() / 8.0f)));

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