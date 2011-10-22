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

    void printString(const QString &str)
    {
        qDebug() << "A string! :" << str;
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

    QByteArray encoded = DynamicQObject::encodeObject(src, DynamicQObject::EncodeSignals);
    qDebug() << encoded.toHex();

    DynamicQObject *recreated = DynamicQObject::createFromEncodedObject(encoded);
    qDebug() << recreated;

    QObject::connect(recreated, SIGNAL(stringSignal(QString)), debug, SLOT(printString(QString)));

    QVariantList vl;
    vl << QVariant(QString("test"));
    recreated->emitSignal("stringSignal(QString)", vl);
}

#include "main.moc"
