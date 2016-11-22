#include "task.h"

Task::Task(const std::string &message, TASKFLAG flag):
    m_message(message),
    m_flag(flag)
{
}


const std::string& Task::getMessage() const
{
    return m_message;
}

Task::TASKFLAG Task::getFlag() const
{
    return m_flag;
}

void Task::setIP(in_addr _ip)
{
    ip = _ip;
}

int Task::getS_fd() const
{
    return s_fd;
}

void Task::setS_fd(int _fd)
{
    s_fd = _fd;
}

std::string Task::getData() const
{
    std::string re;
    std::stringstream stream;
    stream << s_fd;
    std::string tmp = stream.str();
    if (m_flag == CONNECT) {
        re = tmp + " " + ::inet_ntoa(ip) + std::string("----->") + "CONNECT!    " + m_message;
    } else {
        if (m_flag == DISCONNECT) {
            re = tmp + " " + ::inet_ntoa(ip) + std::string("----->") + "DISCONNECT   " + m_message;;
        } else if(m_flag == TALKING){
            re = tmp + " " + ::inet_ntoa(ip) + std::string("----->Talk: ") + m_message;
        } else if(m_flag == FUNCTION) {
            re = tmp + " " + ::inet_ntoa(ip) + std::string("--->Function: ") + m_message;
        } else if (m_flag == CASTING) {
            re = tmp + " " + ::inet_ntoa(ip) + std::string("--->Casting: ") + m_message;
        } else if (m_flag == RESULTING) {
            re = m_message;
        }
    }
    return re;
}
