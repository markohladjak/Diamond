#include <Net/INetService.h>
#include <Net/NetAddress.h>
#include <Net/NetMessage.h>

namespace diamon {

void INetService::SetMessageEnds(NetMessage &msg, const NetAddress *from, const NetAddress *to) {
	if (from) msg._from = *from;
	if (to) msg._to = *to;
}

}
