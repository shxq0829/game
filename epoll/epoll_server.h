#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H

#include <map>
#include <list>

#include "zepoll.h"
#include "socketheads.h"
#include "task.h"
#include <stdlib.h>
#include <sstream>

typedef std::pair<int, in_addr> FDtoIP;
typedef std::pair<int, std::string> FDtoSTR;
typedef enum {SCISSOR = 1, PAPER, ROCK} CASTFLAG;

/**
 * @brief The Epoll_server class 服务器
 */
class Epoll_server
{
public:
    Epoll_server(int port);
    ~Epoll_server();
    int bind();
    int listen();
    void poweroff();
    bool states() const;
private:
    enum  {BLOCKLOG = 5};

    bool isValid() const;

    int acceptSocketEpoll();
    int readSocketEpoll(const epoll_event &ev);
    int writeSocketEpoll(const epoll_event &ev);

    void doTask(const Task &t);
    int doCastMission();

    int _port;
    int server_socket_fd;
    Epoll *_epoll;
    sockaddr_in server_addr;
    sockaddr_in client_addr;
    epoll_event m_event;
    bool on;
    bool nameflag;
    int receiveflag;


    static int setNonblocking(int socket_fd);


    std::list<FDtoIP> fd_IP;
    std::list<FDtoSTR> ip_str;
    std::map<int, std::string> mapFDtoSTR;
    std::map<int, int> mapHit;
    std::map<std::string, int> mapScore;

};

#endif //EPOLL_SERVER_H
