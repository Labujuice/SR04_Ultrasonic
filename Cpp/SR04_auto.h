#include <stdio.h>
#include <stdint.h>
#include <cstring>

typedef enum
{
    SR04_header_e = 0,
    SR04_data_e,
    SR04_chksum_e
} SR04_decode_state_e;


class SR04
{
private:
    /* data */
public:
    SR04(/* args */);
    ~SR04();

    bool parse(uint8_t byte);

    uint16_t distance;
};

SR04::SR04(/* args */)
{
}

SR04::~SR04()
{
}


bool SR04::parse(uint8_t byte)
{
    static SR04_decode_state_e state = SR04_header_e;
    static uint8_t buf[2];
    static uint8_t checksum;
    static uint8_t lencnt = 0;
    switch (state)
    {
    case SR04_header_e:
        if(byte == 0xFF)
        {
            state = SR04_data_e;
            checksum = 0x00;
            memset(buf, 0, sizeof(buf));
            lencnt = 0;
        }
        else
        {
            //do nothing, keep waiting header
        }
        break;

    case SR04_data_e:
        buf[lencnt] = byte;
        checksum += byte;
        lencnt++;
        if(lencnt >= 2)
        {
            state = SR04_chksum_e;
        }
        break;

    case SR04_chksum_e:
        state = SR04_header_e;
        if(checksum == byte)
        {
            distance = (uint16_t)buf[0] << 8 | (uint16_t)buf[1];
            return true;
        }
        else
        {
            return false;
        }
        break;

    default:
        state = SR04_header_e;
        break;
    }
    return false;
}