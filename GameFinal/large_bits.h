#ifndef __GF_LARGE_BITS_CLASS_H__
#define __GF_LARGE_BITS_CLASS_H__

#include "gfTypes.h"

namespace gf
{
	template<size_t bitCount>
	class large_bits
	{
	public:
		large_bits()
		{
			memset(m_bits, 0, sizeof(m_bits));
		}

		large_bits(u32 x) 
		{
			memset(m_bits, 0, sizeof(m_bits));
			m_bits[bitCount / 32 - 1] = x;
		}

		void set(int pos) {
			int i = (bitCount - 1 - pos) / 32;
			int j = 31 - (pos - (3 - i) * 32);

			// 设置第i个int的第j位
			m_bits[i] |= (1 << (31 - j));
		}

		void unset(int pos) {
			int i = (bitCount - 1 - pos) / 32;
			int j = 31 - (pos - (3 - i) * 32);
			m_bits[i] |= (1 << (31 - j));
		}

		bool is_set(int pos) {
			int i = (bitCount - 1 - pos) / 32;
			int j = 31 - (pos - (3 - i) * 32);
			return (m_bits[i] & (1 << (31 - j))) != 0;
		}

		bool operator==(const large_bits<bitCount>& x) const 
		{
			return memcmp(m_bits, x.m_bits, sizeof(m_bits)) == 0;
		}
		
		bool operator!=(const large_bits<bitCount>& x) const 
		{
			return !(*this == x);
		}

		bool operator < (const large_bits<bitCount>& x) const
		{
			for (int i = 0; i < bitCount / 32; i++) {
				if (m_bits[i] != x.m_bits[i])
					return (m_bits[i] < x.m_bits[i]);
			}
			return false;
		}

		bool operator > (const large_bits<bitCount>& x) const
		{
			for (int i = 0; i < bitCount / 32; i++) {
				if (m_bits[i] != x.m_bits[i])
					return (m_bits[i] > x.m_bits[i]);
			}
			return false;
		}

		bool operator <= (const large_bits<bitCount>& x) const
		{
			for (int i = 0; i < bitCount / 32; i++) {
				if (m_bits[i] != x.m_bits[i])
					return (m_bits[i] < x.m_bits[i]);
			}
			return true;
		}

		bool operator >= (const large_bits<bitCount>& x) const
		{
			for (int i = 0; i < bitCount / 32; i++) {
				if (m_bits[i] != x.m_bits[i])
					return (m_bits[i] > x.m_bits[i]);
			}
			return true;
		}

		std::string toString() 
		{
			std::ostringstream stream;
			for (int i = 0; i < bitCount / 32; i++) {
				int x = m_bits[i];
				for (int j = 0; j < 32; j++) {
					if (x & (1 << (31 - j)))
						stream << '1';
					else
						stream << '0';
				}
				if (i != bitCount / 32 - 1)
					stream << ",";
			}
			return stream.str();
		}
		

	private:
		u32			m_bits[bitCount / 32];
	};

	using large_bits_128 = large_bits<128>;
	using large_bits_256 = large_bits<256>;
}


#endif