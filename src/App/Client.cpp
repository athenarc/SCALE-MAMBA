#include "Client.h"

sedp::Client::Client(unsigned int id, unsigned int max_players):
  client_id{id}, max_players{max_players}
{
  cout << "Client " << client_id << endl;
  
}

sedp::Client::~Client() {
    cout << "Closing client..." << endl;
    close(socket_id);
}

int sedp::Client::connect_to_player(const char* ip_address, int port) {
  struct sockaddr_in addr;
  int socket_id = socket(AF_INET , SOCK_STREAM , 0);

  if (socket_id == -1)
	{
		printf("Could not create socket");
	}

  bzero(&addr, sizeof(addr));
  addr.sin_family= AF_INET;
  addr.sin_addr.s_addr= INADDR_ANY;
  addr.sin_port= htons(port);

  if (connect(socket_id , (struct sockaddr *)&addr , sizeof(addr)) < 0)
	{
		perror("connect failed. Error");
    return -1;
	}

  players.push_back(socket_id);
  send_int_to(0, client_id);
  cout << "Connected to player " << receive_int_from(0) << endl;
  return 1;
}

sedp::State sedp::Client::get_state(){
  return protocol_state;
}

int sedp::Client::get_id(){
  return client_id;
}

void sedp::Client::run_protocol() {
  int dataset_size = 10; // Temp: We need to count our dataset size.
  int counter = 0;

  while(protocol_state != State::DATASET_ACCEPTED){
    switch(protocol_state) {
      case State::INITIAL: {
        cout << "Counting my dataset size..." << endl;
        send_int_to(0, dataset_size);
        protocol_state = State::RANDOMNESS_SENT;
        break;
      }

      case State::RANDOMNESS_SENT: {
        cout << "Listening for shares..." <<endl;
        sleep(3);

        counter = dataset_size;

        while (counter > 0){
          int share = receive_int_from(0);
          cout << share << endl;
          counter--;
        }

        protocol_state = State::MASK_DATA;
        break;
      }

      case State::MASK_DATA: {
        cout << "Masking data..." <<endl;
        sleep(3);
        counter = 0;

        while (counter < dataset_size) {
          send_int_to(0, counter + counter);
          counter++;
        }

        protocol_state = State::DATASET_ACCEPTED;
        break;
      }

      case State::DATASET_ACCEPTED: {
        cout << "Data has been sent!" <<endl;
        break;
      }

    } // end switch
  } // end while-loop
} // end run_protocol

void sedp::Client::send_int_to(unsigned int player_id, unsigned int x)
{
  uint8_t buff[4];
  INT_TO_BYTES(buff, x);

  if (player_id >= max_players) {
      cout << "No player " << player_id <<endl;
      exit(-1);
  }

  send_msg(players.at(player_id), buff, 4);
}

int sedp::Client::receive_int_from(unsigned int player_id)
{
  uint8_t buff[4];

  if (player_id >= max_players) {
      cout << "No player " << player_id <<endl;
      exit(-1);
  }

  receive_msg(players.at(player_id), buff, 4);
  return BYTES_TO_INT(buff);
}

void sedp::Client::send_msg(int socket, uint8_t *msg, int len)
{
  if (send(socket, msg, len, 0) != len)
    {
      throw Networking_error("Send error - 1 ");
    }
}

void sedp::Client::receive_msg(int socket, uint8_t *msg, int len)
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
