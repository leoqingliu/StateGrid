#include "stdafx.h"
#include "Packet.h"

CSafeDeque<Packet_t> thePacketManager;
CCriticalSection thePacketManagerLock;
