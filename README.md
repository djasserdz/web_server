# Simple C Web Server

A basic HTTP web server written in C. This server demonstrates:

- Creating a TCP socket
- Accepting client connections
- Parsing simple HTTP GET requests
- Sending basic HTTP responses

---

---

## How to Compile

```bash
gcc main.c src/server.c src/request.c src/response.c -Iinclude -o myserver


./myserver

```
