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
#ifndef _GUIIMAGEASYNC_H_
#define _GUIIMAGEASYNC_H_

#include <gui/GuiImage.h>
#include <mutex>
#include <system/CThread.h>
#include <vector>

class GuiImageAsync : public GuiImage {
public:
    GuiImageAsync(const uint8_t *imageBuffer, const uint32_t &imageBufferSize, GuiImageData *preloadImg);

    GuiImageAsync(const std::string &filename, GuiImageData *preloadImg);

    virtual ~GuiImageAsync();

    static void clearQueue();

    static void removeFromQueue(GuiImageAsync *image) {
        threadRemoveImage(image);
    }

    //! don't forget to LOCK GUI if using this asynchron call
    sigslot::signal1<GuiImageAsync *> imageLoaded;

    static void threadExit();

private:
    static void threadInit();

    GuiImageData *imgData;
    std::string filename;
    const uint8_t *imgBuffer;
    const uint32_t imgBufferSize;

    static void guiImageAsyncThread(CThread *thread, void *arg);

    static void threadAddImage(GuiImageAsync *Image);

    static void threadRemoveImage(GuiImageAsync *Image);

    static std::vector<GuiImageAsync *> imageQueue;
    static CThread *pThread;
    static std::recursive_mutex *pMutex;
    static uint32_t threadRefCounter;
    static GuiImageAsync *pInUse;
    static bool bExitRequested;
};

#endif /*_GUIIMAGEASYNC_H_*/
