/*-
 * Copyright (c) 2014-2019 Hans Petter Selasky. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _MIDIPP_CHANSEL_H_
#define	_MIDIPP_CHANSEL_H_

#include "midipp.h"
#include "midipp_gridlayout.h"

class MppChanSelDiagValue : public QObject
{
	Q_OBJECT;
public:
	MppChanSelDiagValue() { };
	~MppChanSelDiagValue() { };
	int value;
	QDialog *parent;

public slots:
	void handle_released(int);
};

class MppChanSelDiag : public QDialog, public QGridLayout
{
public:
	MppChanSelDiag(QWidget *, int, int, int = 0xFFFF);
	~MppChanSelDiag();

	MppButton *butChannel[16];
	MppChanSelDiagValue value;
};

class MppChanSel : public QPushButton
{
	Q_OBJECT;

public:
	MppChanSel(MppMainWindow *, int, int);
	~MppChanSel();

	void setChannelMask(uint16_t);
	void setValue(int);
	int value();
private:
	int channel;
	int haveAny;
	uint16_t channelMask;
	MppMainWindow *mw;

public slots:
	void handle_released();
signals:
	void valueChanged(int);
};

#endif		/* _MIDIPP_CHANSEL_H_ */
