TEMPLATE        = app
unix:CONFIG     += qt warn_on release
win32:CONFIG    += windows warn_on release
HEADERS         += midipp.h
HEADERS         += midipp_bpm.h
HEADERS         += midipp_decode.h
HEADERS         += midipp_devices.h
HEADERS         += midipp_echotab.h
HEADERS         += midipp_import.h
HEADERS         += midipp_looptab.h
HEADERS         += midipp_mainwindow.h
HEADERS         += midipp_mutemap.h
HEADERS         += midipp_scores.h
HEADERS         += midipp_spinbox.h
HEADERS	        += midipp_pattern.h
SOURCES         += midipp.cpp
SOURCES         += midipp_bpm.cpp
SOURCES         += midipp_decode.cpp
SOURCES         += midipp_devices.cpp
SOURCES         += midipp_echotab.cpp
SOURCES         += midipp_import.cpp
SOURCES         += midipp_looptab.cpp
SOURCES         += midipp_mainwindow.cpp
SOURCES         += midipp_mutemap.cpp
SOURCES         += midipp_scores.cpp
SOURCES         += midipp_pattern.cpp
SOURCES         += midipp_spinbox.cpp
target.path	= /usr/local/bin
TARGET          = midipp
QTDIR_build:REQUIRES="contains(QT_CONFIG, full-config)"
unix:LIBS	+= -lumidi20
INSTALLS	+= target
