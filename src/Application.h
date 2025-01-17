#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "menu/MainWindow.h"
#include "system/CThread.h"
#include "gui/video/CVideo.h"

// forward declaration
class FreeTypeGX;

class Application : public CThread {
public:
      Application();

    virtual ~Application();
    
    static Application *applicationInstance;
    static bool exitApplication;
    static bool quitRequest;
    
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

    CVideo *getVideo(void) const {
        return video;
    }

    MainWindow *getMainWindow(void) const {
        return mainWindow;
    }

    GuiSound *getBgMusic(void) const {
        return bgMusic;
    }

    int exec(void);

    void fadeOut(void);

    void quit(int code);

private:

    bool procUI(void);

    void executeThread(void);

    GuiSound *bgMusic;
    CVideo *video;
    MainWindow *mainWindow;
    FreeTypeGX *fontSystem;
    GuiController *controller[5]{};
    int exitCode;
    BOOL sFromHBL = FALSE;
};

#endif //_APPLICATION_H