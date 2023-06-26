#include <intx/intx.hpp>

using namespace intx;

inline constexpr result_with_carry<uint64_t> _subc(
    uint64_t x, uint64_t y, bool carry = false) noexcept
{
#if __has_builtin(__builtin_subcll)
    if (!std::is_constant_evaluated())
    {
        unsigned long long carryout = 0;  // NOLINT(google-runtime-int)
        const auto d = __builtin_subcll(x, y, carry, &carryout);
        static_assert(sizeof(d) == sizeof(uint64_t));
        return {d, static_cast<bool>(carryout)};
    }
#elif __has_builtin(__builtin_ia32_sbb_u64)
    if (!std::is_constant_evaluated())
    {
        unsigned long long d = 0;  // NOLINT(google-runtime-int)
        static_assert(sizeof(d) == sizeof(uint64_t));
        const auto carryout = __builtin_ia32_sbb_u64(carry, x, y, &d);
        return {d, static_cast<bool>(carryout)};
    }
#endif

    const auto d = x - y;
    const auto carry1 = x < y;
    const auto e = d - carry;
    const auto carry2 = d < uint64_t{carry};
    return {e, carry1 || carry2};
}


[[gnu::noinline]] static auto neg(const uint192& y)
{
    uint192 z;
    bool k = false;
    for (size_t i = 0; i < 3; ++i)
    {
        auto t = _subc(0, y[i], k);
        z[i] = t.value;
        k = t.carry;
    }
    return z;
}

int main()
{
    auto d = neg(1);
    if (d[2] != ~uint64_t{})
        return 1;

    return 0;
}
