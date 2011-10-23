/* Qt meta-object forwarding (http://github.com/special/qt-meta-forwarding)
 *
 * Copyright 2011 John Brooks <john.brooks@dereferenced.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "signalinterceptor.h"
#include <QMetaMethod>
#include <QDebug>

SignalInterceptor::SignalInterceptor(QObject *t, QObject *parent)
    : DynamicQObject("SignalInterceptor", parent), target(t)
{
    const QMetaObject *tm = target.data()->metaObject();

    addSignal("signal(QByteArray,QVariantList)", "signature,parameters");

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

#ifndef QT_NO_DEBUG
    qDebug() << "SignalInterceptor:" << target.data() << "signal" << method.signature();

    QList<QByteArray> names = method.parameterNames();
    for (int i = 0; i < parameters.size(); ++i)
        qDebug() << "SignalInterceptor:  " << names[i] << parameters[i];
#endif

    QByteArray signal = method.signature();
    signal.remove(0, qstrlen("intercept_"));

    QVariantList sp;
    sp << signal << QVariant(parameters);
    emitSignal("signal(QByteArray,QVariantList)", sp);
}
