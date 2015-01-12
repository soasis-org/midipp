Name:           midipp
Version:        1.2.15
Release:        1%{?dist}
Summary:        MIDI Player Pro
Group:          Graphical Desktop/Applications/Multimedia
License:        BSD-2-Clause
URL:            http://www.selasky.org/hans_petter/midistudio
Source0:        http://www.selasky.org/hans_petter/distfiles/%{name}-%{version}.tar.bz2
Source1:        http://www.selasky.org/hans_petter/distfiles/libumidi-2.0.13.tar.bz2

BuildRequires:  make gcc libjack-devel phonon-devel qt-devel qt-settings qtwebkit-devel
Requires:       qt

%description
MIDI Player Pro allows you to play any kind of MIDI music in seconds
with your fingertips. List of supported features:

- Raw MIDI.
- Jack MIDI.
- Import from lyrics sites (chorded lyrics)
- Import from GuitarPro v3 and v4 format.
- Loading and saving from and to standard v1.0 MIDI files.
- Realtime MIDI processing.
- Simple sequence looping.
- 30000 BPM MIDI recording and playback.
- Undo/Redo support.
- Printing music like PDF.

%prep
%setup -q -a 1

%build
qmake-qt4 PREFIX=/usr DESTDIR=$RPM_BUILD_ROOT HAVE_STATIC=YES HAVE_JACK=YES LIBUMIDIPATH=libumidi-2.0.13 midipp.pro
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
INSTALL_ROOT=$RPM_BUILD_ROOT
export INSTALL_ROOT

%make_install
unset INSTALL_ROOT

%files
/usr/bin/midipp
/usr/share/applications/midipp.desktop
/usr/share/pixmaps/midipp.png
%doc

%changelog
* Sun Dec 14 2014 HPS hps@selasky.org 1.2.15-1
* Sun Dec 14 2014 HPS hps@selasky.org 1.2.14-1
- Improved lyrics import and export functionality
- Added support for two more chords
* Thu Oct 16 2014 HPS hps@selasky.org 1.2.13-1
- Updated ViewMode selection to be more interactive
- Moved all device selection to configuration tab
* Fri Sep 23 2014 HPS hps@selasky.org 1.2.12-1
- Created RPM package.
- Some minor compile fixes.
