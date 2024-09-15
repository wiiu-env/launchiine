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
#include <sounds/SoundDecoder.hpp>
#include <span>
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

class OggDecoder : public SoundDecoder {
public:
    explicit OggDecoder(std::span<uint8_t> snd);

    ~OggDecoder() override;

    int32_t Rewind() override;

    int32_t Read(uint8_t *buffer, int32_t buffer_size, int32_t pos) override;

protected:
    void OpenFile();

    OggVorbis_File ogg_file{};
    vorbis_info *ogg_info{};
};
