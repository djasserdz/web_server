# Simple C Web Server

A basic HTTP web server written in C. This server demonstrates:

- Creating a TCP socket
- Accepting client connections
- Parsing all http requests
- Sending http responses
- add static file serving
- added config file
- added logging

---

---

## How to Compile

```bash
gcc main.c src/config.c src/server.c src/request.c src/response.c src/log.c src/handler.c -Iinclude -o myserver


./myserver

```
