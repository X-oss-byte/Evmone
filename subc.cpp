using u64 = unsigned long long;

struct result_with_carry
{
    u64 value;
    u64 carry;
};

inline result_with_carry _subc(u64 x, u64 y, u64 carry) noexcept
{
    u64 carryout = 0;
    const auto d = __builtin_subcll(x, y, carry, &carryout);
    return {d, carryout};
}


[[gnu::noinline]] static auto neg(const u64* y)
{
    u64 k = 0;
    auto t = _subc(0, y[0], k);
    k = t.carry;
    t = _subc(0, y[1], k);
    k = t.carry;
    t = _subc(0, y[2], k);
    return t.value;
}

int main()
{
    const u64 y[] = {1, 0, 0};
    auto d = neg(y);
    if (d != ~u64{})
        return 1;

    return 0;
}
