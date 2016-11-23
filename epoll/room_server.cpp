#include <map>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <memory>
#define random(x) (rand()%1000)

//用户出招信息
class UserCast
{
public:
    //用户id
    int id;
    //出招值
    //默认0为未出招，1-3为石头/剪子/布
    int cast;
    //是否获胜
    bool isWinner;
    //构造
    UserCast(int _id)
    {
        id = _id;
        cast = 0;
    }
    //出招
    void DoCast(int _cast)
    {
        cast = _cast;
    }
};

//房间信息
class Room
{
public:
    //房间id
    int id;

    //对战用户信息
    std::pair<UserCast*, UserCast*> userPair;

    //是否完成对战
    bool isEnded;

    //构造
    Room(int _id, int userId)
    {
        id = _id;
        UserCast* creator = new UserCast(userId);
        userPair  = std::pair<UserCast*, UserCast*>(creator, NULL);
    }

    //添加挑战者
    void AddPlayer(int userId)
    {
        UserCast* player = new UserCast(userId);
        userPair.second = player;
    }

    //用户出招
    void DoCast(int userId, int cast)
    {
        //找到用户并设置出招值
        if (userPair.first->id == userId)
            userPair.first->DoCast(cast);
        else
            userPair.second->DoCast(cast);

        //若两人都已出招，判定结果
        if (userPair.first->cast && userPair.second->cast)
            DoJudge();
    }

    //析构
    ~Room()
    {
        //释放资源
        delete userPair.first;
        if (userPair.second)
            delete userPair.second;
    }

private:
    //胜负判定
    void DoJudge()
    {
        //判定过程TODO，将胜者的isWinner设为true，将房间的isEnded设为true

        isEnded = true;
    }
};

//用户id到房间id的映射
std::map<int, int> map_userId2roomId;

//加入一人，等待匹配的房间集合
std::list<Room*> list_waitingRooms;

//已加入两人，等待出招的房间集合
std::map<int, Room*> map_playRooms;

int generateRoomId()
{
    return random(1000);
}

void DealResult(UserCast* usercast)
{

}

//用户发起战局的处理。
//参数：用户id
void ClientOpen(int userId)
{
    //1、查找用户是否已在房间
    std::map<int, int>::iterator it = map_userId2roomId.find(userId);

    //2、若已在房间，返回提示
    if (it != map_userId2roomId.end())
    {
        //提示用户已在房间中
    }

    //3、若未在房间，创建房间，将自己加入房间，添加映射关系
    int roomId = generateRoomId();//生成房间id
    Room* room = new Room(roomId, userId);
    map_userId2roomId[userId] = roomId;

    //4、将房间加入等待匹配列表
    list_waitingRooms.push_back(room);

    //5、提示用户发起对战成功，等待匹配

}

//用户加入战局的处理
//参数：用户id
void ClientJoin(int userId)
{
    //1、查找用户是否已在房间，若已在，给予提示并返回
    std::map<int, int>::iterator it = map_userId2roomId.find(userId);
    if (it == map_userId2roomId.end())
    {
        //
        return;
    }

    //2、判断是否有等待分配的房间。若没有房间，给予提示并返回
    if (list_waitingRooms.empty())
    {
        //
        return;
    }

    //3、从待分配房间队列中找到队首房间，加入
    Room* room = list_waitingRooms.front();
    room->AddPlayer(userId);

    //4、将房间从待分配集合移动至对战集合
    list_waitingRooms.pop_front();
    map_playRooms[room->id] = room;

    //5、提示用户加入成功

}

//用户出招处理
//参数：用户id，出招值
void ClientCast(int userId, int cast)
{
    //1、查找用户是否在房间中
    std::map<int, int>::iterator it = map_userId2roomId.find(userId);
    if (it == map_userId2roomId.end())
    {
        //用户为加入房间，返回
    }

    //2、查找该房间是否是等待出招房间
    std::map<int, Room*>::iterator it_room = map_playRooms.find(it->first);
    if (it_room == map_playRooms.end())
    {
        //用户未匹配到对手，返回
    }

    //3、用户出招
    it_room->second->DoCast(userId, cast);

    //4、判断是否已产生结果，若是，则处理结果
    if (it_room->second->isEnded)
    {
        //处理过程TODO，如记录胜负信息，告知用户结果等
        DealResult(it_room->second->userPair.first);
        DealResult(it_room->second->userPair.second);

        //清除房间信息、用户与房间映射信息，释放相关资源
        map_playRooms.erase(it_room);
        map_userId2roomId.erase(map_userId2roomId.find(it_room->second->userPair.first->id));
        map_userId2roomId.erase(map_userId2roomId.find(it_room->second->userPair.second->id));
        //delete it_room;

        return;
    }

    //5、提示用户出招成功，等待对方出招

}


//处理用户放弃
//参数：用户id
void ClientQuit(int userId)
{
    //1、查找用户是否在房间中，若未在房间，则提示并返回

    //2、查找用户是否在等待匹配房间中，若是，则提示用户已放弃，从list_waitingRooms中删除房间，回收资源，从map_userId2roomId中删除映射关系，返回

    //3、此时说明用户在等待出招房间中，提示用户已放弃，提示另一用户对方放弃，从map_playRooms中删除房间，回收资源，从map_userId2roomId中删除映射关系，返回
}



