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


#ifndef __SOUNDFILE_H
#define __SOUNDFILE_H


#include <stdio.h>
#include <stdint.h>
#include <sndfile.h>

class SoundFile
{
public:

	enum ERwMode
	{
		eRwModeNone,
		eRwModeRead,
		eRwModeWrite
	};
	
	enum EAudioCodec
	{
		eAudioCodecOther,
		eAudioCodecCaf,
		eAudioCodecWav,
		eAudioCodecAmb
	};
	
	enum EBitDepth
	{
		eBitDepthOther,
		eBitDepth16Bits,
		eBitDepth24Bits,
		eBitDepth32Bits,
		eBitDepthFloat,
	};
	
	enum EError
	{
		eErrorNone = 0,
		eErrorRwMode = -1,
		eErrorAudioCodec = -2,
		eErrorBitDepth = -3,
		eErrorOpen = -4,
		eErrorSeek = -5,
		eErrorData = -6,
		eErrorRead = -7,
		eErrorWrite = -8
	};
	
	SoundFile (void);
	~SoundFile (void);
	
	int getReadWriteMode(void) const { return _readWriteMode; }
	int getAudioCodec(void) const { return _audioCodec; }
	int getBitDepth(void) const { return _bitDepth; }
	int getSampleRate(void) const { return _sampleRate; }
	int getNbChannels(void) const { return _nbChannels; }
	uint32_t getNbSamples(void) const { return _nbSamples; }
	
	int open_read(const char *name);
	int open_write(const char *name, int audioCodec, int bitDepth, int sampleRate, int nbChannels);
	int close(void);
	
	int seek(uint32_t posit);
	int read(float *data, uint32_t frames);
	int write(float *data, uint32_t frames);
	
private:
	
	enum { BUFFSIZE = 1024 };
	
	void reset (void);
	
	SNDFILE*  _sndfile;
	int       _readWriteMode;
	int       _audioCodec;
	int       _bitDepth;
	int       _sampleRate;
	int       _nbChannels;
	uint32_t  _nbSamples;
};

#endif

