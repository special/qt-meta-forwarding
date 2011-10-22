#include "dynamicqobject.h"
#include <QMetaMethod>
#include <QDebug>
#include <QDataStream>

const QMetaObjectExtraData DynamicQObject::staticMetaObjectExtraData = {
    0, qt_static_metacall
};

DynamicQObject::DynamicQObject(const char *cn, QObject *parent)
    : QObject(parent), classname(cn), initialized(false)
{
    /* Per Q_MOC_OUTPUT_REVISION 63 (Qt 4.8.0), the structure of
     * meta_data is:
     *    0 revision
     *    1 classname
     *    2 classinfo
     *    4 methods
     *    6 properties
     *    8 enums/sets
     *   10 constructors
     *   12 flags
     *   13 signalCount
     *
     * The meaning of those fields is documented here inline where necessary.
     * Offsets are an index into the meta_data array, which is an array of uint,
     * or into string_data, which is a char string. The double \0 after classname
     * is used as a null field where necessary. */

    string_data.append(classname);
    string_data.append("\0\0", 2);

    meta_data.resize(15);
    meta_data[0] = 6; // revision
}

void DynamicQObject::addSlot(const char *signature, const char *parameterNames)
{
    addMethod(signature, parameterNames, 0x0a);
}

void DynamicQObject::addSignal(const char *signature, const char *parameterNames)
{
    addMethod(signature, parameterNames, 0x05);
    meta_data[13]++; // signal_count
}

void DynamicQObject::addMethod(const char *isig, const char *iparam, uint flags)
{
    QByteArray signature = QMetaObject::normalizedSignature(isig);
    QByteArray parameters;

    if (!iparam)
    {
        /* Generate parameter names based on the number of parameters in the signature */
        int inparams = 0, intemplate = 0;
        int pcount = 0;
        for (int i = 0; i < signature.size(); ++i)
        {
            switch (signature[i])
            {
            case '(': inparams++; break;
            case ')': inparams--; break;
            case '<': intemplate++; break;
            case '>': intemplate--; break;
            case ',': pcount++; break;
            default: if (inparams && !pcount) pcount++; break;
            }
        }

        for (int i = 0; i < pcount; ++i)
        {
            parameters.append("param");
            parameters.append(QByteArray::number(pcount));
            parameters.append(',');
        }
        if (!parameters.isEmpty())
            parameters.chop(1);
    }
    else
        parameters = iparam;

    /* Offset of the method data in meta_data; initialize on the first method */
    int offset = meta_data[5];
    if (!offset)
        meta_data[5] = offset = 14;
    offset += (5*meta_data[4]);

    if (offset+5 >= meta_data.size())
        meta_data.resize(offset+6);

    Q_ASSERT(string_data[string_data.size()-1] == '\0');
    meta_data[offset] = string_data.size();
    string_data.append(signature);
    string_data.append('\0');

    if (!parameters.isEmpty())
    {
        meta_data[offset+1] = string_data.size();
        string_data.append(parameters);
        string_data.append('\0');
    }
    else
        meta_data[offset+1] = classname.size()+1; // known \0

    /* type and tag are always null for now */
    meta_data[offset+2] = meta_data[offset+3] = classname.size()+1;

    /* flags */
    meta_data[offset+4] = flags;

    /* method count */
    meta_data[4]++;
}

const QMetaObject *DynamicQObject::metaObject() const
{
    if (!initialized ||
        dynamicMetaObject.d.stringdata != string_data.constData() ||
        dynamicMetaObject.d.data != meta_data.constData())
    {
        if (initialized)
        {
            qWarning() << "DynamicQObject" << classname << ":" << "Metaobject format "
                       << "changed after initialization; this may cause problems.";
        }

        /* Remove constness */
        QMetaObject *m = const_cast<QMetaObject*>(&dynamicMetaObject);
        m->d.stringdata = string_data.constData();
        m->d.data = meta_data.constData();
        m->d.superdata = &QObject::staticMetaObject;
        m->d.extradata = &staticMetaObjectExtraData;
        const_cast<DynamicQObject*>(this)->initialized = true;
    }

    return &dynamicMetaObject;
}

void *DynamicQObject::qt_metacast(const char *clname)
{
    /* No metacast support, because of the weird situation (DynamicQObject
     * isn't actually in the metaobject heirarchy, and we can't cast to subclasses.
     * If you want it for your subclass, reimplement, compare the name, and static_cast.
     */
    return QObject::qt_metacast(clname);
}

void DynamicQObject::qt_static_metacall(QObject *o, QMetaObject::Call c, int id, void **a)
{
    if (c == QMetaObject::InvokeMetaMethod)
        static_cast<DynamicQObject*>(o)->handle_metacall(id, a);
}

int DynamicQObject::qt_metacall(QMetaObject::Call c, int id, void **a)
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

void DynamicQObject::handle_metacall(int id, void **a)
{
    int localid = id;
    id += QObject::staticMetaObject.methodCount();
    QMetaMethod method = metaObject()->method(id);

    if (method.methodType() == QMetaMethod::Signal)
    {
        /* Metacalls to a signal emit that signal. */
        QMetaObject::activate(this, metaObject(), localid, a);
        return;
    }

    QVariantList parameters;
    QList<QByteArray> types = method.parameterTypes();
    for (int i = 0; i < types.size(); ++i)
    {
        int type = QVariant::nameToType(types[i]);
        if (type == QVariant::Invalid)
        {
            qWarning() << "DynamicQObject" << classname << ":" << "Method "
                       << method.signature() << "parameter" << i << "of type"
                       << types[i] << "cannot be converted to QVariant. "
                       << "Register custom types with qRegisterMetaType. Method "
                       << "call ignored.";
            return;
        }

        parameters.append(QVariant(type, a[i+1]));
    }

    metaMethodCall(method, parameters);
}

void DynamicQObject::metaMethodCall(const QMetaMethod &method, QVariantList parameters)
{
    Q_UNUSED(method);
    Q_UNUSED(parameters);
}

bool DynamicQObject::emitSignal(const char *signature, QVariantList parameters)
{
    int index = metaObject()->indexOfSignal(QMetaObject::normalizedSignature(signature).constData());
    if (index < 0)
        return false;

    emitSignal(index, parameters);
    return true;
}

void DynamicQObject::emitSignal(int index, QVariantList parameters)
{
    index -= QObject::staticMetaObject.methodCount();

    void **a = new void*[parameters.size()+1];
    a[0] = 0;
    for (int i = 0; i < parameters.size(); ++i)
        a[i+1] = parameters[i].data();

    QMetaObject::activate(this, metaObject(), index, a);

    delete[] a;
}

QByteArray DynamicQObject::encodeObject(QObject *object, EncodedMetaTypes types)
{
    if (!object)
        return QByteArray();

    QByteArray re;
    QDataStream d(&re, QIODevice::WriteOnly);

    const QMetaObject *mo = object->metaObject();

    d << mo->className();

    /* Skip QObject methods */
    for (int i = QObject::staticMetaObject.methodCount(); i < mo->methodCount(); ++i)
    {
        QMetaMethod m = mo->method(i);
        if ((m.methodType() == QMetaMethod::Signal && (types & EncodeSignals)) ||
            (m.methodType() == QMetaMethod::Slot && (types & EncodeSlots)))
        {
            d << m.methodType();
            d << m.signature();

            QByteArray parameters;
            QList<QByteArray> pl = m.parameterNames();
            foreach (QByteArray p, pl) {
                parameters.append(p);
                parameters.append(',');
            }
            if (!parameters.isEmpty())
                parameters.chop(1);

            d << parameters;
        }
    }

    d << -1;

    return re;
}

DynamicQObject *DynamicQObject::createFromEncodedObject(const QByteArray &encoded)
{
    QDataStream d(encoded);

    QByteArray className;
    d >> className;

    DynamicQObject *obj = new DynamicQObject(className.constData());

    for (;;)
    {
        int type = -1;
        d >> type;
        if (type < 0)
            break;

        QByteArray signature;
        QByteArray parameters;
        d >> signature >> parameters;

        if (signature.isEmpty())
            break;

        switch (type)
        {
        case QMetaMethod::Signal:
            obj->addSignal(signature.constData(), parameters.constData());
            break;
        case QMetaMethod::Slot:
            obj->addSlot(signature.constData(), parameters.constData());
            break;
        default:
            qDebug() << "DynamicQObject: Skipping unsupported type" << type <<
                        "from encoded object of type" << className;
        }
    }

    qDebug() << "DynamicQObject: Created from encoded object of type" << className;

    for (int i = 0; i < obj->metaObject()->methodCount(); ++i)
    {
        QMetaMethod m = obj->metaObject()->method(i);
        qDebug() << "DynamicQObject:   " << i << m.methodType() << m.signature();
    }

    return obj;
}
