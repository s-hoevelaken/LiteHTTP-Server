# LiteHTTP-Server
A lightweight web server built in C that serves static files (HTML, CSS, JS) and supports query parameters. It includes a custom logging system with adjustable log levels (DEBUG, INFO, WARN, ERROR), and basic error handling (404, 500).

### Requirements
GCC (or another C compiler)
Make

### How to Run

1. **Clone the repository:**

   First, clone the repository to your local machine:
   ```bash
   git clone https://github.com/yourusername/LiteHTTP-Server.git
   cd LiteHTTP-Server
    ```
2. **Build the project:**

   Use the make command to build the project. This compiles the source code and links all necessary files
   ```bash
   make
    ```
3. **Run the server:**

   To start the server, run the compiled binary:
   ```bash
   ./bin/server
    ```
4. **Access the server:**

   Open your web browser and navigate to http://localhost:8080/ to see the server in action.
