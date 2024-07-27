/***************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * for WiiXplorer 2010
 ***************************************************************************/
#ifndef BUFFER_CIRCLE_HPP_
#define BUFFER_CIRCLE_HPP_

#include <cstddef>
#include <stdint.h>
#include <vector>

class BufferCircle {
public:
    //!> Constructor
    BufferCircle();

    //!> Destructor
    ~BufferCircle();

    //!> Set circle size
    void Resize(int32_t size);

    //!> Get the circle size
    int32_t Size() {
        return SoundBuffer.size();
    };

    //!> Set/resize the buffer size
    void SetBufferBlockSize(int32_t size);

    //!> Remove a buffer
    void RemoveBuffer(int32_t pos);

    //!> Set all buffers clear
    void ClearBuffer();

    //!> Free all buffers
    void FreeBuffer();

    //!> Switch to next buffer
    void LoadNext();

    //!> Get the current buffer
    uint8_t *GetBuffer() {
        return GetBuffer(which);
    };

    //!> Get a buffer at a position
    uint8_t *GetBuffer(int32_t pos) {
        if (!Valid(pos)) {
            return NULL;
        } else {
            return SoundBuffer[pos];
        }
    };

    //!> Get current buffer size
    uint32_t GetBufferSize() {
        return GetBufferSize(which);
    };

    //!> Get buffer size at position
    uint32_t GetBufferSize(int32_t pos) {
        if (!Valid(pos)) {
            return 0;
        } else {
            return BufferSize[pos];
        }
    };

    //!> Is current buffer ready
    bool IsBufferReady() {
        return IsBufferReady(which);
    };

    //!> Is  a buffer at a position ready
    bool IsBufferReady(int32_t pos) {
        if (!Valid(pos)) {
            return false;
        } else {
            return BufferReady[pos];
        }
    };

    //!> Set a buffer at a position to a ready state
    void SetBufferReady(int32_t pos, bool st);

    //!> Set the buffersize at a position
    void SetBufferSize(int32_t pos, int32_t size);

    //!> Get the current position in the circle
    uint16_t Which() {
        return which;
    };

    //!> Get the next location
    inline uint16_t Next() {
        return (which + 1 >= Size()) ? 0 : which + 1;
    }

    inline uint16_t Prev() {
        if (Size() == 0) {
            return 0;
        } else {
            return ((int32_t) which - 1 < 0) ? Size() - 1 : which - 1;
        }
    }

protected:
    //!> Check if the position is a valid position in the vector
    bool Valid(int32_t pos) {
        return !(pos < 0 || pos >= Size());
    };

    uint16_t which;
    uint32_t BufferBlockSize;
    std::vector<uint8_t *> SoundBuffer;
    std::vector<uint32_t> BufferSize;
    std::vector<bool> BufferReady;
};

#endif
