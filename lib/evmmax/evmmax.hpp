// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <intx/intx.hpp>
#include <memory>
#include <string_view>

// TODO(intx): Add ""_u384.
inline constexpr auto operator""_u384(const char* s)
{
    return intx::from_string<intx::uint384>(s);
}

inline constexpr uint64_t mul_inv64(uint64_t base) noexcept
{
    uint64_t result = 1;
    for (auto _ = 0; _ < 64; ++_)
    {
        result *= base;
        base *= base;
    }
    return result;
}

namespace evmmax
{
using bytes_view = std::basic_string_view<uint8_t>;

template <typename UintT>
class ModArith
{
public:
    using uint = UintT;

    uint mod;
    uint r_squared;
    uint64_t mod_inv = 0;

    explicit ModArith(const UintT& modulus);
    constexpr explicit ModArith(const UintT& modulus, const UintT& r_squared) :
        mod{modulus}, r_squared(r_squared), mod_inv{mul_inv64(-modulus[0])}
    {}

    uint to_mont(const uint& x) const noexcept;
    uint from_mont(const uint& x) const noexcept;

    uint mul(const uint& x, const uint& y) const noexcept;

    uint add(const uint& x, const uint& y) const noexcept;

    uint sub(const uint& x, const uint& y) const noexcept;

    uint inv(const uint& x) const noexcept;

    uint div(const uint& x, const uint& y) const noexcept;

    uint pow(const uint& x, const uint& y) const noexcept;

    uint neg(const uint& x) const noexcept;
};

// class ModState
// {
// public:
//     ModArith arith = // TODO
//     size_t num_elems = 0;
//     std::unique_ptr<uint[]> elems;
// };

// std::unique_ptr<ModState> setup(bytes_view modulus, size_t vals_used);
}  // namespace evmmax
