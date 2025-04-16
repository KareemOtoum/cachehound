# Cachehound  

**Cachehound** is a networked, persistent LRU (Least Recently Used) cache written in C++20.  
It supports fast in-memory access, multithreaded network handling with a thread pool and binary snapshot-based disk persistence.  

<img width="664" alt="image" src="https://github.com/user-attachments/assets/c022dd5f-7fa7-441a-a85b-ba8ca04f9c81" />  

Features:
- ⚡ Efficient LRU eviction
- 🌐 Multithreaded network server using a thread pool
- 🚀 Fast and concurrent client handling  
- 💾 Binary serialization for disk persistence
- 🔁 Easy save/load interface

### Building and Running  
run the following commands to clone and run in bash with g++ and make installed
```bash
git clone https://github.com/KareemOtoum/cachehound.git
cd cachehound
make run
```
