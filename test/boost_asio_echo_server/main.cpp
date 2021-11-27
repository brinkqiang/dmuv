#include <iostream>
#include <memory>
#include <atomic>

#include "asio.hpp"

using namespace asio;

struct SocketStr
{
    std::shared_ptr<ip::tcp::socket> pSocket;
    char readBuffer[1024 * 32];
    char writeBuffer[1024 * 32];
};
using SocketPtr = std::shared_ptr<SocketStr>;

void startAccept(io_service& io, ip::tcp::acceptor& acceptor);
void startRead(SocketPtr ptr);

std::atomic<uint64_t> cnt(0);

void write(SocketPtr ptr, size_t size)
{
    ptr->pSocket->async_write_some(buffer(ptr->writeBuffer, size), [ptr](const asio::error_code& error, std::size_t size)
    {
        startRead(ptr);
    });
}

void onRead(SocketPtr ptr, const asio::error_code& error, std::size_t size)
{
    if (error)
    {
        return;
    }
    cnt += 1;
    std::copy(ptr->readBuffer, ptr->readBuffer + size, ptr->writeBuffer);
    write(ptr, size);
}

void startRead(SocketPtr ptr)
{
    auto pSocket = ptr->pSocket;
    auto buff = ptr->readBuffer;
    uint64_t size = sizeof(ptr->readBuffer);
    pSocket->async_read_some(buffer(buff, size), std::bind(&onRead, ptr, std::placeholders::_1, std::placeholders::_2));
}

void onNewConnection(io_service& io, ip::tcp::acceptor& acceptor, SocketPtr ptr, asio::error_code error)
{
    if (error)
    {
        return;
    }
    startRead(ptr);
    startAccept(io, acceptor);
}

void startAccept(io_service& io, ip::tcp::acceptor& acceptor)
{
    SocketPtr ptr = std::make_shared<SocketStr>();
    ptr->pSocket = std::make_shared<ip::tcp::socket>(io);
    acceptor.async_accept(*(ptr->pSocket), std::bind(&onNewConnection, std::ref(io), std::ref(acceptor), ptr, std::placeholders::_1));
}

//void onTimer(deadline_timer& timer, const system::error_code& ec)
//{
//    std::cout << "send data:" << (cnt >> 10) << " kbyte/s" << std::endl;
//    cnt = 0;
//    startTimer(timer);
//}

//void startTimer(deadline_timer& timer)
//{
//    timer.expires_at(timer.expires_at() + asio::posix_time::milliseconds(1000));
//    timer.async_wait(std::bind(&onTimer, std::ref(timer), std::placeholders::_1));
//}

#include "dmutil.h"
#include "dmtimermodule.h"
#include "dmsingleton.h"
#include "dmthread.h"
#include "dmconsole.h"
#include "dmtypes.h"

class CMain :
    public IDMConsoleSink,
    public IDMThread,
    public CDMThreadCtrl,
    public CDMTimerNode,
    public TSingleton<CMain> {
    friend class TSingleton<CMain>;

    typedef enum {
        eTimerID_UUID = 0,
        eTimerID_STOP,
    } ETimerID;

    typedef enum {
        eTimerTime_UUID = 1000,
        eTimerTime_STOP = 20000,
    } ETimerTime;

public:

    virtual void ThrdProc() {
        SetTimer(eTimerID_UUID, eTimerTime_UUID);

        io_service io;
        ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), 10012));
        startAccept(io, acceptor);

        std::cout << "test start" << std::endl;

        bool bBusy = false;

        while (!m_bStop) {
            bBusy = false;

            if (CDMTimerModule::Instance()->Run()) {
                bBusy = true;
            }

            if (io.poll_one()) {
                bBusy = true;
            }

            if (!bBusy) {
                SleepMs(1);
            }
        }

        std::cout << "test stop" << std::endl;
    }

    virtual void Terminate() {
        m_bStop = true;
    }

    virtual void OnCloseEvent() {
        Stop();
    }

    virtual void OnTimer(uint64_t qwIDEvent, dm::any& oAny) {
        switch (qwIDEvent) {
        case eTimerID_UUID:
        {
            std::cout << DMFormatDateTime() << " " << cnt << std::endl;
            cnt = 0;
        }
        break;
        case eTimerID_STOP:
        {
            std::cout << DMFormatDateTime() << " test stopping..." << std::endl;
            Stop();
        }
        break;

        default:
            break;
        }
    }

private:
    CMain()
        : m_bStop(false) {
        HDMConsoleMgr::Instance()->SetHandlerHook(this);
    }

    virtual ~CMain() {

    }

private:
    bool __Run() {
        return false;
    }
private:
    volatile bool   m_bStop;
};

int main(int argc, char* argv[]) {
    CMain::Instance()->Start(CMain::Instance());
    CMain::Instance()->WaitFor();
    return 0;
}
