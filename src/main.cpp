#include "server.hpp"

int main(int argc, char* argv[])
{
    if(argc != 2)  {
        printf("Usage: ./sieci_projekt <port>\n");
        exit(EXIT_FAILURE);
    }
    int port = static_cast<int>(strtol(argv[1], nullptr, 10));

    Server server(port);
    server.run();

    return 0;
}
