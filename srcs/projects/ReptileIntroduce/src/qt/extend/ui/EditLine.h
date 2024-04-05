#ifndef EDITLINE_H_H_HEAD__FILE__
#define EDITLINE_H_H_HEAD__FILE__
#pragma once
#include  <QLineEdit>
class EditLine : public QLineEdit {
	Q_OBJECT;
public:
	EditLine( ) : QLineEdit( nullptr ) {
	}
	EditLine( QWidget *parent ) : QLineEdit( parent ) {
	}
	EditLine( const QString &chars, QWidget *parent )
	: QLineEdit( chars, parent ) {
	}
	~EditLine( ) override;
protected:
	void paintEvent( QPaintEvent * ) override;
};

#endif // EDITLINE_H_H_HEAD__FILE__
