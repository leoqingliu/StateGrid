#pragma once

#include "NamedPipe.h"

class PipeClient
{
public:
	PipeClient(void);
	~PipeClient(void);

public:
	NamedPipe *Connect(LPCTSTR name);
};
