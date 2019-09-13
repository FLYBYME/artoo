#include "mavlink.h"
#include "telemetry.h"

mavlink_message_t Mavlink::currentMsg;
mavlink_status_t Mavlink::status;

Mavlink Mavlink::instance;
Mavlink::Mavlink() {
	syncDeadline = SysTime::now();
}

void Mavlink::onMavlinkData(const uint8_t *bytes, unsigned len)
{
    /*
     * A chunk of mavlink data has arrived.
     *
     * It may or may not contain a complete mavlink packet, so we maintain
     * a mavlink_message_t persistently to assemble packets as they arrive.
     */

    for (unsigned i = 0; i < len; ++i) {
        if (mavlink_parse_char(MAVLINK_COMM_0, bytes[i], &currentMsg, &status)) {


            if(currentMsg.sysid == 1){
                UAV::instance1.onMavlinkMsg(&currentMsg);
            }else if(currentMsg.sysid == 2){
                UAV::instance2.onMavlinkMsg(&currentMsg);
            }else if(currentMsg.sysid == 3){
                UAV::instance3.onMavlinkMsg(&currentMsg);
            }

            //Telemetry::instance.onMavlinkMsg(&currentMsg);
        }
    }
}

bool Mavlink::producePacket(HostProtocol::Packet &p)
{
	
	const SysTime::Ticks now = SysTime::now();
	if (now > syncDeadline) {
		syncDeadline = now + SysTime::msTicks(1000);;
		mavlink_message_t msg;
		// Pack the message
		mavlink_msg_heartbeat_pack(ArtooSysID, ArtooComponentID, &msg, 0, 0, 0, 0, 0);
		packetizeMsg(p, &msg);
		return true;
	}
	return false;
}
void Mavlink::packetizeMsg(HostProtocol::Packet &p, const mavlink_message_t *msg)
{
    /*
     * Helper to pack a mavlink packet into a hostprotocol packet.
     *
     * More copying than would be ideal... maybe we implement our
     * own version of mavlink_msg_to_send_buffer() if this is a problem.
     */

    uint8_t mavlinkTxBuf[MAVLINK_MAX_PACKET_LEN];
    uint16_t len = mavlink_msg_to_send_buffer(mavlinkTxBuf, msg);

    p.delimitSlip();
    p.appendSlip(HostProtocol::Mavlink);
    p.appendSlip(mavlinkTxBuf, len);
    p.delimitSlip();
}
