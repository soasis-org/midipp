/*-
 * Copyright (c) 2012-2019 Hans Petter Selasky. All rights reserved.
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
#include "midipp_database.h"
#include "midipp_checkbox.h"
#include "midipp_buttonmap.h"
#include "midipp_custom.h"
#include "midipp_shortcut.h"
#include "midipp_show.h"
#include "midipp_sheet.h"
#include "midipp_instrument.h"
#include "midipp_devsel.h"

MppSettings :: MppSettings(MppMainWindow *_parent, const QString & fname)
  : QSettings(fname)
{
	mw = _parent;

	save_instruments = -1;
	save_viewmode = -1;
	save_devices = -1;
	save_font = -1;
	save_database_url = -1;
	save_database_data = -1;
	save_custom = -1;
	save_shortcut = -1;

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
		val = value(str).toInt();
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

QByteArray
MppSettings :: byteArrayDefault(const QString &str, const QByteArray &n)
{
	QByteArray val;

	if (contains(str))
		val = value(str).toByteArray();
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
	setValue("save_instruments", save_instruments ? 1 : 0);
	setValue("save_viewmode", save_viewmode ? 1 : 0);
	setValue("save_devices", save_devices ? 1 : 0);
	setValue("save_font", save_font ? 1 : 0);
	setValue("save_database_url", save_database_url ? 1 : 0);
	setValue("save_database_data", save_database_data ? 1 : 0);
	setValue("save_custom", save_custom ? 1 : 0);
	setValue("save_shortcut", save_shortcut ? 1 : 0);
	endGroup();

	if (save_viewmode) {
		for (x = 0; x != MPP_MAX_VIEWS; x++) {
			beginGroup(concat("view%d", x));
			setValue("basekey192", mw->scores_main[x]->baseKey / MPP_BAND_STEP_192);
			setValue("delay", mw->scores_main[x]->delayNoise);
			switch (mw->scores_main[x]->keyMode) {
			case MM_PASS_NONE_FIXED:
				setValue("keymode", 2);
				break;
			case MM_PASS_NONE_TRANS:
				setValue("keymode", 3);
				break;
			case MM_PASS_NONE_CHORD_PIANO:
				setValue("keymode", 4);
				break;
			case MM_PASS_NONE_CHORD_AUX:
				setValue("keymode", 5);
				break;
			case MM_PASS_NONE_CHORD_TRANS:
				setValue("keymode", 6);
				break;
			default:
				setValue("keymode", 0);
				break;
			}
			switch (mw->scores_main[x]->noteMode) {
			case MM_NOTEMODE_SYSEX:
				setValue("notemode", 1);
				break;
			default:
				setValue("notemode", 0);
				break;
			}
			setValue("inputchannel", mw->scores_main[x]->inputChannel);
			setValue("synthchannel", mw->scores_main[x]->synthChannel);
			setValue("synthchannelbase", mw->scores_main[x]->synthChannelBase);
			setValue("synthchanneltreb", mw->scores_main[x]->synthChannelTreb);
			setValue("auxchannel", mw->scores_main[x]->auxChannel);
			setValue("auxchannelbase", mw->scores_main[x]->auxChannelBase);
			setValue("auxchanneltreb", mw->scores_main[x]->auxChannelTreb);
			setValue("synthdevice", mw->scores_main[x]->synthDevice);
			setValue("synthdevicebase", mw->scores_main[x]->synthDeviceBase);
			setValue("synthdevicetreb", mw->scores_main[x]->synthDeviceTreb);
			setValue("synthvolume", mw->trackVolume[MPP_DEFAULT_TRACK(x)]);
			setValue("synthvolumebase", mw->trackVolume[MPP_BASS_TRACK(x)]);
			setValue("synthvolumetreb", mw->trackVolume[MPP_TREBLE_TRACK(x)]);
			setValue("chordcontrast", mw->scores_main[x]->chordContrast);
			setValue("chordnormalize", mw->scores_main[x]->chordNormalize);
			setValue("songevents", mw->scores_main[x]->songEventsOn);
			endGroup();
		}
	}

	if (save_instruments) {
		beginGroup("instruments");
		for (x = 0; x != 16; x++) {
			setValue(concat("bank%d", x), mw->tab_instrument->spn_instr_bank[x]->value());
			setValue(concat("prog%d", x), mw->tab_instrument->spn_instr_prog[x]->value());
			setValue(concat("mute%d", x), (int)mw->tab_instrument->cbx_instr_mute[x]->isChecked());
		}
		endGroup();
	}

	if (save_devices) {
		for (y = 0; y != MPP_MAX_DEVS; y++) {
			beginGroup(concat("device%d", y));
			setValue("group", mw->but_config_sel[y]->value());
			setValue("device", mw->led_config_dev[y]->text());
			setValue("play", (int)mw->cbx_config_dev[y][0]->isChecked());
			setValue("muteprog", mw->muteProgram[y]);
			setValue("mutepedal", mw->mutePedal[y]);
			setValue("enablelocalkeys", mw->enableLocalKeys[y]);
			setValue("disablelocalkeys", mw->disableLocalKeys[y]);
			setValue("mutenonchannel", mw->muteAllNonChannel[y]);
			setValue("muteallcontrol", mw->muteAllControl[y]);
			for (x = 0; x != MPP_MAX_VIEWS; x++)
				setValue(concat("view%d", x), (int)mw->cbx_config_dev[y][1 + x]->isChecked());
			for (x = 0; x != 16; x++)
				setValue(concat("mute%d", x), mw->muteMap[y][x]);
			endGroup();
		}
	}
	if (save_font) {
		beginGroup("font");
		setValue("default", mw->defaultFont.toString());
		setValue("editor", mw->editFont.toString());
		setValue("print", mw->printFont.toString());
#ifndef HAVE_NO_SHOW
		setValue("show", mw->tab_show_control->showFont.toString());
#endif
		endGroup();
	}
	if (save_database_url) {
		beginGroup("database_url");
		setValue("location", mw->tab_database->location->text());
		endGroup();
	}
	if (save_database_data) {
		beginGroup("database_data");
		setValue("array", mw->tab_database->input_data);
		endGroup();
	}
	if (save_custom) {
		beginGroup("custom_data");
		for (x = 0; x != MPP_CUSTOM_MAX; x++)
			setValue(concat("command%d", x), mw->tab_custom->custom[x].led_send->text());
		endGroup();
	}
	if (save_shortcut) {
		beginGroup("shortcut_data");
		for (x = 0; x != MPP_SHORTCUT_MAX; x++)
			setValue(QString(mw->tab_shortcut->shortcut_desc[x]), mw->tab_shortcut->led_cmd[x]->text());
		endGroup();
	}
}

void
MppSettings :: doLoad(void)
{
	int x;
	int y;

	save_instruments = valueDefault("global/save_instruments", -1);
	save_viewmode = valueDefault("global/save_viewmode", -1);
	save_devices = valueDefault("global/save_devices", -1);
	save_font = valueDefault("global/save_font", -1);
	save_database_url = valueDefault("global/save_database_url", -1);
	save_database_data = valueDefault("global/save_database_data", -1);
	save_custom = valueDefault("global/save_custom", -1);
	save_shortcut = valueDefault("global/save_shortcut", -1);

	if (save_viewmode > 0) {
		for (x = 0; x != MPP_MAX_VIEWS; x++) {
			int baseKey = valueDefault(concat("view%d/basekey192", x), MPP_DEFAULT_BASE_KEY / MPP_BAND_STEP_192);
			int delayNoise = valueDefault(concat("view%d/delay", x), 25);
			int keyMode = valueDefault(concat("view%d/keymode", x), 0);
			int noteMode = valueDefault(concat("view%d/notemode", x), 0);
			int inputChannel = valueDefault(concat("view%d/inputchannel", x), -1);
			int synthChannel = valueDefault(concat("view%d/synthchannel", x), (x == 1) ? 9 : 0);
			int synthChannelBase = valueDefault(concat("view%d/synthchannelbase", x), -1);
			int synthChannelTreb = valueDefault(concat("view%d/synthchanneltreb", x), -1);
			int auxChannel = valueDefault(concat("view%d/auxchannel", x), -1);
			int auxChannelBase = valueDefault(concat("view%d/auxchannelbase", x), -1);
			int auxChannelTreb = valueDefault(concat("view%d/auxchanneltreb", x), -1);
			int synthDevice = valueDefault(concat("view%d/synthdevice", x), -1);
			int synthDeviceBase = valueDefault(concat("view%d/synthdevicebase", x), -1);
			int synthDeviceTreb = valueDefault(concat("view%d/synthdevicetreb", x), -1);
			int synthVolume = valueDefault(concat("view%d/synthvolume", x), -1);
			int synthVolumeBase = valueDefault(concat("view%d/synthvolumebase", x), -1);
			int synthVolumeTreb = valueDefault(concat("view%d/synthvolumetreb", x), -1);
			int chordContrast = valueDefault(concat("view%d/chordcontrast", x), 128);
			int chordNormalize = valueDefault(concat("view%d/chordnormalize", x), 128);
			int songEvents = valueDefault(concat("view%d/songevents", x), 0);

			if (delayNoise < 0 || delayNoise > 255)
				delayNoise = 0;
			if (inputChannel < 0 || inputChannel > 15)
				inputChannel = -1;
			if (synthChannel < 0 || synthChannel > 15)
				synthChannel = 0;
			if (synthChannelBase < 0 || synthChannelBase > 15)
				synthChannelBase = -1;
			if (synthChannelTreb < 0 || synthChannelTreb > 15)
				synthChannelTreb = -1;
			if (auxChannel < 0 || auxChannel > 15)
				auxChannel = -1;
			if (auxChannelBase < 0 || auxChannelBase > 15)
				auxChannelBase = -1;
			if (auxChannelTreb < 0 || auxChannelTreb > 15)
				auxChannelTreb = -1;
			if (synthDevice < 0 || synthDevice >= MPP_MAX_DEVS)
				synthDevice = -1;
			if (synthDeviceBase < 0 || synthDeviceBase >= MPP_MAX_DEVS)
				synthDeviceBase = -1;
			if (synthDeviceTreb < 0 || synthDeviceTreb >= MPP_MAX_DEVS)
				synthDeviceTreb = -1;
			if (synthVolume < 0 || synthVolume > MPP_VOLUME_MAX)
				synthVolume = MPP_VOLUME_UNIT;
			if (synthVolumeBase < 0 || synthVolumeBase > MPP_VOLUME_MAX)
				synthVolumeBase = MPP_VOLUME_UNIT;
			if (synthVolumeTreb < 0 || synthVolumeTreb > MPP_VOLUME_MAX)
				synthVolumeTreb = MPP_VOLUME_UNIT;
			if (chordContrast < 0 || chordContrast > 255)
				chordContrast = 128;
			if (chordNormalize < 0 || chordNormalize > 1)
				chordNormalize = 1;
			if (songEvents < 0 || songEvents > 1)
				songEvents = 0;

			mw->atomic_lock();
			mw->scores_main[x]->baseKey = baseKey * MPP_BAND_STEP_192;
			mw->scores_main[x]->delayNoise = delayNoise;
			switch (keyMode) {
			case 2:
				mw->scores_main[x]->keyMode = MM_PASS_NONE_FIXED;
				break;
			case 3:
				mw->scores_main[x]->keyMode = MM_PASS_NONE_TRANS;
				break;
			case 4:
				mw->scores_main[x]->keyMode = MM_PASS_NONE_CHORD_PIANO;
				break;
			case 5:
				mw->scores_main[x]->keyMode = MM_PASS_NONE_CHORD_AUX;
				break;
			case 6:
				mw->scores_main[x]->keyMode = MM_PASS_NONE_CHORD_TRANS;
				break;
			default:
				mw->scores_main[x]->keyMode = MM_PASS_ALL;
				break;
			}
			switch (noteMode) {
			case 1:
				mw->scores_main[x]->noteMode = MM_NOTEMODE_SYSEX;
				break;
			default:
				mw->scores_main[x]->noteMode = MM_NOTEMODE_NORMAL;
				break;
			}

			mw->scores_main[x]->inputChannel = inputChannel;
			mw->scores_main[x]->synthChannel = synthChannel;
			mw->scores_main[x]->synthChannelBase = synthChannelBase;
			mw->scores_main[x]->synthChannelTreb = synthChannelTreb;
			mw->scores_main[x]->auxChannel = auxChannel;
			mw->scores_main[x]->auxChannelBase = auxChannelBase;
			mw->scores_main[x]->auxChannelTreb = auxChannelTreb;
			mw->scores_main[x]->synthDevice = synthDevice;
			mw->scores_main[x]->synthDeviceBase = synthDeviceBase;
			mw->scores_main[x]->synthDeviceTreb = synthDeviceTreb;
			mw->trackVolume[MPP_DEFAULT_TRACK(x)] = synthVolume;
			mw->trackVolume[MPP_BASS_TRACK(x)] = synthVolumeBase;
			mw->trackVolume[MPP_TREBLE_TRACK(x)] = synthVolumeTreb;
			mw->scores_main[x]->chordContrast = chordContrast;
			mw->scores_main[x]->chordNormalize = chordNormalize;
			mw->scores_main[x]->songEventsOn = songEvents;
			mw->atomic_unlock();

			mw->dlg_mode[x]->update_all();
		}

		int value[2];

		mw->atomic_lock();
		value[0] = mw->scores_main[0]->keyMode;
		value[1] = mw->scores_main[1]->keyMode;
		mw->atomic_unlock();

		mw->mbm_key_mode_a->setSelection(value[0]);
		mw->mbm_key_mode_b->setSelection(value[1]);
	}

	if (save_instruments > 0) {
		for (x = 0; x != 16; x++) {
			mw->tab_instrument->spn_instr_bank[x]->setValue(valueDefault(concat("instruments/bank%d", x), 0) & 16383);
			mw->tab_instrument->spn_instr_prog[x]->setValue(valueDefault(concat("instruments/prog%d", x), 0) & 127);
			mw->tab_instrument->cbx_instr_mute[x]->setChecked(valueDefault(concat("instruments/mute%d", x), 0) ? 1 : 0);
		}
	}

	if (save_devices > 0) {
		for (y = 0; y != MPP_MAX_DEVS; y++) {
			mw->but_config_sel[y]->setValue(valueDefault(concat("device%d/group", y), y));
			mw->led_config_dev[y]->setText(stringDefault(concat("device%d/device", y), ""));
			mw->cbx_config_dev[y][0]->setChecked(valueDefault(concat("device%d/play", y), 0) ? 1 : 0);
			for (x = 0; x != MPP_MAX_VIEWS; x++) {
				mw->cbx_config_dev[y][1 + x]->setChecked(
				    valueDefault(concat("device%d/view%d", y, x), (x == 0)));
			}
			int muteProgram = valueDefault(concat("device%d/muteprog", y), 0) ? 1 : 0;
			int mutePedal = valueDefault(concat("device%d/mutepedal", y), 0) ? 1 : 0;
			int enableLocalKeys = valueDefault(concat("device%d/enablelocalkeys", y), 0) ? 1 : 0;
			int disableLocalKeys = valueDefault(concat("device%d/disablelocalkeys", y), 0) ? 1 : 0;
			int muteAllControl = valueDefault(concat("device%d/muteallcontrol", y), 0) ? 1 : 0;
			int muteAllNonChannel = valueDefault(concat("device%d/mutenonchannel", y), 0) ? 1 : 0;

			int mute[16];

			for (x = 0; x != 16; x++)
				mute[x] = valueDefault(concat("device%d/mute%d", y, x), 0) ? 1 : 0;

			mw->atomic_lock();
			mw->muteProgram[y] = muteProgram; 
			mw->mutePedal[y] = mutePedal; 
			mw->enableLocalKeys[y] = enableLocalKeys; 
			mw->disableLocalKeys[y] = disableLocalKeys; 
			mw->muteAllControl[y] = muteAllControl;
			mw->muteAllNonChannel[y] = muteAllNonChannel; 

			for (x = 0; x != 16; x++)
				mw->muteMap[y][x] = mute[x];
			mw->atomic_unlock();
		}
	}
	if (save_font > 0) {
		/* default font */
		mw->defaultFont.fromString(
		    stringDefault("font/default",
		    "Sans Serif,-1,20,5,75,0,0,0,0,0"));
		if (mw->defaultFont.pixelSize() < 1)
			mw->defaultFont.setPixelSize(20);
#ifndef HAVE_NO_SHOW
		/* show font */
		mw->tab_show_control->showFont.fromString(
		    stringDefault("font/show",
		    "Sans Serif,-1,24,5,75,0,0,0,0,0"));
		if (mw->tab_show_control->showFont.pixelSize() < 1)
			mw->tab_show_control->showFont.setPixelSize(20);
#endif

		mw->handle_compile(1);

		/* editor font */
		mw->editFont.fromString(
		    stringDefault("font/editor",
#ifdef __APPLE__
		    "Courier New,-1,14,5,50,0,0,0,0,0"
#else
		    "Monospace,-1,14,5,50,0,0,0,0,0"
#endif
		    ));
		if (mw->editFont.pixelSize() < 1)
			mw->editFont.setPixelSize(14);

		for (x = 0; x != MPP_MAX_VIEWS; x++) {
			mw->scores_main[x]->editWidget->setFont(mw->editFont);
			mw->scores_main[x]->sheet->update();
		}
		mw->tab_help->setFont(mw->editFont);
		mw->tab_import->editWidget->setFont(mw->editFont);

		/* print font */
		mw->printFont.fromString(
		    stringDefault("font/print",
		    "Sans Serif,-1,18,5,75,0,0,0,0,0"));
	}
	if (save_database_url > 0) {
		mw->tab_database->location->setText(
		    stringDefault("database_url/location", MPP_DEFAULT_URL));
	}
	if (save_database_data > 0) {
		mw->tab_database->input_data =
		  byteArrayDefault("database_data/array", QByteArray());
		mw->tab_database->handle_download_finished_sub();
	}
	if (save_custom > 0) {
		for (x = 0; x != MPP_CUSTOM_MAX; x++) {
			mw->tab_custom->custom[x].led_send->setText(
			   stringDefault(concat("custom_data/command%d", x), ""));
		}
	}
	if (save_shortcut > 0) {
		for (x = 0; x != MPP_SHORTCUT_MAX; x++) {
			mw->tab_shortcut->led_cmd[x]->setText(
			    stringDefault(QString("shortcut_data/") +
			    QString(mw->tab_shortcut->shortcut_desc[x]), ""));
		}
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
	mbox.setWindowIcon(QIcon(MppIconFile));
	mbox.setWindowTitle(MppVersion);

	int ret = mbox.exec();

	if (ret != QMessageBox::Yes)
		return;

	clear();

	beginGroup("global");
	setValue("save_instruments", 1);
	setValue("save_viewmode", 1);
	setValue("save_devices", 1);
	setValue("save_font", 1);
	setValue("save_database_url", 1);
	setValue("save_database_data", 1);
	setValue("save_custom", 1);
	setValue("save_shortcut", 1);
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

	cbx_instruments = new MppCheckBox();
	cbx_viewmode = new MppCheckBox();
	cbx_deviceconfig = new MppCheckBox();
	cbx_font = new MppCheckBox();
	cbx_database_url = new MppCheckBox();
	cbx_database_data = new MppCheckBox();
	cbx_custom = new MppCheckBox();
	cbx_shortcut = new MppCheckBox();

	but_ok = new QPushButton(tr("Close"));
	but_reset = new QPushButton(tr("Reset"));

	connect(but_ok, SIGNAL(released()), this, SLOT(accept()));
	connect(but_reset, SIGNAL(released()), this, SLOT(handle_reset()));

	setWindowTitle(tr("Configuration save and load selection"));
	setWindowIcon(QIcon(MppIconFile));

	gl->addWidget(new QLabel(tr("Save instrument settings")), 0, 0, 1, 1);
	gl->addWidget(new QLabel(tr("Save view mode settings")), 1, 0, 1, 1);
	gl->addWidget(new QLabel(tr("Save device configuration")), 2, 0, 1, 1);
	gl->addWidget(new QLabel(tr("Save font selection")), 3, 0, 1, 1);
	gl->addWidget(new QLabel(tr("Save database URL")), 4, 0, 1, 1);
	gl->addWidget(new QLabel(tr("Save database contents")), 5, 0, 1, 1);
	gl->addWidget(new QLabel(tr("Save custom MIDI commands")), 6, 0, 1, 1);
	gl->addWidget(new QLabel(tr("Save shortcut MIDI commands")), 7, 0, 1, 1);

	gl->addWidget(cbx_instruments, 0, 2, 1, 2);
	gl->addWidget(cbx_viewmode, 1, 2, 1, 2);
	gl->addWidget(cbx_deviceconfig, 2, 2, 1, 2);
	gl->addWidget(cbx_font, 3, 2, 1, 2);
	gl->addWidget(cbx_database_url, 4, 2, 1, 2);
	gl->addWidget(cbx_database_data, 5, 2, 1, 2);
	gl->addWidget(cbx_custom, 6, 2, 1, 2);
	gl->addWidget(cbx_shortcut, 7, 2, 1, 2);

	gl->addWidget(but_reset, 8, 1, 1, 1);
	gl->addWidget(but_ok, 8, 2, 1, 1);
}

MppSettingsWhat :: ~MppSettingsWhat(void)
{

}

void
MppSettingsWhat :: doShow(void)
{
	cbx_instruments->setChecked(ms->save_instruments ? 1 : 0);
	cbx_viewmode->setChecked(ms->save_viewmode ? 1 : 0);
	cbx_deviceconfig->setChecked(ms->save_devices ? 1 : 0);
	cbx_font->setChecked(ms->save_font ? 1 : 0);
	cbx_database_url->setChecked(ms->save_database_url ? 1 : 0);
	cbx_database_data->setChecked(ms->save_database_data ? 1 : 0);
	cbx_custom->setChecked(ms->save_custom ? 1 : 0);
	cbx_shortcut->setChecked(ms->save_shortcut ? 1 : 0);

	exec();

	ms->save_instruments = cbx_instruments->isChecked() ? 1 : 0;
	ms->save_viewmode = cbx_viewmode->isChecked() ? 1 : 0;
	ms->save_devices = cbx_deviceconfig->isChecked() ? 1 : 0;
	ms->save_font = cbx_font->isChecked() ? 1 : 0;
	ms->save_database_url = cbx_database_url->isChecked() ? 1 : 0;
	ms->save_database_data = cbx_database_data->isChecked() ? 1 : 0;
	ms->save_custom = cbx_custom->isChecked() ? 1 : 0;
	ms->save_shortcut = cbx_shortcut->isChecked() ? 1 : 0;
}

void
MppSettingsWhat :: handle_reset(void)
{
	cbx_instruments->setChecked(1);
	cbx_viewmode->setChecked(1);
	cbx_deviceconfig->setChecked(1);
	cbx_font->setChecked(1);
	cbx_database_url->setChecked(1);
	cbx_database_data->setChecked(1);
	cbx_custom->setChecked(1);
	cbx_shortcut->setChecked(1);
}
