#include <intx/intx.hpp>

using namespace intx;

[[gnu::noinline]] static auto sub(const uint192& x, const uint192& y)
{
    return x - y;
}

int main()
{
    if (sub(0, 1) != ~uint192{})
        return 1;

    return 0;
}
