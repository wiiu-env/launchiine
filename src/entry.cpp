#include "Application.h"
#include "StorageUtils.h"
#include "common/common.h"
#include "utils/logger.h"
#include <whb/log_cafe.h>
#include <whb/log_module.h>
#include <whb/log_udp.h>

int32_t main(int32_t argc, char **argv) {
    initLogging();

    DEBUG_FUNCTION_LINE("Starting launchiine " LAUNCHIINE_VERSION "");

    initExternalStorage();

    DEBUG_FUNCTION_LINE("Start main application");
    Application::instance()->exec();

    DEBUG_FUNCTION_LINE("Main application stopped");
    Application::destroyInstance();

    DEBUG_FUNCTION_LINE("Peace out...");

    deinitLogging();
    return 0;
}
