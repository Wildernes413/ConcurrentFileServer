#include "client.h"

int main() {
    FileClient client;
    if (client.Connect("26.254.219.0", 8080)) {
        client.Run();
    }
    return 0;
}