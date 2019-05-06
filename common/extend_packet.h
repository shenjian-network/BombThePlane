//Copyright 2019 Tongji
//License:GPL
//Author:zyc
//This is packet extended for BombThePlane.
#ifndef ExtendPacket
#define ExtendPacket
#include "packet_head.h"

/* 基类 */
class ExtendPacketBase{
public: 
    ExtendPacketBase();
    ExtendPacketBase(const PacketHead& ph);
    virtual ~ExtendPacketBase(){}
    virtual void get_string(char* s);
    PacketHead get_packet_head();
    virtual void set_string(const PacketHead& ph,const char* s);//包含自身的报头，和后面收到的length的字符串
private:
    PacketHead my_head; //报头共8字节
};

/* 游戏状态包 */
class ExtendPacketStatus:public ExtendPacketBase
{
public:
    ExtendPacketStatus();
    ExtendPacketStatus(const PacketHead& ph,const char* uname1,const char* uname2);
    ~ExtendPacketStatus() {}
    void get_string(char* s);
    char* get_player_one_name();
    char* get_player_two_name();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char player_one_name[33];
    char player_two_name[33];
};

/* 建立/销毁游戏包 */
class ExtendPacketBuildAndDestroy:public ExtendPacketBase
{
public:
    ExtendPacketBuildAndDestroy();
    ExtendPacketBuildAndDestroy(const PacketHead& ph,const char* sname,const char* rname);
    ~ExtendPacketBuildAndDestroy() {}
    void get_string(char* s);
    char* get_send_name();
    char* get_receive_name();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char send_name[33];
    char receive_name[33];
};

/* 正在游戏包 */
class ExtendPacketPlaying:public ExtendPacketBase
{
public:
    ExtendPacketPlaying();
    ExtendPacketPlaying(const PacketHead& ph,const unsigned short locs,const unsigned short locb);
    ~ExtendPacketPlaying() {}
    void get_string(char* s);
    unsigned short get_loc_small();
    unsigned short get_loc_big();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    unsigned short loc_small;
    unsigned short loc_big; 
};
#endif
