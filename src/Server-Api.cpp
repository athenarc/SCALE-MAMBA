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

  // open_channel(0)
  Server s(id, 14000 + id, max_clients);
  s.init();

  // open_channel(1)
  // get_public_input(1)
  int n = s.get_data_size();
  cout << "size: " << n << endl;


  // open_channel(2)
  vector<int> shares;

  for (int i = 0; i < n; i++) {
    shares.push_back(rand() % 1000000);
  }

  for (int i = 0; i < n; i++) {
    s.put_random_triple(shares.at(i));
  }

  // open_channel(3)
  vector<int>& data = s.get_data();

  for (auto &d : data) {
    // get_private_input(3);
    cout << d << endl;
  }
} 
