#include <QCoreApplication>
#include "sourceobject.h"
#include "dynamicqobject.h"
#include "signalinterceptor.h"
#include <QMetaMethod>
#include <QDebug>

class DebugObject : public QObject
{
    Q_OBJECT

public:
    DebugObject() { }

public slots:
    void signalReceived(const QByteArray &signature, const QVariantList &parameters)
    {
        qDebug() << "Signal intercepted:" << signature;
    }
};

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
    DebugObject *debug = new DebugObject;
    QObject::connect(ic, SIGNAL(signal(QByteArray,QVariantList)), debug, SLOT(signalReceived(QByteArray,QVariantList)));

    src->sendSignals();
}

#include "main.moc"
