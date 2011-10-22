#ifndef SIGNALINTERCEPTOR_H
#define SIGNALINTERCEPTOR_H

#include "dynamicqobject.h"
#include <QWeakPointer>

class SignalInterceptor : public DynamicQObject
{
public:
    explicit SignalInterceptor(QObject *target, QObject *parent = 0);

protected:
    virtual void metaMethodCall(const QMetaMethod &method, QVariantList parameters);

private:
    QWeakPointer<QObject> target;
};

#endif // SIGNALINTERCEPTOR_H
