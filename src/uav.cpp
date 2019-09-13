#include "uav.h"
#include "ui_topbar.h"
#include "ui.h"
#include "mavlink.h"
#include "hostprotocol.h"
#include "inputs.h"


#include "stm32/common.h"

#include "ui_color.h"
#include "gfx.h"

#include <stdio.h>

UAV UAV::instance1;
UAV UAV::instance2;
UAV UAV::instance3;


UAV::UAV() :
	linkConnCounter(0),
	telemetry(),
	uavMode(0),
	uavType(0),
	currentLoc(),
	home(),
	mode(FlightMode::MANUAL) {
	for (unsigned i = 0; i < arraysize(channels); ++i) {
		channels[i] = 1000;
	}
}


void UAV::onMavlinkMsg(const mavlink_message_t * msg) {

	resetLinkConnCount(msg);

	switch (msg->msgid) {
	case MAVLINK_MSG_ID_HEARTBEAT:
		mavlink_msg_heartbeat_decode(msg, &heartbeat);

		//uint32_t newMode = heartbeat.custom_mode;
		if (uavType != heartbeat.type) {
			uavType = heartbeat.type;
		}
		if (uavMode != heartbeat.custom_mode) {
			uavMode = heartbeat.custom_mode;
			if (isActive())
				UiTopBar::instance.setTitle(modeToString());
		}
		break;

	case MAVLINK_MSG_ID_COMMAND_ACK:
		mavlink_command_ack_t ack;
		mavlink_msg_command_ack_decode(msg, &ack);
		//if (ack.command == command.id) {
		//	command.state = Command::Complete;   // completion handlers may update this
		//}
		cmdAcknowledged(&ack);
		break;
	case MAVLINK_MSG_ID_RADIO_STATUS:
		mavlink_msg_radio_status_decode(msg, &radioStatus);
		break;

	case MAVLINK_MSG_ID_GPS_RAW_INT:
		mavlink_msg_gps_raw_int_decode(msg, &gps_raw_int);
		break;
	case MAVLINK_MSG_ID_GPS2_RAW:
		mavlink_msg_gps2_raw_decode(msg, &gps_raw_int2);
		break;

	case MAVLINK_MSG_ID_STATUSTEXT:
		mavlink_msg_statustext_decode(msg, &statustext);
		Ui::instance.statusText();
		break;

	case MAVLINK_MSG_ID_VFR_HUD:
		mavlink_msg_vfr_hud_decode(msg, &vfr_hud);
		break;
	case MAVLINK_MSG_ID_MISSION_ITEM:
		mavlink_mission_item_t item;
		mavlink_msg_mission_item_decode(msg, &item);

		if (home.update(item)) {
			//Ui::instance.pendEvent(Event::HomeLocationChanged);
		}
		break;
	case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: {
		Coord2D c;
		mavlink_msg_global_position_int_decode(msg, &global_pos);
		// lat and long are degrees * 1E7, so convert back
		c.set(global_pos.lat / 1e7, global_pos.lon / 1e7);

		currentLoc.set(c.lat(), c.lng());
	}
											 break;
	case MAVLINK_MSG_ID_BATTERY_STATUS:

		mavlink_msg_battery_status_decode(msg, &battery_status);

		break;
	}
}

void UAV::forceRTL() {

	forcedRTL = true;

	requestFlightModeChange(SMART_RTL, RTL, RTL);
}

bool UAV::onButtonEvent(Button *b, Button::Event e) {
	if (!isActive()) {
		return false;
	}
	forcedRTL = false;
	if (e == Button::ClickRelease && b->id() == Io::ButtonRTL) {
		requestFlightModeChange(SMART_RTL, RTL, RTL);
		return true;
	}
	else if (e == Button::ClickRelease && b->id() == Io::ButtonLoiter) {
		requestFlightModeChange(HOLD, LOITER, LOITER);
		return true;
	}


	else if (e == Button::ClickRelease && b->id() == Io::ButtonA) {
		requestFlightModeChange(MANUAL, ALT_HOLD, FLY_BY_WIRE_B);
		return true;
	}
	else if ((e == Button::LongHoldRelease || e == Button::HoldRelease) && b->id() == Io::ButtonA) {
		requestFlightModeChange(ACRO, POSHOLD, FLY_BY_WIRE_A);
		return true;
	}


	else if (e == Button::ClickRelease && b->id() == Io::ButtonB) {
		requestFlightModeChange(STEERING, STABILIZE, MANUAL);
		return true;
	}
	else if ((e == Button::LongHoldRelease || e == Button::HoldRelease) && b->id() == Io::ButtonB) {
		requestFlightModeChange(AUTO, AUTO, STABILIZE);
		return true;
	}

	return false;
}
void UAV::resetLinkConnCount(const mavlink_message_t * msg) {
	/*
	 * Any mavlink msg resets the link count if we're already connected.
	 * Otherwise, we require a heartbeat to re-establish connection - this
	 * helps ensure we have up to date state info about critical fields like
	 * arm state, flight mode, system status, and ekf flags.
	 */
	sysid = msg->sysid;
	if (linkIsConnected()) {
		if (linkConnCounter != 0) {
			if (isActive())
				UiTopBar::instance.setTitle(modeToString());
		}
		linkConnCounter = 0;
	}
	else {
		if (msg->msgid == MAVLINK_MSG_ID_HEARTBEAT) {
			if (isActive())
				UiTopBar::instance.setTitle(modeToString());
			linkConnCounter = 0;
		}
	}

}
void UAV::sysHeartbeat() {
	/*
	 * called from heartbeat task.
	 * Do any work that requires periodic attention.
	 */

	if (linkIsConnected()) {
		if (++linkConnCounter >= LINK_CONN_DURATION) {
			if (isActive()) {
				if (sysid == 1) {
					UiTopBar::instance.setTitle("1:sysHeartbeat");
				}
				else if (sysid == 2) {
					UiTopBar::instance.setTitle("2:sysHeartbeat");
				}
				else if (sysid == 3) {
					UiTopBar::instance.setTitle("3:sysHeartbeat");
				}
				else {
					UiTopBar::instance.setTitle("0:sysHeartbeat");
				}
			}

			linkDisconnected();
		}
	}
}

void UAV::linkConnected() {
	online = true;
}

void UAV::linkDisconnected() {
	online = false;
}
void UAV::requestFlightModeChange(FlightMode rover, FlightMode copter, FlightMode plane) {
	/*
	 * helper to request flight mode change
	 */

	 //if (mode != m) {
	if (uavType == 1) {
		command.flightMode = plane;
	}
	else if (uavType == 10) {
		command.flightMode = rover;
	}
	else {
		command.flightMode = copter;
	}
	command.set(Command::SetFlightMode);
	//}
}

void UAV::cmdAcknowledged(const mavlink_command_ack_t *ack) {
	/*
	 * A command has been acked by the vehicle.
	 * Tick along to our next state as appropriate.
	 */

	switch (ack->command) {
	case MAV_CMD_COMPONENT_ARM_DISARM:
		if (ack->result != MAV_RESULT_ACCEPTED) {
			Ui::instance.statusText("onArmAck");
		}
		break;

	case MAVLINK_MSG_ID_SET_MODE:
		//onFlightModeAck(ack);
		Ui::instance.statusText("onFlightModeAck");
		break;

	case MAV_CMD_NAV_TAKEOFF:
		if (ack->result == MAV_RESULT_ACCEPTED) {
			Ui::instance.statusText("TakeoffAscending");
			//takeoffState = TakeoffAscending;
		}
		else {
			Ui::instance.statusText("onTakeoffFailed");
			//Ui::instance.arming.onTakeoffFailed();
		}
		break;
	}

	if (ack->result != MAV_RESULT_ACCEPTED) {

		char s[32];
		sprintf(s, "mavlink nack. cmd %d, result %d\n", ack->command, ack->result);
		Ui::instance.statusText(s);
		DBG(("mavlink nack. cmd %d, result %d\n", ack->command, ack->result));
	}
}




void UAV::rcOverRide(HostProtocol::Packet &p)
{

	//The first 4 channels are thro/roll/pitch/yaw.
	
	if (isActive()) {
		for (unsigned i = 0; i < 4; ++i) {
			const StickAxis &stick = Inputs::stick(Io::StickID(i));
			channels[i] = stick.angularPPMValue();
		}
	}



	const StickAxis & stickGimbalY = Inputs::stick(Io::StickGimbalY);
	const StickAxis & StickGimbalRate = Inputs::stick(Io::StickGimbalRate);

	channels[6] = clamp((int)StickGimbalRate.angularPPMValue(), 1500, 2000);
	channels[7] = clamp((int)stickGimbalY.angularPPMValue(), 1000, 2000);

	int i = 0;
	int add = scale((int)channels[6], 1500, 2000, 0, 250);
	if (add != 0) {
		channels[0] = clamp((int)channels[0] + add, 1000, 2000);
	}

	mavlink_message_t msg;
	mavlink_msg_rc_channels_override_pack(Mavlink::instance.ArtooSysID,
		Mavlink::instance.ArtooComponentID,
		&msg,
		sysid, 1,
		channels[1], channels[2], channels[0], channels[3],
		channels[4], channels[5], channels[6], channels[7]
		//, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000
	);


	Mavlink::packetizeMsg(p, &msg);
}
bool UAV::producePacket(HostProtocol::Packet &p)
{
	/*
	 * Our opportunity to send a message via the host.
	 */

	if (command.state == Command::Pending) {
		mavlink_message_t msg;
		sendCmd(&msg);
		Mavlink::packetizeMsg(p, &msg);
		command.state = Command::Sent;
		return true;
	}

	return false;
}
void UAV::sendCmd(mavlink_message_t *msg) {
	/*
	 * Produce a mavlink packet for the currently pending command.
	 * Called when we have a command ready to be sent to the vehicle.
	 */

	switch (command.id) {
	case Command::SetArmState:
		mavlink_msg_command_long_pack(Mavlink::ArtooSysID, Mavlink::ArtooComponentID, msg,
			sysid, MAV_COMP_ID_SYSTEM_CONTROL,
			MAV_CMD_COMPONENT_ARM_DISARM,
			0,                    // confirmation
			(command.arm == Armed) ? 1 : 0,  // 1 for arm, 0 for disarm
			(command.arm == DisarmForce) ? 0 : 0,
			0, 0, 0, 0, 0);    // params 3-7
		break;

		//case Command::Takeoff:
		//	mavlink_msg_command_long_pack(Mavlink::ArtooSysID, Mavlink::ArtooComponentID, msg,
		//		Mavlink::SoloSysID, 1,
		//		MAV_CMD_NAV_TAKEOFF,
		//		0,                // confirmation
		//		0, 0, 0, 0, 0, 0, // params 1-6
		//		TakeoffAltitude); // param 7 - altitude in meters
		//	break;

	case Command::SetFlightMode:
		mavlink_msg_set_mode_pack(
			Mavlink::instance.ArtooSysID,
			Mavlink::instance.ArtooComponentID,
			msg,
			sysid,
			MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, modeEnumToType(command.flightMode));
		break;

		//case Command::FlyButtonClick:
		//	mavlink_msg_command_long_pack(Mavlink::ArtooSysID, Mavlink::ArtooComponentID, msg,
		//		Mavlink::SoloSysID, MAV_COMP_ID_SYSTEM_CONTROL,
		//		MAV_CMD_SOLO_BTN_FLY_CLICK,
		//		0,                    // confirmation
		//		0, 0, 0, 0, 0, 0, 0);    // params 1-7
		//	break;

		//case Command::FlyButtonHold:
		//	mavlink_msg_command_long_pack(Mavlink::ArtooSysID, Mavlink::ArtooComponentID, msg,
		//		Mavlink::SoloSysID, MAV_COMP_ID_SYSTEM_CONTROL,
		//		MAV_CMD_SOLO_BTN_FLY_HOLD,
		//		0,                    // confirmation
		//		TakeoffAltitude,
		//		0, 0, 0, 0, 0, 0);    // params 2-7
		//	break;

		//case Command::PauseButtonClick:
		//	mavlink_msg_command_long_pack(Mavlink::ArtooSysID, Mavlink::ArtooComponentID, msg,
		//		Mavlink::SoloSysID, MAV_COMP_ID_SYSTEM_CONTROL,
		//		MAV_CMD_SOLO_BTN_PAUSE_CLICK,
		//		0,                    // confirmation
		//		0,                    // param 1: inShot not used yet
		//		0, 0, 0, 0, 0, 0);    // params 2-7
		//	break;

		//case Command::GetHomeWaypoint:
		//	mavlink_msg_mission_request_pack(Mavlink::ArtooSysID, Mavlink::ArtooComponentID, msg,
		//		Mavlink::SoloSysID, Mavlink::SoloComponentID,
		//		command.waypoint);
		//	break;

	default:
		break;
	}
}

const int UAV::modeEnumToTypeOption(int plane, int copter, int rover) {
	return uavType == 1 ? plane : uavType == 10 ? rover : copter;
}

bool UAV::isOnline() {

	return online;
}
bool UAV::isActive() {

	return active;
}
void UAV::setActive(bool s) {
	if (s == true) {
		if (isOnline()) {
			UiTopBar::instance.setTitle(modeToString());
		}
		else {
			if (sysid == 1) {
				UiTopBar::instance.setTitle("1:lost");
			}
			else if (sysid == 2) {
				UiTopBar::instance.setTitle("2:lost");
			}
			else if (sysid == 3) {
				UiTopBar::instance.setTitle("3:lost");
			}
			else {
				UiTopBar::instance.setTitle("0:wait");
			}

		}
	}
	else {
		channels[0] = 1500;
		channels[1] = 1500;
		channels[2] = 1500;
		channels[3] = 1500;
	}
	active = s;
}

const int UAV::modeEnumToType(FlightMode m) {
	switch (m) {
	case ACRO:
		return modeEnumToTypeOption(4, 1, 4);
	case ALT_HOLD:
		return modeEnumToTypeOption(12, 2, 4);
	case AUTO:
		return modeEnumToTypeOption(10, 3, 10);
	case AUTOTUNE:
		return modeEnumToTypeOption(8, 15, 4);
	case AVOID_ADSB:
		return modeEnumToTypeOption(14, 19, 4);
	case BRAKE:
		return modeEnumToTypeOption(12, 17, 4);
	case CIRCLE:
		return modeEnumToTypeOption(1, 7, 4);
	case CRUISE:
		return modeEnumToTypeOption(7, 5, 4);
	case DRIFT:
		return modeEnumToTypeOption(12, 11, 4);
	case FLIP:
		return modeEnumToTypeOption(12, 14, 4);
	case FLOWHOLD:
		return modeEnumToTypeOption(12, 22, 4);
	case FLY_BY_WIRE_A:
		return modeEnumToTypeOption(5, 5, 4);
	case FLY_BY_WIRE_B:
		return modeEnumToTypeOption(6, 5, 4);
	case FOLLOW:
		return modeEnumToTypeOption(12, 23, 6);
	case GUIDED:
		return modeEnumToTypeOption(15, 4, 15);
	case GUIDED_NOGPS:
		return modeEnumToTypeOption(12, 20, 4);
	case HOLD:
		return modeEnumToTypeOption(12, 5, 4);
	case LAND:
		return modeEnumToTypeOption(12, 9, 4);
	case LOITER:
		return modeEnumToTypeOption(12, 5, 5);
	case MANUAL:
		return modeEnumToTypeOption(0, 5, 0);
	case POSHOLD:
		return modeEnumToTypeOption(12, 16, 4);
	case QAUTOTUNE:
		return modeEnumToTypeOption(22, 5, 4);
	case QHOVER:
		return modeEnumToTypeOption(18, 5, 4);
	case QLAND:
		return modeEnumToTypeOption(20, 5, 4);
	case QLOITER:
		return modeEnumToTypeOption(19, 5, 4);
	case QRTL:
		return modeEnumToTypeOption(21, 5, 4);
	case QSTABILIZE:
		return modeEnumToTypeOption(17, 5, 4);
	case RTL:
		return modeEnumToTypeOption(11, 6, 11);
	case SIMPLE:
		return modeEnumToTypeOption(12, 5, 7);
	case SMART_RTL:
		return modeEnumToTypeOption(12, 21, 12);
	case SPORT:
		return modeEnumToTypeOption(12, 13, 4);
	case STABILIZE:
		return modeEnumToTypeOption(2, 0, 4);
	case STEERING:
		return modeEnumToTypeOption(12, 5, 3);
	case THROW:
		return modeEnumToTypeOption(12, 18, 4);
	case TRAINING:
		return modeEnumToTypeOption(3, 5, 4);
	}
}
const char *UAV::modeToString() {

	if (uavType == 1) {
		switch (uavMode) {
		case 0:  return "MANUAL";
		case 1:  return "CIRCLE";
		case 2:  return "STABILIZE";
		case 3:  return "TRAINING";
		case 4:  return "ACRO";
		case 5:  return "FLY_BY_WIRE_A";
		case 6:  return "FLY_BY_WIRE_B";
		case 7:  return "CRUISE";
		case 8:  return "AUTOTUNE";
		case 10: return "AUTO";
		case 11: return "RTL";
		case 12: return "LOITER";
		case 14: return "AVOID_ADSB";
		case 15: return "GUIDED";
		case 16: return "INITIALISING";
		case 17: return "QSTABILIZE";
		case 18: return "QHOVER";
		case 19: return "QLOITER";
		case 20: return "QLAND";
		case 21: return "QRTL";
		case 22: return " QAUTOTUNE";
		}
	}
	else if (uavType == 10) {
		switch (uavMode) {
		case 0:  return "MANUAL";
		case 1:  return "ACRO";
		case 3:  return "STEERING";
		case 4:  return "HOLD";
		case 5:  return "LOITER";
		case 6:  return "FOLLOW";
		case 7:  return "SIMPLE";
		case 10: return "AUTO";
		case 11: return "RTL";
		case 12: return "SMART_RTL";
		case 15: return "GUIDED";
		case 16: return "INITIALISING";
		}
	}
	else {
		switch (uavMode) {
		case 0:  return "STABILIZE";
		case 1:  return "ACRO";
		case 2:  return "ALT_HOLD";
		case 3:  return "AUTO";
		case 4:  return "GUIDED";
		case 5:  return "LOITER";
		case 6:  return "RTL";
		case 7:  return "CIRCLE";
		case 9:  return "LAND";
		case 11: return "DRIFT";
		case 13: return "SPORT";
		case 14: return "FLIP";
		case 15: return "AUTOTUNE";
		case 16: return "POSHOLD";
		case 17: return "BRAKE";
		case 18: return "THROW";
		case 19: return "AVOID_ADSB";
		case 20: return "GUIDED_NOGPS";
		case 21: return "SMART_RTL";
		case 22: return "FLOWHOLD";
		case 23: return "FOLLOW";
		case 24: return "ZIGZAG";
		}
	}

}