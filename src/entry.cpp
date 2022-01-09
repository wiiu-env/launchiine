#include "common/common.h"
#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <whb/log_module.h>
#include "utils/logger.h"
#include "Application.h"

int32_t main(int32_t argc, char **argv) {
    bool moduleInit;
    bool cafeInit = false;
    bool udpInit = false;

    if (!(moduleInit = WHBLogModuleInit())) {
        cafeInit = WHBLogCafeInit();
        udpInit = WHBLogUdpInit();
    }
    DEBUG_FUNCTION_LINE("Starting launchiine " LAUNCHIINE_VERSION "");

    DEBUG_FUNCTION_LINE("Start main application");
    Application::instance()->exec();

    DEBUG_FUNCTION_LINE("Main application stopped");
    Application::destroyInstance();

    DEBUG_FUNCTION_LINE("Peace out...");

    if (cafeInit) {
        WHBLogCafeDeinit();
    }

    if (udpInit) {
        WHBLogUdpDeinit();
    }

    if (moduleInit) {
        WHBLogModuleDeinit();
    }
    return 0;
}
