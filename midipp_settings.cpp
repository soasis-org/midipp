/*-
 * Copyright (c) 2012 Hans Petter Selasky. All rights reserved.
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

#include "midipp_mainwindow.h"
#include "midipp_scores.h"
#include "midipp_settings.h"
#include "midipp_volume.h"
#include "midipp_mode.h"
#include "midipp_import.h"

MppSettings :: MppSettings(MppMainWindow *_parent, const QString & fname)
  : QSettings(fname)
{
	mw = _parent;

	save_volume = -1;
	save_instruments = -1;
	save_viewmode = -1;
	save_devices = -1;
	save_font = -1;

	but_config_save = new QPushButton(tr("Config Save"));
	but_config_clean = new QPushButton(tr("Config Clean"));
	but_config_what = new QPushButton(tr("Config What"));
	but_config_load = new QPushButton(tr("Config Load"));

	connect(but_config_save, SIGNAL(released()), this, SLOT(handle_save()));
	connect(but_config_clean, SIGNAL(released()), this, SLOT(handle_clean()));
	connect(but_config_what, SIGNAL(released()), this, SLOT(handle_what()));
	connect(but_config_load, SIGNAL(released()), this, SLOT(handle_load()));

	mpp_what = new MppSettingsWhat(this);
}

MppSettings :: ~MppSettings(void)
{
	delete mpp_what;
}

int
MppSettings :: valueDefault(const QString &str, int n)
{
	int val;

	if (contains(str))
		val = value(str).toUInt();
	else
		val = n;

	return (val);
}

QString
MppSettings :: stringDefault(const QString &str, const QString &n)
{
	QString val;

	if (contains(str))
		val = value(str).toString();
	else
		val = n;

	return (val);
}

QString
MppSettings :: concat(const char *fmt, int num, int sub)
{
	char buf[80];
	snprintf(buf, sizeof(buf), fmt, num, sub);
	return (QString(buf));
}

void
MppSettings :: doSave(void)
{
	int x;
	int y;

	clear();

	beginGroup("global");
	setValue("save_volume", save_volume ? 1 : 0);
	setValue("save_instruments", save_instruments ? 1 : 0);
	setValue("save_viewmode", save_viewmode ? 1 : 0);
	setValue("save_devices", save_devices ? 1 : 0);
	setValue("save_font", save_font ? 1 : 0);
	endGroup();

	if (save_viewmode) {
		for (x = 0; x != MPP_MAX_VIEWS; x++) {
			beginGroup(concat("view%d", x));
			setValue("basekey", mw->scores_main[x]->baseKey);
			setValue("cmdkey", mw->scores_main[x]->cmdKey);
			setValue("delay", mw->scores_main[x]->delayNoise);
			setValue("inmask", mw->scores_main[x]->devInputMask);
			setValue("keymode", mw->scores_main[x]->keyMode);
			setValue("synthchannel", mw->scores_main[x]->synthChannel);
			setValue("chordcontrast", mw->scores_main[x]->chordContrast);
			setValue("chordnormalize", mw->scores_main[x]->chordNormalize);
			endGroup();
		}
	}

	if (save_volume) {
		beginGroup("volume");
		for (x = 0; x != 16; x++) {
			setValue(concat("play%d", x), mw->spn_volume_play[x]->value());
			setValue(concat("synth%d", x), mw->spn_volume_synth[x]->value());
		}
		endGroup();
	}

	if (save_instruments) {
		beginGroup("instruments");
		for (x = 0; x != 16; x++) {
			setValue(concat("bank%d", x), mw->spn_instr_bank[x]->value());
			setValue(concat("prog%d", x), mw->spn_instr_prog[x]->value());
			setValue(concat("mute%d", x), (int)mw->cbx_instr_mute[x]->isChecked());
		}
		endGroup();
	}

	if (save_devices) {
		for (y = 0; y != MPP_MAX_DEVS; y++) {
			beginGroup(concat("device%d", y));
			setValue("device", mw->led_config_dev[y]->text());
			setValue("play", (int)mw->cbx_config_dev[y][0]->isChecked());
			setValue("rec", (int)mw->cbx_config_dev[y][1]->isChecked());
			setValue("synth", (int)mw->cbx_config_dev[y][2]->isChecked());
			setValue("muteprog", mw->muteProgram[y]);
			setValue("mutepedal", mw->mutePedal[y]);
			setValue("enablelocalkeys", mw->enableLocalKeys[y]);
			setValue("disablelocalkeys", mw->disableLocalKeys[y]);
			setValue("muteallcontrol", mw->muteAllControl[y]);

			for (x = 0; x != 16; x++)
				setValue(concat("mute%d", x), mw->muteMap[y][x]);
			endGroup();
		}
	}
	if (save_font) {
		beginGroup("font");
		setValue("default", mw->defaultFont.toString());
		setValue("editor", mw->editFont.toString());
		endGroup();
	}
}

void
MppSettings :: doLoad(void)
{
	int x;
	int y;

	save_volume = valueDefault("global/save_volume", -1);
	save_instruments = valueDefault("global/save_instruments", -1);
	save_viewmode = valueDefault("global/save_viewmode", -1);
	save_devices = valueDefault("global/save_devices", -1);
	save_font = valueDefault("global/save_font", -1);

	if (save_viewmode > 0) {
		for (x = 0; x != MPP_MAX_VIEWS; x++) {
			int baseKey = valueDefault(concat("view%d/basekey", x), C4);
			int cmdKey = valueDefault(concat("view%d/cmdkey", x), C3);
			int delayNoise = valueDefault(concat("view%d/delay", x), 25);
			int devInputMask = valueDefault(concat("view%d/inmask", x), x ? 0 : -1);
			int keyMode = valueDefault(concat("view%d/keymode", x), 0);
			int synthChannel = valueDefault(concat("view%d/synthchannel", x), 0);
			int chordContrast = valueDefault(concat("view%d/chordcontrast", x), 128);
			int chordNormalize = valueDefault(concat("view%d/chordnormalize", x), 128);

			if (baseKey < 0 || baseKey > 127)
				baseKey = 0;
			if (cmdKey < 0 || cmdKey > 127)
				cmdKey = 0;
			if (delayNoise < 0 || delayNoise > 255)
				delayNoise = 0;
			if (keyMode < 0 || keyMode >= MM_PASS_MAX)
				keyMode = 0;
			if (synthChannel < 0 || synthChannel > 15)
				synthChannel = 0;
			if (chordContrast < 0 || chordContrast > 255)
				chordContrast = 128;
			if (chordNormalize < 0 || chordNormalize > 1)
				chordNormalize = 1;

			pthread_mutex_lock(&mw->mtx);
			mw->scores_main[x]->baseKey = baseKey;
			mw->scores_main[x]->cmdKey = cmdKey;
			mw->scores_main[x]->delayNoise = delayNoise;
			mw->scores_main[x]->devInputMask = devInputMask;
			mw->scores_main[x]->keyMode = keyMode;
			mw->scores_main[x]->synthChannel = synthChannel;
			mw->scores_main[x]->chordContrast = chordContrast;
			mw->scores_main[x]->chordNormalize = chordNormalize;
			pthread_mutex_unlock(&mw->mtx);

			mw->dlg_mode[x]->update_all();
		}
		mw->sync_key_mode();
	}

	if (save_volume > 0) {
		for (x = 0; x != 16; x++) {
			int play_vol;
			int synth_vol;
			play_vol = valueDefault(concat("volume/play%d", x), MPP_VOLUME_UNIT);
			if (play_vol < 0 || play_vol > MPP_VOLUME_MAX)
				play_vol = MPP_VOLUME_UNIT;
			synth_vol = valueDefault(concat("volume/synth%d", x), MPP_VOLUME_UNIT);
			if (synth_vol < 0 || synth_vol > MPP_VOLUME_MAX)
				synth_vol = MPP_VOLUME_UNIT;
			mw->spn_volume_play[x]->setValue(play_vol);
			mw->spn_volume_synth[x]->setValue(synth_vol);
		}
	}

	if (save_instruments > 0) {
		for (x = 0; x != 16; x++) {
			mw->spn_instr_bank[x]->setValue(valueDefault(concat("instruments/bank%d", x), 0) & 16383);
			mw->spn_instr_prog[x]->setValue(valueDefault(concat("instruments/prog%d", x), 0) & 127);
			mw->cbx_instr_mute[x]->setChecked(valueDefault(concat("instruments/mute%d", x), 0) ? 1 : 0);
		}
	}

	if (save_devices > 0) {
		for (y = 0; y != MPP_MAX_DEVS; y++) {
			mw->led_config_dev[y]->setText(stringDefault(concat("device%d/device", y), ""));
			mw->cbx_config_dev[y][0]->setChecked(valueDefault(concat("device%d/play", y), 0) ? 1 : 0);
			mw->cbx_config_dev[y][1]->setChecked(valueDefault(concat("device%d/rec", y), 0) ? 1 : 0);
			mw->cbx_config_dev[y][2]->setChecked(valueDefault(concat("device%d/synth", y), 0) ? 1 : 0);

			int muteProgram = valueDefault(concat("device%d/muteprog", y), 0) ? 1 : 0;
			int mutePedal = valueDefault(concat("device%d/mutepedal", y), 0) ? 1 : 0;
			int enableLocalKeys = valueDefault(concat("device%d/enablelocalkeys", y), 0) ? 1 : 0;
			int disableLocalKeys = valueDefault(concat("device%d/disablelocalkeys", y), 0) ? 1 : 0;
			int muteAllControl = valueDefault(concat("device%d/muteallcontrol", y), 0) ? 1 : 0;

			int mute[16];

			for (x = 0; x != 16; x++)
				mute[x] = valueDefault(concat("device%d/mute%d", y, x), 0) ? 1 : 0;

			pthread_mutex_lock(&mw->mtx);
			mw->muteProgram[y] = muteProgram; 
			mw->mutePedal[y] = mutePedal; 
			mw->enableLocalKeys[y] = enableLocalKeys; 
			mw->disableLocalKeys[y] = disableLocalKeys; 
			mw->muteAllControl[y] = muteAllControl; 

			for (x = 0; x != 16; x++)
				mw->muteMap[y][x] = mute[x];
			pthread_mutex_unlock(&mw->mtx);
		}
	}
	if (save_font > 0) {
		mw->defaultFont.fromString(
		    stringDefault("font/default",
		    "Sans Serif,-1,20,5,75,0,0,0,0,0"));
		if (mw->defaultFont.pixelSize() < 1)
			mw->defaultFont.setPixelSize(20);
		mw->handle_compile(1);

		mw->editFont.fromString(
		    stringDefault("font/editor",
		    "Monospace,-1,14,5,50,0,0,0,0,0"));
		if (mw->editFont.pixelSize() < 1)
			mw->editFont.setPixelSize(14);

		for (x = 0; x != MPP_MAX_VIEWS; x++)
			mw->scores_main[x]->editWidget->setFont(mw->editFont);

		mw->tab_help->setFont(mw->editFont);
		mw->tab_import->editWidget->setFont(mw->editFont);
	}
}

void
MppSettings :: handle_save(void)
{
	doSave();
	sync();
}

void
MppSettings :: handle_clean(void)
{
	QMessageBox mbox;

	mbox.setText("Do you want to clear the configuration?");
	mbox.setInformativeText("This step cannot be undone!");
	mbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	mbox.setDefaultButton(QMessageBox::No);
	mbox.setIcon(QMessageBox::Critical);
	mbox.setWindowIcon(QIcon(QString(MPP_ICON_FILE)));

	int ret = mbox.exec();

	if (ret != QMessageBox::Yes)
		return;

	clear();

	beginGroup("global");
	setValue("save_volume", 1);
	setValue("save_instruments", 1);
	setValue("save_viewmode", 1);
	setValue("save_devices", 1);
	setValue("save_font", 1);
	endGroup();

	doLoad();
}

void
MppSettings :: handle_what(void)
{
	mpp_what->doShow();
}

void
MppSettings :: handle_load(void)
{
	doLoad();

	mw->handle_config_apply();
}

MppSettingsWhat :: MppSettingsWhat(MppSettings *_parent)
  : QDialog()
{
	ms = _parent;

	gl = new QGridLayout(this);

	lbl_volume = new QLabel(tr("Save volume settings"));
	lbl_instruments = new QLabel(tr("Save instrument settings"));
	lbl_viewmode = new QLabel(tr("Save view mode"));
	lbl_deviceconfig = new QLabel(tr("Save device configuration"));
	lbl_font = new QLabel(tr("Save font selection"));

	cbx_volume = new QCheckBox();
	cbx_instruments = new QCheckBox();
	cbx_viewmode = new QCheckBox();
	cbx_deviceconfig = new QCheckBox();
	cbx_font = new QCheckBox();

	but_ok = new QPushButton(tr("Close"));
	but_reset = new QPushButton(tr("Reset"));

	connect(but_ok, SIGNAL(released()), this, SLOT(accept()));
	connect(but_reset, SIGNAL(released()), this, SLOT(handle_reset()));

	setWindowTitle(tr("Configuration save and load selection"));
	setWindowIcon(QIcon(QString(MPP_ICON_FILE)));

	gl->addWidget(lbl_volume, 0, 0, 1, 1);
	gl->addWidget(lbl_instruments, 1, 0, 1, 1);
	gl->addWidget(lbl_viewmode, 2, 0, 1, 1);
	gl->addWidget(lbl_deviceconfig, 3, 0, 1, 1);
	gl->addWidget(lbl_font, 4, 0, 1, 1);

	gl->addWidget(cbx_volume, 0, 2, 1, 2);
	gl->addWidget(cbx_instruments, 1, 2, 1, 2);
	gl->addWidget(cbx_viewmode, 2, 2, 1, 2);
	gl->addWidget(cbx_deviceconfig, 3, 2, 1, 2);
	gl->addWidget(cbx_font, 4, 2, 1, 2);

	gl->addWidget(but_reset, 5, 1, 1, 1);
	gl->addWidget(but_ok, 5, 2, 1, 1);
}

MppSettingsWhat :: ~MppSettingsWhat(void)
{

}

void
MppSettingsWhat :: doShow(void)
{
	cbx_volume->setChecked(ms->save_volume ? 1 : 0);
	cbx_instruments->setChecked(ms->save_instruments ? 1 : 0);
	cbx_viewmode->setChecked(ms->save_viewmode ? 1 : 0);
	cbx_deviceconfig->setChecked(ms->save_devices ? 1 : 0);
	cbx_font->setChecked(ms->save_font ? 1 : 0);

	exec();

	ms->save_volume = cbx_volume->isChecked() ? 1 : 0;
	ms->save_instruments = cbx_instruments->isChecked() ? 1 : 0;
	ms->save_viewmode = cbx_viewmode->isChecked() ? 1 : 0;
	ms->save_devices = cbx_deviceconfig->isChecked() ? 1 : 0;
	ms->save_font = cbx_font->isChecked() ? 1 : 0;
}

void
MppSettingsWhat :: handle_reset(void)
{
	cbx_volume->setChecked(1);
	cbx_instruments->setChecked(1);
	cbx_viewmode->setChecked(1);
	cbx_deviceconfig->setChecked(1);
	cbx_font->setChecked(1);
}