#include <iostream>
#include "App/Protocol.h"
#include "App/Server.h"

using namespace std;

int main(int argc, const char *argv[]) {
  int id, data_size, counter, datum, share, max_c =3;
  vector<int> data, Shares;

  if (argc < 2) {
    cout << "Usage: ./Server-Api.x <server_id>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);

  Server s(id, 14000 + id, max_c);

  s.inpf.open("Server_data.txt");

  if (!s.inpf) {
        cout << "Unable to open file" <<endl;
        exit(-1); 
    }

  s.accept_clients();

  // The protocol starts here after connections 
  // have been established

  while(1){
    switch(s.get_state()){
      case State::INITIAL :
          data_size = s.receive_int_from(0);

          if (data_size <= 0){
            cout << "Data size must be positive" << endl;
            exit(-1);
          }

          cout << "Got your message: " << data_size << endl;
          s.state_transition();
          break;


      case State::RANDOMNESS_SENT :
          cout << "Sending my Shares..." << endl;
          sleep(3);
          counter = data_size;

          while (counter > 0){
            s.inpf >> share;
            Shares.push_back(share);
            s.send_int_to(0, share);
            counter--;
          }

          s.state_transition();
          break;


      case State::MASK_DATA :
          cout << "Importing data..." <<endl;
          sleep(3);
          counter = 0;

          while (counter < data_size){
            datum = s.receive_int_from(0);
            data.push_back(datum);
            counter++;
          }

          counter = 0;

          while (counter < data_size){
            cout << data.at(counter) << endl;
            counter++;
          }

          s.state_transition();
          break;

      case State::DATASET_ACCEPTED :
          cout << "Data has been imported. Decrypting..." <<endl;
          counter = 0;

          while (counter < data_size){
            share = Shares.at(counter);
            datum = data.at(counter);
            cout << datum - share << endl;
            counter++;
          }

          return 0;
    } // end switch
  } // end while-loop
} // end main