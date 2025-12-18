/****************************************************************************
** Meta object code from reading C++ file 'SeismicEventTable.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/SeismicEventTable.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SeismicEventTable.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_SeismicEventTable_t {
    uint offsetsAndSizes[30];
    char stringdata0[18];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[3];
    char stringdata4[4];
    char stringdata5[4];
    char stringdata6[10];
    char stringdata7[17];
    char stringdata8[13];
    char stringdata9[16];
    char stringdata10[20];
    char stringdata11[5];
    char stringdata12[13];
    char stringdata13[12];
    char stringdata14[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SeismicEventTable_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SeismicEventTable_t qt_meta_stringdata_SeismicEventTable = {
    {
        QT_MOC_LITERAL(0, 17),  // "SeismicEventTable"
        QT_MOC_LITERAL(18, 13),  // "eventSelected"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 2),  // "id"
        QT_MOC_LITERAL(36, 3),  // "lat"
        QT_MOC_LITERAL(40, 3),  // "lon"
        QT_MOC_LITERAL(44, 9),  // "magnitude"
        QT_MOC_LITERAL(54, 16),  // "onRefreshClicked"
        QT_MOC_LITERAL(71, 12),  // "onAddClicked"
        QT_MOC_LITERAL(84, 15),  // "onDeleteClicked"
        QT_MOC_LITERAL(100, 19),  // "onSearchTextChanged"
        QT_MOC_LITERAL(120, 4),  // "text"
        QT_MOC_LITERAL(125, 12),  // "onRowClicked"
        QT_MOC_LITERAL(138, 11),  // "QModelIndex"
        QT_MOC_LITERAL(150, 5)   // "index"
    },
    "SeismicEventTable",
    "eventSelected",
    "",
    "id",
    "lat",
    "lon",
    "magnitude",
    "onRefreshClicked",
    "onAddClicked",
    "onDeleteClicked",
    "onSearchTextChanged",
    "text",
    "onRowClicked",
    "QModelIndex",
    "index"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SeismicEventTable[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    4,   50,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    0,   59,    2, 0x08,    6 /* Private */,
       8,    0,   60,    2, 0x08,    7 /* Private */,
       9,    0,   61,    2, 0x08,    8 /* Private */,
      10,    1,   62,    2, 0x08,    9 /* Private */,
      12,    1,   65,    2, 0x08,   11 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double, QMetaType::Double,    3,    4,    5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, 0x80000000 | 13,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject SeismicEventTable::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SeismicEventTable.offsetsAndSizes,
    qt_meta_data_SeismicEventTable,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SeismicEventTable_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SeismicEventTable, std::true_type>,
        // method 'eventSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'onRefreshClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAddClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onRowClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>
    >,
    nullptr
} };

void SeismicEventTable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SeismicEventTable *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->eventSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[4]))); break;
        case 1: _t->onRefreshClicked(); break;
        case 2: _t->onAddClicked(); break;
        case 3: _t->onDeleteClicked(); break;
        case 4: _t->onSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onRowClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SeismicEventTable::*)(int , double , double , double );
            if (_t _q_method = &SeismicEventTable::eventSelected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *SeismicEventTable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SeismicEventTable::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SeismicEventTable.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SeismicEventTable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SeismicEventTable::eventSelected(int _t1, double _t2, double _t3, double _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
