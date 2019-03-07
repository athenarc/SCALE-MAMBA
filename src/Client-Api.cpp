#include <iostream>
#include "App/Client.h"

using namespace std;
using namespace sedp;

int main(int argc, const char *argv[]) {

  int id, int_msg, f, counter, datum, mask, data_size, share;
  vector<int> Shares, my_data;

  if (argc < 2) {
    cout << "Usage: ./Client-Api.x <client_id>"<<endl;
    exit(-1);
  }

  id = atoi(argv[1]);

  Client c(id, 3);

  f = c.connect_to_player("127.0.0.1", 14000);
  
  if (f == -1){
    exit(-1);
  }

  c.inpf.open("Client_data.txt");

  if (!c.inpf) {
    cout << "Unable to open file" <<endl;
    exit(-1);
  }

  // The protocol starts here after connections 
  // have been established

  while(1){
    switch(c.get_state()){
      case State::INITIAL :
          cout << "Counting my dataset size..." << endl;
          sleep(1);
          c.inpf >> datum;
          my_data.push_back(datum);
          data_size = 1;

          while (!c.inpf.eof()){
            c.inpf >> datum;
            my_data.push_back(datum);
            data_size++;
          }

          c.inpf.close();
          c.send_int_to(0, data_size);
          c.state_transition();
          break;

      case State::RANDOMNESS_SENT :
          cout << "Listening for shares..." <<endl;
          sleep(3);
          counter = data_size;

          while (counter > 0){
            share = c.receive_int_from(0);
            cout << share << endl;
            Shares.push_back(share);
            counter--;
          }

          c.state_transition();
          break;


      case State::MASK_DATA :
          cout << "Masking data..." <<endl;
          sleep(3);
          counter = 0;

          while (counter < data_size){
            share = Shares.at(counter);
            datum = my_data.at(counter);
            mask = share + datum;
            c.send_int_to(0, mask);
            counter++;
          }

          c.state_transition();
          break;


      case State::DATASET_ACCEPTED :
        cout << "Data has been sent!" <<endl;
        return 0;
    }
  }
  
  return 0;
}