TEMPLATE = app
TARGET = signal-forwarding
QT = core network

SOURCES += \
    main.cpp \
    sourceobject.cpp \
    signalinterceptor.cpp \
    dynamicqobject.cpp

HEADERS += \
    sourceobject.h \
    signalinterceptor.h \
    dynamicqobject.h
