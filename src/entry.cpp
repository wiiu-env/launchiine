#include <string.h>
#include <nsysnet/socket.h>
#include "utils/logger.h"
#include "common/common.h"
#include "Application.h"

int32_t main(int32_t argc, char **argv) {
    socket_lib_init();
    log_init();
    DEBUG_FUNCTION_LINE("Starting launchiine " LAUNCHIINE_VERSION "\n");

    DEBUG_FUNCTION_LINE("Start main application\n");
    Application::instance()->exec();

    DEBUG_FUNCTION_LINE("Main application stopped\n");
    Application::destroyInstance();

    DEBUG_FUNCTION_LINE("Peace out...\n");
    return 0;
}
