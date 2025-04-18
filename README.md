# Cachehound  

**Cachehound** is a networked, persistent LRU (Least Recently Used) cache database written in C++20.  
It supports fast in-memory access, multithreaded network handling via worker threads and binary formatted disk persistence.  

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

## 🚀 Running the Server
The server has an optional `capacity` parameter (default = `10,000`).
```bash
git clone https://github.com/KareemOtoum/cachehound.git
cd cachehound
make server capacity=<x>
```
<img width="537" alt="image" src="https://github.com/user-attachments/assets/58bcc592-fae2-42aa-a63f-cf768cfca3e5" />

## 🧪 Running the Client
The client accepts optional `ip` and `port` parameters (defaults: `127.0.0.1`, `5050`).
```bash
git clone https://github.com/KareemOtoum/cachehound.git
cd cachehound
make client ip=<x> port=<x>
```

# Architecture & Design  

```txt
         ┌────────────┐
         │  Client    │
         └────┬───────┘
              │
              ▼
       ┌────────────┐
       │Main thread │ Accepts new connections
       └────┬───────┘
            ▼
     ┌──────────────┐
     │ Dispatcher   │ Round-robin to workers
     └────┬─────────┘
          ▼
    ┌────────────┐
    │ Worker[N]  │ Each with its own epoll loop
    └────┬───────┘
         ▼
  ┌──────────────┐
  │ LRU Cache    │ Shared across all workers
  └──────────────┘

```
