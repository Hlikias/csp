/****************************************************************************
** Meta object code from reading C++ file 'tcpserver.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../NetManager/tcpserver.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcpserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN9TCPserverE_t {};
} // unnamed namespace

template <> constexpr inline auto TCPserver::qt_create_metaobjectdata<qt_meta_tag_ZN9TCPserverE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TCPserver",
        "error",
        "",
        "message",
        "servermanagerMsg",
        "QHostAddress",
        "host",
        "msg",
        "connectionRejected",
        "clientIp",
        "clientPort",
        "reason",
        "statsUpdated",
        "totalConnections",
        "uniqueIps",
        "activityUpdated",
        "onNewConnection",
        "onClientDisconnected",
        "qintptr",
        "socketDescriptor",
        "onConnectionRejected",
        "ip",
        "onConnManagerStats"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'error'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'servermanagerMsg'
        QtMocHelpers::SignalData<void(const QHostAddress &, QString)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'connectionRejected'
        QtMocHelpers::SignalData<void(QHostAddress, quint16, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 9 }, { QMetaType::UShort, 10 }, { QMetaType::QString, 11 },
        }}),
        // Signal 'statsUpdated'
        QtMocHelpers::SignalData<void(const int &, const int &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 13 }, { QMetaType::Int, 14 },
        }}),
        // Signal 'activityUpdated'
        QtMocHelpers::SignalData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onNewConnection'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClientDisconnected'
        QtMocHelpers::SlotData<void(const qintptr &)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Slot 'onConnectionRejected'
        QtMocHelpers::SlotData<void(const QHostAddress &, const QString &)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 5, 21 }, { QMetaType::QString, 11 },
        }}),
        // Slot 'onConnManagerStats'
        QtMocHelpers::SlotData<void(const int &, const int &)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 }, { QMetaType::Int, 14 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TCPserver, qt_meta_tag_ZN9TCPserverE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TCPserver::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9TCPserverE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9TCPserverE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9TCPserverE_t>.metaTypes,
    nullptr
} };

void TCPserver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TCPserver *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->error((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->servermanagerMsg((*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->connectionRejected((*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 3: _t->statsUpdated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->activityUpdated(); break;
        case 5: _t->onNewConnection(); break;
        case 6: _t->onClientDisconnected((*reinterpret_cast<std::add_pointer_t<qintptr>>(_a[1]))); break;
        case 7: _t->onConnectionRejected((*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->onConnManagerStats((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TCPserver::*)(const QString & )>(_a, &TCPserver::error, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPserver::*)(const QHostAddress & , QString )>(_a, &TCPserver::servermanagerMsg, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPserver::*)(QHostAddress , quint16 , const QString & )>(_a, &TCPserver::connectionRejected, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPserver::*)(const int & , const int & )>(_a, &TCPserver::statsUpdated, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPserver::*)()>(_a, &TCPserver::activityUpdated, 4))
            return;
    }
}

const QMetaObject *TCPserver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TCPserver::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9TCPserverE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "RunTimeMarkable"))
        return static_cast< RunTimeMarkable*>(this);
    return QObject::qt_metacast(_clname);
}

int TCPserver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void TCPserver::error(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void TCPserver::servermanagerMsg(const QHostAddress & _t1, QString _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void TCPserver::connectionRejected(QHostAddress _t1, quint16 _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3);
}

// SIGNAL 3
void TCPserver::statsUpdated(const int & _t1, const int & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void TCPserver::activityUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
