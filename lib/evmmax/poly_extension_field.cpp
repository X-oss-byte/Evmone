// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "poly_extension_field.hpp"
#include "bn254.hpp"
#include <array>

namespace evmmax
{
struct BN245FieldModulus
{
    static constexpr auto MODULUS = BN254Mod;
};

struct ModCoeffs2
{
    static constexpr uint8_t DEGREE = 2;
    static constexpr const uint256 MODULUS_COEFFS[DEGREE] = {1, 0};
};

struct ModCoeffs12
{
    static constexpr uint8_t DEGREE = 12;
    static constexpr uint256 MODULUS_COEFFS[DEGREE] =
        {82, 0, 0, 0, 0, 0, -18, 0, 0, 0, 0, 0};
};

using FE2 = class PolyExtFieldElem<uint256, ModCoeffs2, BN245FieldModulus>;
using FE12 = class PolyExtFieldElem<uint256, ModCoeffs12, BN245FieldModulus>;

static auto fe2t = FE2({1,1});
static auto s2 = FE2::add(fe2t, fe2t);
static auto m2 = FE2::mul(fe2t, fe2t);
static auto d2 = FE2::div(fe2t, 5);
static auto p2 = FE2::pow(fe2t, 5);
static auto inv2 = FE2::inv(fe2t);

static auto fe12t = FE12({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
static auto s12 = FE12::add(fe12t, fe12t);
static auto m12 = FE12::mul(fe12t, fe12t);
static auto d12 = FE12::div(fe12t, 5);
static auto inv12 = FE12::inv(fe12t);

static auto dp2 = FE2::poly_rounded_div({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1,1,1});
}
