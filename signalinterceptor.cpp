#include "signalinterceptor.h"
#include <QMetaMethod>
#include <QDebug>

const QMetaObjectExtraData SignalInterceptor::staticMetaObjectExtraData = {
    0, qt_static_metacall
};

SignalInterceptor::SignalInterceptor(QObject *target, QObject *parent)
    : QObject(parent)
{
    qDebug() << "constructor";

    const QMetaObject *tm = target->metaObject();
    int md_size = 15 + (5*tm->methodCount());

    string_data.append("SignalInterceptor\0\0", 19);
    meta_data = new uint[md_size];
    memset(meta_data, 0, md_size);
    meta_data[0] = 6; // revision

    int md_pos = 14;
    meta_data[5] = md_pos; // index of the first method data in this array

    int slot_count = 0;
    for (int i = 0; i < tm->methodCount(); ++i)
    {
        QMetaMethod signal = tm->method(i);
        if (signal.methodType() != QMetaMethod::Signal)
            continue;

        slot_count++;

        meta_data[md_pos++] = string_data.size();
        string_data.append("intercept_");
        string_data.append(signal.signature());
        string_data.append('\0');

        /* parameters */
        QList<QByteArray> parameters = signal.parameterNames();
        if (!parameters.isEmpty())
        {
            meta_data[md_pos++] = string_data.size();
            foreach (QByteArray p, parameters) {
                string_data.append(p);
                string_data.append(',');
            }
            string_data[string_data.size()-1] = '\0';
        }
        else
            meta_data[md_pos++] = 18;

        /* type, tag; 18 is a \0 */
        meta_data[md_pos++] = 18;
        meta_data[md_pos++] = 18;

        /* flags; what does this mean? */
        meta_data[md_pos++] = 0x0a;
    }

    meta_data[4] = slot_count;
    qDebug() << slot_count;

    dynamicMetaObject.d.superdata = &QObject::staticMetaObject;
    dynamicMetaObject.d.stringdata = string_data;
    dynamicMetaObject.d.data = meta_data;
    dynamicMetaObject.d.extradata = &staticMetaObjectExtraData;

    for (int i = 0; i < tm->methodCount(); ++i)
    {
        QMetaMethod signal = tm->method(i);
        if (signal.methodType() != QMetaMethod::Signal)
            continue;

        QByteArray signalName = "2";
        signalName.append(signal.signature());

        QByteArray slotName = "1intercept_";
        slotName.append(signal.signature());

        connect(target, signalName.constData(), this, slotName.constData());
    }
}

SignalInterceptor::~SignalInterceptor()
{
    delete[] meta_data;
}

void SignalInterceptor::qt_static_metacall(QObject *o, QMetaObject::Call c, int id, void **a)
{
    Q_ASSERT(o->inherits("SignalInterceptor"));
    if (c == QMetaObject::InvokeMetaMethod)
        static_cast<SignalInterceptor*>(o)->handle_metacall(id, a);
}

const QMetaObject *SignalInterceptor::metaObject() const
{
    qDebug() << "metaObject";
    return &dynamicMetaObject;
}

void *SignalInterceptor::qt_metacast(const char *clname)
{
    if (!clname)
        return 0;
    if (!strcmp(clname, "SignalInterceptor"))
        return static_cast<void*>(const_cast<SignalInterceptor*>(this));
    return QObject::qt_metacast(clname);
}

int SignalInterceptor::qt_metacall(QMetaObject::Call c, int id, void **a)
{
    id = QObject::qt_metacall(c, id, a);
    if (id < 0)
        return id;

    if (c == QMetaObject::InvokeMetaMethod)
    {
        handle_metacall(id, a);
        id -= id;
    }

    return id;
}

void SignalInterceptor::handle_metacall(int id, void **a)
{
    id += QObject::staticMetaObject.methodCount();
    QMetaMethod method = metaObject()->method(id);

    qDebug() << "qt_metacall:" << method.signature();

    QList<QByteArray> parameters = method.parameterTypes();
    for (int i = 0; i < parameters.size(); ++i)
    {
        int type = QVariant::nameToType(parameters[i]);
        if (type == QVariant::Invalid)
        {
            qDebug() << "qt_metacall: Invalid type" << parameters[i];
            return;
        }

        QVariant v = QVariant(type, a[i+1]);
        qDebug() << "qt_metacall:" << i << v;
    }
}

