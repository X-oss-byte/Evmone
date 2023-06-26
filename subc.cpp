#include <intx/intx.hpp>

using namespace intx;

[[gnu::noinline]] static auto neg(const uint192& y)
{
    uint192 x;
    uint192 z;
    bool k = false;
    for (size_t i = 0; i < 3; ++i)
    {
        auto t = subc(x[i], y[i], k);
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
