FROM ubuntu:22.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y g++-9 gcc-9 cmake build-essential gdb
RUN apt-get -y install git

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 20
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 20

WORKDIR /celonis
COPY . .

RUN git clone https://github.com/alfmunny/EVA01.git; \
    cd EVA01; \
    cmake .; \
    make; \
    cp -f lib/libeva01.so ../lib/libeva01;


RUN git clone https://github.com/rpclib/rpclib.git; \
    cd rpclib; \
    cmake .; \
    make; \
    cp -f librpc.a ../lib/librpc.a;

RUN cmake .; make;

# EXPOSE 8081

CMD ["bin/secondary", "localhost:8080", "8081"]
