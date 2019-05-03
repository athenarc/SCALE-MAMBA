#!/bin/bash
helpFunction()
{
   echo ""
   echo "Usage: $0 -n <name> -d <domain>"
   echo -e "\t-n Root CA file name"
   echo -e "\t-d Domain name (Common Name)"
   exit 1 # Exit script after printing help
}

while getopts "c:k:d:n:" opt
do
   case "$opt" in
      n ) name="$OPTARG" ;;
      d ) cn="$OPTARG" ;;
      ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameters are empty
if [ -z "$name" ] || [ -z "$cn" ]
then
   echo "Some or all of the parameters are empty";
   helpFunction
fi

openssl genrsa -out $name.key 4096
openssl req -new -x509 -days 1826 -key $name.key -out $name.crt -subj "/C=GR/ST=Attica/L=Athens/O=ATHENA RIC/OU=IMSI/CN=$cn"
openssl x509 -in $name.crt -out $name.pem -outform PEM
