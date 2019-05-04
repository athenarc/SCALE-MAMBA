#!/bin/bash

helpFunction()
{
   echo ""
   echo "Usage: $0 -n <id> -c <certificate> -k <key> -d <domain>"
   echo -e "\t-n Node ID"
   echo -e "\t-c Root certificate"
   echo -e "\t-k Root key"
   echo -e "\t-d Domain name (Common Name)"
   exit 1 # Exit script after printing help
}

while getopts "c:k:d:n:" opt
do
   case "$opt" in
      n ) id="$OPTARG" ;;
      c ) cert="$OPTARG" ;;
      k ) key="$OPTARG" ;;
      d ) cn="$OPTARG" ;;
      ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameters are empty
if [ -z "$id" ] || [ -z "$cert" ] || [ -z "$key" ] || [ -z "$cn" ]
then
   echo "Some or all of the parameters are empty";
   helpFunction
fi

openssl genrsa -out Player$id.key 2048

openssl req -new -key Player$id.key -out Player$id.csr -subj "/C=GR/ST=Attica/L=Athens/O=ATHENA RIC/OU=IMSI/CN=$cn"

openssl x509 -req -days 1000 -in Player$id.csr -CA $cert -CAkey $key -set_serial 0101 -out Player$id.crt -sha256
