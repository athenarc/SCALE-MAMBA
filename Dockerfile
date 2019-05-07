FROM debian:stretch

SHELL ["/bin/bash", "-c"]

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y \
  yasm \
  python \
  gcc \
  g++ \
  cmake \
  make \
  curl \
  wget \
  apt-transport-https \
  m4 \
  zip \
  unzip \
  vim

RUN mkdir SCALE-MAMBA
COPY . /SCALE-MAMBA

WORKDIR /SCALE-MAMBA
RUN ./install_dependencies.sh

RUN echo 'export mylocal="$HOME/local"' >> ~/.bashrc
RUN echo 'export PATH="${mylocal}/openssl/bin/:${PATH}"' >> ~/.bashrc
RUN echo 'export C_INCLUDE_PATH="${mylocal}/openssl/include/:${C_INCLUDE_PATH}"' >> ~/.bashrc
RUN echo 'export CPLUS_INCLUDE_PATH="${mylocal}/openssl/include/:${CPLUS_INCLUDE_PATH}"' >> ~/.bashrc
RUN echo 'export LIBRARY_PATH="${mylocal}/openssl/lib/:${LIBRARY_PATH}"' >> ~/.bashrc
RUN echo 'export LD_LIBRARY_PATH="${mylocal}/openssl/lib/:${LD_LIBRARY_PATH}"' >> ~/.bashrc
RUN echo 'export C_INCLUDE_PATH="${mylocal}/mpir/include/:${C_INCLUDE_PATH}"' >> ~/.bashrc
RUN echo 'export CPLUS_INCLUDE_PATH="${mylocal}/mpir/include/:${CPLUS_INCLUDE_PATH}"' >> ~/.bashrc
RUN echo 'export LIBRARY_PATH="${mylocal}/mpir/lib/:${LIBRARY_PATH}"' >> ~/.bashrc
RUN echo 'export LD_LIBRARY_PATH="${mylocal}/mpir/lib/:${LD_LIBRARY_PATH}"' >> ~/.bashrc
RUN echo 'export CPLUS_INCLUDE_PATH="${mylocal}/cryptopp/include/:${CPLUS_INCLUDE_PATH}"' >> ~/.bashrc
RUN echo 'export LIBRARY_PATH="${mylocal}/cryptopp/lib/:${LIBRARY_PATH}"' >> ~/.bashrc
RUN echo 'export LD_LIBRARY_PATH="${mylocal}/cryptopp/lib/:${LD_LIBRARY_PATH}"' >> ~/.bashrc

RUN source ~/.bashrc

RUN echo ${PATH}
RUN echo ${C_INCLUDE_PATH}

RUN cp CONFIG CONFIG.mine

RUN echo 'ROOT = $HOME/SCALE-MAMBA' >> CONFIG.mine
RUN echo 'OSSL = ${mylocal}/openssl' >> CONFIG.mine

RUN make progs

CMD ["bin/bash"]
