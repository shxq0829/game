#include "epoll_server.h"

#include <iostream>


//static char welcom[] = "welcom to my epoll_server";
//static char sorry[] = "Sorry! This is a simple demo,so not any function!";
//static char buf[BUFSIZ];


Epoll_server::Epoll_server(int port):_port(port),
    server_socket_fd(-1),
    _epoll(0),
    on(true),
    nameflag(false)
{


}


Epoll_server::~Epoll_server()
{
    if (isValid()) {
        ::close(server_socket_fd);
    }
    delete _epoll;

}


inline
bool Epoll_server::isValid() const
{
    return server_socket_fd > 0;
}


inline
void Epoll_server::poweroff()
{
    on = false;
}



inline
bool Epoll_server::states() const
{
    return on;
}



int Epoll_server::setNonblocking(int socket_fd)
{
    int opts;
    opts = fcntl(socket_fd, F_GETFL);
    if (opts < 0) {
        return -1;
    } else
    {
        opts = opts | O_NONBLOCK;
        if (fcntl(socket_fd, F_SETFL, opts) < 0) {

            return -1;
        }
    }
    return 0;
}


void Epoll_server::doTask(const Task &t)
{
    std::list<FDtoIP>::iterator ite = fd_IP.begin();
    std::list<FDtoIP>::iterator ite1 = fd_IP.end();
    std::list<IPtoSTR>::iterator it = ip_str.begin();
    std::list<IPtoSTR>::iterator it1 = ip_str.end();
    for (;ite != fd_IP.end();++ite) {
        if ((*ite).first != t.getS_fd()) {
            memset(&m_event, '\0', sizeof(m_event));
            m_event.events = EPOLLOUT | EPOLLET;
            Task *c = new Task(t);
            c->setS_fd((*ite).first);
            m_event.data.ptr = static_cast<void*>(c);
            _epoll->mod((*ite).first, &m_event);
        } else {
            ite1 = ite;
        }
    }
    for (; it != ip_str.end(); ++it) {
        if((*it).first == t.getS_fd()) {
            it1 = it;
        }
    }
    if (t.getFlag() == Task::DISCONNECT) {
        if (ite1 != fd_IP.end()) {
            fd_IP.erase(ite1);
            ip_str.erase(it1);      //delete user information
        }

    }

}
/**
 * @brief Epoll_server::acceptSocketEpoll 有用户接入
 * @return
 */
int Epoll_server::acceptSocketEpoll()
{
    nameflag = true;
    socklen_t len = sizeof(struct sockaddr_in);
    int connect_fd;
    while ((connect_fd = ::accept(server_socket_fd,
                                  (struct sockaddr*)(&client_addr), &len)) > 0) {
        if (setNonblocking(connect_fd) < 0) {
            ::close(connect_fd);
            continue;
        }
        m_event.data.fd = connect_fd;
        m_event.events = EPOLLIN | EPOLLET;
        if (_epoll->add(connect_fd, &m_event) < 0) {
            ::close(connect_fd);
            continue;
        } else {

            fd_IP.push_back(FDtoIP(connect_fd, client_addr.sin_addr));
            Task t("come in", Task::CONNECT);
            t.setIP(client_addr.sin_addr);
            t.setS_fd(connect_fd);
            doTask(t);
        }
    }

    if (connect_fd == -1 && errno != EAGAIN && errno != ECONNABORTED
            && errno != EPROTO && errno !=EINTR) {
        return -1;

    }
    return 0;
}


int Epoll_server::readSocketEpoll(const epoll_event &ev)
{
    int n = 0;
    int nread = 0;
    char buf[BUFSIZ]={'\0'};
    while ((nread = ::read(ev.data.fd, buf + n, BUFSIZ-1)) > 0) {
        n += nread;
    }
    if(nameflag) {
        nameflag = false;
        ip_str.push_back(IPtoSTR(ev.data.fd, buf));
        mapScore.insert(std::pair<std::string, int>(buf, 0));
        std::cout << "1:" << buf << " map:" << mapScore[buf] << std::endl;
        return 0;
    }
    if (nread == -1 && errno != EAGAIN) {
        return -1;
    }

    std::list<FDtoIP>::iterator ite = fd_IP.begin();
    for (;ite != fd_IP.end();++ite) {
        if ((*ite).first == ev.data.fd) {
            break;
        }
    }

    if (nread == 0) {
        strcpy(buf, " disconet  left ");
        Task t(buf,Task::DISCONNECT);
        t.setIP(client_addr.sin_addr);
        t.setS_fd((*ite).first);
        doTask(t);
    } else if(strcmp(buf,"info") == 0) {                    // User Information
        std::string tmp = "UserInfo:\n";
        std::list<IPtoSTR>::iterator it = ip_str.begin();
        for(; it != ip_str.end(); ++it) {
            tmp += (*it).second;
            tmp += "\t";
            std::stringstream stream;
            stream << mapScore[(*it).second];
            tmp += stream.str();
            tmp += "\n";
        }
        Task t(tmp,Task::FUNCTION);
        t.setIP(client_addr.sin_addr);
        t.setS_fd((*ite).first);
        doTask(t);
        //std::cout << tmp <<std::endl;
    } else if(strcmp(buf,"scissor")) {
        std::string tmp = "you_cast_scissor";
        Task t(tmp,Task::CASTING);
        t.setIP(client_addr.sin_addr);
        t.setS_fd((*ite).first);
        doTask(t);
        std::cout << tmp <<std::endl;
    } else {
        Task t(buf,Task::TALKING);
        t.setIP(client_addr.sin_addr);
        t.setS_fd((*ite).first);
        doTask(t);
    }

    //    Task *t = new Task(buf,Task::DISCONNECT);
    //    t->setIP((*ite).second);
    //    t->setS_fd((*ite).first);

    // m_event.data.fd = ev.data.fd;
    // m_event.events = ev.events;
    return 0;//_epoll->mod(m_event.data.fd, &m_event);
}


int Epoll_server::writeSocketEpoll(const epoll_event &ev)
{

    Task *t = static_cast<Task*>(ev.data.ptr);
    const char* buf = t->getData().data();
    int nwrite = 0, data_size = strlen(buf);
    int fd = t->getS_fd();
    int n = data_size;
    delete t;
    while (n > 0) {
        nwrite = ::write(fd, buf + data_size - n, n);
        if (nwrite < 0) {
            if (nwrite == -1 && errno != EAGAIN) {
                return -1;
            }
            break;
        }
        n -= nwrite;
    }

    memset(&m_event, '\0', sizeof(m_event));
    // m_event.events &= ~EPOLLOUT;
    m_event.events = EPOLLIN | EPOLLET;
    m_event.data.fd = fd;
    if (_epoll->mod(fd, &m_event) < 0) {
        ::close(m_event.data.fd);
        return -1;
    }
    return 0;
}

/**
 * @brief Epoll_server::bind
 * @return
 */
int Epoll_server::bind()
{
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
        return -1;
    }

    setNonblocking(server_socket_fd); //set fd nonblocking using fcntl() function

    _epoll = new Epoll();
    if (_epoll->create() < 0) {
        return -1;
    }
    memset(&m_event, '\0', sizeof(m_event));
    m_event.data.fd = server_socket_fd;
    m_event.events = EPOLLIN | EPOLLET; // edge-triggers
    _epoll->add(server_socket_fd, &m_event);

    memset(&server_addr, 0 ,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(_port);

    return ::bind(server_socket_fd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr));

}


int Epoll_server::listen()
{
    if (isValid()) {
        if (::listen(server_socket_fd, BLOCKLOG) < 0) {
            return -1;
        } else {
            int num;
            while (on) {
                num = _epoll->wait();
                for (int i = 0;i < num;++i) {
                    /**
                     * 接受连接的连接，把她加入到epoll中
                     */
                    if ((*_epoll)[i].data.fd == server_socket_fd) {
                        if (acceptSocketEpoll() < 0) {
                            break;
                        }
                        continue;

                    }

                    /**
                     * EPOLLIN event
                     */
                    if ((*_epoll)[i].events & EPOLLIN) {
                        if (readSocketEpoll((*_epoll)[i]) < 0) {
                            break;
                        }
                        continue;


                    }
                    std::cout << num << std::endl;
                    /**
                     * EPOLLOUT event
                     */
                    if ((*_epoll)[i].events & EPOLLOUT) {
                        if (writeSocketEpoll((*_epoll)[i]) < 0) {
                            break;
                        }

                    }
                }
            }
        }
    }
    return -1;
}


