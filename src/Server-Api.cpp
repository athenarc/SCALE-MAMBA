#include <iostream>
#include "App/Protocol.h"
#include "App/Server.h"

using namespace std;
using namespace sedp;

int main(int argc, const char *argv[]) {
  int id;
  int max_clients = 3;

  if (argc < 2) {
    cout << "Usage: ./Server-Api.x <server_id>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);

  // use clock as seed
  srand(time(0) + id);

  Server s(id, 14000 + id, max_clients);
  s.init();
} 
