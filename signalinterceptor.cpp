#include "signalinterceptor.h"
#include <QMetaMethod>
#include <QDebug>

SignalInterceptor::SignalInterceptor(QObject *t, QObject *parent)
    : DynamicQObject("SignalInterceptor", parent), target(t)
{
    const QMetaObject *tm = target.data()->metaObject();

    /* Add slots mirroring all signals from the target object */
    for (int i = 0; i < tm->methodCount(); ++i)
    {
        QMetaMethod signal = tm->method(i);
        if (signal.methodType() != QMetaMethod::Signal)
            continue;

        QByteArray signature = "intercept_";
        signature += signal.signature();
        QByteArray parameters;

        /* parameters */
        QList<QByteArray> pl = signal.parameterNames();
        foreach (QByteArray p, pl) {
            parameters.append(p);
            parameters.append(',');
        }
        if (!parameters.isEmpty())
            parameters.chop(1);

        addSlot(signature.constData(), parameters.constData());
    }

    /* Connect all signals to all slots; this must happen after, to avoid
     * modifying the metaobject after creation. */
    for (int i = 0; i < tm->methodCount(); ++i)
    {
        QMetaMethod signal = tm->method(i);
        if (signal.methodType() != QMetaMethod::Signal)
            continue;

        QByteArray signalName = "2";
        signalName.append(signal.signature());

        QByteArray slotName = "1intercept_";
        slotName.append(signal.signature());

        connect(target.data(), signalName.constData(), this, slotName.constData());
    }
}

void SignalInterceptor::metaMethodCall(const QMetaMethod &method, QVariantList parameters)
{
    if (!target)
        return;

    qDebug() << "SignalInterceptor:" << target.data() << "signal" << method.signature();

    QList<QByteArray> names = method.parameterNames();
    for (int i = 0; i < parameters.size(); ++i)
        qDebug() << "SignalInterceptor:  " << names[i] << parameters[i];
}
