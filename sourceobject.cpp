#include "sourceobject.h"
#include <QDateTime>

SourceObject::SourceObject(QObject *parent)
    : QObject(parent)
{
}

void SourceObject::sendSignals()
{
    emit emptySignal();
#ifdef COMPLICATED_SIGNALS
    emit stringSignal(QLatin1String("hello world"));
    emit complicatedSignal(CustomType(100, 200), QVariant(QDateTime::currentDateTime()));
#endif
}
