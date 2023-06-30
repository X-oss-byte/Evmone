// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "evmmax.hpp"

using namespace intx;

extern "C" void mulx_mont_sparse_256(uint64_t* r_ptr, const uint64_t* a_ptr, const uint64_t* b_org,
    const uint64_t* n_ptr, uint64_t n0) noexcept;
extern "C" void mulMont_CIOS_sparebit_asm_adx__limbs95asm95mul95mont95x565495adx95bmi50_424(
    uint64_t* r_ptr, const uint64_t* a_ptr, const uint64_t* b_org, const uint64_t* n_ptr,
    uint64_t n0) noexcept;


namespace evmmax
{
namespace
{
[[maybe_unused]] constexpr auto BLS12384Mod =
    0x1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffaaab_u384;

inline constexpr std::pair<uint64_t, uint64_t> addmul(
    uint64_t t, uint64_t a, uint64_t b, uint64_t c) noexcept
{
    const auto p = umul(a, b) + t + c;
    return {p[1], p[0]};
}

}  // namespace

template <typename UintT>
ModArith<UintT>::ModArith(const UintT& modulus) : mod{modulus}, mod_inv{mul_inv64(-modulus[0])}
{
    static constexpr auto r2 = intx::uint<UintT::num_bits * 2 + 64>{1} << UintT::num_bits * 2;
    r_squared = intx::udivrem(r2, modulus).rem;
}

// std::unique_ptr<ModState> setup(bytes_view modulus, size_t vals_used)
// {
//     if (vals_used > 256)
//         throw std::invalid_argument{"too much elements"};
//
//     if (modulus.size() != sizeof(uint384))
//         throw std::invalid_argument{"incorrect modulus length, expected 384 bits"};
//
//     const auto mod_arg = be::unsafe::load<uint384>(modulus.data());
//     if (mod_arg != BLS12384Mod)
//         throw std::invalid_argument{"only BLS12-384 supported"};
//
//     const auto r_squared = intx::uint<384 * 2 + 64>{1} << 384 * 2;
//     const auto r_squared_mod = intx::udivrem(r_squared, mod_arg).rem;
//
//     auto state = std::make_unique<ModState>();
//     state->mod = mod_arg;
//     state->r_squared = r_squared_mod;
//     state->mod_inv = mul_inv64(-mod_arg[0]);
//     state->num_elems = vals_used;
//     state->elems = std::unique_ptr<uint384[]>(new uint384[vals_used]);
//     return state;
// }

inline void _mulx(uint64_t rdx, uint64_t x, uint64_t& l, uint64_t& h)
{
    const auto r = umul(x, rdx);
    h = r[1];
    l = r[0];
}

inline void _add(bool& c, uint64_t x, uint64_t& y)
{
    const auto r = addc(x, y);
    y = r.value;
    c = r.carry;
}

inline void _adc(bool& c, uint64_t x, uint64_t& y)
{
    const auto r = addc(x, y, c);
    y = r.value;
    c = r.carry;
}
inline void _mov(uint64_t x, uint64_t& y)
{
    y = x;
}


// static void mulx_mont_sparse_256(uint64_t* r_ptr, const uint64_t* a_ptr, const uint64_t* b_org,
//     const uint64_t* n_ptr, uint64_t n0)
//{
//     bool c = false;
//     bool o = false;
//     uint64_t acc[6];
//     uint64_t lo, hi;
//     uint64_t rax, rdx;
//
//     auto b_ptr = b_org;
//     rdx = b_org[0];
//     acc[4] = a_ptr[0];
//     acc[5] = a_ptr[1];
//     lo = a_ptr[2];
//     hi = a_ptr[3];
//
//     _mulx(rdx, acc[4], rax, acc[1]);
//
//     _mulx(rdx, acc[5], acc[5], acc[2]);
//     _mulx(rdx, lo, lo, acc[3]);
//     _add(c, acc[5], acc[1]);
//     _mulx(rdx, hi, hi, acc[4]);
//     _mov(b_ptr[1], rdx);
//     _adc(c, lo, acc[2]);
//     _adc(c, hi, acc[3]);
//     _adc(c, 0, acc[4]);
// }

template <typename UintT>
UintT ModArith<UintT>::mul(const UintT& x, const UintT& y) const noexcept
{
    // if constexpr (std::is_same_v<UintT, uint256>)
    // {
    //     uint64_t r[4];
    //     mulMont_CIOS_sparebit_asm_adx__limbs95asm95mul95mont95x565495adx95bmi50_424(
    //         r, &x[0], &y[0], &mod[0], mod_inv);
    //     return {r[0], r[1], r[2], r[3]};
    // }


    // Coarsely Integrated Op erand Scanning (CIOS) Method
    // Based on 2.3.2 from
    // https://www.microsoft.com/en-us/research/wp-content/uploads/1998/06/97Acar.pdf

    static constexpr auto S = UintT::num_words;

    intx::uint<UintT::num_bits + 64> t;
    for (size_t i = 0; i != S; ++i)
    {
        uint64_t c = 0;
        for (size_t j = 0; j != S; ++j)
        {
            std::tie(c, t[j]) = addmul(t[j], x[j], y[i], c);
        }
        auto tmp = addc(t[S], c);
        t[S] = tmp.value;
        auto d = tmp.carry;

        c = 0;
        auto m = t[0] * mod_inv;
        std::tie(c, t[0]) = addmul(t[0], m, mod[0], c);
        for (size_t j = 1; j != S; ++j)
        {
            std::tie(c, t[j - 1]) = addmul(t[j], m, mod[j], c);
        }
        tmp = addc(t[S], c);
        t[S - 1] = tmp.value;
        t[S] = d + tmp.carry;  // TODO: Untested.
    }

    if (t >= mod)
        t -= mod;

    return static_cast<UintT>(t);
}

template <typename UintT>
UintT ModArith<UintT>::to_mont(const UintT& x) const noexcept
{
    return mul(x, r_squared);
}

template <typename UintT>
UintT ModArith<UintT>::from_mont(const UintT& x) const noexcept
{
    return mul(x, 1);
}

template <typename UintT>
UintT ModArith<UintT>::add(const UintT& x, const UintT& y) const noexcept
{
    const auto s = addc(x, y);  // FIXME: can overflow only if prime is max size (e.g. 255 bits).
    const auto d = subc(s.value, mod);
    return (!s.carry && d.carry) ? s.value : d.value;
}

template <typename UintT>
UintT ModArith<UintT>::sub(const UintT& x, const UintT& y) const noexcept
{
    const auto d = subc(x, y);
    const auto s = d.value + mod;
    return (d.carry) ? s : d.value;
}

template class ModArith<uint256>;
template class ModArith<uint384>;
}  // namespace evmmax
