/****************************************************************************
** Meta object code from reading C++ file 'client.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/client.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_window_t {
    QByteArrayData data[9];
    char stringdata0[114];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_window_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_window_t qt_meta_stringdata_window = {
    {
QT_MOC_LITERAL(0, 0, 6), // "window"
QT_MOC_LITERAL(1, 7, 12), // "startClicked"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 15), // "serverConnected"
QT_MOC_LITERAL(4, 37, 15), // "connectionError"
QT_MOC_LITERAL(5, 53, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(6, 82, 10), // "finishDisc"
QT_MOC_LITERAL(7, 93, 11), // "readyToRead"
QT_MOC_LITERAL(8, 105, 8) // "gotInput"

    },
    "window\0startClicked\0\0serverConnected\0"
    "connectionError\0QAbstractSocket::SocketError\0"
    "finishDisc\0readyToRead\0gotInput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_window[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x0a /* Public */,
       4,    1,   46,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,
       7,    0,   50,    2, 0x0a /* Public */,
       8,    0,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<window *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startClicked(); break;
        case 1: _t->serverConnected(); break;
        case 2: _t->connectionError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 3: _t->finishDisc(); break;
        case 4: _t->readyToRead(); break;
        case 5: _t->gotInput(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject window::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_window.data,
    qt_meta_data_window,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *window::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_window.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
struct qt_meta_stringdata_gameLoop_t {
    QByteArrayData data[3];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_gameLoop_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_gameLoop_t qt_meta_stringdata_gameLoop = {
    {
QT_MOC_LITERAL(0, 0, 8), // "gameLoop"
QT_MOC_LITERAL(1, 9, 9), // "atClkEdge"
QT_MOC_LITERAL(2, 19, 0) // ""

    },
    "gameLoop\0atClkEdge\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_gameLoop[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void gameLoop::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<gameLoop *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->atClkEdge(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject gameLoop::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_gameLoop.data,
    qt_meta_data_gameLoop,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *gameLoop::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *gameLoop::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_gameLoop.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int gameLoop::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
