using u64 = unsigned long long;

[[gnu::noinline]] static auto neg(const u64 y[3])
{
    u64 k = 0;
    __builtin_subcll(0, y[0], k, &k);
    __builtin_subcll(0, y[1], k, &k);
    return __builtin_subcll(0, y[2], k, &k);
}

int main()
{
    const u64 y[] = {1, 0, 0};
    if (neg(y) != ~u64(0))
        return 1;
    return 0;
}
