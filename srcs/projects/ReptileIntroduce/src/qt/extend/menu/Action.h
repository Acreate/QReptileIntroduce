#ifndef ACTION_H_H_HEAD__FILE__
#define ACTION_H_H_HEAD__FILE__
#pragma once
#include <QAction>
class Action : public QAction {
	Q_OBJECT;
public:
	Action( QObject *parent = nullptr );
	~Action( ) override;

};

#endif // ACTION_H_H_HEAD__FILE__
