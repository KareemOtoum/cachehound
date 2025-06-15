# Cachehound  

**Cachehound** is a networked, persistent LRU (Least Recently Used) cache database written in C++20.  
It supports fast in-memory access, multithreaded network handling via worker threads and binary formatted disk persistence and networking protocol.  

<img width="599" alt="image" src="https://github.com/user-attachments/assets/a64b2e10-8a65-45c7-97a2-3005e96cd1d5" />

---

# Features:
- 🧵 Multithreaded network server using worker threads
- 🚀 Fast and concurrent client handling via epoll (non blocking I/O)
- 🌐 Custom networking protocol built ontop of TCP
- 🔁 Easy save/load interface
- 💾 Custom Binary serialization for disk persistence
  
<img width="587" alt="image" src="https://github.com/user-attachments/assets/f8920e01-920f-4800-8cc1-1e138f188a12" />

---

# 🛠️ Build & Run (Linux)
### 📦 Requirements
- g++13
- make

The project uses make to build and run, to get started clone the repo and cd into the directory:
```bash
git clone https://github.com/KareemOtoum/cachehound.git
cd cachehound
```  
## 🚀 Running the Server

The server has an optional `capacity` parameter for the maximum key-value pair entries (default = `10,000`).
```bash
make server capacity=<x>
```
<img width="537" alt="image" src="https://github.com/user-attachments/assets/58bcc592-fae2-42aa-a63f-cf768cfca3e5" />

## 🧪 Running the Client
The client accepts optional `ip` and `port` parameters (defaults: `127.0.0.1`, `5050`).
```bash
make client ip=<x> port=<x>
```  
# Benchmarks  

| Number of Clients | Number of Threads | Throughput (requests/second) |
|-------------------|-------------------|------------------------------|
| 1                 | 1                 | 48                           |
| 100               | 16                | 4,878                        |
| 1000              | 16                | 50,000                       |
| 10,000            | 16                | 400,000                      |


<img width="578" alt="image" src="https://github.com/user-attachments/assets/39b4519d-ad5a-4797-a447-06a7530fae0b" />

