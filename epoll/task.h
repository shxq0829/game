#ifndef TASK_H
#define TASK_H

#include <string>
#include "stdio.h"
#include <sstream>
#include <socketheads.h>

/**
 * @brief The Task class 任务类
 */
class Task
{
public:
    typedef enum {CONNECT = 0, DISCONNECT, TALKING, FUNCTION, CASTING, RESULTING} TASKFLAG;
    Task(const std::string &message, TASKFLAG flag = TALKING);
    const std::string& getMessage() const;
    TASKFLAG getFlag() const;
    void setIP(in_addr _ip);
    int getS_fd() const;

    void setS_fd(int _fd);


    std::string getData() const;


private:
    std::string m_message;
    TASKFLAG m_flag;
    in_addr ip;
    int s_fd;
};

#endif // TASK_H
