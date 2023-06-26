#include <intx/intx.hpp>

using namespace intx;

[[gnu::noinline]] static auto sub(const uint128& x, const uint128& y)
{
    return x - y;
}

int main()
{
    if (sub(0, 1) != ~uint128{})
        return 1;

    return 0;
}
