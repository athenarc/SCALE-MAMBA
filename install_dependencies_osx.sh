mylocal=$1
mkdir -p ${mylocal}
cd ${mylocal}

brew install automake

# install MPIR
git clone https://github.com/wbhart/mpir.git mpir-3.0.0
cd mpir-3.0.0
./autogen.sh
./configure CC=cc --enable-cxx --prefix="${mylocal}/mpir"
make -j8 && make -j8 check && make -j8 install

# install OpenSSL 1.1.0
cd $mylocal
curl -O https://www.openssl.org/source/openssl-1.1.0j.tar.gz
tar -xf openssl-1.1.0j.tar.gz
cd openssl-1.1.0j
./config --prefix="${mylocal}/openssl"
make && make install

# install crypto++
cd $mylocal
curl -O https://www.cryptopp.com/cryptopp800.zip
unzip cryptopp800.zip -d cryptopp800
cd cryptopp800
make && make install PREFIX=${mylocal}/cryptopp
