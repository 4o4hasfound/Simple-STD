#ifndef SSTD_CORE_INCLUDED
#define SSTD_CORE_INCLUDED

#include <iostream>
#include <cassert>

#define SSTD_BEGIN namespace sstd {
#define SSTD_END	}

#define SSTD_DEFAULT = default

#define SSTD_CONSTEXPR constexpr

#define SSTD_INLINE inline

#define SSTD_EXPLICIT explicit

#define SSTD_ASSERT assert

#define SSTD_STATIC_ASSERT static_assert

SSTD_BEGIN

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using sizet = std::size_t;

using Decimal = double;

SSTD_END

#endif