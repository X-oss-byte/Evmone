// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "poly_extension_field.hpp"
#include "bn254.hpp"
#include <vector>
#include <array>

namespace evmmax
{
struct BN245FieldModulus
{
    static constexpr auto MODULUS = BN254Mod;
};

struct ModCoeffs2
{
    static constexpr uint8_t SIZE = 2;
    static constexpr const uint256 MODULUS_COEFFS[SIZE] = {1, 0};
};

struct ModCoeffs12
{
    static constexpr uint8_t SIZE = 12;
    static constexpr uint256 MODULUS_COEFFS[SIZE] =
        {82, 0, 0, 0, 0, 0, -18, 0, 0, 0, 0, 0};
};

using FE2 = class PolyExtFieldElem<uint256, ModCoeffs2, BN245FieldModulus>;
using FE12 = class PolyExtFieldElem<uint256, ModCoeffs12, BN245FieldModulus>;

static auto fe2t = FE2({1,1});
static auto s2 = FE2::add(fe2t, fe2t);
static auto m2 = FE2::mul(fe2t, fe2t);

static auto fe12t = FE12({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
static auto s12 = FE12::add(fe12t, fe12t);
static auto m12 = FE12::mul(fe12t, fe12t);
}
