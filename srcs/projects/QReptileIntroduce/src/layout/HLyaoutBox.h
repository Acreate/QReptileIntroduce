#ifndef HLYAOUTBOX_H_H_HEAD__FILE__
#define HLYAOUTBOX_H_H_HEAD__FILE__
#pragma once
#include <QHBoxLayout>
class HLyaoutBox : public QHBoxLayout {
	Q_OBJECT;
public:
	HLyaoutBox( ) {
	}
	HLyaoutBox( QWidget *parent )
		: QHBoxLayout( parent ) {
	}
	~HLyaoutBox() override;
};

#endif // HLYAOUTBOX_H_H_HEAD__FILE__
