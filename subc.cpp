#include <intx/intx.hpp>

template <typename T>
struct result_with_carry
{
    T value;
    bool carry;

    /// Conversion to tuple of references, to allow usage with std::tie().
    constexpr operator std::tuple<T&, bool&>() noexcept { return {value, carry}; }
};

inline result_with_carry<uint64_t> _subc(
    uint64_t x, uint64_t y, bool carry = false) noexcept
{
    unsigned long long carryout = 0;  // NOLINT(google-runtime-int)
    const auto d = __builtin_subcll(x, y, carry, &carryout);
    return {d, static_cast<bool>(carryout)};
}


[[gnu::noinline]] static auto neg(const intx::uint192& y)
{
    bool k = false;
    auto t = _subc(0, y[0], k);
    k = t.carry;
    t = _subc(0, y[1], k);
    k = t.carry;
    t = _subc(0, y[2], k);
    return t.value;
}

int main()
{
    auto d = neg(1);
    if (d != ~uint64_t{})
        return 1;

    return 0;
}
