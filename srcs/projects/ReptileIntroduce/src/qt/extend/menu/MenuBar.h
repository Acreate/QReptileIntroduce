#ifndef MENUBAR_H_H_HEAD__FILE__
#define MENUBAR_H_H_HEAD__FILE__
#pragma once
#include <QMenuBar>
class MenuBar : public QMenuBar {
	Q_OBJECT;
public:
	MenuBar( QWidget *parent );
	~MenuBar( ) override;
};


#endif // MENUBAR_H_H_HEAD__FILE__
