#ifndef NOVELNETJOB_H_H_HEAD__FILE__
#define NOVELNETJOB_H_H_HEAD__FILE__
#pragma once
#include <QObject>

#include "../export/Plug_export.h"

class IRequestNetInterfaceExtend;
class QNetworkReply;
class RequestConnect;
class PLUG_EXPORT NovelNetJob : public QObject {

	Q_OBJECT;
private:
	QObject *interfaceObjPtr;
	IRequestNetInterfaceExtend *interfaceThisPtr;
public:
	NovelNetJob( QObject *parent, QObject *interface_obj_ptr, IRequestNetInterfaceExtend *interface_this_ptr );
	~NovelNetJob( ) override;
public: // 类的独有属性
	/// <summary>
	/// 爬虫开始工作
	/// </summary>
	/// <returns>实现链接信号操的对象指针</returns>
	void start( );
	/// <summary>
	/// 获取 url
	/// </summary>
	/// <returns>url</returns>
	QString getUrl( ) const;
private slots: // 匹配的信号
	void networkReplyFinished( RequestConnect *requestConnect );
};


#endif // NOVELNETJOB_H_H_HEAD__FILE__
