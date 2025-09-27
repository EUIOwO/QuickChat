#包含一个目录
INCLUDEPATH += $$PWD
#如果DEPENDPATH目录里的头文件发生了变化，所有的cpp都会重新编译
DEPENDPATH += $$PWD

#$$PWD当前目录

HEADERS += \
    $$PWD/chatwindow.h \
    $$PWD/widgethead.h \
    $$PWD/face/facedialog.h \
    $$PWD/face/emojiitem.h

SOURCES += \
    $$PWD/chatwindow.cpp \
    $$PWD/widgethead.cpp \
    $$PWD/face/facedialog.cpp \
    $$PWD/face/emojiitem.cpp

FORMS += \
    $$PWD/chatwindow.ui \
    $$PWD/face/facedialog.ui

