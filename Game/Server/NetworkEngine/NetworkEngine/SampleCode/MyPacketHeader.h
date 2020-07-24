#pragma once

#include "../NetworkComponent/NetworkHeader.h"

enum class EMyPacketHeader : uint16_t
{
	Login,
	Accept,
	Reject,
	Create,
	Move,
	Exit
};