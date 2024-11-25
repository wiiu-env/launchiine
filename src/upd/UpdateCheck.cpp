#include "UpdateCheck.h"
#include <cstring>
#include <string>

const char* UpdateCheck::UPDATE_HOST = "raw.githubusercontent.com";
const char* UpdateCheck::UPDATE_PATH = "/rpdistiso/launchiine/main/version.txt";

bool UpdateCheck::checkForUpdates() {
    socket_lib_init();
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = 443;
    addr.sin_addr.s_addr = inet_addr(UPDATE_HOST);
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        socketclose(sock);
        socket_lib_finish();
        return false;
    }

    char request[256];
    snprintf(request, sizeof(request), 
             "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",
             UPDATE_PATH, UPDATE_HOST);
    send(sock, request, strlen(request), 0);
    
    char response[256];
    recv(sock, response, sizeof(response), 0);
    
    socketclose(sock);
    socket_lib_finish();
    
    std::string latestVersion(response);
    return latestVersion > getCurrentVersion();
}