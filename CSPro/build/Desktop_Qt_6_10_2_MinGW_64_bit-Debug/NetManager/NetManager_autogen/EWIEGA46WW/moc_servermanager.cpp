/****************************************************************************
** Meta object code from reading C++ file 'servermanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../NetManager/servermanager.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'servermanager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13ServerManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto ServerManager::qt_create_metaobjectdata<qt_meta_tag_ZN13ServerManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ServerManager",
        "error",
        "",
        "serverId",
        "message",
        "dataReceived",
        "ProcessedData",
        "data",
        "udpClientOffline",
        "QHostAddress",
        "ip",
        "port",
        "toWinQList",
        "msg",
        "serverStatusUpdated",
        "QList<ServerStatus>",
        "statusList",
        "onDataProcessed",
        "onServerError",
        "onWidgetMsg",
        "host",
        "updateServerStatus"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'error'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 },
        }}),
        // Signal 'dataReceived'
        QtMocHelpers::SignalData<void(const ProcessedData &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'udpClientOffline'
        QtMocHelpers::SignalData<void(const QHostAddress &, quint16)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 }, { QMetaType::UShort, 11 },
        }}),
        // Signal 'toWinQList'
        QtMocHelpers::SignalData<void(const QHostAddress &, QString)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 }, { QMetaType::QString, 13 },
        }}),
        // Signal 'serverStatusUpdated'
        QtMocHelpers::SignalData<void(const QList<ServerStatus> &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 15, 16 },
        }}),
        // Slot 'onDataProcessed'
        QtMocHelpers::SlotData<void(const ProcessedData &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Slot 'onServerError'
        QtMocHelpers::SlotData<void(const QString &)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Slot 'onWidgetMsg'
        QtMocHelpers::SlotData<void(const QHostAddress &, QString)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 9, 20 }, { QMetaType::QString, 13 },
        }}),
        // Slot 'updateServerStatus'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ServerManager, qt_meta_tag_ZN13ServerManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ServerManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13ServerManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13ServerManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13ServerManagerE_t>.metaTypes,
    nullptr
} };

void ServerManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ServerManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->error((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->dataReceived((*reinterpret_cast<std::add_pointer_t<ProcessedData>>(_a[1]))); break;
        case 2: _t->udpClientOffline((*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2]))); break;
        case 3: _t->toWinQList((*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->serverStatusUpdated((*reinterpret_cast<std::add_pointer_t<QList<ServerStatus>>>(_a[1]))); break;
        case 5: _t->onDataProcessed((*reinterpret_cast<std::add_pointer_t<ProcessedData>>(_a[1]))); break;
        case 6: _t->onServerError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onWidgetMsg((*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->updateServerStatus(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ServerManager::*)(const QString & , const QString & )>(_a, &ServerManager::error, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ServerManager::*)(const ProcessedData & )>(_a, &ServerManager::dataReceived, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ServerManager::*)(const QHostAddress & , quint16 )>(_a, &ServerManager::udpClientOffline, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ServerManager::*)(const QHostAddress & , QString )>(_a, &ServerManager::toWinQList, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ServerManager::*)(const QList<ServerStatus> & )>(_a, &ServerManager::serverStatusUpdated, 4))
            return;
    }
}

const QMetaObject *ServerManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ServerManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13ServerManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ServerManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ServerManager::error(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void ServerManager::dataReceived(const ProcessedData & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ServerManager::udpClientOffline(const QHostAddress & _t1, quint16 _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void ServerManager::toWinQList(const QHostAddress & _t1, QString _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void ServerManager::serverStatusUpdated(const QList<ServerStatus> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
