TEMPLATE        = app
unix:CONFIG     += qt warn_on release
win32:CONFIG    += windows warn_on release
HEADERS         = midipp.h midipp_scores.h midipp_mainwindow.h midipp_mutemap.h midipp_looptab.h
SOURCES         = midipp.cpp midipp_scores.cpp midipp_mainwindow.cpp midipp_mutemap.cpp midipp_looptab.cpp
TARGET          = midipp
QTDIR_build:REQUIRES="contains(QT_CONFIG, full-config)"
unix:LIBS      += -lumidi20
