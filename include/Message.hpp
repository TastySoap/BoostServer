#pragma once

#include "Packet.hpp"

template<class TPacket = Packet<>>
struct Message{
	using packet_type = TPacket;
	packet_type packet;
};