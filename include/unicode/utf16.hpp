#pragma once

#include "code_point.hpp"
#include "../encoding/error.hpp"

#include <read.hpp>
#include <write.hpp>
#include <expected.hpp>

namespace utf16 {

	using unit = char16_t;

	template<endianness Endianness = endianness::native>
	struct decoder {

		template<basic_iterator Iterator>
		expected<unicode::code_point, encoding_error>
		constexpr operator () (Iterator&& it) const {
			uint16 u_0 = read<uint16, Endianness>(it);

			if(
				(0      <= u_0 && u_0 <= 0xD7FF) ||
				(0xE000 <= u_0 && u_0 <= 0xFFFF)
			) {
				return unicode::code_point{ u_0 };
			}

			uint16 u_1 = read<uint16, Endianness>(it);

			return unicode::code_point {
				(
					(u_0 & 0b1111111111u) << 10u |
					(u_1 & 0b1111111111u)
				) + 0x10000
			};
		}

	};

	template<endianness Endianness = endianness::native>
	struct encoder {
		
		template<basic_iterator Iterator>
		constexpr void operator () (
			unicode::code_point cp, Iterator&& it
		) const {
			uint32 u = cp;

			if(
				((     0 <= u) && (u <= 0xD7FF)) ||
				((0xE000 <= u) && (u <= 0xFFFF))
			) {
				write<uint16, Endianness>(u, it);
				return;
			}

			u -= 0x10000;
			write<uint16, Endianness>(((u >> 10) & 0b1111111111u) + 0xD800, it);
			write<uint16, Endianness>(( u        & 0b1111111111u) + 0xDC00, it);
		}

		nuint units(unicode::code_point cp) const {
			uint32 u = cp;
			if(
				((     0 <= u) && (u <= 0xD7FF)) ||
				((0xE000 <= u) && (u <= 0xFFFF))
			) {
				return 1;
			}
			return 2;
		}

	};

}