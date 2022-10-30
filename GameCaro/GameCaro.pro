QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Sources/Controller/FileFunction.cpp \
        Sources/Controller/ProtocolFunction.cpp \
        Sources/Controller/client.cpp \
        Sources/Controller/controller.cpp \
        Sources/Invalid/checkinvalid.cpp \
        Sources/Main/main.cpp \
        Sources/Model/AccountList.cpp \
        Sources/Model/BoardModel.cpp \
        Sources/Model/GameModel.cpp \
        Sources/Model/PlayerModel.cpp \
        Sources/View/view.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Headers/Controller/FileFunction.h \
    Headers/Controller/ProtocolFunction.h \
    Headers/Controller/client.h \
    Headers/Controller/controller.h \
    Headers/Exceptions/ClickException.h \
    Headers/Exceptions/CommandException.h \
    Headers/Exceptions/NameException.h \
    Headers/Exceptions/NumberException.h \
    Headers/Exceptions/SizeBoardException.h \
    Headers/Invalid/checkinvalid.h \
    Headers/Model/AccountList.h \
    Headers/Model/BoardModel.h \
    Headers/Model/GameModel.h \
    Headers/Model/PlayerModel.h \
    Headers/View/view.h

DISTFILES += \
    Databases/huy.ini \
    Databases/quan.ini \

