# Distributed Key-Value-Store for Celonis

## Architect

- Primary
    - Static list of secondary nodes
    - Exposes RPC services
    - Accept requests from clients
    - Dockerized
- Secondary
    - Register itself to Primary
    - Provide the Key-Value Storage
    - Dockerized
- Kubernets
    - Configure the nodes
    - Watchdog
    - Load Balancing

