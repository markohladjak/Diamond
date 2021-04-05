/*
 * NetEvent.h
 *
 *  Created on: Feb 27, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NET_NETEVENT_H_
#define INCLUDE_NET_NETEVENT_H_


namespace diamon {

class NetEvent {
public:
	enum NetEvents {
		UNKNOW_EVENT,
		DEVICE_STATUS_CHANGED,
		REQUEST_DEVICE_STATE,
		DEVICE_STATE_REPORT,
		VERSION_REQUEST,
		VERSION_REPORT
	};

	NetEvent() {}
	NetEvent(int event): _event((NetEvents)event) {}

	operator String() { return String((int)_event); }
	bool operator == (NetEvents event) { return event == _event; }

private:

	NetEvents _event = UNKNOW_EVENT;
};

}


#endif /* INCLUDE_NET_NETEVENT_H_ */
