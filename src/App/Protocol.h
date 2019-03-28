#ifndef _Protocol
#define _Protocol

namespace sedp {
  enum class State { INITIAL, HANDSHAKE, RANDOMNESS, DATA, FINISHED };
}

#endif
