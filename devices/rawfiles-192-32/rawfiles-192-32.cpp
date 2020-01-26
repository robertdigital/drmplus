#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ Decoder
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
 *    along with DRM+Decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	File reader:
 *	For the files with 32 bit raw data 
 */
#include        <QLabel>
#include        <QFileDialog>
#include        "radio-constants.h"
#include        "radio.h"
#include	<ctime>
#include	"raw-reader-192-32.h"
#include	"rawfiles-192-32.h"

//
	rawFiles_32::rawFiles_32 (RadioInterface *mr,
	                          int32_t	rate,
	                          RingBuffer<std::complex<float>> *b) :
	                              deviceHandler (mr) {
	theRate		= rate;
	myFrame		= new QFrame;
	setupUi	(myFrame);
	myFrame		-> show ();
	QString	replayFile
	              = QFileDialog::
	                 getOpenFileName (myFrame,
	                                  tr ("load file .."),
	                                  QDir::homePath (),
	                                  tr ("iq (*.*)"));
	replayFile	= QDir::toNativeSeparators (replayFile);
	myReader_32	= new rawReader_32 (replayFile, rate, b);
	connect (myReader_32, SIGNAL (set_progressBar (int)),
	         this, SLOT (set_progressBar (int)));
	connect (myReader_32, SIGNAL (dataAvailable (int)),
	         this, SLOT (handleData (int)));
	nameofFile	-> setText (replayFile);
	fileProgress	 -> setValue (10);
	set_progressBar	(10);
	this	-> lastFrequency	= Khz (94000);
	connect (fileProgress, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_progressBar (int)));
}

	rawFiles_32::~rawFiles_32 () {
	if (myReader_32 != NULL)
	   delete myReader_32;
	myReader_32	= NULL;
	delete	myFrame;
}

void    rawFiles_32::handle_progressBar	(int f) {
        myReader_32	-> setFileat (f);
}

void    rawFiles_32::set_progressBar     (int f) {
	fileProgress	 -> setValue (f);
}

bool	rawFiles_32::restartReader	() {
	if (myReader_32 != NULL) {
	   bool b = myReader_32 -> restartReader ();
	   return b;
	}
	else
	   return false;
}

void	rawFiles_32::stopReader		(void) {
	if (myReader_32 != NULL)
	   myReader_32	-> stopReader ();
}

void	rawFiles_32::exit		(void) {
	if (myReader_32 != NULL)
	   myReader_32	-> stopReader ();
}

int16_t	rawFiles_32::bitDepth		(void) {
	return 16;
}

void	rawFiles_32::reset		() {
	if (myReader_32 != NULL)
	   myReader_32	-> reset ();
}

int32_t	rawFiles_32::getRate		() {
	return 192000;
}

void	rawFiles_32::handleData		(int a) {
	emit dataAvailable (a);
}

