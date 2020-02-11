#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ decoder
 *
 *    DRM+ decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM+ decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM+ decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"drm-decoder.h"
#include	"post-processor.h"
#include	"aac-processor.h"
#include	"xheaac-processor.h"
#ifdef  __WITH_FDK_AAC__
#include        "fdk-aac.h"
#elif   __WITH_FAAD__
#include        "drm-aacdecoder.h"
#endif

	postProcessor::postProcessor	(drmDecoder *drm,
	                                 drmParameters *params,
	                                 int shortId) {
	this	-> parent	= drm;
	this	-> params	= params;
#ifdef  __WITH_FDK_AAC__
        my_aacDecoder           = new fdkAAC    (drm, params);
#elif   __WITH_FAAD__
        my_aacDecoder           = new DRM_aacDecoder (drm, params);
#else
        my_aacDecoder           = new decoderBase ();
#endif

	my_aacProcessor		= new aacProcessor	(drm, params,
	                                                      my_aacDecoder);
	my_xheaacProcessor	= new xheaacProcessor	(drm, params,
	                                                      my_aacDecoder);

	connect (this, SIGNAL (show_audioMode (QString)),
                 parent, SLOT (show_audioMode (QString)));

}

	postProcessor::~postProcessor	() {
	delete	my_aacDecoder;
	delete	my_aacProcessor;
	delete	my_xheaacProcessor;
}

void	postProcessor::process	(uint8_t *buf_1,
	                         uint8_t *buf_2, int shortId) {
int	streamId	= params -> subChannels [shortId]. streamId;
int	startPosA	= params -> theStreams [streamId]. offsetHigh;
int	startPosB	= params -> theStreams [streamId]. offsetLow;
int	lengthA		= params -> theStreams [streamId]. lengthHigh;
int	lengthB		= params -> theStreams [streamId]. lengthLow;

	uint8_t dataVec [2 * 8 * (lengthA + lengthB)];
	memcpy (dataVec, &buf_1 [startPosA * 8], lengthA * 8);
	memcpy (&dataVec [lengthA * 8],
	                   &buf_2 [startPosA * 8], lengthA * 8);
	memcpy (&dataVec [2 * lengthA * 8],
	                   &buf_1 [startPosB * 8], lengthB * 8);
	memcpy (&dataVec [(2 * lengthA + lengthB) * 8],
	                   &buf_2 [startPosB * 8], lengthB * 8);
//	if (params -> subChannels [shortId]. is_audioService) 
	if (params -> theStreams [streamId]. audioStream)
	   processAudio (dataVec, streamId,
	                 0,           2 * lengthA,
	                 2 * lengthA, 2 * lengthB);
//	else		// apparently a data service
//	   processData (dataVec, streamId,
//	                0,           2 * lengthA,
//	                2 * lengthA, 2 * lengthB);
}

void	postProcessor::processAudio (uint8_t *v, int16_t streamIndex,
	                             int16_t startHigh, int16_t lengthHigh,
	                             int16_t startLow,  int16_t lengthLow) {
uint8_t	audioCoding	= params -> theStreams [streamIndex]. audioCoding;

	switch (audioCoding) {
	   case 0:		// AAC
	      show_audioMode (QString ("AAC"));
	      my_aacProcessor -> process_aac (v, streamIndex,
	                                      startHigh, lengthHigh,
	                                      startLow,  lengthLow);
	      return;

	   case 3:		// xHE_AAC
	      show_audioMode (QString ("xHE-AAC"));
#ifdef	__WITH_FDK_AAC__
	      my_xheaacProcessor -> process_usac (v, streamIndex,
	                                          startHigh, lengthHigh,
	                                          startLow,  lengthLow);
#endif
	      return;

	   default:
	      fprintf (stderr,
	               "unsupported format audioCoding (%d)\n", audioCoding);
	      return;
	}
}
