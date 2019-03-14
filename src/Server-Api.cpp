#include <iostream>
#include "App/Protocol.h"
#include "App/Server.h"

using namespace std;
using namespace sedp;

int main(int argc, const char *argv[]) {
  int id;
  int max_clients;

  if (argc < 3) {
    cout << "Usage: ./Server-Api.x <server_id> <total_clients>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);
  max_clients = atoi(argv[2]);

  // use clock as seed
  srand(time(0) + id);

  Server s(id, 14000 + id, max_clients);
  s.init();
} 
