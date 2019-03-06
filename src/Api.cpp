#include <iostream>
#include "App/Protocol.h"
#include "App/Server.h"

using namespace std;

int main(int argc, const char *argv[]) {
  int id, data_size;
  
  if (argc > 1){
    id = atoi(argv[1]);
  }
  else {
    cout << "Enter Player Id. Exiting"<<endl;
    return -1;
  }
  int max_c =3;
  Server s(id, 14000 + id, max_c);

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
    case State::CLIENT_INFO_RECEIVED :
        cout << "Ok Client_rcvd" <<endl;
        s.state_transition();
        break;
    case State::RANDOMNESS_SENT :
        cout << "Ok rand" <<endl;
        s.state_transition();
        break;
    case State::DATASET_ACCEPTED :
        cout << "Ok data acc" <<endl;
        return 0;
          }
    }

  }

  // while(1){
  //   int a = s.receive_int_from(0);
  //   cout << a << endl;
  //   s.send_int_to(0, a*a);
  // 
