// 引入 Drogon 框架的主头文件，包含所有核心功能
#include <drogon/drogon.h>

int main(int argc, char *argv[])
{
    // 设置日志级别为 kTrace，表示输出最详细的日志，方便调试
    drogon::app().setLogLevel(trantor::Logger::kTrace);

    // 如果命令行参数传入了配置文件路径，则使用该路径，否则使用默认的 "config.json"
    std::string config_path=(argc>1)?argv[1]:"config.json";
    // 加载配置文件，Drogon 通过该配置文件读取服务器相关配置（端口、线程数等）
    drogon::app().loadConfigFile(config_path);

    // 记录一条信息日志，提示服务器即将启动
    LOG_INFO << "Server is starting...";

    // 启动 Drogon HTTP 服务器，开始处理请求，进入事件循环
    drogon::app().run();
    // 程序正常退出
    return 0;
}
