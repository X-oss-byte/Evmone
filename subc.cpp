#include <intx/intx.hpp>

using namespace intx;

[[gnu::noinline]] static auto sub(const uint192& x, const uint192& y)
{
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
    if (sub(0, 1) != ~uint192{})
        return 1;

    return 0;
}
