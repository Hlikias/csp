#ifndef XMLMANAGER_H
#define XMLMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QXmlStreamReader>
#include <QMutex>
#include <QDateTime>
struct userInfo
{
    QString UserID;
    QString UserName;
    uint QQNumber;
    QString PassWord;
    bool AutoLogin;
    bool RememberPassword;
    QString Theme;
    QString Language;
    QString FontSize;
    QString SaveHistory;
    QString Hotkey;
    QDateTime LastLoginTime;
};
struct serverConfig
{
    // Network
    QString host;
    int port;
    QList<int> maxConnectionsOptions;  // 存储多个可选值
    QList<int> workerThreadsOptions;
    QList<int> timeoutOptions;

    // Resource
    int maxUsers;
    int bufferSize;
    int sessionTimeout;
    int memoryLimitMB;   // 去掉单位，存数字

    // Log
    QStringList logLevelOptions;       // debug,info,warning,error
    QString logFile;
    int logMaxSize;
    int logMaxBackups;

    // Business
    QStringList allowGuestOptions;     // Guest,Administrator,Developer
    QString encryptData;               // Hash 或其他
    QString uploadPath;
    QString downloadPath;
    int messageRetentionDays;
};

struct serverParam{
    // Network
    QString m_host;
    int m_port;
    int m_maxConnectionsOptions;  // 存储多个可选值
    int m_workerThreadsOptions;
    int m_timeoutOptions;

    // Resource
    int m_maxUsers;
    int m_bufferSize;
    int m_sessionTimeout;
    int m_memoryLimitMB;   // 去掉单位，存数字

    // Log
    QString m_logLevelOptions;       // debug,info,warning,error
    QString m_logFile;
    int m_logMaxSize;
    int m_logMaxBackups;

    // Business
    QString m_allowGuestOptions;     // Guest,Administrator,Developer
    QString m_encryptData;               // Hash 或其他
    QString m_uploadPath;
    QString m_downloadPath;
    int m_messageRetentionDays;
};
class xmlmanager : public QObject
{
    Q_OBJECT // 必须添加Q_OBJECT宏，支持信号槽/元对象系统
private:
    // 私有化构造函数（继承QObject，需指定父对象）
    explicit xmlmanager(QObject *parent = nullptr);
    // 禁用拷贝/赋值（单例核心）
    xmlmanager(const xmlmanager&) = delete;
    xmlmanager& operator=(const xmlmanager&) = delete;

    // 静态成员：线程安全锁
    static QMutex m_mutex;
    // 配置存储：键值对（层级用"."分隔，小写）
    QMap<QString, QMap<QString,QVariant>> m_config_map;
    QMap<QString, userInfo> m_user_info_map;
    QMap<QString,serverConfig> m_server_info_map;

    // 核心：读取XML文件
    bool read_xml(const QString& xml_path);
    // 递归解析XML节点
    void parse_node(QXmlStreamReader& reader, const QString& parent_key = "");
    userInfo parse_user_node(QXmlStreamReader& reader, const QString& startElementName);
    serverConfig parse_server_node(QXmlStreamReader& reader, const QString& startElementName);
    // 新增：读取用户配置文件（私有方法，内部调用）
    // 读取服务器配置
    bool read_user_config(const QString& user_config_path);
    bool read_server_config(const QString& config_path);
    QList<int> parseIntList(const QString& str);

public:
    // 单例入口：返回唯一实例的引用（核心）
    static xmlmanager& getinstance();

    // 初始化配置（程序启动时调用一次）
    bool init(const QString& xml_path = "app_config.xml");
    // 新增：内部取值方法（不加锁，供init内部调用）
    QString get_string_inner(const QString& path, const QString & kind, const QString& default_val="" );
    // 通用取值接口
    QString get_string(const QString& path, const QString & kind,const QString& default_val = "");
    bool get_bool(const QString& path, const QString & kind, bool default_val = false);
    int get_int(const QString& path, const QString & kind, int default_val = 0);
    double get_double(const QString& path, const QString & kind, double default_val = 0.0);
    // 便携调用
    const QMap<QString, userInfo>& getUserInfoMap() const { return m_user_info_map; }
    const QMap<QString, serverConfig>& getServerInfoMap() const { return m_server_info_map; }


    // 便捷取值接口
    inline QString get_login_qss_path() { return get_string("Style","LoginQssPath","style.loginqsspath"); }
    inline QString get_qq_number() { return get_string("ConfigPath","UserConfigPath","user.qqnumber"); }
    inline int get_window_width() { return get_int("App","WindowWidth",0); }

    // 可选：写入配置（内存）
    void set_value(const QString& path, const QString & kind, const QVariant& value);
    bool save_user_config(const QString& user_config_path = "");
    // 可选：保存配置到XML文件
    bool save_config(const QString& xml_path = "app_config.xml");
    bool create_default_user_config(const QString& path);

signals:
    // 可选：配置变更信号（继承QObject的优势）
    void config_changed(const QString& path, const QString & kind, const QVariant& value);
};

#endif // XMLMANAGER_H
