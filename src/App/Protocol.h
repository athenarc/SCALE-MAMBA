#ifndef _Protocol
#define _Protocol

namespace sedp {
  enum class State { INITIAL, HANDSHAKE, RANDOMNESS_SENT, PRIVATE_INPUTS, DATASET_ACCEPTED };
}

#endif
