#include <QCoreApplication>
#include "sourceobject.h"
#include "dynamicqobject.h"
#include "signalinterceptor.h"
#include <QMetaMethod>
#include <QDebug>

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<CustomType>();

    DynamicQObject *test = new DynamicQObject("TestObject");
    test->addSignal("mySignal(int)");
    test->addSlot("mySlot(int)");
    QObject::connect(test, SIGNAL(mySignal(int)), test, SLOT(mySlot(int)));

    SourceObject *src = new SourceObject;
    SignalInterceptor *ic = new SignalInterceptor(src);
    src->sendSignals();
}
