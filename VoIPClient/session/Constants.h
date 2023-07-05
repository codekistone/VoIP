#pragma once

namespace CallState {
	constexpr int STATE_DIALING = 1;
	constexpr int STATE_RINGING = 2;
	constexpr int STATE_ACTIVE = 4;
	constexpr int STATE_DISCONNECTED = 7;
	constexpr int STATE_IDLE = 8;
}