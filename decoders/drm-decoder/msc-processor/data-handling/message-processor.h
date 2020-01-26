#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ decoder
 *
 *    DRM+ Decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM+ Decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM+ Decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__MESSAGE_PROCESSOR__
#define	__MESSAGE_PROCESSOR__

#include	<QObject>
#include	<QString>
#include	"basics.h"

class	drmDecoder;

class	messageProcessor: public QObject {
Q_OBJECT
public:
		messageProcessor	(drmDecoder *);
		~messageProcessor	(void);
	void	processMessage		(uint8_t *, int16_t);
private:
	drmDecoder	*drm;
	uint8_t		messageState;
	bool		check_CRC (uint8_t *v, int16_t cnt);
	void		addSegment (uint8_t *v, int16_t cnt);
	void		setBit (uint8_t *v, int16_t ind, uint8_t bit);
	int16_t		byteCount;
	int16_t		byteTeller;
	uint8_t 	theMessage [24];
	QString		myMessage;
signals:
	void		sendMessage (QString);
};

#endif

