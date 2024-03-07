#ifndef DEVICEINFO_H
#define DEVICEINFO_H


#include <cstdint>


class DeviceInfo
{
public:
    typedef struct _info_t {
        uint32_t time;
    } info_t;

protected:
    static info_t info;

public:
    struct time
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
};

#endif // DEVICEINFO_H
