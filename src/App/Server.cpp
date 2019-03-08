#include "Server.h"

sedp::Server::Server(unsigned int id, unsigned int port, unsigned int max_clients):
  ProtocolEntity(), player_id{id}, port_number{port}, max_clients{max_clients}, dataset_size{0}
{
  cout << "Server (Player) " << player_id << ": Start listening at port " << port << endl;
  socket_id = OpenListener(port_number, max_clients);
}

sedp::Server::~Server() {
  cout << "Closing server...." << endl;
  close(socket_id);
}

void sedp::Server::accept_clients() {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t len = sizeof(addr);

  cout << "Waiting for client connection" << endl;

  int client_sd = accept(socket_id, (struct sockaddr *) &addr, &len);

  if (client_sd == -1)
  {
    string err= "Unable to accept connections : Error code " + errno;
    throw Networking_error(err);
  }

  printf("Accepted Connection: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

  int client_id = receive_int_from(client_sd);

  cout << "Client with id " << client_id << " connected." <<endl;

  clients.insert(make_pair(client_id, client_sd));
  send_int_to(client_sd, player_id);
}

sedp::State sedp::Server::get_state(){
  return protocol_state;
}

void sedp::Server::get_dataset_size() {
  dataset_size = receive_int_from(clients.find(0)->second);     
}


void sedp::Server::send_random_triples() {

  cout << "Sending my Shares..." << endl;

  sleep(3);
  int counter = dataset_size;

  while (counter > 0) {
    send_int_to(clients.find(0)->second, counter); // Need to send actuall shares!
    counter--;
  }

}

void sedp::Server::get_private_inputs() {

  cout << "Importing data..." <<endl;
  sleep(3);
  int counter = 0;

  while (counter < dataset_size){
    int datum = receive_int_from(clients.find(0)->second);
    cout << datum << endl;
    counter++;
  }
}

void sedp::Server::run_protocol() {
  while(protocol_state != State::DATASET_ACCEPTED){
    switch(protocol_state) {
      case State::INITIAL: {
        get_dataset_size();
        protocol_state = State::RANDOMNESS_SENT;
        break;
      }

      case State::RANDOMNESS_SENT: {
        send_random_triples();
        protocol_state = State::PRIVATE_INPUTS;
        break;
      }

      case State::PRIVATE_INPUTS: {
        get_private_inputs();
        protocol_state = State::DATASET_ACCEPTED;
        break;
      }

      case State::DATASET_ACCEPTED: {
        cout << "Data has been imported." <<endl;
        break;
      }

    } // end switch
  } // end while-loop
} // end run_protocol
