#ifndef SIGNALINTERCEPTOR_H
#define SIGNALINTERCEPTOR_H

#include <QObject>

class SignalInterceptor : public QObject
{
public:
    explicit SignalInterceptor(QObject *target, QObject *parent = 0);
    virtual ~SignalInterceptor();

    /* MOC */
    static const QMetaObjectExtraData staticMetaObjectExtraData;
    static void qt_static_metacall(QObject *, QMetaObject::Call, int, void **);
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **);

private:
    QMetaObject dynamicMetaObject;
    QByteArray string_data;
    uint *meta_data;

    void handle_metacall(int id, void **a);
};

#endif // SIGNALINTERCEPTOR_H
