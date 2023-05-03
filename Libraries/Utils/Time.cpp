#include "Time.hpp"

namespace Utils::Time
{

    int64_t GetCurrentMillisecondTimestamp()
    {
        struct timespec ts{};
        timespec_get(&ts, TIME_UTC);
        return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }

}