#ifndef _Protocol
#define _Protocol

namespace sedp {
  enum class State { INITIAL, RANDOMNESS_SENT, MASK_DATA, DATASET_ACCEPTED };
}

#endif