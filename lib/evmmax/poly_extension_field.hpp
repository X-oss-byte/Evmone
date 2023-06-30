// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "evmmax.hpp"
#include "span"
#include "vector"
#include "intx/intx.hpp"

namespace evmmax
{
using uint256 = intx::uint256;
using namespace intx;


template<typename UintT, typename ModCoeffsT, typename FiledModulusT>
class PolyExtFieldElem
{
    std::vector<UintT> coeffs;
    static constexpr auto size = ModCoeffsT::SIZE;
    static constexpr auto arith = ModArith<UintT>(FiledModulusT::MODULUS, 0);

public:
    explicit PolyExtFieldElem()
    {
        coeffs.reserve(size);
    }

    explicit PolyExtFieldElem(std::vector<UintT>&& _coeffs) : coeffs(_coeffs)
    {
        assert(coeffs.size() == size);
    }

    static inline constexpr PolyExtFieldElem add(const PolyExtFieldElem& x, const PolyExtFieldElem& y)
    {
        PolyExtFieldElem result;

        for(size_t i = 0; i < size; ++i)
            result.coeffs[i] = x.arith.add(x.coeffs[i], y.coeffs[i]);

        return result;
    }

    static inline constexpr PolyExtFieldElem sub(const PolyExtFieldElem& x, const PolyExtFieldElem& y)
    {
        PolyExtFieldElem result;

        for(size_t i = 0; i < size; ++i)
            result.coeffs[i] = arith.mul(x.coeffs[i], y.coeffs[i]);

        return result;
    }

    static inline constexpr PolyExtFieldElem mul(const PolyExtFieldElem& x, const UintT& c)
    {
        PolyExtFieldElem result;

        for(size_t i = 0; i < size; ++i)
            result.coeffs[i] = arith.mul(x.coeffs[i], c);

        return result;
    }

    static inline constexpr PolyExtFieldElem mul(const PolyExtFieldElem& x, const PolyExtFieldElem& y)
    {
        std::vector<UintT> b(2 * size - 1);

        // Multiply
        for (size_t i = 0; i < size; ++i)
        {
            for (size_t j = 0; j < size; ++j)
                b[i + j] = arith.mul(x.coeffs[i], y.coeffs[j]);
        }

        // Reduce by irreducible polynomial (extending polynomial)
        while(b.size() > size)
        {
            auto top = b.back();
            auto exp = b.size() - size - 1;
            b.pop_back();
            for (size_t i = 0; i < size; ++i)
                b[i + exp] -= arith.mul(top, ModCoeffsT::MODULUS_COEFFS[i]);
        }

        return PolyExtFieldElem(std::move(b));
    }
};

} // namespace evmmax


