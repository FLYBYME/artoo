#include "telemetry.h"



Telemetry::Telemetry(){

}

void Telemetry::init(){

}

void Telemetry::onMavlinkMsg(const mavlink_message_t * msg){
    /*
     * Handle a complete msg that has been received from Solo
     */


       switch (msg->msgid) {
           case MAVLINK_MSG_ID_HEARTBEAT:
               mavlink_msg_heartbeat_decode(msg, &heartbeat);


               break;
           case MAVLINK_MSG_ID_RADIO_STATUS:
               mavlink_msg_radio_status_decode(msg, &radioStatus);
               break;
       }
}
