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

  int client_sd = accept(socket_id, (struct sockaddr *) &addr, &len);

  if (client_sd == -1)
  {
    string err= "Unable to accept connections : Error code " + errno;
    throw Networking_error(err);
  }

  printf("Accepted Connection: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

  uint8_t buff[4];
  receive_msg(client_sd, buff, 4);
  int client_id = BYTES_TO_INT(buff);

  cout << "Client with id " << client_id << " connected." <<endl;

  clients.insert(make_pair(client_id, client_sd));
  send_int_to(client_id, player_id);
}

sedp::State sedp::Server::get_state(){
  return protocol_state;
}

void sedp::Server::run_protocol() {
  int dataset_size = 0; // Temp: We need to have a structure that saves the client's info such as dataset_size etc. Maybe in map<int, map<int, string, ...>>
  int counter = 0;

  while(protocol_state != State::DATASET_ACCEPTED){
    switch(protocol_state) {
      case State::INITIAL: {
        dataset_size = receive_int_from(0);         
        protocol_state = State::RANDOMNESS_SENT;
        break;
      }

      case State::RANDOMNESS_SENT: {
        cout << "Sending my Shares..." << endl;

        sleep(3);
        counter = dataset_size;

        while (counter > 0) {
          send_int_to(0, counter); // Need to send actuall shares!
          counter--;
        }

        protocol_state = State::MASK_DATA;
        break;
      }

      case State::MASK_DATA: {
        cout << "Importing data..." <<endl;
        sleep(3);
        counter = 0;

        while (counter < dataset_size){
          int datum = receive_int_from(0);
          cout << datum << endl;
          counter++;
        }

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

void sedp::Server::send_int_to(unsigned int client_id, unsigned int x)
{
  uint8_t buff[4];
  INT_TO_BYTES(buff, x);
  send_msg(clients.find(client_id)->second, buff, 4);
}


int sedp::Server::receive_int_from(unsigned int client_id)
{ 
  uint8_t buff[4];
  receive_msg(clients.find(client_id)->second, buff, 4);
  return BYTES_TO_INT(buff);
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
