#include "Client.h"

sedp::Client::Client(unsigned int id, unsigned int max_players):
  client_id{id}, max_players{max_players}, dataset_size{0}
{
  cout << "Client " << client_id << endl;
  
}

sedp::Client::~Client() {
  cout << "Closing client..." << endl;
  close(socket_id);
}

int sedp::Client::connect_to_player(struct sockaddr_in addr) {
  
  int socket_id = socket(AF_INET , SOCK_STREAM , 0);

  if (socket_id == -1)
	{
    throw Networking_error("Receiving error - 1");
	}

  if (connect(socket_id , (struct sockaddr *)&addr , sizeof(addr)) < 0)
	{
		perror("Connect failed. Error");
    return -1;
	}

  players.push_back(socket_id);
  send_int_to(socket_id, client_id);
  cout << "Connected to player " << receive_int_from(socket_id) << endl;
  return 1;
}

void sedp::Client::connect_to_players(vector <struct sockaddr_in> player_addresses){
  int counter = 0;
  struct sockaddr_in addr;
  while(counter < max_players){
    addr = player_addresses.at(counter);
    connect_to_player(addr);
    counter++;
  }

}

int sedp::Client::get_id(){
  return client_id;
}

void sedp::Client::send_dataset_size(int player_id) {
  cout << "Counting my dataset size..." << endl;
  dataset_size = 10;
  send_int_to(players.at(player_id), dataset_size);
}

void sedp::Client::send_private_inputs(int player_id) {

  cout << "Sending private data..." <<endl;
  sleep(3);
  int counter = 0;

  while (counter < dataset_size) {
    send_int_to(players.at(player_id), counter + counter);
    counter++;
  }

}

void sedp::Client::get_random_triples(int player_id) {

  cout << "Listening for shares..." <<endl;
  sleep(3);

  int counter = dataset_size;

  while (counter > 0){
    int share = receive_int_from(players.at(player_id));
    cout << share << endl;
    counter--;
  }

}

void sedp::Client::run_protocol(int player_id) {
  protocol_states.push_back(State::INITIAL);
  while(protocol_states.at(player_id) != State::DATASET_ACCEPTED){
    switch(protocol_states.at(player_id)) {
      case State::INITIAL: {
        send_dataset_size(player_id);
        protocol_states.at(player_id) = State::RANDOMNESS_SENT;
        break;
      }

      case State::RANDOMNESS_SENT: {
        get_random_triples(player_id);
        protocol_states.at(player_id) = State::PRIVATE_INPUTS;
        break;
      }

      case State::PRIVATE_INPUTS: {
        send_private_inputs(player_id);
        protocol_states.at(player_id) = State::DATASET_ACCEPTED;
        break;
      }

      case State::DATASET_ACCEPTED:{
        break;
      }
      
    } // end switch
  } // end while-loop
} // end run_protocol
