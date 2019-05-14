FROM debian:stretch

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y \
  gnupg2 \
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
  vim \
  build-essential

RUN mkdir SCALE-MAMBA
COPY . /SCALE-MAMBA

WORKDIR /SCALE-MAMBA
RUN ./install_dependencies.sh /local

ENV PATH="/local/openssl/bin/:${PATH}"
ENV C_INCLUDE_PATH="/local/openssl/include/:${C_INCLUDE_PATH}"
ENV CPLUS_INCLUDE_PATH="/local/openssl/include/:${CPLUS_INCLUDE_PATH}"
ENV LIBRARY_PATH="/local/openssl/lib/:${LIBRARY_PATH}"
ENV LD_LIBRARY_PATH="/local/openssl/lib/:${LD_LIBRARY_PATH}"
ENV C_INCLUDE_PATH="/local/mpir/include/:${C_INCLUDE_PATH}"
ENV CPLUS_INCLUDE_PATH="/local/mpir/include/:${CPLUS_INCLUDE_PATH}"
ENV LIBRARY_PATH="/local/mpir/lib/:${LIBRARY_PATH}"
ENV LD_LIBRARY_PATH="/local/mpir/lib/:${LD_LIBRARY_PATH}"
ENV CPLUS_INCLUDE_PATH="/local/cryptopp/include/:${CPLUS_INCLUDE_PATH}"
ENV LIBRARY_PATH="/local/cryptopp/lib/:${LIBRARY_PATH}"
ENV LD_LIBRARY_PATH="/local/cryptopp/lib/:${LD_LIBRARY_PATH}"

RUN cp CONFIG CONFIG.mine

RUN echo 'ROOT = /SCALE-MAMBA' >> CONFIG.mine
RUN echo 'OSSL = /local/openssl' >> CONFIG.mine

WORKDIR /SCALE-MAMBA/src
RUN make

CMD ["bin/bash"]
