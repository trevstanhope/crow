#include "crow.h"
#include "json.h"
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <sstream>

using namespace cv;

class ExampleLogHandler : public crow::ILogHandler {
    public:
        void log(std::string message, crow::LogLevel level) override {
//            cerr << "ExampleLogHandler -> " << message;
        }
};

struct ExampleMiddleware {
    std::string message;

    ExampleMiddleware() {
        message = "foo";
    }

    void setMessage(std::string newMsg)
    {
        message = newMsg;
    }

    struct context
    {
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        CROW_LOG_DEBUG << " - MESSAGE: " << message;
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {
        // no-op
    }
};

int main(int argc, char** argv ) {

    VideoCapture cap(0); // capture the video from web cam
    if ( !cap.isOpened() ) {  // if not success, exit program
         return -1;
    }

    Mat imgBGR;
    bool bSuccess = cap.read(imgBGR);
    if (bSuccess) {
        Mat imgHSV;
        cvtColor(imgBGR, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
    }
    else {
        return -1;
    }

    crow::App<ExampleMiddleware> app;

    app.get_middleware<ExampleMiddleware>().setMessage("hello");

    CROW_ROUTE(app, "/")
        .name("hello")
    ([]{
        return "Hello World!";
    });

    CROW_ROUTE(app, "/about")
    ([](){
        return "About Crow example.";
    });

    // simple json response
    // To see it in action enter {ip}:18080/json
    CROW_ROUTE(app, "/config")
    ([]{
        crow::json::wvalue x;
        x["message"] = "Hello, World!";
        return x;
    });

    CROW_ROUTE(app, "/update")
        .methods("POST"_method)
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x)
            return crow::response(400);
        int sum = x["a"].i()+x["b"].i();
        std::ostringstream os;
        os << sum;
        return crow::response{os.str()};
    });

    // ignore all log
    crow::logger::setLogLevel(crow::LogLevel::DEBUG);
    //crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

    app.port(18080)
        .multithreaded()
        .run();
}
