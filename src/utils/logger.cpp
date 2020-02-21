#include <thread>
#include <mutex>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <utils/logger.h>
#include <nsysnet/socket.h>
#include <coreinit/debug.h>

#include <coreinit/systeminfo.h>
#include <coreinit/thread.h>

static int32_t log_socket __attribute__((section(".data")))= -1;
static struct sockaddr_in connect_addr __attribute__((section(".data")));
static volatile int32_t log_lock __attribute__((section(".data"))) = 0;
static std::recursive_mutex _lock;

void log_init_() {
    int32_t broadcastEnable = 1;
    log_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (log_socket < 0)
        return;

    setsockopt(log_socket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    memset(&connect_addr, 0, sizeof(struct sockaddr_in));
    connect_addr.sin_family = AF_INET;
    connect_addr.sin_port = 4405;
    connect_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
}

void log_print_(const char *str) {
    // socket is always 0 initially as it is in the BSS
    if(log_socket < 0) {
        return;
    }

    _lock.lock();

    int32_t len = strlen(str);
    int32_t ret;
    while (len > 0) {
        int32_t block = len < 1400 ? len : 1400; // take max 1400 bytes per UDP packet
        ret = sendto(log_socket, str, block, 0, (struct sockaddr *)&connect_addr, sizeof(struct sockaddr_in));
        if(ret < 0)
            break;

        len -= ret;
        str += ret;
    }

    _lock.unlock();
}

void OSFatal_printf(const char *format, ...) {
    char tmp[512];
    tmp[0] = 0;
    va_list va;
    va_start(va, format);
    if((vsprintf(tmp, format, va) >= 0)) {
        OSFatal(tmp);
    }
    va_end(va);
}

void log_printf_(const char *format, ...) {
    if(log_socket < 0) {
        return;
    }

    char tmp[512];
    tmp[0] = 0;

    va_list va;
    va_start(va, format);
    if((vsprintf(tmp, format, va) >= 0)) {
        log_print_(tmp);
    }
    va_end(va);
}
