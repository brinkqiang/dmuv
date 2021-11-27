#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <set>

// uvpp
#include "uvpp/loop.hpp"
#include "uvpp/timer.hpp"
#include "uvpp/async.hpp"
#include "uvpp/work.hpp"
#include "uvpp/tcp.hpp"
#include "uvpp/idle.hpp"
#include "uvpp/resolver.hpp"

#include <memory>
#include <limits>

void myerror(uvpp::error e)
{
    if (e)
        std::cout << "myerror: " << e.str() << std::endl;
}

void myread(const char* buf, ssize_t len)
{
    std::cout << "myread: " << buf << len << std::endl;
}

int main()
{
    uvpp::loop loop;
    uvpp::Resolver resolver(loop);
    resolver.resolve("0.0.0.0", [](const uvpp::error& error, bool ip4, const std::string& addr)
    {
        if (error)
        {
            std::cout << error.str() << std::endl;
            return;
        }
        std::cout << (ip4 ? "IP4" : "IP6") << ": " << addr << std::endl;
    });

    uvpp::Tcp tcp(loop);
    if (!tcp.bind("0.0.0.0", 33013))
    {
        return 0;
    }

    tcp.listen(myerror, 5);

    uvpp::Tcp client(loop);
    tcp.accept(client);

    client.read_start(myread);

    uvpp::Tcp tcp2(loop);
    if (!tcp2.connect("127.0.0.1", 33013, [](auto e) {
        if (!e)
        {
            std::cout << "connected: " << std::endl;
        }
        else
        {
            std::cout << "connected failed: " << e.str() << std::endl;
        }
    }))
    {
        std::cout << "error connect\n";
    }

    std::thread t([&loop]()
    {
        std::cout << "Thread started: " << std::this_thread::get_id() << std::endl;
        try
        {
            if (!loop.run())
            {
                std::cout << "error run\n";
            }
            std::cout << "Quit from event loop" << std::endl;
        }
        catch (...)
        {
            std::cout << "exception\n";
        }
    });

    t.join();

    return 0;
}
