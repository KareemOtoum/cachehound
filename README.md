# Cachehound  

**Cachehound** is a lightweight, persistent LRU (Least Recently Used) cache written in C++.  
It supports fast in-memory access and binary snapshot-based disk persistence and a thread pool to handle
many clients.

Features:
- ⚡ Efficient LRU eviction
- 💾 Binary serialization for disk persistence
- 🔁 Easy save/load interface
- 🛠️ Clean, modular codebase  

### Building and Running  
run the following commands to clone and run in bash with g++ and make installed
```bash
git clone https://github.com/KareemOtoum/cachehound.git
cd cachehound
make run
```