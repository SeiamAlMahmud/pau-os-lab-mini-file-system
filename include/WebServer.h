#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "FileSystem.h"
#include <thread>
#include <memory>
#include <atomic>

namespace httplib {
    class Server;
}

class WebServer {
public:
    WebServer(FileSystem& fs);
    ~WebServer();

    void start(int port = 8080);
    void stop();

private:
    void setupRoutes();
    
    FileSystem& fs_;
    std::unique_ptr<httplib::Server> svr_;
    std::thread serverThread_;
    std::atomic<bool> running_;
};

#endif
