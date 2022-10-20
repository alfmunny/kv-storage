FROM ubuntu

RUN apt-get update && apt-get install -y g++-9 gcc-9 cmake build-essential gdb

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 20
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 20


WORKDIR /celonis

COPY . .

RUN cmake .; make;

# EXPOSE 8081

CMD ["bin/secondary", "localhost:8080", "8081"]
