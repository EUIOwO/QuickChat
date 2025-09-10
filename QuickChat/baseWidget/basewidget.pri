#包含一个目录
INCLUDEPATH += $$PWD
#如果DEPENDPATH目录里的头文件发生了变化，所有的cpp都会重新编译
DEPENDPATH += $$PWD

#$$PWD当前目录

HEADERS += \
    $$PWD/animationstackedwidget.h\
    $$PWD/chatstackedwidget.h \
    $$PWD/customwidget.h \
    $$PWD/clineedit.h \
    $$PWD/qqlist/qqlistviewchild.h \
    $$PWD/qqlist/qqlistviewgroup.h \
    $$PWD/qqlist/qqlistwidget.h \
    $$PWD/qqlist/qqpopmenuwidget.h \
    $$PWD/chatbubble.h

SOURCES += \
    $$PWD/animationstackedwidget.cpp\
    $$PWD/chatstackedwidget.cpp \
    $$PWD/customwidget.cpp \
    $$PWD/clineedit.cpp \
    $$PWD/qqlist/qqlistviewchild.cpp \
    $$PWD/qqlist/qqlistviewgroup.cpp \
    $$PWD/qqlist/qqlistwidget.cpp \
    $$PWD/qqlist/qqpopmenuwidget.cpp \
    $$PWD/chatbubble.cpp

FORMS += \
    $$PWD/qqlist/qqlistviewgroup.ui

