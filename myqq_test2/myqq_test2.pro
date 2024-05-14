QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addmember.cpp \
    adm_dnu.cpp \
    administrator.cpp \
    chatrecords.cpp \
    chatsearch.cpp \
    chatwindow.cpp \
    confirm.cpp \
    filerec.cpp \
    filesend.cpp \
    friendchatwindow.cpp \
    groupchatwindow.cpp \
    login.cpp \
    main.cpp \
    server.cpp \
    user.cpp \
    usermanager.cpp \
    widget.cpp

HEADERS += \
    addmember.h \
    adm_dnu.h \
    administrator.h \
    chatrecords.h \
    chatsearch.h \
    chatwindow.h \
    confirm.h \
    filerec.h \
    filesend.h \
    friendchatwindow.h \
    groupchatwindow.h \
    login.h \
    server.h \
    user.h \
    usermanager.h \
    widget.h

FORMS += \
    addmember.ui \
    adm_dnu.ui \
    administrator.ui \
    chatrecords.ui \
    chatsearch.ui \
    chatwindow.ui \
    confirm.ui \
    filerec.ui \
    filesend.ui \
    friendchatwindow.ui \
    groupchatwindow.ui \
    login.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    bcg.qrc
