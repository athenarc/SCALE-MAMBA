#include <thread>
#include <mutex>
#include "Client.h"


sedp::Client::Client(unsigned int id, unsigned int max_players):
  client_id{id}, max_players{max_players}, dataset_size{0}
{
  cout << "Client " << client_id << endl;
  
}

sedp::Client::~Client() {
  cout << "Closing client..." << endl;

  for (vector<int>::iterator it = players.begin() ; it != players.end(); ++it){
    close(*it);
  }

  cout << "Client closed!"<<endl;
}

void sedp::Client::init() {
  int datum;

  inpf.open("Client_data" + to_string(client_id) + ".txt");

  if (!inpf){
    throw file_error("Unable to read file..");
  }

  while (inpf >> datum) {
    data.push_back(datum);
  }

  inpf.close();
  dataset_size = data.size();
  cout << "Dataset size: " << dataset_size << endl;

  // Initialize shares matrix
  for (int i = 0; i < dataset_size; i++) {
    vector<int> v;

    for (unsigned int j = 0; j < max_players; j++) {
      v.push_back(0);
    }

    shares.push_back(v);
  }
}

int sedp::Client::connect_to_player(string ip, int port) {
  
  int socket_id = OpenConnection(ip, port);
 
  return socket_id;
}

void sedp::Client::handshake(int player_id) {
  lock_guard<mutex> g{mtx};
  send_int_to(players.at(player_id), client_id);
  receive_int_from(players.at(player_id));
}

void sedp::Client::connect_to_players(const vector <pair <string, int>>& player_addresses) {
  vector<pair <string, int>>::const_iterator it;

  for (it = player_addresses.begin() ; it != player_addresses.end(); ++it) {
    int player_sd = connect_to_player((*it).first, (*it).second);
    players.push_back(player_sd);
  }

}

int sedp::Client::get_id(){
  return client_id;
}

void sedp::Client::compute_mask() {
  for (vector<vector <int>>::iterator it = shares.begin() ; it != shares.end(); ++it){
    int mask = 0;

    for (vector <int>::iterator it2 = (*it).begin() ; it2 != (*it).end(); ++it2){
      mask = mask + *it2;
    }

    masked_data.push_back(mask);
  }
}

void sedp::Client::send_dataset_size(int player_id) {
  lock_guard<mutex> g{mtx};
  send_int_to(players.at(player_id), dataset_size);
  cout << "Succesfully sent dataset size!" <<endl;
}

void sedp::Client::send_private_inputs(int player_id) {
  lock_guard<mutex> g{mtx};
  cout << "Sending private data..." << endl;
  sleep(3);

  for (int i = 0; i < dataset_size; i++) {
    send_int_to(players.at(player_id), data.at(i) - masked_data.at(i));
  }

  cout << "\nSuccesfully sent my data to player " + to_string(player_id) + "!" << endl;

}

void sedp::Client::get_random_triples(int player_id) {
  lock_guard<mutex> g{mtx};
  cout << "\nListening for shares of player " + to_string(player_id) + "..." << endl;
  sleep(3);

  for (int i = 0; i < dataset_size; i++) {
    int share = receive_int_from(players.at(player_id));
    shares.at(i).at(player_id) = share;
  }

  cout << "Succesfully received shares of player " + to_string(player_id) + "!" << endl;
}

void sedp::Client::run_protocol() {
  while(protocol_state != State::DATASET_ACCEPTED) {
    switch(protocol_state) {
      case State::INITIAL: {
        execute(&Client::handshake);
        protocol_state= State::HANDSHAKE;
        break;
      }

      case State::HANDSHAKE: {
        execute(&Client::send_dataset_size);
        protocol_state = State::RANDOMNESS_SENT;
        break;
      }

      case State::RANDOMNESS_SENT: {
        execute(&Client::get_random_triples);
        compute_mask();
        protocol_state = State::PRIVATE_INPUTS;

        break;
      }

      case State::PRIVATE_INPUTS: {
        execute(&Client::send_private_inputs);
        protocol_state = State::DATASET_ACCEPTED;
        
        break;
      }

      case State::DATASET_ACCEPTED:{
        break;
      }
      
    } // end switch
  } // end while-loop
} // end run_protocol
