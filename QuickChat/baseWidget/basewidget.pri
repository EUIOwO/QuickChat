#包含一个目录
INCLUDEPATH += $$PWD
#如果DEPENDPATH目录里的头文件发生了变化，所有的cpp都会重新编译
DEPENDPATH += $$PWD

#$$PWD当前目录

HEADERS += \
    $$PWD/animationstackedwidget.h

SOURCES += \
    $$PWD/animationstackedwidget.cpp
