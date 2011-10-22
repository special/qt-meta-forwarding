#ifndef DYNAMICQOBJECT_H
#define DYNAMICQOBJECT_H

#include <QObject>
#include <QVector>
#include <QVariant>

/* DynamicQObject is a QObject that can create metaobject data at runtime.
 * The practical result is that you can define objects whose signals, slots,
 * and properties are determined at construction time rather than compile
 * time, such as objects that mirror the (meta-)methods of other types.
 *
 * To avoid unexpected behavior, it's intended that all metadata is stable
 * after construction (or immediately thereafter). No guarantees are made
 * about behavior if metadata is changed after the first call to metaObject().
 *
 * DO NOT attempt to use Q_OBJECT or the standard metaobject system in a
 * subclass. This is not intended to add dynamic pieces to existing classes.
 *
 * No private Qt API is used, and nothing beyond what moc's generated files
 * make use of, so this class should not be broken other than by API/ABI
 * changes in Qt (major releases).
 */
class DynamicQObject : public QObject
{
public:
    enum EncodedMetaType
    {
        EncodeSignals = 1,
        EncodeSlots = 1 << 1
    };
    Q_DECLARE_FLAGS(EncodedMetaTypes, EncodedMetaType)

    explicit DynamicQObject(const char *classname, QObject *parent = 0);

    /* Add a slot with the given signature, which should NOT be created
     * using the SLOT() macro. If the method has parameters, parameterNames
     * should be a comma-separated list of the variable names of each
     * parameter, but it will be approximated if missing. */
    void addSlot(const char *signature, const char *parameterNames = 0);

    /* Add a signal, as above. Do not use the SIGNAL() macro. */
    void addSignal(const char *signature, const char *parameterNames = 0);

    void addMethod(const char *signature, const char *parameterNames, uint flags);

    bool emitSignal(const char *signature, QVariantList parameters);
    void emitSignal(int index, QVariantList parameters);

    static QByteArray encodeObject(QObject *object, EncodedMetaTypes types);
    static DynamicQObject *createFromEncodedObject(const QByteArray &encoded);

protected:

    /* Called when meta-methods created in this class are invoked. The base class
     * implementation does nothing. */
    virtual void metaMethodCall(const QMetaMethod &method, QVariantList parameters);

public:

    /* Internal API */
    static const QMetaObjectExtraData staticMetaObjectExtraData;
    static void qt_static_metacall(QObject *, QMetaObject::Call, int, void **);

    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **);

private:
    const QByteArray classname;
    QMetaObject dynamicMetaObject;
    QByteArray string_data;
    QVector<uint> meta_data;
    bool initialized;

    void handle_metacall(int id, void **a);
};

#endif // DYNAMICQOBJECT_H
