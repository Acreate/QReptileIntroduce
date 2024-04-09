#ifndef WEBURLINFOWIDGET_H_H_HEAD__FILE__
#define WEBURLINFOWIDGET_H_H_HEAD__FILE__

#pragma once
#include <QWidget>

class IRequestNetInterface;
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
/// </summary>
class WebUrlInfoWidget : public QWidget {

	Q_OBJECT;
public: // 模型切换宏
	enum class Show_Mode {
		Inster,
		Info
	};
private: // 静态成员
	static QMap< NovelInfoWidget *, QVector< WebUrlInfoWidget * > > pathCount;
	static QMap< WebUrlInfoWidget *, QString > webHost;
	static NovelInfoWidget *overNovelInfoWidgetPtr( QObject *converPtr );
	static NovelInfoWidget *overNovelInfoWidgetPtrTry( QObject *converPtr, Exception *tryResult );
	static void setConverError( Exception *tryResult );
private: // 构造类时候必须初始化
	QSettings *webPageSetting;
	IRequestNetInterface *requestNetInterface;
	NovelInfoWidget *parent;
	Show_Mode currentMode;
private: // 配置文件当中的关键 key
	static const QString settingHostKey;
	static const QString settingUrlKey;
private:
	WebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent, IRequestNetInterface *requestNetInterface, Qt::WindowFlags f = Qt::WindowFlags( ) );
public:
	static WebUrlInfoWidget *generateWebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent, IRequestNetInterface *requestNetInterface, Qt::WindowFlags f = Qt::WindowFlags( ) );
	~WebUrlInfoWidget( ) override;
private: // 组件容器
	QList< QWidget * > *insterComponent;
	QList< QWidget * > *infoComponent;
private: // 小说存在的时候显示的组件
	HLayoutBox *hasNovelInfoLayout;  // 主要布局
	Button *loadDll; // 加载 dll 按钮
	EditLine *urlInput; // url 地址
	QComboBox *optionBoxWidget; // url 协议
	CountEditWidget *allCount; // 总计
	CountEditWidget *typeCount; // 类型计数
	Button *startBtn; // 开始获取
	Button *saveBtn; // 开始获取
private: // 小说不存在的时候显示的组件
	Button *insertlNovelInfoBtn; // 插入小说信息按钮
private: /// 组件初始化
	/// <summary>
	/// 初始化组件属性
	/// </summary>
	void initComponentPropertys( QSettings *webPageSetting, NovelInfoWidget *novelInfoWidget, IRequestNetInterface *requestNetInterface );
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
	void insterCompoentToLists( );
	/// <summary>
	/// 初始化实例
	/// </summary>
	/// <param name="webPageSetting">配置文件对象指针</param>
	/// <param name="novelInfoWidget">父节点对象指针</param>
	/// <param name="requestNetInterface">请求对象接口处理接口</param>
	void initInstance( QSettings *webPageSetting, NovelInfoWidget *novelInfoWidget, IRequestNetInterface *requestNetInterface );
public: // 属性

	QSettings *getWebPageSetting( ) const {
		return webPageSetting;
	}
	IRequestNetInterface *getRequestNetInterface( ) const {
		return requestNetInterface;
	}
	NovelInfoWidget *getParent( ) const {
		return parent;
	}
	QString getUrl( ) const;
	QString getHttpType( ) const;
	void setUrl( const QString &url );
	/// <summary>
	/// 设置显示模式
	/// </summary>
	/// <param name="show_mode">模式</param>
	void toggle( Show_Mode show_mode );

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
Q_SIGNALS:
	/// <summary>
	/// 窗口重置大小信号
	/// </summary>
	void widgetReseize( int width, int height );
	/// <summary>
	/// 保存按钮被点击
	/// </summary>
	void saveBtnClick( );
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
	void save( );
};

#endif // WEBURLINFOWIDGET_H_H_HEAD__FILE__
