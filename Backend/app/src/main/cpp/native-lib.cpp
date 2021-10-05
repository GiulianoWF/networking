#include <jni.h>
#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <unistd.h>

#include <boost/asio.hpp>

#include <server-socket/server.h>
#include "server-socket/echo-service.h"
#include "client-socket/client.h"

#include "logs/logs.h"

using boost::asio::ip::tcp;

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_backend_MainActivity_pingServer(
        JNIEnv* env,
        jobject /* this */) {

    std::string resposta = "Could not find";
    try
    {
        resposta = ping_server();
    }
    catch (std::exception e)
    {
        resposta = e.what();
    }

    return env->NewStringUTF(resposta.c_str());
}

#include <server-socket/server.cpp>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_backend_MyBackend_mainCpp(JNIEnv *env, jobject thiz) {
    std::string resposta = "V1";
    return env->NewStringUTF(resposta.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_backend_EndlessService_mainCpp(JNIEnv *env, jobject thiz, jlong pointer) {
    int * shutdown;
    shutdown = reinterpret_cast<int *>(pointer);

    std::thread server_thread([shutdown]() {
        // Way to gracefully stop the server
        while (*shutdown) {
            const int port = 9001;
            try {
                std::shared_ptr<Tcp_service_interface> echo_service = std::make_shared<Echo_reverse_service>();
                std::cout << "Server starting on port " << port << std::endl;
                Tcp_server tcp_server(echo_service, port, shutdown);
            }
            catch (std::exception &e) {
                std::cerr << "Exception: " << e.what()
                          << "\n"; //TODO cerr is redirected to null in android
            }
        }
        LOGG("Backend cpp close", std::string("Deleting shutdown variable"));
        delete shutdown;
    });
    server_thread.detach();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_backend_EndlessService_signalShutdownBackend(JNIEnv *env, jobject thiz, jlong pointer) {
    int * shutdown;
    shutdown = reinterpret_cast<int *>(pointer);
    LOGG("Backend cpp close", std::to_string((jlong)shutdown));
    *shutdown = 0;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_backend_EndlessService_initBackend(JNIEnv *env, jobject thiz) {
    int * shutdown;
    shutdown = (int *)malloc(sizeof(int));
    *shutdown = 1;

    return reinterpret_cast<jlong>(shutdown);
}