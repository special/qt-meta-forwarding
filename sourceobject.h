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

#ifndef SOURCEOBJECT_H
#define SOURCEOBJECT_H

#include <QObject>
#include <QHash>
#include <QVariant>

#define COMPLICATED_SIGNALS

struct CustomType
{
    int a;
    int b;

    CustomType() : a(0), b(0) { }
    CustomType(int a, int b)
        : a(a), b(b)
    {
    }
};

Q_DECLARE_METATYPE(CustomType)

class SourceObject : public QObject
{
    Q_OBJECT

public:
    explicit SourceObject(QObject *parent = 0);

    void sendSignals();

signals:
    void emptySignal();
#ifdef COMPLICATED_SIGNALS
    void stringSignal(const QString &str);
    void complicatedSignal(const CustomType &custom, const QVariant &variant);
#endif
};

#endif // SOURCEOBJECT_H
