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
