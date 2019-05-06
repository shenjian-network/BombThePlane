/****************************************************************************
** Meta object code from reading C++ file 'tcpclient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../client/tcpclient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcpclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TcpClient_t {
    QByteArrayData data[21];
    char stringdata0[365];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TcpClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TcpClient_t qt_meta_stringdata_TcpClient = {
    {
QT_MOC_LITERAL(0, 0, 9), // "TcpClient"
QT_MOC_LITERAL(1, 10, 9), // "readyRead"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 18), // "clientDisconnected"
QT_MOC_LITERAL(4, 40, 19), // "on_loginBtn_clicked"
QT_MOC_LITERAL(5, 60, 20), // "on_signupBtn_clicked"
QT_MOC_LITERAL(6, 81, 18), // "on_sendBtn_clicked"
QT_MOC_LITERAL(7, 100, 23), // "on_changePwdBtn_clicked"
QT_MOC_LITERAL(8, 124, 26), // "on_changePwdAckBtn_clicked"
QT_MOC_LITERAL(9, 151, 29), // "on_changePwdCancelBtn_clicked"
QT_MOC_LITERAL(10, 181, 31), // "on_showPwdCheckBox_stateChanged"
QT_MOC_LITERAL(11, 213, 16), // "userLabelClicked"
QT_MOC_LITERAL(12, 230, 10), // "timeUpdate"
QT_MOC_LITERAL(13, 241, 9), // "configGUI"
QT_MOC_LITERAL(14, 251, 10), // "sendConfig"
QT_MOC_LITERAL(15, 262, 3), // "cls"
QT_MOC_LITERAL(16, 266, 12), // "askForReview"
QT_MOC_LITERAL(17, 279, 24), // "on_fileDialogBtn_clicked"
QT_MOC_LITERAL(18, 304, 10), // "acceptRecv"
QT_MOC_LITERAL(19, 315, 24), // "cancelRecvFileDataActive"
QT_MOC_LITERAL(20, 340, 24) // "cancelSendFileDataActive"

    },
    "TcpClient\0readyRead\0\0clientDisconnected\0"
    "on_loginBtn_clicked\0on_signupBtn_clicked\0"
    "on_sendBtn_clicked\0on_changePwdBtn_clicked\0"
    "on_changePwdAckBtn_clicked\0"
    "on_changePwdCancelBtn_clicked\0"
    "on_showPwdCheckBox_stateChanged\0"
    "userLabelClicked\0timeUpdate\0configGUI\0"
    "sendConfig\0cls\0askForReview\0"
    "on_fileDialogBtn_clicked\0acceptRecv\0"
    "cancelRecvFileDataActive\0"
    "cancelSendFileDataActive"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TcpClient[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x08 /* Private */,
       3,    0,  110,    2, 0x08 /* Private */,
       4,    0,  111,    2, 0x08 /* Private */,
       5,    0,  112,    2, 0x08 /* Private */,
       6,    0,  113,    2, 0x08 /* Private */,
       7,    0,  114,    2, 0x08 /* Private */,
       8,    0,  115,    2, 0x08 /* Private */,
       9,    0,  116,    2, 0x08 /* Private */,
      10,    0,  117,    2, 0x08 /* Private */,
      11,    0,  118,    2, 0x08 /* Private */,
      12,    0,  119,    2, 0x08 /* Private */,
      13,    0,  120,    2, 0x08 /* Private */,
      14,    0,  121,    2, 0x08 /* Private */,
      15,    0,  122,    2, 0x08 /* Private */,
      16,    0,  123,    2, 0x08 /* Private */,
      17,    0,  124,    2, 0x08 /* Private */,
      18,    0,  125,    2, 0x08 /* Private */,
      19,    0,  126,    2, 0x08 /* Private */,
      20,    0,  127,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TcpClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TcpClient *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->readyRead(); break;
        case 1: _t->clientDisconnected(); break;
        case 2: _t->on_loginBtn_clicked(); break;
        case 3: _t->on_signupBtn_clicked(); break;
        case 4: _t->on_sendBtn_clicked(); break;
        case 5: _t->on_changePwdBtn_clicked(); break;
        case 6: _t->on_changePwdAckBtn_clicked(); break;
        case 7: _t->on_changePwdCancelBtn_clicked(); break;
        case 8: _t->on_showPwdCheckBox_stateChanged(); break;
        case 9: _t->userLabelClicked(); break;
        case 10: _t->timeUpdate(); break;
        case 11: _t->configGUI(); break;
        case 12: _t->sendConfig(); break;
        case 13: _t->cls(); break;
        case 14: _t->askForReview(); break;
        case 15: _t->on_fileDialogBtn_clicked(); break;
        case 16: _t->acceptRecv(); break;
        case 17: _t->cancelRecvFileDataActive(); break;
        case 18: _t->cancelSendFileDataActive(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject TcpClient::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_TcpClient.data,
    qt_meta_data_TcpClient,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TcpClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TcpClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TcpClient.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int TcpClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
