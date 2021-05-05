/*
 * NetMsg.h
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NETMSG_H_
#define INCLUDE_NETMSG_H_

#include <WString.h>
#include <Net/INetService.h>

namespace diamon {


class NetMessage
{
	friend void INetService::SetMessageEnds(NetMessage &msg, const NetAddress *form, const NetAddress *to);

protected:
	NetAddress _from;
	NetAddress _to;

	comdata_sptr_t _context;

public:
	NetMessage() {}
	NetMessage(CommunicationData *ctx) { SetContext(ctx); }
	NetMessage(const String& json) { Deserialize(json); }

	NetAddress& From() { return _from; }
	NetAddress& To() { return _to; }

	CommunicationData& Context() { return *_context.get(); }

	operator String ();

	void SetContext(CommunicationData *ctx);

	void Deserialize(const String& json);
};

}

#endif /* INCLUDE_NETMSG_H_ */
