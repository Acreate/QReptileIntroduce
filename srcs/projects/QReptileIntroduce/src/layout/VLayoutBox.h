#ifndef VLAYOUTBOX_H_H_HEAD__FILE__
#define VLAYOUTBOX_H_H_HEAD__FILE__
#pragma once
#include <QVBoxLayout>
class VLayoutBox : public QVBoxLayout{
	Q_OBJECT;
public:
	VLayoutBox( ) {
	}
	VLayoutBox( QWidget *parent )
	: QVBoxLayout( parent ) {
	}
	~VLayoutBox() override;
};


#endif // VLAYOUTBOX_H_H_HEAD__FILE__
