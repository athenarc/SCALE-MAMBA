#include "Server.h"

sedp::Server::Server(unsigned int id, unsigned int port, unsigned int max_clients):
  player_id{id}, port_number{port}, max_clients{max_clients}
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

  int client = accept(socket_id, (struct sockaddr *) &addr, &len);

  printf("Accepted Connection: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

  uint8_t buff[4];
  receive_msg(client, buff, 4);

  cout << "Client " << BYTES_TO_INT(buff) << " connected." <<endl;

  clients.push_back(client);
  send_int_to(0, player_id);
  
  if (client == -1)
  {
    string err= "Unable to accept connections : Error code " + errno;
    throw Networking_error(err);
  }
  
}

sedp::State sedp::Server::get_state(){
  return protocol_state;
}


void sedp::Server::state_transition(){
  switch(protocol_state){
    case State::INITIAL :
        protocol_state = State::RANDOMNESS_SENT;
        break;
    case State::RANDOMNESS_SENT :
        protocol_state = State::MASK_DATA;
        break;
    case State::MASK_DATA :
        protocol_state = State::DATASET_ACCEPTED;
        break;
    case State::DATASET_ACCEPTED :
        break;
  }
}

void sedp::Server::send_int_to(unsigned int client_id, unsigned int x)
{
  uint8_t buff[4];
  INT_TO_BYTES(buff, x);
  send_msg(clients.at(client_id), buff, 4);
}

int sedp::Server::receive_int_from(unsigned int client_id)
{ 
  if (client_id < max_clients) {
    uint8_t buff[4];
    receive_msg(clients.at(client_id), buff, 4);
    return BYTES_TO_INT(buff);
  } else{
    cout << "No connection" <<endl;
    exit(-1);
  }
}

void sedp::Server::send_msg(int socket, uint8_t *msg, int len)
{
  if (send(socket, msg, len, 0) != len)
    {
      throw Networking_error("Send error - 1 ");
    }
}

void sedp::Server::receive_msg(int socket, uint8_t *msg, int len)
{
  int i = 0, j;
  while (len - i > 0)
    {
      j = recv(socket, msg + i, len - i, 0);
      if (j < 0)
        {
          throw Networking_error("Receiving error - 1");
        }
      i = i + j;
    }
}
