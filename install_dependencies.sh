mylocal=$1
mkdir -p ${mylocal}
cd ${mylocal}

apt-get install g++ yasm python cmake make m4 zip unzip -y

# install MPIR
curl -O 'http://mpir.org/mpir-3.0.0.tar.bz2'
tar xf mpir-3.0.0.tar.bz2
cd mpir-3.0.0
./configure --enable-cxx --prefix="${mylocal}/mpir"
make && make check && make install

# install OpenSSL 1.1.0
cd $mylocal
curl -O https://www.openssl.org/source/openssl-1.1.1c.tar.gz
tar -xf openssl-1.1.1c.tar.gz
cd openssl-1.1.1c
./config --prefix="${mylocal}/openssl"
make && make install

# install crypto++
cd $mylocal
curl -O https://www.cryptopp.com/cryptopp800.zip
unzip cryptopp800.zip -d cryptopp800
cd cryptopp800
make && make install PREFIX=${mylocal}/cryptopp
