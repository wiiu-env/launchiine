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
#pragma once

#include <map>
#include "gui/GuiTitleBrowser.h"
#include <gui/GuiParticleImage.h>

class GuiIconGrid : public GuiTitleBrowser, public sigslot::has_slots<> {
public:
    GuiIconGrid(int32_t w, int32_t h, int32_t GameIndex);
    virtual ~GuiIconGrid();

    void setSelectedGame(int32_t idx);
    int32_t getSelectedGame(void);

    void update(GuiController * t);
    void draw(CVideo *pVideo);
    void process();
private:
    static const int32_t MAX_ROWS = 3;
    static const int32_t MAX_COLS = 5;

    GuiSound *buttonClickSound;

    int32_t selectedGame = 0;

    GuiParticleImage particleBgImage;
};
