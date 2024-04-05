#ifndef HLAYOUTBOX_H_H_HEAD__FILE__
#define HLAYOUTBOX_H_H_HEAD__FILE__

#pragma once
#include <QHBoxLayout>
class HLayoutBox : public QHBoxLayout {
	Q_OBJECT;
public:
	HLayoutBox( ) {
	}
	HLayoutBox( QWidget *parent )
	: QHBoxLayout( parent ) {
	}
	~HLayoutBox( ) override;
};

#endif // HLAYOUTBOX_H_H_HEAD__FILE__
