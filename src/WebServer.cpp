#include "WebServer.h"
#include "httplib.h"
#include <iostream>

WebServer::WebServer(FileSystem& fs) : fs_(fs), running_(false) {
    svr_ = std::make_unique<httplib::Server>();
    setupRoutes();
}

WebServer::~WebServer() {
    stop();
}

void WebServer::setupRoutes() {
    svr_->set_mount_point("/", "./gui");

    svr_->Get("/api/ls", [this](const httplib::Request& req, httplib::Response& res) {
        std::string path = req.has_param("path") ? req.get_param_value("path") : "";
        std::string json = fs_.listDirectoryJson(path);
        res.set_content(json, "application/json");
    });

    svr_->Post("/api/mkdir", [this](const httplib::Request& req, httplib::Response& res) {
        std::string path = req.has_param("path") ? req.get_param_value("path") : "";
        if (fs_.makeDirectory(path)) {
            res.set_content("{\"status\": \"success\"}", "application/json");
        } else {
            res.set_content("{\"status\": \"error\"}", "application/json");
        }
    });

    svr_->Post("/api/create", [this](const httplib::Request& req, httplib::Response& res) {
        std::string path = req.has_param("path") ? req.get_param_value("path") : "";
        if (fs_.createFile(path)) {
            res.set_content("{\"status\": \"success\"}", "application/json");
        } else {
            res.set_content("{\"status\": \"error\"}", "application/json");
        }
    });

    svr_->Get("/api/read", [this](const httplib::Request& req, httplib::Response& res) {
        std::string path = req.has_param("path") ? req.get_param_value("path") : "";
        if (fs_.fileExists(path)) {
            std::string content = fs_.getFileContent(path);
            res.set_content(content, "text/plain");
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    svr_->Post("/api/write", [this](const httplib::Request& req, httplib::Response& res) {
        std::string path = req.has_param("path") ? req.get_param_value("path") : "";
        std::string content = req.body;
        
        if (!fs_.fileExists(path)) {
            fs_.createFile(path);
        }
        
        if (fs_.writeFile(path, content, false)) {
            res.set_content("{\"status\": \"success\"}", "application/json");
        } else {
            res.set_content("{\"status\": \"error\"}", "application/json");
        }
    });

    svr_->Post("/api/delete", [this](const httplib::Request& req, httplib::Response& res) {
        std::string path = req.has_param("path") ? req.get_param_value("path") : "";
        // We'll try deleteFile first, if it fails, try removeDirectory
        if (fs_.deleteFile(path) || fs_.removeDirectory(path)) {
            res.set_content("{\"status\": \"success\"}", "application/json");
        } else {
            res.set_content("{\"status\": \"error\"}", "application/json");
        }
    });
}

void WebServer::start(int port) {
    if (running_) return;
    running_ = true;
    serverThread_ = std::thread([this, port]() {
        svr_->listen("localhost", port);
    });
}

void WebServer::stop() {
    if (running_) {
        svr_->stop();
        if (serverThread_.joinable()) {
            serverThread_.join();
        }
        running_ = false;
    }
}
