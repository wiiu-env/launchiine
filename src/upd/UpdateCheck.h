#ifndef UPDATE_CHECK_H
#define UPDATE_CHECK_H

#include <nsysnet/socket.h>
#include <coreinit/debug.h>
#include <string>

class UpdateCheck {
public:
    static bool checkForUpdates();
    static std::string getCurrentVersion() { return "0.2.1"; }
private:
    static const char* UPDATE_HOST;
    static const char* UPDATE_PATH;
};

#endif
