# Distributed Key-Value-Store Celonis Challenge

Auther: Yuanchen Zhang
GitHub: https://github.com/alfmunny

## Architect

- Primary Node
    - Static list of secondary nodes
    - Exposes RPC services
    - Accept requests from clients
    - Simple implementation of sharding.
        - Static mapping
        - Map key to an unique shard(one secondary node)
    - Dockerized
- Secondary Node
    - Provide the Key-Value Storage
    - Read Write Lock to separate the read and write IO
    - Dockerized
- Kubernets
    - Configure the nodes
    - Load Balancing
    - Watchdog, replica, etc..

Advanced(ideas): 
- Dynamic scaling with service discovery
    - Heartbeat from secondary nodes to primary node
    - Regester the secondary nodes onto primary node while receiving heartbeat.
    - Zookeeper

![Architect](./architect.png)

## Library

- [rpclib](https://github.com/rpclib/rpclib): simple RPC communication
- [EVA01](https://github.com/alfmunny/EVA01)(self-written library): For logging, read-write mutex, timer, thread-pool, fiber, iomanager and etc. .

## Build local

- Linux
- gcc-9

## Build in docker

Build the image in docker

```bash
docker build . -t celonis-kv
```

## Run

### Start secondary nodes

Secondary node 1 on 8081:

```bash
docker run -ti -p 8081:8081 celonis-kv bin/secondary localhost:8080 8081
```

Secondary node 2 on 8082:

```bash
docker run -ti -p 8081:8081 celonis-kv bin/secondary localhost:8080 8081
```

### Start primary nodes

```bash
docker run -ti -p 8080:8080 celonis-kv bin/primary 8080 172.17.0.2:8081 172.17.0.3:8082
```

### Test with client

Connect to the primary nodes

```bash
docker run -ti celonis-kv bin/main localhost 8080
```

## Deploy with Kubernets

