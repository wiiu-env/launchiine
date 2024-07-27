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
#include "../fs/CFile.hpp"
#include <gui/GuiImageAsync.h>
#include <unistd.h>

std::vector<GuiImageAsync *> GuiImageAsync::imageQueue;
CThread *GuiImageAsync::pThread             = NULL;
std::recursive_mutex *GuiImageAsync::pMutex = NULL;
uint32_t GuiImageAsync::threadRefCounter    = 0;
bool GuiImageAsync::bExitRequested          = false;
GuiImageAsync *GuiImageAsync::pInUse        = NULL;

GuiImageAsync::GuiImageAsync(const uint8_t *imageBuffer, const uint32_t &imageBufferSize, GuiImageData *preloadImg)
    : GuiImage(preloadImg), imgData(NULL), imgBuffer(imageBuffer), imgBufferSize(imageBufferSize) {
    threadInit();
    threadAddImage(this);
}

GuiImageAsync::GuiImageAsync(const std::string &file, GuiImageData *preloadImg)
    : GuiImage(preloadImg), imgData(NULL), filename(file), imgBuffer(NULL), imgBufferSize(0) {
    threadInit();
    threadAddImage(this);
}

GuiImageAsync::~GuiImageAsync() {
    threadRemoveImage(this);
    while (pInUse == this)
        OSSleepTicks(OSMicrosecondsToTicks(1000));

    if (imgData) {
        delete imgData;
    }

    //threadExit();
}

void GuiImageAsync::threadAddImage(GuiImageAsync *Image) {
    pMutex->lock();
    imageQueue.push_back(Image);
    pMutex->unlock();
    pThread->resumeThread();
}

void GuiImageAsync::threadRemoveImage(GuiImageAsync *image) {
    pMutex->lock();
    for (uint32_t i = 0; i < imageQueue.size(); ++i) {
        if (imageQueue[i] == image) {
            imageQueue.erase(imageQueue.begin() + i);
            break;
        }
    }
    pMutex->unlock();
}

void GuiImageAsync::clearQueue() {
    pMutex->lock();
    imageQueue.clear();
    pMutex->unlock();
}

void GuiImageAsync::guiImageAsyncThread(CThread *thread, void *arg) {
    while (!bExitRequested) {
        if (imageQueue.empty() && !bExitRequested) {
            pThread->suspendThread();
        }

        if (!imageQueue.empty() && !bExitRequested) {
            pMutex->lock();
            pInUse = imageQueue.front();
            imageQueue.erase(imageQueue.begin());
            pMutex->unlock();

            if (!pInUse) {
                continue;
            }


            if (pInUse->imgBuffer && pInUse->imgBufferSize) {
                pInUse->imgData = new GuiImageData(pInUse->imgBuffer, pInUse->imgBufferSize);
            } else {
                uint8_t *buffer     = NULL;
                uint64_t bufferSize = 0;

                CFile file(pInUse->filename, CFile::ReadOnly);
                if (file.isOpen()) {
                    uint64_t filesize = file.size();
                    buffer            = (uint8_t *) malloc(filesize);
                    if (buffer != NULL) {
                        uint32_t blocksize = 0x4000;
                        uint32_t done      = 0;
                        int32_t readBytes  = 0;
                        while (done < filesize) {
                            if (done + blocksize > filesize) {
                                blocksize = filesize - done;
                            }
                            readBytes = file.read(buffer + done, blocksize);
                            if (readBytes <= 0) {
                                break;
                            }
                            done += readBytes;
                        }
                        if (done == filesize) {
                            bufferSize = filesize;
                        } else {
                            free(buffer);
                        }
                    }
                    file.close();
                }

                if (buffer != NULL && bufferSize > 0) {
                    pInUse->imgData = new GuiImageData(buffer, bufferSize, GX2_TEX_CLAMP_MODE_MIRROR);

                    //! free original image buffer which is converted to texture now and not needed anymore
                    free(buffer);
                }
            }

            if (pInUse->imgData) {
                if (pInUse->imgData->getTexture()) {
                    pInUse->width     = pInUse->imgData->getWidth();
                    pInUse->height    = pInUse->imgData->getHeight();
                    pInUse->imageData = pInUse->imgData;
                } else {
                    delete pInUse->imgData;
                    pInUse->imgData = NULL;
                }
            }
            pInUse->imageLoaded(pInUse);
            pInUse = NULL;
        }
    }
}

void GuiImageAsync::threadInit() {
    if (pThread == NULL) {
        bExitRequested = false;
        pMutex         = new std::recursive_mutex();
        pThread        = CThread::create(GuiImageAsync::guiImageAsyncThread, NULL, CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 10);
        pThread->resumeThread();
    }

    ++threadRefCounter;
}

void GuiImageAsync::threadExit() {
    if (threadRefCounter) {
        --threadRefCounter;
    }

    if (/*(threadRefCounter == 0) &&*/ (pThread != NULL)) {
        bExitRequested = true;
        delete pThread;
        delete pMutex;
        pThread = NULL;
        pMutex  = NULL;
    }
}
