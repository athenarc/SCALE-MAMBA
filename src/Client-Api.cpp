#include <iostream>
#include "App/Client.h"

using namespace std;
using namespace sedp;

int main(int argc, const char *argv[]) {

  int id;

  if (argc < 2) {
    cout << "Usage: ./Client-Api.x <client_id>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);
  int max_players = 1;

  Client c(id, max_players);

  int f = c.connect_to_player("127.0.0.1", 14000);
  
  if (f == -1){
    exit(-1);
  }

  c.run_protocol();
  
  return 0;
}
