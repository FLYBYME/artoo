#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "stm32/common.h"

#include "mavlink/c_library/ardupilotmega/mavlink.h"
#include "mavlink/c_library/common/mavlink.h"

#include "tasks.h"

#include <limits.h>

struct Telemetry {

public:
    Telemetry();


    void init();
    void onMavlinkMsg(const mavlink_message_t * msg);

    struct CounterStruct {
        unsigned long sendBytes = 0;
        unsigned long send = 0;

        unsigned long recivedBytes = 0;
        unsigned long recived = 0;

        unsigned long hops = 0;

        unsigned long retransmissions = 0;

        unsigned long lost = 0;
    } counter;



    mavlink_heartbeat_t heartbeat;
    mavlink_radio_status_t radioStatus;
private:


};

#endif // TELEMETRY_H