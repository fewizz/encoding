#pragma once

#include "code_point.hpp"
#include "../encoding/error.hpp"

#include <core/read.hpp>
#include <core/write.hpp>
#include <core/expected.hpp>

namespace utf8 {

	struct decoder {

		template<typename Iterator>
		expected<unicode::code_point, encoding_error>
		constexpr operator () (Iterator&& it) const {
			uint8 b_0 = read<uint8>(it);

			if(b_0 >> 7u == 0u) {
				return unicode::code_point{ b_0 };
			}

			uint8 b_1 = read<uint8>(it);

			if(b_0 >> 5u == 0b110u) {
				return unicode::code_point {
					(b_0 & 0b11111u ) << 6u |
					(b_1 & 0b111111u)
				};
			}

			uint8 b_2 = read<uint8>(it);

			if(b_0 >> 4u == 0b1110u) {
				return unicode::code_point {
					(b_0 & 0b1111u  ) << (6u + 6u) |
					(b_1 & 0b111111u) <<  6u |
					(b_2 & 0b111111u)
				};
			}

			uint8 b_3 = read<uint8>(it);

			if(b_0 >> 3u == 0b11110u) {
				return unicode::code_point {
					(b_0 & 0b111u   ) << (6u + 6u + 6u) |
					(b_1 & 0b111111u) << (6u + 6u) |
					(b_2 & 0b111111u) <<  6u |
					(b_3 & 0b111111u)
				};
			}

			return encoding_error::invalid;
		}

	};

	struct encoder {

		template<typename Iterator>
		constexpr void
		operator () (unicode::code_point cp, Iterator&& it) const {
			uint32 u = cp;

			if(u >> 7u == 0u) {
				write<uint8>(u, it);
				return;
			}

			if(u >> (6u + 5u) == 0u) {
				write<uint8>(0b110u << 5u |  u >> 6u,       it);
				write<uint8>(0b10u  << 6u | (u & 0b111111), it);
				return;
			}

			if(u >> (6u + 6u + 4u) == 0u) {
				write<uint8>((0b1110u << 4u) | (u >> (6u + 6u) & 0b1111  ), it);
				write<uint8>((0b10u   << 6u) | (u >> (6u)      & 0b111111), it);
				write<uint8>((0b10u   << 6u) | (u              & 0b111111), it);
				return;
			}

			if(u >> (6u + 6u + 6u + 3u) == 0u) {
				write<uint8>(
					(0b11110u << 3u) | (u >> (6u + 6u + 6u) & 0b111   ), it
				);
				write<uint8>(
					(0b10u    << 6u) | (u >> (6u + 6u)      & 0b111111), it
				);
				write<uint8>(
					(0b10u    << 6u) | (u >> 6u             & 0b111111), it
				);
				write<uint8>(
					(0b10u    << 6u) | (u                   & 0b111111), it
				);
				return;
			}
		}

	};

}