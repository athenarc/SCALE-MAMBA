#ifndef _Api
#define _Api

#include "Server.h"
#include "Client.h"

namespace sedp {
  Server listen_for_clients(); // Start listening for client connections
  void accept_clients(Server& s); // Accepts clients
  void get_next_item(); // Get next item from dataset
  Client set_client(); // Set up a client
  void send_private_inputs(Client& c); // Allows a client to send a private input
}

#endif
