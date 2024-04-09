#ifndef QTMORC_H_H_HEAD__FILE__
#define QTMORC_H_H_HEAD__FILE__
#pragma once
#ifdef  QT_VERSION
/*
信号链接信号槽，它总是使用 this 对象来作为信号槽
param signals_obj_ptr :  信号产生对象
param signals_class_name :  信号发生类名
param signal_name :  信号名称（不包含类名称）
param receive_obj_ptr :  信号槽对象
param receive_class_name :  信号接受类名
param receive_slots_name :  信号槽名称（不包含类名称）
return static Connection: 返回链接操作对象
*/
#define QT_CONNECT( signals_obj_ptr, signals_class_name, signal_name, receive_obj_ptr, receive_class_name, receive_slots_name) QObject::connect( signals_obj_ptr, &signals_class_name::signal_name, receive_obj_ptr, &receive_class_name::receive_slots_name );

/*
信号链接信号槽，它总是使用 this 对象来作为信号槽
param signals_obj_ptr :  信号产生对象
param signals_class_name :  信号发生类名
param signal_name :  信号名称（不包含类名称）
param receive_class_name :  信号接受类名
param receive_slots_name :  信号槽名称（不包含类名称）
return static Connection: 返回链接操作对象
*/
#define QT_CONNECT_AUTO_THIS( signals_obj_ptr, signals_class_name, signal_name, receive_class_name, receive_slots_name) QObject::connect( signals_obj_ptr, &signals_class_name::signal_name, this, &receive_class_name::receive_slots_name );

/*
同名信号链接信号槽(信号与信号槽名称相同)，它总是使用 this 对象来作为信号槽
param signals_obj_ptr :  信号产生对象
param receive_class_name :  信号接受类名
param signals_class_name :  信号发生类名
param signal_and_slots_name :  信号与信号槽名称（不包含类名称），信号与信号槽名称相等
return static Connection: 返回链接操作对象
*/
#define QT_CONNECT_AUTO_THIS_ALIKE_NAME( signals_obj_ptr, receive_class_name, signals_class_name, signal_and_slots_name) QObject::connect( signals_obj_ptr, &signals_class_name::signal_and_slots_name, this, &receive_class_name::signal_and_slots_name );

/*
比较类名是否相等
param qt_obj_ptr :  对象指针
param qt_calss_name :  类（类名）
return bool: 相等返回 true
*/
#define QT_COMPARE_CLASS_METAOBJECT_NAME(qt_obj_ptr, qt_calss_name) ( (qt_obj_ptr)->metaObject( )->className( ) == qt_calss_name::staticMetaObject.className( ) )
#endif

#endif // QTMORC_H_H_HEAD__FILE__
