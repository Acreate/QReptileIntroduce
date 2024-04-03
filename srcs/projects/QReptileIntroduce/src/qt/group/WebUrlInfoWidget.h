#ifndef WEBURLINFOWIDGET_H_H_HEAD__FILE__
#define WEBURLINFOWIDGET_H_H_HEAD__FILE__

#pragma once
#include <QWidget>

class NovelInfoWidget;
class QScrollArea;
class QComboBox;
class QSettings;
class EditLine;
class QLabel;
class Button;
class HLayoutBox;
class WebUrlInfoWidget : public QWidget {

	Q_OBJECT;
public: // 模型切换宏
	enum class Show_Mode {
		Inster,
		Info
	};
private: // 静态成员
	static QMap< NovelInfoWidget *, unsigned long long > pathCount;
private: // 构造类时候必须初始化
	QSettings *webPageSetting;
public:
	WebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent );
	~WebUrlInfoWidget( ) override;
private: // 小说存在的时候显示的组件
	HLayoutBox *hasNovelInfoLayout;  // 主要布局
	QLabel *urlSortIndex; // 排序
	EditLine *urlInput; // url 地址
	QComboBox *optionBoxWidget; // url 协议
	Button *addNovelAllInCountBtn; // 添加重量的计数
	Button *subNovelAllInCountBtn; // 减去重量的计数
	Button *addNovelTypeInCountBtn; // 添加类型的计数
	Button *subNovelTypeInCountBtn; // 减去类型的计数
	Button *saveBtn; // 保存配置
private: // 小说不存在的时候显示的组件
	Button *insertlNovelInfoBtn; // 插入小说信息按钮
public:
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
		QObject *object = parent( );
		if( object ) {
			auto novelInfoWidget = qobject_cast< NovelInfoWidget * >( object );
			if( novelInfoWidget && pathCount.contains( novelInfoWidget ) )
				return pathCount[ novelInfoWidget ];
		}
		return 0;
	}
	bool setSettingInstance( NovelInfoWidget *parent, QSettings *settings ) {
		if( parent && pathCount.contains( parent ) ) {
			this->webPageSetting = settings;
			return true;
		}
		return false;
	}
Q_SIGNALS:
	/// <summary>
	/// 切换状态之后信号诞生
	/// </summary>
	void toggled( Show_Mode status );
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
