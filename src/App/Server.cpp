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
  // Choose random triplet shares and output into a Player_data file
  outf.open("Player_" + to_string(player_id) +"_random_shares.txt");
  if (!outf) {
    throw file_error("Unable to open file...");
  }
  int counter = 0;
  while (counter < dataset_size){
    outf << rand() % 1000000 << endl;
    counter++;
  }
  outf.close();
}


void sedp::Server::send_random_triples() {

  cout << "Sending my Shares..." << endl;

  sleep(3);
  int share, counter = dataset_size;
  inpf.open("Player_" + to_string(player_id) +"_random_shares.txt");
  if (!inpf){
    cout << "Unable to open file." << endl;
    exit(-1);
  }

  while (counter > 0) {
    inpf >> share;
    Shares.push_back(share);
    send_int_to(clients.find(0)->second, share); // Need to send actuall shares!
    counter--;
  }
  inpf.close();
  cout << " Succesfully sent my shares!"<<endl;
}

void sedp::Server::get_private_inputs() {

  cout << "Importing data..." <<endl;
  sleep(3);
  int counter = 0;
  outf.open("Player_out" + to_string(player_id) + ".txt");
  if (!outf){
    throw file_error("Unable to open out files...");
  }
  while (counter < dataset_size){
    int datum = receive_int_from(clients.find(0)->second);
    int secret_share;
    if (player_id == 0){
      secret_share = datum + Shares.at(counter);
    }
    else {
      secret_share = Shares.at(counter);
    }
    outf << secret_share << endl;
    counter++;
  }
  outf.close();
}

void sedp::Server::run_protocol() {
  while(1){
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

      case State::DATASET_ACCEPTED:{
        cout << "Import protocol completed!" << endl;
        exit(1);
      }

    } // end switch
  } // end while-loop
} // end run_protocol
