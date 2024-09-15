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
#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "menu/MainWindow.h"
#include "system/CThread.h"
#include <video/CVideo.h>
#include <memory>
#include <array>

// forward declaration
class FreeTypeGX;

class Application : public CThread {
public:
    static Application *instance() {
        if (!applicationInstance)
            applicationInstance = new Application();
        return applicationInstance;
    }

    static void destroyInstance() {
        if (applicationInstance) {
            delete applicationInstance;
            applicationInstance = nullptr;
        }
    }

    [[nodiscard]] CVideo *getVideo() const {
        return video.get();
    }

    [[nodiscard]] MainWindow *getMainWindow() const {
        return mainWindow.get();
    }

    [[nodiscard]] GuiSound *getBgMusic() const {
        return bgMusic.get();
    }

    int exec();

    void fadeOut();

    void quit(int code);

private:
    Application();

    ~Application() override;

    bool procUI();

    static Application *applicationInstance;
    static bool exitApplication;
    static bool quitRequest;

    void executeThread() override;

    std::unique_ptr<GuiSound> bgMusic;
    std::unique_ptr<CVideo> video;
    std::unique_ptr<MainWindow> mainWindow;
    std::shared_ptr<FreeTypeGX> fontSystem;
    std::array<std::unique_ptr<GuiController>, 5> controller;
    int exitCode;
};

#endif //_APPLICATION_H
