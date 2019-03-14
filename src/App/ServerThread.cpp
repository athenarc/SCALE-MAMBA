#include "ServerThread.h"

sedp::ServerThread::ServerThread(int client_sd, int player_id):
  ProtocolEntity(), client_sd{client_sd}, player_id{player_id}, dataset_size{0} {
    cout << "Server thread created: Client id: " << client_sd << ", Player id: " << player_id << endl;
  }

sedp::ServerThread::~ServerThread() {
  cout << "Closing ServerThread...." << endl;
  close(client_sd);
}

void sedp::ServerThread::handshake() {

  client_id = receive_int_from(client_sd);
  cout << "Client with id " << client_id << " connected." <<endl;
  send_int_to(client_sd, player_id);
}

void sedp::ServerThread::get_dataset_size() {
  dataset_size = receive_int_from(client_sd);

  for (int i = 0; i < dataset_size; i++) {
    shares.push_back(rand() % 1000000);
  }

  outf.close();
}

void sedp::ServerThread::send_random_triples() {

  cout << "Sending my Shares..." << endl;
  sleep(3);

  for (int i = 0; i < dataset_size; i++) {
    send_int_to(client_sd, shares.at(i)); // Need to send actuall shares!
  }

  cout << " Succesfully sent my shares!" << endl;
}

void sedp::ServerThread::get_private_inputs() {

  cout << "Importing data..." << endl;
  sleep(3);

  for (int i = 0; i < dataset_size; i++) {
    int datum = receive_int_from(client_sd);
    int secret_share;
    
    secret_share = datum + shares.at(i);

    data.push_back(secret_share);
  }
}

void sedp::ServerThread::run_protocol() {
  while(protocol_state != State::DATASET_ACCEPTED) {
    switch(protocol_state) {
      case State::INITIAL: {
        handshake();
        protocol_state = State::HANDSHAKE;
        break;
      }

      case State::HANDSHAKE: {
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
        break; // this case in never reached!
      }

    } // end switch
  } // end while-loop
} // end run_protocol

vector<int> sedp::ServerThread::get_data() {
  return data;
}
