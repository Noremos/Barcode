QT += quick quickcontrols2 charts

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../source/barcontainer.cpp \
        D:/MyA/Libraries/Include/QML/imageitem.cpp \
        ../../source/Component.cpp \
        ../../source/Hole.cpp \
        ../../source/barcode.cpp \
        ../../source/barcodeCreator.cpp \
        ../../source/point.cpp \
        backend.cpp \
        bimageprovider.cpp \
        main.cpp \
        selmat.cpp

RESOURCES += qml.qrc
RESOURCES += qtquickcontrols2.conf

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    D:/MyA/Libraries/Include/QML/imageitem.h \
    ../../include/Component.h \
    ../../include/Hole.h \
    ../../include/barcode.h \
    ../../include/barcodeCreator.h \
    ../../include/point.h \
    ProcessType.h \
    backend.h \
    bimageprovider.h \
    selmat.h

INCLUDEPATH +=  $$PWD/../../include



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../MyA/Libraries/Libs/ -lopencv_world411
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../MyA/Libraries/Libs/ -lopencv_world411d
else:unix: LIBS += -L$$PWD/../../../../../MyA/Libraries/Libs/ -lopencv_world411

INCLUDEPATH += $$PWD/../../../../../MyA/Libraries/Include
DEPENDPATH += $$PWD/../../../../../MyA/Libraries/Include

DISTFILES += \
    ../test/loaded/-22.png \
    as.astylerc



