#include <cstdlib>
#include <malloc.h>
#include "utils/logger.h"

#include "StorageUtils.h"

#include <coreinit/thread.h>
#include <coreinit/title.h>
#include <nn/acp/save.h>
#include <nn/spm.h>
#include <nsysuhs/uhs.h>
#include <sysapp/title.h>

static void InitEmptyExternalStorage() {
    DEBUG_FUNCTION_LINE("Fallback to empty ExtendedStorage");
    nn::spm::VolumeId empty{};
    nn::spm::SetDefaultExtendedStorageVolumeId(empty);

    nn::spm::StorageIndex storageIndex = 0;
    nn::spm::SetExtendedStorage(&storageIndex);
}

static int numberUSBStorageDevicesConnected() {
    DEBUG_FUNCTION_LINE("Check if USB Storage is connected");
    auto *handle = (UhsHandle *) memalign(0x40, sizeof(UhsHandle));
    if (!handle) {
        return -1;
    }
    memset(handle, 0, sizeof(UhsHandle));
    auto *config = (UhsConfig *) memalign(0x40, sizeof(UhsConfig));
    if (!config) {
        free(handle);
        return -2;
    }
    memset(config, 0, sizeof(UhsConfig));

    config->controller_num = 0;
    uint32_t size          = 5120;
    void *buffer           = memalign(0x40, size);
    if (!buffer) {
        free(handle);
        free(config);
        return -3;
    }
    memset(buffer, 0, size);

    config->buffer      = buffer;
    config->buffer_size = size;

    if (UhsClientOpen(handle, config) != UHS_STATUS_OK) {
        DEBUG_FUNCTION_LINE("UhsClient failed");
        free(handle);
        free(config);
        free(buffer);
        return -4;
    }

    UhsInterfaceProfile profiles[10];
    UhsInterfaceFilter filter = {
            .match_params = MATCH_ANY};

    UHSStatus result;
    if ((result = UhsQueryInterfaces(handle, &filter, profiles, 10)) <= UHS_STATUS_OK) {
        DEBUG_FUNCTION_LINE("UhsQueryInterfaces failed");
        UhsClientClose(handle);
        free(handle);
        free(config);
        free(buffer);
        return -5;
    }

    auto found = 0;
    for (int i = 0; i < (int) result; i++) {
        if (profiles[i].if_desc.bInterfaceClass == USBCLASS_STORAGE) {
            DEBUG_FUNCTION_LINE("Found USBCLASS_STORAGE");
            found++;
        }
    }

    UhsClientClose(handle);
    free(handle);
    free(config);
    free(buffer);
    return found;
}

void initExternalStorage() {
    if (OSGetTitleID() == _SYSGetSystemApplicationTitleId(SYSTEM_APP_ID_MII_MAKER)) {
        // nn::spm functions always call OSFatal when they fail, so we make sure have permission to use
        // the lib before actually using it.
        return;
    }
    int connectedStorage = 0;
    if ((connectedStorage = numberUSBStorageDevicesConnected()) <= 0) {
        nn::spm::Initialize();
        InitEmptyExternalStorage();
        nn::spm::Finalize();
        return;
    }

    DEBUG_FUNCTION_LINE("Connected StorageDevices = %d", connectedStorage);

    nn::spm::Initialize();

    nn::spm::StorageListItem items[0x20];
    int tries  = 0;
    bool found = false;

    while (tries < 1200) { // Wait up to 20 seconds, like the Wii U Menu
        int32_t numItems = nn::spm::GetStorageList(items, 0x20);

        DEBUG_FUNCTION_LINE("Number of items: %d", numItems);

        for (int32_t i = 0; i < numItems; i++) {
            if (items[i].type == nn::spm::STORAGE_TYPE_WFS) {
                nn::spm::StorageInfo info{};
                if (nn::spm::GetStorageInfo(&info, &items[i].index) == 0) {
                    DEBUG_FUNCTION_LINE("Using %s for extended storage", info.path);

                    nn::spm::SetExtendedStorage(&items[i].index);
                    ACPMountExternalStorage();

                    nn::spm::SetDefaultExtendedStorageVolumeId(info.volumeId);

                    found = true;
                    break;
                }
            }
        }
        if (found || (connectedStorage == numItems)) {
            DEBUG_FUNCTION_LINE("Found all expected items, breaking.");
            break;
        }
        OSSleepTicks(OSMillisecondsToTicks(16));
        tries++;
    }
    if (!found) {
        DEBUG_FUNCTION_LINE("USB Storage is connected but either it doesn't have a WFS partition or we ran into a timeout.");
        InitEmptyExternalStorage();
    }

    nn::spm::Finalize();
}