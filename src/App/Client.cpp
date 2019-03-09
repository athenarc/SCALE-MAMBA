#include <thread>
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

void sedp::Client::connect_to_players(vector <pair <string,int>> player_addresses){
  struct sockaddr_in addr;
  vector <struct sockaddr_in> Player_addresses; 
  for (vector<pair <string,int>>::iterator it = player_addresses.begin() ; it != player_addresses.end(); ++it){
    string IP = (*it).first;
    int port = (*it).second;
    bzero(&addr, sizeof(addr));
    addr.sin_family= AF_INET;
    addr.sin_addr.s_addr= INADDR_ANY;
    addr.sin_port= htons(port);
    Player_addresses.push_back(addr);
  }
  for (vector<struct sockaddr_in>::iterator it = Player_addresses.begin() ; it != Player_addresses.end(); ++it){
    connect_to_player(*it);
  }

}

int sedp::Client::get_id(){
  return client_id;
}

void sedp::Client::compute_mask(){
  for (vector<vector <int>>::iterator it = Shares.begin() ; it != Shares.end(); ++it){
        int mask = 0;
        for (vector <int>::iterator it2 = (*it).begin() ; it2 != (*it).end(); ++it2){
          mask = mask + *it2;
        }
      Mask.push_back(mask);
    }
}

void sedp::Client::send_dataset_size() {
  int datum;
  thread t[max_players];
  cout << "Counting my dataset size..." << endl;
  sleep(1);
  inpf.open("Client_data" + to_string(client_id) + ".txt");
  if (!inpf){
    throw file_error("Unable to read file..");
  }
  inpf >> datum;
  my_data.push_back(datum);
  dataset_size++;

  while (!inpf.eof()){
    inpf >> datum;
    my_data.push_back(datum);
    dataset_size++;
  }
  inpf.close();

  for (unsigned int i =0; i<players.size(); i++){
    t[i] = thread(&Client::send_int_to, this, players.at(i), dataset_size);
  }
  //wait for threads to finish
  for (unsigned int i =0; i<players.size(); i++){
    t[i].join();
  }
  cout << "Succesfully sent dataset size!" <<endl;

  // Initialize shares matrix
  unsigned int counter = 0;
  while (counter < dataset_size){
    vector<int> v;
    for (unsigned int i =0; i<players.size(); i++){
      v.push_back(0);
    }
    Shares.push_back(v);
    counter++;
  }

}

void sedp::Client::send_private_inputs(int player_id) {

  cout << "Sending private data..." <<endl;
  sleep(3);
  int counter = 0;

  while (counter < dataset_size) {
    send_int_to(players.at(player_id), my_data.at(counter) - Mask.at(counter));
    counter++;
  }
  cout << "\nSuccesfully sent my data to player " + to_string(player_id) + "!" <<endl;

}

void sedp::Client::get_random_triples(int player_id) {

  cout << "\nListening for shares of player" + to_string(player_id) + "..." <<endl;
  sleep(3);

  int counter = 0;

  while (counter < dataset_size){
    int share = receive_int_from(players.at(player_id));
    Shares.at(counter).at(player_id) = share;
    counter++;
  }
  cout << "Succesfully received shares of player" + to_string(player_id) + "!" <<endl;
}

void sedp::Client::run_protocol() {
  
  protocol_states.push_back(State::INITIAL);
  while(1){
    switch(protocol_states.at(0)) {
      case State::INITIAL: {
        send_dataset_size();
        for (vector<State>::iterator it = protocol_states.begin() ; it != protocol_states.end(); ++it){
          (*it) = State::RANDOMNESS_SENT;
        }
        break;
      }

      case State::RANDOMNESS_SENT: {
        thread t[players.size()];
        for (unsigned int i = 0; i<players.size(); i++){
          t[i] = thread(&Client::get_random_triples, this, i);
        }
        for (unsigned int i= 0; i<players.size(); i++){
          t[i].join();
        }

        compute_mask();
        
        for (vector<State>::iterator it = protocol_states.begin() ; it != protocol_states.end(); ++it){
          (*it) = State::PRIVATE_INPUTS;
        }
        break;
      }

      case State::PRIVATE_INPUTS: {
        thread t[players.size()];
        for (unsigned int i =0; i<players.size(); i++){
          t[i] = thread(&Client::send_private_inputs, this, i);
        }
        for (unsigned int i=0; i<players.size(); i++){
          t[i].join();
        }
        
        for (vector<State>::iterator it = protocol_states.begin() ; it != protocol_states.end(); ++it){
          (*it) = State::DATASET_ACCEPTED;
        }
        break;
      }

      case State::DATASET_ACCEPTED:{
        cout << "Import Protocol completed!" << endl;
        exit(1);
      }
      
    } // end switch
  } // end while-loop
} // end run_protocol
