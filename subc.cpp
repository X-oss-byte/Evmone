#include <intx/intx.hpp>

using namespace intx;

[[gnu::noinline]] static auto sub(const uint256& x, const uint256& y)
{
    return x - y;
}

int main()
{
    if (sub(0, 1) != ~uint256{})
        return 1;

    return 0;
}
