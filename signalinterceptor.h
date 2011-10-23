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
