#ifndef _PIPE_PROTOCOL_H
#define _PIPE_PROTOCOL_H

#define PIPE_PROTOCOL_ID	0x12345678
struct pipe_protocol_t {
	DWORD dwProtocolId;
	BYTE uType;
	BYTE uParams[512];
};

#endif