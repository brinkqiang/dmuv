/*
    Copyright © 2017-2020, orcaer@yeah.net  All rights reserved.

    Author: orcaer@yeah.net

    Last modified: 2019-10-20

    Description: https://github.com/wlgq2/uv-cpp
*/

#include <iostream>
#include <atomic>
#include <uv11.hpp>

using namespace uv;


int main(int argc, char** args)
{
    EventLoop* loop = EventLoop::DefaultLoop();

    SocketAddr addr("0.0.0.0", 10005, SocketAddr::Ipv4);

    std::atomic<uint64_t> dataSize(0);
    std::atomic<uint64_t> datacount(0);

    uv::TcpServer server(loop);
    server.setMessageCallback([&](uv::TcpConnectionPtr ptr, const char* data, ssize_t size)
    {
        dataSize += size;
        datacount += 1;
        ptr->write(data, size, nullptr);
        ptr->write(data, size, nullptr);
    });
    //心跳超时
    //server.setTimeout(15);
    server.bindAndListen(addr);

    uv::Timer timer(loop, 1000, 1000, [&](uv::Timer* ptr)
    {
        std::cout << "send data:" << (dataSize / (1024 * 1024)) << " M/s cout: " << datacount << std::endl;
        dataSize = 0;
        datacount = 0;
    });
    timer.start();
    loop->run();
}
