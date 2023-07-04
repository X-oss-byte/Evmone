// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "../utils/utils.hpp"
#include <evmmax/evmmax.hpp>
#include <evmmax/poly_extension_field.hpp>
#include <evmmax/bn254.hpp>
#include <gtest/gtest.h>

using namespace intx;

const auto BLS12384ModBytes =
    "1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffaaab"_hex;

constexpr auto BLS12384Mod =
    0x1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffaaab_u384;

TEST(evmmax, setup_bls12_384)
{
    const evmmax::ModArith<intx::uint384> s{BLS12384Mod};
    EXPECT_EQ(s.mod_inv, 0x89f3fffcfffcfffd);
}

TEST(evmmax, r_squared)
{
    const evmmax::ModArith<intx::uint384> s{BLS12384Mod};
    EXPECT_EQ(s.r_squared,
        0x11988fe592cae3aa9a793e85b519952d67eb88a9939d83c08de5476c4c95b6d50a76e6a609d104f1f4df1f341c341746_u384);
}

TEST(evmmax, to_mont)
{
    const evmmax::ModArith<intx::uint384> s{BLS12384Mod};
    EXPECT_EQ(s.to_mont(1),
        0x15f65ec3fa80e4935c071a97a256ec6d77ce5853705257455f48985753c758baebf4000bc40c0002760900000002fffd_u384);
}

TEST(evmmax, from_mont)
{
    const evmmax::ModArith<intx::uint384> s{BLS12384Mod};
    EXPECT_EQ(
        s.from_mont(
            0x15f65ec3fa80e4935c071a97a256ec6d77ce5853705257455f48985753c758baebf4000bc40c0002760900000002fffd_u384),
        1_u384);
}

TEST(evmmax, mont_of_zero_is_zero)
{
    const evmmax::ModArith<intx::uint384> s{BLS12384Mod};
    EXPECT_EQ(s.to_mont(0_u384), 0_u384);
}

TEST(evmmax, mul_mont_384)
{
    const evmmax::ModArith<intx::uint384> s{BLS12384Mod};

    const auto a = 2_u384;
    const auto b = s.mod - 1;

    const auto am = s.to_mont(a);
    const auto bm = s.to_mont(b);
    const auto pm = s.mul(am, bm);
    const auto p = s.from_mont(pm);

    EXPECT_EQ(p, udivrem(umul(a, b), s.mod).rem);
}

TEST(evmmax, add)
{
    const evmmax::ModArith<intx::uint384> s{BLS12384Mod};

    const auto a = 2_u384;
    const auto b = s.mod - 1;

    const auto am = s.to_mont(a);
    const auto bm = s.to_mont(b);
    const auto pm = s.add(am, bm);
    const auto p = s.from_mont(pm);

    const auto ax = intx::uint<decltype(s)::uint::num_bits + 64>{a};
    EXPECT_EQ(p, udivrem(ax + b, s.mod).rem);
}

TEST(evmmax, sub_10_1)
{
    const evmmax::ModArith<intx::uint384> s{BLS12384Mod};

    const auto a = 10_u384;
    const auto b = 1_u384;

    const auto am = s.to_mont(a);
    const auto bm = s.to_mont(b);
    const auto pm = s.sub(am, bm);
    const auto p = s.from_mont(pm);
    EXPECT_EQ(p, 9_u384);
}

TEST(evmmax, sub_1_10)
{
    const evmmax::ModArith<intx::uint384> s{BLS12384Mod};

    const auto a = 1_u384;
    const auto b = 10_u384;

    const auto am = s.to_mont(a);
    const auto bm = s.to_mont(b);
    const auto pm = s.sub(am, bm);
    const auto p = s.from_mont(pm);
    EXPECT_EQ(p, s.mod - 9_u384);
}

TEST(evmmax, bn254_add)
{
    const evmmax::ModArith s{BN254Mod};

    const auto a = 2_u256;
    const auto b = s.mod - 1;

    const auto am = s.to_mont(a);
    const auto bm = s.to_mont(b);
    const auto pm = s.add(am, bm);
    const auto p = s.from_mont(pm);

    EXPECT_EQ(p, addmod(a, b, s.mod));
}

TEST(evmmax, bn254_mul)
{
    const evmmax::ModArith s{BN254Mod};

    const auto a = 2_u256;
    const auto b = s.mod - 1;

    const auto am = s.to_mont(a);
    const auto bm = s.to_mont(b);
    const auto pm = s.mul(am, bm);
    const auto p = s.from_mont(pm);

    EXPECT_EQ(p, mulmod(a, b, s.mod));
}

TEST(evmmax, field_operations)
{
    using namespace evmmax;

    static auto fe2t = bn254::FE2({1,1});
    static auto s2 = bn254::FE2::add(fe2t, fe2t);
    static auto m2 = bn254::FE2::mul(fe2t, fe2t);
    static auto d2 = bn254::FE2::div(fe2t, 5);
    static auto inv2 = bn254::FE2::inv(fe2t);

    static auto fe12t = bn254::FE12({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
    static auto s12 = bn254::FE12::add(fe12t, fe12t);
    static auto m12 = bn254::FE12::mul(fe12t, fe12t);
    static auto d12 = bn254::FE12::div(fe12t, 5);
    static auto inv12 = bn254::FE12::inv(fe12t);

    static auto dp2 = bn254::FE2::poly_rounded_div({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1,1,1});

    // bn254 curve y^2 = x^3 + 3
    //auto B = uint256(3);
    // Twisted curve over FQ**2
    auto B2 = bn254::FE2::div(bn254::FE2({3, 0}), bn254::FE2({9, 1}));
    // Extension curve over FQ**12; same b value as over FQ
    auto B12 = bn254::FE12({3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});

    bn254::is_on_curve(B2, B2, B2);
}
