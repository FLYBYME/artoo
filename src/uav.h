#ifndef UAV_H
#define UAV_H

#include "tasks.h"
#include "hostprotocol.h"
#include "telemetry.h"
#include "geo.h"
#include "home.h"
#include "button.h"


#include "stm32/common.h"
#include "stm32/systime.h"

#include "mavlink/c_library/ardupilotmega/mavlink.h"
#include "mavlink/c_library/common/mavlink.h"
#include "mavlink.h"

class UAV {
public:
	UAV();  // do not implement

	static UAV instance1;
	static UAV instance2;
	static UAV instance3;
	static UAV current() {
		if (instance1.isActive()) {
			return instance1;
		}
		if (instance2.isActive()) {
			return instance2;
		}
		if (instance3.isActive()) {
			return instance3;
		}
		return instance1;

	};
	enum FlightMode {
		ACRO,
		ALT_HOLD,
		AUTO,
		AUTOTUNE,
		AVOID_ADSB,
		BRAKE,
		CIRCLE,
		CRUISE,
		DRIFT,
		FLIP,
		FLOWHOLD,
		FLY_BY_WIRE_A,
		FLY_BY_WIRE_B,
		FOLLOW,
		GUIDED,
		GUIDED_NOGPS,
		HOLD,
		LAND,
		LOITER,
		MANUAL,
		POSHOLD,
		QAUTOTUNE,
		QHOVER,
		QLAND,
		QLOITER,
		QRTL,
		QSTABILIZE,
		RTL,
		SIMPLE,
		SMART_RTL,
		SPORT,
		STABILIZE,
		STEERING,
		THROW,
		TRAINING,
		ZIGZAG,
	};
	enum ArmState {
		Disarmed = 0,
		Armed = 1,
		DisarmForce = 2,
	};
	struct Command {
		enum State {
			Complete,   // complete
			Pending,    // command ready to be sent
			Sent,       // sent, waiting for ack
		};

		// we pend a mix of MAV_CMD and mavlink msg ids,
		// so must map between them to avoid conflicts.
		enum ID {
			None,
			SetFlightMode,
			SetArmState,
			GetHomeWaypoint,
			Takeoff,
			FlyButtonClick,
			FlyButtonHold,
			PauseButtonClick,
		};

		ID id;
		State state;
		// data that must accompany this command
		union {
			FlightMode flightMode;
			uint8_t arm;    // arm, disarm, or force disarm
			uint16_t waypoint;
		};

		void set(ID id_) {
			ASSERT(state != Pending);
			id = id_;
			state = Pending;
			HostProtocol::instance.requestTransaction();
		}
	};


	void onMavlinkMsg(const mavlink_message_t * msg);

	void resetLinkConnCount(const mavlink_message_t * msg);
	void sysHeartbeat();
	void linkConnected();
	void linkDisconnected();

	void rcOverRide(HostProtocol::Packet &p);
	void requestRC() {
		rcRequested = true;
	}


	bool isOnline();
	bool isActive();
	void setActive(bool s);
	void forceRTL();
	int getSysid() {
		return sysid;
	};
	const char *getType() {
		if (uavType == 1) {
			return "PLANE";
		}
		else if (uavType == 10) {
			return "ROVER";
		}
		else {
			return "COPTER";
		}
	};

	bool producePacket(HostProtocol::Packet &p);

	const char *modeToString();
	bool onButtonEvent(Button *b, Button::Event e);


	const int modeEnumToTypeOption(int plane, int copter, int rover);
	const int modeEnumToType(FlightMode m);

	void requestFlightModeChange(FlightMode rover, FlightMode copter, FlightMode plane);


	void sendCmd(mavlink_message_t *msg);
	void cmdAcknowledged(const mavlink_command_ack_t *ack);



	ALWAYS_INLINE bool linkIsConnected() const {
		return linkConnCounter < LINK_CONN_DURATION;
	}


	Telemetry telemetry;

	double distanceFromTakeoff() const {
		if (home.loc().isEmpty() || currentLoc.isEmpty()) {
			return 0;
		}
		return geo::distanceInMeters(home.loc(), currentLoc);
	}

	mavlink_heartbeat_t heartbeat;
	mavlink_radio_status_t radioStatus;
	mavlink_gps_raw_int_t gps_raw_int;
	mavlink_gps2_raw_t gps_raw_int2;
	mavlink_statustext_t statustext;
	mavlink_vfr_hud_t vfr_hud;
	mavlink_global_position_int_t global_pos;
	mavlink_battery_status_t battery_status;
private:

	static const unsigned CHANNEL_COUNT = 8;

	uint16_t channels[CHANNEL_COUNT];

	SysTime::Ticks syncDeadline;

	FlightMode mode;
	uint32_t uavMode;
	uint8_t uavType;

	bool sendRC = false;
	bool online = false;
	bool active = false;
	int sysid = 5;
	bool forcedRTL = false;

	bool rcRequested = false;

	char uavModeString[32];
	char uavTypeString[32];

	unsigned linkConnCounter;

	static const unsigned LINK_CONN_DURATION = Tasks::HEARTBEAT_HZ * 3;

	Coord2D currentLoc;
	Home home;

	Command command;
};

#endif // UAV_H
