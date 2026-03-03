#include "xmlmanager.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QDebug>

// 初始化静态互斥锁
QMutex xmlmanager::m_mutex;

// 私有构造函数（继承QObject，父对象为nullptr）
xmlmanager::xmlmanager(QObject *parent)
    : QObject(parent) // 调用QObject构造函数
{
    m_config_map.clear(); // 初始化配置容器
}

// 单例核心：返回唯一实例的引用（懒汉式 + 线程安全）
xmlmanager& xmlmanager::getinstance()
{
    // 局部静态变量：C++11及以上保证线程安全，且自动销毁（符合QObject生命周期）
    static xmlmanager instance(nullptr);
    return instance;
}

// 初始化配置：加载XML文件
bool xmlmanager::init(const QString& xml_path)
{
    QMutexLocker locker(&m_mutex);
    m_config_map.clear();

    // 第一步：读取主配置文件（app_config.xml）
    if (!read_xml(xml_path)) {
        qWarning() << "[xmlmanager] 主配置文件读取失败：" << xml_path;
        return false;
    }

    // 第二步：从主配置中获取用户配置文件路径
    QString user_config_path = get_string_inner("ConfigPath","UserConfigPath", "user_config.xml");
    // 第三步：读取用户配置文件
    if (!read_user_config(user_config_path)) {
        qWarning() << "[xmlmanager] 用户配置文件读取失败（非致命）：" << user_config_path;
        // 此处返回true，避免用户配置不存在导致程序退出
        return true;
    }

    QString server_config_path = get_string_inner("ConfigPath","ServerConfigPath", "server_config.xml");
    // 第三步：读取用户配置文件
    if (!read_server_config(server_config_path)) {
        qWarning() << "[xmlmanager] 用户配置文件读取失败（非致命）：" << user_config_path;
        // 此处返回true，避免用户配置不存在导致程序退出
        return true;
    }

    qInfo() << "[xmlmanager] 主配置+用户配置加载完成，共" << m_config_map.count() << "项";
    return true;
}

// 读取XML文件核心逻辑
bool xmlmanager::read_xml(const QString& xml_path)
{
    //QString path = ":/new/prefix2/config/app_config.xml";
    QFile file(xml_path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "[xmlmanager] 配置文件打开失败：" << xml_path << "错误：" << file.errorString();
        return false;
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartDocument) continue;

        // 解析根节点<AppConfig>
        if (token == QXmlStreamReader::StartElement && reader.name() == "AppConfig") {
            parse_node(reader);
        }
    }

    if (reader.hasError()) {
        qWarning() << "[xmlmanager] XML解析错误：" << reader.errorString();
        file.close();
        return false;
    }

    file.close();
    qInfo() << "[xmlmanager] 配置加载成功，共读取:" << m_config_map.count() << "项";
    return true;
}

// 递归解析XML嵌套节点
void xmlmanager::parse_node(QXmlStreamReader& reader, const QString& parent_key)
{
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        // 1. 处理开始节点（StartElement）
        if (token == QXmlStreamReader::StartElement) {
            // 当前节点名（统一转小写，避免大小写问题）
            QString current_node = reader.name().toString().toLower();

            // 情况1：parent_key为空 → 当前是一级节点（如Style/User/App）
            if (parent_key.isEmpty()) {
                // 递归解析该一级节点的子节点（二级节点），parent_key传入一级节点名
                parse_node(reader, current_node);
            }
            // 情况2：parent_key不为空 → 当前是二级节点（如LoginQssPath/QQNumber）
            else {
                // 读取二级节点的文本值
                if (reader.readNext() == QXmlStreamReader::Characters) {
                    QString value = reader.text().toString().trimmed();
                    // 存储到双层Map：外层Key=parent_key（一级节点），内层Key=current_node（二级节点）
                    m_config_map[parent_key][current_node] = value;
                    // 跳过EndElement，避免循环提前终止
                    reader.readNext();
                }
            }
        }
        // 2. 处理结束节点（EndElement）→ 返回上一级递归
        if (token == QXmlStreamReader::EndElement) {
            break;
        }
    }
}

//
userInfo xmlmanager::parse_user_node(QXmlStreamReader& reader, const QString& startElementName)
{
    userInfo info;
    // 初始化默认值
    info.UserID = 0;
    info.QQNumber = 0;
    info.AutoLogin = false;
    info.RememberPassword = false;

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString tagName = reader.name().toString();
            QString value;

            token = reader.readNext();
            if (token == QXmlStreamReader::Characters) {
                value = reader.text().toString().trimmed();
                reader.readNext(); // 跳过 EndElement
            } else if (token == QXmlStreamReader::EndElement) {
                // 空标签，value 保持为空
            }

            // 根据标签名填充结构体成员
            if (tagName == "UserID") {
                info.UserID = value;
            } else if (tagName == "UserName") {
                info.UserName = value;
            } else if (tagName == "QQNumber") {
                info.QQNumber = value.toUInt();
            } else if (tagName == "Password") {
                info.PassWord = value;
            } else if (tagName == "AutoLogin") {
                info.AutoLogin = (value.toLower() == "true" || value == "1");
            } else if (tagName == "RememberPassword") {
                info.RememberPassword = (value.toLower() == "true" || value == "1");
            } else if (tagName == "Theme") {
                info.Theme = value;
            } else if (tagName == "Language") {
                info.Language = value;
            } else if (tagName == "FontSize") {
                info.FontSize = value;
            } else if (tagName == "SaveHistory") {
                info.SaveHistory = value;
            } else if (tagName == "Hotkey") {
                info.Hotkey = value;
            } else if (tagName == "LastLoginTime") {
                if (!value.isEmpty()) {
                    info.LastLoginTime = QDateTime::fromString(value, Qt::ISODate);
                }
            }
        }
        else if (token == QXmlStreamReader::EndElement) {
            // 检查是否回到了当前用户节点的结束标签
            if (reader.name() == startElementName) {
                break;
            }
        }
    }
    return info;
}

serverConfig xmlmanager::parse_server_node(QXmlStreamReader& reader, const QString& startElementName)
{
    serverConfig cfg;
    // 初始化默认值
    cfg.host = "0.0.0.0";
    cfg.port = 8080;
    cfg.maxUsers = 500;
    cfg.bufferSize = 8192;
    cfg.sessionTimeout = 1800;
    cfg.memoryLimitMB = 1024;
    cfg.logFile = "server.log";
    cfg.logMaxSize = 10485760;
    cfg.logMaxBackups = 5;
    cfg.encryptData.clear();
    cfg.uploadPath = "./uploads";
    cfg.downloadPath = "./downloads";
    cfg.messageRetentionDays = 30;
    cfg.maxConnectionsOptions.clear();
    cfg.workerThreadsOptions.clear();
    cfg.timeoutOptions.clear();
    cfg.logLevelOptions.clear();
    cfg.allowGuestOptions.clear();

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString categoryName = reader.name().toString(); // 如 "Network", "Resource" 等

            // 进入类别节点内部，解析其子元素
            while (!reader.atEnd() && !reader.hasError()) {
                QXmlStreamReader::TokenType innerToken = reader.readNext();

                if (innerToken == QXmlStreamReader::StartElement) {
                    QString tagName = reader.name().toString(); // 如 "Host", "Port"
                    // 直接读取该元素的文本内容（自动处理内部空白和子元素）
                    QString value = reader.readElementText().trimmed();

                    // 根据标签名填充 cfg
                    if (tagName == "Host") {
                        cfg.host = value;
                    } else if (tagName == "Port") {
                        cfg.port = value.toInt();
                    } else if (tagName == "MaxConnections") {
                        cfg.maxConnectionsOptions = parseIntList(value);
                    } else if (tagName == "WorkerThreads") {
                        cfg.workerThreadsOptions = parseIntList(value);
                    } else if (tagName == "ConnectionTimeout") {
                        cfg.timeoutOptions = parseIntList(value);
                    } else if (tagName == "MaxUsers") {
                        cfg.maxUsers = value.toInt();
                    } else if (tagName == "BufferSize") {
                        cfg.bufferSize = value.toInt();
                    } else if (tagName == "SessionTimeout") {
                        cfg.sessionTimeout = value.toInt();
                    } else if (tagName == "MemoryLimitMB") {
                        cfg.memoryLimitMB = value.toInt();
                    } else if (tagName == "Level") {
                        cfg.logLevelOptions = value.split(',', Qt::SkipEmptyParts);
                    } else if (tagName == "File") {
                        cfg.logFile = value;
                    } else if (tagName == "MaxSize") {
                        cfg.logMaxSize = value.toInt();
                    } else if (tagName == "MaxBackups") {
                        cfg.logMaxBackups = value.toInt();
                    } else if (tagName == "AllowGuest") {
                        cfg.allowGuestOptions = value.split(',', Qt::SkipEmptyParts);
                    } else if (tagName == "EncryptData") {
                        cfg.encryptData = value;
                    } else if (tagName == "UploadPath") {
                        cfg.uploadPath = value;
                    } else if (tagName == "DownloadPath") {
                        cfg.downloadPath = value;
                    } else if (tagName == "MessageRetentionDays") {
                        cfg.messageRetentionDays = value.toInt();
                    }
                } else if (innerToken == QXmlStreamReader::EndElement) {
                    if (reader.name() == categoryName) {
                        break; // 当前类别节点解析完毕
                    }
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            if (reader.name() == startElementName) {
                break; // ServerConfig 节点解析完毕
            }
        }
    }
    return cfg;
}

// 获取字符串配置
QString xmlmanager::get_string(const QString& path, const QString & kind, const QString& default_val)
{
    QMutexLocker locker(&m_mutex);
    QString path_lower = path.toLower();
    QString kind_lower = kind.toLower();
    if (m_config_map.contains(path_lower)) {
        const QMap<QString, QVariant>& innerMap = m_config_map[path_lower];
        if (innerMap.contains(kind_lower))
        {
            return innerMap[kind_lower].toString();
        }
    }
    qWarning() << "[xmlmanager] 配置项不存在：path=" << path << ", kind=" << kind
               << "使用默认值：" << default_val;
    return default_val;
}

// 获取布尔配置
bool xmlmanager::get_bool(const QString& path, const QString & kind, bool default_val)
{
    QMutexLocker locker(&m_mutex);
    QString path_lower = path.toLower();
    QString kind_lower = kind.toLower();

    if (m_config_map.contains(path_lower)) {
        const QMap<QString, QVariant>& innerMap = m_config_map[path_lower];
        if (innerMap.contains(kind_lower))
        {
            QString val = innerMap[kind_lower].toString().toLower();
            return (val == "true" || val == "1" || val == "yes");
        }
    }
    qWarning() << "[xmlmanager] 配置项不存在：path=" << path << ", kind=" << kind
               << "使用默认值：" << default_val;
    return default_val;
}

// 获取整数配置
int xmlmanager::get_int(const QString& path, const QString & kind, int default_val)
{
    QMutexLocker locker(&m_mutex);
    QString path_lower = path.toLower();
    QString kind_lower = kind.toLower();

    if (m_config_map.contains(path_lower)) {
        const QMap<QString, QVariant>& innerMap = m_config_map[path_lower];
        if (innerMap.contains(kind_lower)) {
            bool ok;
            int val = innerMap[kind_lower].toString().toDouble(&ok);
            if (ok) return val;
        }


    }
    qWarning() << "[xmlmanager] 配置项错误：path=" << path << ", kind=" << kind
               << "，使用默认值：" << default_val;
    return default_val;
}

// 获取浮点数配置
double xmlmanager::get_double(const QString& path, const QString & kind, double default_val)
{
    QMutexLocker locker(&m_mutex);

    QString path_lower = path.toLower();
    QString kind_lower = kind.toLower();

    if (m_config_map.contains(path_lower)) {
        const QMap<QString, QVariant>& innerMap = m_config_map[path_lower];
        if (innerMap.contains(kind_lower)) {
            bool ok;
            double val = innerMap[kind_lower].toString().toDouble(&ok);
            if (ok) return val;
        }
    }

    qWarning() << "[xmlmanager] 配置项错误：path=" << path << ", kind=" << kind
               << "，使用默认值：" << default_val;
    return default_val;
}

// 设置配置值（内存中）
void xmlmanager::set_value(const QString& kind, const QString& path, const QVariant& value)
{
    QMutexLocker locker(&m_mutex);
    QString kind_lower = kind.toLower();
    QString path_lower = path.toLower();

    // 获取或创建内层 QMap
    QMap<QString, QVariant>& innerMap = m_config_map[kind_lower]; // operator[] 会自动创建空内层 map
    innerMap.insert(path_lower, value);

    // 发送配置变更信号（可根据需要调整信号参数）
    emit config_changed(kind_lower, path_lower, value);
}

// 保存配置到XML文件
bool xmlmanager::save_config(const QString& xml_path)
{
    QMutexLocker locker(&m_mutex);
    QFile file(xml_path);
    if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
        qWarning() << "[xmlmanager] 配置保存失败：" << xml_path << "错误：" << file.errorString();
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("AppConfig");

    // 遍历外层 map：kind -> 内层 map
    for (auto outerIt = m_config_map.constBegin(); outerIt != m_config_map.constEnd(); ++outerIt) {
        const QString& kind = outerIt.key();                 // 外层键，如 "style"
        const QMap<QString, QVariant>& innerMap = outerIt.value();

        // 如果内层 map 为空，可以跳过（或写入空标签）
        if (innerMap.isEmpty())
            continue;

        writer.writeStartElement(kind);   // 写入 <kind>

        // 遍历内层 map：path -> value
        for (auto innerIt = innerMap.constBegin(); innerIt != innerMap.constEnd(); ++innerIt) {
            const QString& path = innerIt.key();
            const QVariant& value = innerIt.value();

            // 写入 <path>value</path>
            writer.writeTextElement(path, value.toString());
        }

        writer.writeEndElement();          // 结束 </kind>
    }

    writer.writeEndElement();   // </AppConfig>
    writer.writeEndDocument();

    file.close();
    qInfo() << "[xmlmanager] 配置保存成功：" << xml_path;
    return true;
}

//
bool xmlmanager::read_user_config(const QString& user_config_path)
{
    if (!QFile::exists(user_config_path)) {
        if (!create_default_user_config(user_config_path)) {
            qWarning() << "[xmlmanager] 创建默认用户配置文件失败";
            return false;
        }
        qInfo() << "[xmlmanager] 已创建默认用户配置文件:" << user_config_path;
    }

    QFile file(user_config_path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "[xmlmanager] 用户配置文件打开失败：" << user_config_path << file.errorString();
        return false;
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartDocument) continue;

        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "UserConfig") {
                // 进入 <UserConfig>，开始解析其下的各个用户节点
                while (!reader.atEnd() && !reader.hasError()) {
                    token = reader.readNext();
                    if (token == QXmlStreamReader::StartElement) {
                        // 当前节点名，如 "User1"
                        QString userNodeName = reader.name().toString();
                        // 解析该用户节点下的所有子元素
                        userInfo info = parse_user_node(reader, userNodeName);
                        // 以用户名为键存入 map
                        if (!info.UserName.isEmpty()) {
                            m_user_info_map[info.UserID] = info;
                        } else {
                            qWarning() << "[xmlmanager] 用户用户名为空，忽略该用户";
                        }
                    } else if (token == QXmlStreamReader::EndElement) {
                        if (reader.name() == "UserConfig") {
                            break;
                        }
                    }
                }
            }
        }
    }

    if (reader.hasError()) {
        qWarning() << "[xmlmanager] 用户配置文件解析错误：" << reader.errorString();
        file.close();
        return false;
    }

    file.close();
    qInfo() << "[xmlmanager] 用户配置加载成功，共" << m_user_info_map.count() << "个用户";
    return true;
}

//
bool xmlmanager::read_server_config(const QString& config_path)
{
    QFile file(config_path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "[xmlmanager] 服务器配置文件打开失败：" << config_path << file.errorString();
        return false;
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartDocument) continue;

        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "ServerConfig") {
                // 解析 ServerConfig 节点，获取配置对象
                serverConfig cfg = parse_server_node(reader, "ServerConfig");
                // 存入 map，此处使用固定键 "main"（可根据需求扩展）
                m_server_info_map["main"] = cfg;
                break; // 只处理第一个 ServerConfig 节点，解析完即可退出
            }
        }
    }

    if (reader.hasError()) {
        qWarning() << "[xmlmanager] 服务器配置文件解析错误：" << reader.errorString();
        file.close();
        return false;
    }

    file.close();
    qInfo() << "[xmlmanager] 服务器配置加载成功";
    return true;
}

// 新增：创建默认用户配置文件
bool xmlmanager::create_default_user_config(const QString& path)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
        qWarning() << "[xmlmanager] 无法创建默认用户配置文件:" << file.errorString();
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("UserConfig");

    // 写入默认用户 User1
    writer.writeStartElement("User1");
    writer.writeTextElement("UserID", "1");
    writer.writeTextElement("UserName", "默认用户");
    writer.writeTextElement("QQNumber", "123");
    writer.writeTextElement("Password", "123");
    writer.writeTextElement("AutoLogin", "false");
    writer.writeTextElement("RememberPassword", "false");
    writer.writeTextElement("Theme", "default");
    writer.writeTextElement("Language", "zh_CN");
    writer.writeTextElement("FontSize", "12");
    writer.writeTextElement("SaveHistory", "true");
    writer.writeTextElement("Hotkey", "Ctrl+Alt+Z");
    writer.writeTextElement("LastLoginTime", "");
    writer.writeEndElement(); // </User1>

    // 如需更多默认用户，可在此添加 <User2> 等

    writer.writeEndElement(); // </UserConfig>
    writer.writeEndDocument();

    file.close();
    qInfo() << "[xmlmanager] 默认用户配置文件已创建:" << path;
    return true;
}

// 内部取值方法（不加锁，仅init内部调用）
QString xmlmanager::get_string_inner(const QString& path, const QString & kind, const QString& default_val)
{
    QString path_lower = path.toLower();
    QString kind_lower = kind.toLower();
    if (m_config_map.contains(path_lower)) {
        const QMap<QString, QVariant>& innerMap = m_config_map[path_lower];
        if (innerMap.contains(kind_lower))
        {
            return innerMap[kind_lower].toString();
        }
    }
    qWarning() << "[xmlmanager] 配置项不存在：path=" << path << ", kind=" << kind
               << "使用默认值：" << default_val;
    return default_val;
}

QList<int> xmlmanager::parseIntList(const QString& str)
{
    QList<int> list;
    for (const QString& s : str.split(',', Qt::SkipEmptyParts)) {
        bool ok;
        int val = s.toInt(&ok);
        if (ok) list.append(val);
    }
    return list;
}
