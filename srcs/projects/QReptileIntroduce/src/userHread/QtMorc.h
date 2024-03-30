#ifndef QTMORC_H_H_HEAD__FILE__
#define QTMORC_H_H_HEAD__FILE__
#pragma once

#define QT_CONNECT( signals_obj_ptr, signals_class_name, signal_name, receive_obj_ptr, receive_class_name, receive_slots_name) QObject::connect( signals_obj_ptr, &signals_class_name::signal_name, receive_obj_ptr, &receive_class_name::receive_slots_name );
#define QT_CONNECT_AUTO_THIS( signals_obj_ptr, signals_class_name, signal_name, receive_class_name, receive_slots_name) QObject::connect( signals_obj_ptr, &signals_class_name::signal_name, this, &receive_class_name::receive_slots_name );
#define QT_CONNECT_AUTO_THIS_ALIKE_NAME( signals_obj_ptr, receive_class_name, signals_class_name, signal_name) QObject::connect( signals_obj_ptr, &signals_class_name::signal_name, this, &receive_class_name::signal_name );

#endif // QTMORC_H_H_HEAD__FILE__
