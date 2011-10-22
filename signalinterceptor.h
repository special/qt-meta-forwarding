#ifndef SIGNALINTERCEPTOR_H
#define SIGNALINTERCEPTOR_H

#include "dynamicqobject.h"
#include <QWeakPointer>

/* Intercept all signals emitted by an object and repackage them for convenient handling.
 *
 * This object will emit the 'signal' signal for any signals emitted by the target object.
 * That signal includes the normalized signature (e.g. "mySignal(int)") and a QVariantList
 * of all parameters. All parameter types must be registered (by the same name) with
 * qRegisterMetaType, or they will not be captured (this is the same restriction as with
 * queued signals).
 *
 * The destroyed() signal will be relayed as well. Once the target object is deleted, no
 * further signals will be emitted.
 */
class SignalInterceptor : public DynamicQObject
{
public:
    explicit SignalInterceptor(QObject *target, QObject *parent = 0);

    /* Included for code completion benefit only. This method doesn't actually exist. */
signals:
    void signal(const QByteArray &signature, const QVariantList &parameters);

protected:
    virtual void metaMethodCall(const QMetaMethod &method, QVariantList parameters);

private:
    QWeakPointer<QObject> target;
};

#endif // SIGNALINTERCEPTOR_H
