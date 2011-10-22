#include <QCoreApplication>
#include "sourceobject.h"
#include "signalinterceptor.h"
#include <QMetaMethod>
#include <QDebug>

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<CustomType>();

    SourceObject *src = new SourceObject;
    SignalInterceptor *ic = new SignalInterceptor(src);
    src->sendSignals();
}
