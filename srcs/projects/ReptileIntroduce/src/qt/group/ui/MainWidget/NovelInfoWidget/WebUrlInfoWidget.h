#ifndef WEBURLINFOWIDGET_H_H_HEAD__FILE__
#define WEBURLINFOWIDGET_H_H_HEAD__FILE__

#pragma once
#include <QWidget>

#include "WebUrlInfoWidget/CountEditWidget.h"

#include "interface/IRequestNetInterfaceExtend.h"

class IRequestNetInterfaceExtend;
class CountEditWidget;
class Exception;
class NovelInfoWidget;
class QScrollArea;
class QComboBox;
class QSettings;
class EditLine;
class QLabel;
class Button;
class HLayoutBox;

/// <summary>
/// web 请求组件
/// IRequestNetInterface 装饰类
/// </summary>
class WebUrlInfoWidget : public QWidget, public IRequestNetInterface {

	Q_OBJECT;

private: // 静态成员
	static QMap< NovelInfoWidget *, QVector< WebUrlInfoWidget * > > pathCount;
	static QMap< WebUrlInfoWidget *, QString > webHost;
	static NovelInfoWidget *overNovelInfoWidgetPtr( QObject *converPtr );
	static NovelInfoWidget *overNovelInfoWidgetPtrTry( QObject *converPtr, Exception *tryResult );
	static void setConverError( Exception *tryResult );
private: // 构造类时候必须初始化
	QSettings *webPageSetting;
	IRequestNetInterfaceExtend *requestNetInterface;
	NovelInfoWidget *parent;
public:
	std::shared_ptr<void> getData( ) override;
	QUrl getUrl( ) override;
	QMap<QString, QUrl> getTypeUrls( const QNetworkReply &networkReply ) override;
	NovelPtrList getTypePageNovels( const QNetworkReply &networkReply, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) override;
	INovelInfoSharedPtr getUrlNovelInfo( const QNetworkReply &networkReply, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) override;
	QUrl getNext( const QNetworkReply &networkReply, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) override;
	void novelTypeEnd( const NovelPtrList &saveNovelInfos ) override;
	void endHost( const NovelPtrList &saveNovelInfos ) override;
private: // 配置文件当中的关键 key
	static const QString settingHostKey;
	static const QString settingUrlKey;
private:
	WebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent, IRequestNetInterfaceExtend *requestNetInterface, Qt::WindowFlags f = Qt::WindowFlags( ) );
public:
	static WebUrlInfoWidget *generateWebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent, IRequestNetInterfaceExtend *requestNetInterface, Qt::WindowFlags f = Qt::WindowFlags( ) );
	~WebUrlInfoWidget( ) override;
private: // 小说存在的时候显示的组件
	HLayoutBox *hasNovelInfoLayout;  // 主要布局
	Button *loadDll; // 加载 dll 按钮
	EditLine *urlInput; // url 地址
	QComboBox *optionBoxWidget; // url 协议
	CountEditWidget *allCount; // 总计
	CountEditWidget *typeCount; // 类型计数
	Button *startBtn; // 开始获取
	Button *saveBtn; // 开始获取

private: /// 组件初始化
	/// <summary>
	/// 初始化组件属性
	/// </summary>
	void initComponentPropertys( );
	/// <summary>
	/// 初始化组件文本
	/// </summary>
	void initComponentText( );
	/// <summary>
	/// 初始化（构建）组件对象
	/// </summary>
	void initComponentInstance( );
	/// <summary>
	/// 初始化槽链接
	/// </summary>
	void initComponentConnect( );
	/// <summary>
	/// 插入组件到列表当中
	/// </summary>
	void insterCompoentToLayout( );
	/// <summary>
	/// 初始化实例
	/// </summary>
	/// <param name="webPageSetting">配置文件对象指针</param>
	/// <param name="novelInfoWidget">父节点对象指针</param>
	/// <param name="requestNetInterface">请求对象接口处理接口</param>
	void initInstance( QSettings *webPageSetting, NovelInfoWidget *novelInfoWidget, IRequestNetInterfaceExtend *requestNetInterface );
public: // 属性
	long long getAllCountValue( ) {
		return allCount->getValue( );
	};
	long long getTypeCountValue( ) {
		return typeCount->getValue( );
	}
	QSettings *getWebPageSetting( ) const {
		return webPageSetting;
	}
	IRequestNetInterfaceExtend *getRequestNetInterface( ) const {
		return requestNetInterface;
	}
	NovelInfoWidget *getParent( ) const {
		return parent;
	}
	QString getHttpType( ) const;

	/// <summary>
	/// 返回基于父节点当中的个数，不存在时候，该值为 0
	/// </summary>
	/// <returns>个数</returns>
	unsigned long long getAtPathCount( ) const {
		return pathCount[ parent ].count( );
	}
	bool setSettingInstance( NovelInfoWidget *parent, QSettings *settings ) {
		if( parent && pathCount.contains( parent ) ) {
			this->webPageSetting = settings;
			return true;
		}
		return false;
	}
protected:
	void resizeEvent( QResizeEvent *event ) override;
	void computerSize( );
Q_SIGNALS:
	/// <summary>
	/// 窗口重置大小信号
	/// </summary>
	void widgetReseize( int width, int height );
	/// <summary>
	/// 开始按钮被点击
	/// </summary>
	void startBtnClick( );
	/// <summary>
	/// 插入按键
	/// </summary>
	void insterNovelInfo( );
	/// <summary>
	/// 增加总体计数
	/// </summary>
	void addNovelAllInCount( );
	/// <summary>
	/// 减去总体计数
	/// </summary>
	void subNovelAllInCount( );
	/// <summary>
	/// 增加类型计数
	/// </summary>
	void addNovelTypeInCount( );
	/// <summary>
	/// 减去类型计数
	/// </summary>
	void subNovelTypeInCount( );
	/// <summary>
	/// 保存
	/// </summary>
	void clickSaveBtn( );

	/// <summary>
	/// 加载按钮被点击
	/// </summary>
	void clickLoadBtn( );
};

#endif // WEBURLINFOWIDGET_H_H_HEAD__FILE__
