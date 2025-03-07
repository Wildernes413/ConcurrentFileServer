#include "server.h"

int main() {
    FileServer server;
    server.Start(8080);
    return 0;
}