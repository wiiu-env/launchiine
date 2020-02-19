#pragma once

#include <gui/Gui.h>
#include <gui/sigslot.h>

class GuiTitleBrowser : public GuiFrame {
public:
    GuiTitleBrowser(int32_t w, int32_t h, int32_t GameIndex) : GuiFrame(w, h) {}
    virtual ~GuiTitleBrowser() {}

    virtual void setSelectedGame(int32_t idx) = 0;
    virtual int32_t getSelectedGame(void) = 0;

    sigslot::signal2<GuiTitleBrowser *, int32_t> gameLaunchClicked;
    sigslot::signal2<GuiTitleBrowser *, int32_t> gameSelectionChanged;
};
