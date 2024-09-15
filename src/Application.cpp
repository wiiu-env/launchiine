/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "Application.h"
#include "common/common.h"
#include "resources/Resources.h"
#include "system/memory.h"
#include "utils/logger.h"
#include <coreinit/core.h>
#include <coreinit/foreground.h>
#include <coreinit/title.h>
#include <gui/FreeTypeGX.h>
#include <gui/VPadController.h>
#include <gui/WPadController.h>
#include <proc_ui/procui.h>
#include <sounds/SoundHandler.hpp>
#include <sysapp/launch.h>
#include <thread>

Application *Application::applicationInstance = nullptr;
bool Application::exitApplication             = false;
bool Application::quitRequest                 = false;

Application::Application()
    : CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x800000), bgMusic(nullptr), video(nullptr), mainWindow(nullptr), fontSystem(nullptr), exitCode(0) {
    controller[0] = std::make_unique<VPadController>(GuiTrigger::CHANNEL_1);
    controller[1] = std::make_unique<WPadController>(GuiTrigger::CHANNEL_2);
    controller[2] = std::make_unique<WPadController>(GuiTrigger::CHANNEL_3);
    controller[3] = std::make_unique<WPadController>(GuiTrigger::CHANNEL_4);
    controller[4] = std::make_unique<WPadController>(GuiTrigger::CHANNEL_5);

    //! create bgMusic
    bgMusic = std::make_unique<GuiSound>(Resources::GetFile("bgMusic.ogg"));
    bgMusic->SetLoop(true);
    bgMusic->Play();
    bgMusic->SetVolume(50);

    exitApplication = false;

    ProcUIInit(OSSavesDone_ReadyToRelease);
}

Application::~Application() {
    DEBUG_FUNCTION_LINE("Clear resources");
    Resources::Clear();

    DEBUG_FUNCTION_LINE("Stop sound handler");
    SoundHandler::DestroyInstance();

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
        mainWindow->drawDrc(*video);

        GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_ALWAYS);
        fadeOut.draw(*video);
        GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL);

        video->drcDrawDone();

        //! start rendering TV
        video->prepareTvRendering();

        mainWindow->drawTv(*video);

        GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_ALWAYS);
        fadeOut.draw(*video);
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
                fontSystem.reset();

                DEBUG_FUNCTION_LINE("delete video");
                video.reset();

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
                    video = std::make_unique<CVideo>(GX2_TV_SCAN_MODE_720P, GX2_DRC_RENDER_MODE_SINGLE);
                    DEBUG_FUNCTION_LINE("Video size %i x %i", video->getTvWidth(), video->getTvHeight());

                    //! setup default Font
                    DEBUG_FUNCTION_LINE("Initialize main font system");
                    fontSystem = std::make_shared<FreeTypeGX>(Resources::GetFile("font.ttf"), true);
                    GuiText::setPresetFont(fontSystem);

                    if (mainWindow == nullptr) {
                        DEBUG_FUNCTION_LINE("Initialize main window");
                        mainWindow = std::make_unique<MainWindow>(video->getTvWidth(), video->getTvHeight());
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
            mainWindow->update(*i);
        }

        //! start rendering DRC
        video->prepareDrcRendering();
        mainWindow->drawDrc(*video);
        video->drcDrawDone();

        //! start rendering TV
        video->prepareTvRendering();
        mainWindow->drawTv(*video);
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
    mainWindow.reset();

    DEBUG_FUNCTION_LINE("delete fontSystem");
    fontSystem.reset();


    DEBUG_FUNCTION_LINE("delete video");
    video.reset();

    DEBUG_FUNCTION_LINE("deinitialize memory");
    libgui_memoryRelease();
}
