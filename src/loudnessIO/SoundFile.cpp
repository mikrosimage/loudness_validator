// -------------------------------------------------------------------------
//
//    Copyright (C) 2009-2010 Fons Adriaensen <fons@linuxaudio.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// -------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include "SoundFile.hpp"

namespace Loudness
{
namespace io
{

SoundFile::SoundFile(void)
{
    reset();
}

SoundFile::~SoundFile(void)
{
    close();
}

void SoundFile::reset(void)
{
    _sndfile = 0;
    _readWriteMode = eRwModeNone;
    _audioCodec = eAudioCodecOther;
    _bitDepth = eBitDepthOther;
    _sampleRate = 0;
    _nbChannels = 0;
    _nbSamples = 0;
}

int SoundFile::open_read(const char* name)
{
    SF_INFO I;

    if(_readWriteMode)
        return eErrorRwMode;
    reset();

    if((_sndfile = sf_open(name, SFM_READ, &I)) == 0)
        return eErrorOpen;

    _readWriteMode = eRwModeRead;

    switch(I.format & SF_FORMAT_TYPEMASK)
    {
        case SF_FORMAT_CAF:
            _audioCodec = eAudioCodecCaf;
            break;
        case SF_FORMAT_WAV:
            _audioCodec = eAudioCodecWav;
            break;
        case SF_FORMAT_WAVEX:
            if(sf_command(_sndfile, SFC_WAVEX_GET_AMBISONIC, 0, 0) == SF_AMBISONIC_B_FORMAT)
                _audioCodec = eAudioCodecAmb;
            else
                _audioCodec = eAudioCodecWav;
    }

    switch(I.format & SF_FORMAT_SUBMASK)
    {
        case SF_FORMAT_PCM_16:
            _bitDepth = eBitDepth16Bits;
            break;
        case SF_FORMAT_PCM_24:
            _bitDepth = eBitDepth24Bits;
            break;
        case SF_FORMAT_PCM_32:
            _bitDepth = eBitDepth32Bits;
            break;
        case SF_FORMAT_FLOAT:
            _bitDepth = eBitDepthFloat;
            break;
    }

    _sampleRate = I.samplerate;
    _nbChannels = I.channels;
    _nbSamples = I.frames;

    return 0;
}

int SoundFile::open_write(const char* name, int audioCodec, int bitDepth, int sampleRate, int nbChannels)
{
    SF_INFO I;

    if(_readWriteMode)
        return eErrorRwMode;
    if(!sampleRate || !nbChannels)
        return eErrorOpen;
    reset();

    switch(audioCodec)
    {
        case eAudioCodecCaf:
            I.format = SF_FORMAT_CAF;
            break;
        case eAudioCodecWav:
        case eAudioCodecAmb:
            I.format = (nbChannels > 2) ? SF_FORMAT_WAVEX : SF_FORMAT_WAV;
            break;
        default:
            return eErrorAudioCodec;
    }

    switch(bitDepth)
    {
        case eBitDepth16Bits:
            I.format |= SF_FORMAT_PCM_16;
            break;
        case eBitDepth24Bits:
            I.format |= SF_FORMAT_PCM_24;
            break;
        case eBitDepth32Bits:
            I.format |= SF_FORMAT_PCM_32;
            break;
        case eBitDepthFloat:
            I.format |= SF_FORMAT_FLOAT;
            break;
        default:
            return eErrorBitDepth;
    }

    I.samplerate = sampleRate;
    I.channels = nbChannels;
    I.sections = 1;

    if((_sndfile = sf_open(name, SFM_WRITE, &I)) == 0)
        return eErrorOpen;

    if(audioCodec == eAudioCodecAmb)
    {
        sf_command(_sndfile, SFC_WAVEX_SET_AMBISONIC, 0, SF_AMBISONIC_B_FORMAT);
    }

    _readWriteMode = eRwModeWrite;
    _audioCodec = audioCodec;
    _bitDepth = bitDepth;
    _sampleRate = sampleRate;
    _nbChannels = nbChannels;
    return 0;
}

int SoundFile::close(void)
{
    if(_sndfile)
        sf_close(_sndfile);
    reset();
    return 0;
}

int SoundFile::seek(uint32_t posit)
{
    if(!_sndfile)
        return eErrorRwMode;
    if(sf_seek(_sndfile, posit, SEEK_SET) != posit)
        return eErrorSeek;
    return 0;
}

int SoundFile::read(float* data, uint32_t frames)
{
    if(_readWriteMode != eRwModeRead)
        return eErrorRwMode;
    return sf_readf_float(_sndfile, data, frames);
}

int SoundFile::write(float* data, uint32_t frames)
{
    int i;
    uint32_t k;
    float *p, v;

    if(_readWriteMode != eRwModeWrite)
        return eErrorRwMode;

    if(_bitDepth != eBitDepthFloat)
    {
        for(i = 0; i < _nbChannels; i++)
        {
            p = data + i;
            for(k = 0; k < frames; k++)
            {
                v = *p;
                if(v > 1.0f)
                    v = 1.0f;
                else if(v < -1.0f)
                    v = -1.0f;
                *p = v;
                p += _nbChannels;
            }
        }
    }
    return sf_writef_float(_sndfile, data, frames);
}
}
}
