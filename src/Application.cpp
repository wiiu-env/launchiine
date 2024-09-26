#include "Application.h"
#include "common/common.h"
#include "resources/Resources.h"
#include "utils/AsyncExecutor.h"
#include "utils/logger.h"
#include <coreinit/core.h>
#include <coreinit/foreground.h>
#include <coreinit/title.h>
#include <coreinit/thread.h>
#include "gui/FreeTypeGX.h"
#include "gui/VPadController.h"
#include "gui/WPadController.h"
#include "gui/memory.h"
#include "gui/sounds/SoundHandler.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <proc_ui/procui.h>
#include <sysapp/switch.h>
#include <sysapp/launch.h>
#include <sysapp/title.h>
#include <nsysuhs/uhs.h>
#include <thread>
#include <malloc.h>
#include <nn/spm.h>
#include <nn/acp/save.h>

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
    int numConnectedStorage;
    int maxTries = 1200; // Wait up to 20 seconds, like the Wii U Menu
    if ((numConnectedStorage = numberUSBStorageDevicesConnected()) <= 0) {
        maxTries = 1; // Only try once if no USBStorageDrive is connected
    } else {
        DEBUG_FUNCTION_LINE("Connected StorageDevices = %d", numConnectedStorage);
    }

    nn::spm::Initialize();

    nn::spm::StorageListItem items[0x20];
    int tries  = 0;
    bool found = false;

    while (tries < maxTries) {
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
        if (found || (numConnectedStorage == numItems)) {
            DEBUG_FUNCTION_LINE("Found all expected items, breaking.");
            break;
        }
        OSSleepTicks(OSMillisecondsToTicks(16));
        tries++;
    }
    if (!found) {
        if (numConnectedStorage > 0) {
            DEBUG_FUNCTION_LINE("USB Storage is connected but either it doesn't have a WFS partition or we ran into a timeout.");
        }
        InitEmptyExternalStorage();
    }

    nn::spm::Finalize();
}
Application *Application::applicationInstance = nullptr;
bool Application::exitApplication             = false;
bool Application::quitRequest                 = false;

Application::Application()
    : CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x800000), bgMusic(nullptr), video(nullptr), mainWindow(nullptr), fontSystem(nullptr), exitCode(0) {
    controller[0] = new VPadController(GuiTrigger::CHANNEL_1);
    controller[1] = new WPadController(GuiTrigger::CHANNEL_2);
    controller[2] = new WPadController(GuiTrigger::CHANNEL_3);
    controller[3] = new WPadController(GuiTrigger::CHANNEL_4);
    controller[4] = new WPadController(GuiTrigger::CHANNEL_5);

    //! create bgMusic
    bgMusic = new GuiSound(Resources::GetFile("bgMusic.ogg"), Resources::GetFileSize("bgMusic.ogg"));
    bgMusic->SetLoop(true);
    bgMusic->Play();
    bgMusic->SetVolume(50);

    AsyncExecutor::execute([] { DEBUG_FUNCTION_LINE("Hello"); });

    exitApplication = false;

    ProcUIInit(OSSavesDone_ReadyToRelease);
}

Application::~Application() {
    DEBUG_FUNCTION_LINE("Destroy music");
    delete bgMusic;

    DEBUG_FUNCTION_LINE("Destroy controller");

    for (auto &i : controller) {
        delete i;
    }

    DEBUG_FUNCTION_LINE("Clear resources");
    Resources::Clear();

    DEBUG_FUNCTION_LINE("Stop sound handler");
    SoundHandler::DestroyInstance();

    DEBUG_FUNCTION_LINE("Clear AsyncExecutor");
    AsyncExecutor::destroyInstance();

    ProcUIShutdown();
}

int32_t Application::exec() {
    //! start main GX2 thread
    resumeThread();
    //! now wait for thread to finish
    shutdownThread();

    return exitCode;
}

void Application::quit(int32_t code) {
    exitCode        = code;
    exitApplication = true;
    quitRequest     = true;
}

void Application::fadeOut() {
    GuiImage fadeOut(video->getTvWidth(), video->getTvHeight(), (GX2Color){0, 0, 0, 255});

    for (int32_t i = 0; i < 255; i += 10) {
        if (i > 255)
            i = 255;

        fadeOut.setAlpha(i / 255.0f);

        //! start rendering DRC
        video->prepareDrcRendering();
        mainWindow->drawDrc(video);

        GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_ALWAYS);
        fadeOut.draw(video);
        GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL);

        video->drcDrawDone();

        //! start rendering TV
        video->prepareTvRendering();

        mainWindow->drawTv(video);

        GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_ALWAYS);
        fadeOut.draw(video);
        GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL);

        video->tvDrawDone();

        //! as last point update the effects as it can drop elements
        mainWindow->updateEffects();

        video->waitForVSync();
    }
}

bool Application::procUI() {
    bool executeProcess = false;

    switch (ProcUIProcessMessages(true)) {
        case PROCUI_STATUS_EXITING: {
            DEBUG_FUNCTION_LINE("PROCUI_STATUS_EXITING");
            exitCode        = EXIT_SUCCESS;
            exitApplication = true;
            break;
        }
        case PROCUI_STATUS_RELEASE_FOREGROUND: {
            DEBUG_FUNCTION_LINE("PROCUI_STATUS_RELEASE_FOREGROUND");
            if (video != nullptr) {
                // we can turn ofF the screen but we don't need to and it will display the last image
                video->tvEnable(true);
                video->drcEnable(true);

                DEBUG_FUNCTION_LINE("delete fontSystem");
                delete fontSystem;
                fontSystem = nullptr;

                DEBUG_FUNCTION_LINE("delete video");
                delete video;
                video = nullptr;

                DEBUG_FUNCTION_LINE("deinitialze memory");
                libgui_memoryRelease();
                ProcUIDrawDoneRelease();
            } else {
                ProcUIDrawDoneRelease();
            }
            break;
        }
        case PROCUI_STATUS_IN_FOREGROUND: {
            if (!quitRequest) {
                if (video == nullptr) {
                    DEBUG_FUNCTION_LINE("PROCUI_STATUS_IN_FOREGROUND");
                    DEBUG_FUNCTION_LINE("initialze memory");
                    libgui_memoryInitialize();

                    DEBUG_FUNCTION_LINE("Initialize video");
                    video = new CVideo(GX2_TV_SCAN_MODE_720P, GX2_DRC_RENDER_MODE_SINGLE);
                    DEBUG_FUNCTION_LINE("Video size %i x %i", video->getTvWidth(), video->getTvHeight());

                    //! setup default Font
                    DEBUG_FUNCTION_LINE("Initialize main font system");
                    auto *fontSystem = new FreeTypeGX(Resources::GetFile("font.ttf"), Resources::GetFileSize("font.ttf"), true);
                    GuiText::setPresetFont(fontSystem);

                    if (mainWindow == nullptr) {
                        DEBUG_FUNCTION_LINE("Initialize main window");
                        mainWindow = new MainWindow(video->getTvWidth(), video->getTvHeight());
                    }
                }
                executeProcess = true;
            }
            break;
        }
        case PROCUI_STATUS_IN_BACKGROUND:
        default:
            break;
    }

    return executeProcess;
}

void Application::executeThread() {
    DEBUG_FUNCTION_LINE("Entering main loop");

    //! main GX2 loop (60 Hz cycle with max priority on core 1)
    while (!exitApplication) {
        if (!procUI()) {
            continue;
        }

        mainWindow->lockGUI();
        mainWindow->process();

        //! Read out inputs
        for (auto &i : controller) {
            if (!i->update(video->getTvWidth(), video->getTvHeight()))
                continue;

            //! update controller states
            mainWindow->update(i);
        }

        //! start rendering DRC
        video->prepareDrcRendering();
        mainWindow->drawDrc(video);
        video->drcDrawDone();

        //! start rendering TV
        video->prepareTvRendering();
        mainWindow->drawTv(video);
        video->tvDrawDone();

        //! enable screen after first frame render
        if (video->getFrameCount() == 0) {
            video->tvEnable(true);
            video->drcEnable(true);
        }

        //! as last point update the effects as it can drop elements
        mainWindow->updateEffects();
        mainWindow->unlockGUI();

        video->waitForVSync();
    }

    if (bgMusic) {
        bgMusic->SetVolume(0);
    }

    DEBUG_FUNCTION_LINE("delete mainWindow");
    delete mainWindow;
    mainWindow = nullptr;

    DEBUG_FUNCTION_LINE("delete fontSystem");
    delete fontSystem;
    fontSystem = nullptr;

    DEBUG_FUNCTION_LINE("delete video");
    delete video;
    video = nullptr;

    DEBUG_FUNCTION_LINE("deinitialize memory");
    libgui_memoryRelease();
}