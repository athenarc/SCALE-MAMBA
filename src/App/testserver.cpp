#include "Server.h"
#include "System/Player.cpp"


int main(){
  SSL *ssl;
  int socket_id;
  int port_number = 12345;
  SSL_load_error_strings();	
  OpenSSL_add_ssl_algorithms();
  OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
  SSL_CTX * ctx= InitCTX();
  // Load in my certificates
  string str_crt= "/home/gpik/SCALE-MAMBA/Cert-Store/Player" + to_string(0+1) + ".crt";
  string str_key= str_crt.substr(0, str_crt.length() - 3) + "key";
  LoadCertificates(ctx, str_crt.c_str(), str_key.c_str());
  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
  NULL);
  string str= "/home/gpik/SCALE-MAMBA/Cert-Store/RootCA.crt";
  SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file(str.c_str()));
  SSL_CTX_load_verify_locations(ctx, str.c_str(), NULL);
  ssl = SSL_new(ctx);
  socket_id = OpenListener(port_number, 3);
  int ret = SSL_set_fd(ssl, socket_id);
  if (ret == 0){
    throw SSL_error("SSL_set_fd");
  }
  cout << "Server (Player) " << 0 << ": Start listening at port " << port_number << endl;

  int i =0;
  struct sockaddr_in addr;
  while (1){
    i++;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);

    cout << "Waiting for client connection" << endl;

    int client_sd = SSL_accept(ssl);
    if (client_sd <= 0){
        cout << SSL_get_error(ssl, client_sd) << endl;
        ERR_print_errors_fp(stdout);
        throw SSL_error("SSL_accept"); 
    }
  }
  cout << "SSL_connection established"<<endl;

  printf("Accepted Connection: %s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
  }
