//Copyright 2019 Tongji
//License:GPL
//Author:zyc
//This is packet extended for BombThePlane.
#include "extend_packet.h"

/* ExtendPacketBase */
ExtendPacketBase::ExtendPacketBase():my_head(){}
ExtendPacketBase::ExtendPacketBase(const PacketHead& ph)
{
    my_head=ph;
}
void ExtendPacketBase::get_string(char* s)
{
    my_head.get_string(s);
}
PacketHead ExtendPacketBase::get_packet_head()
{
    return my_head;
}
void ExtendPacketBase::set_string(const PacketHead& ph,const char* s)
{
    my_head=ph;
}
/* ExtendPacketStatus */
ExtendPacketStatus::ExtendPacketStatus():ExtendPacketBase(){}
ExtendPacketStatus::ExtendPacketStatus(const PacketHead& ph,const char*uname1,const char* uname2):ExtendPacketBase(ph)
{
    strcpy(player_one_name,uname1);
    strcpy(player_two_name,uname2);
    player_one_name[32]=0;
    player_two_name[32]=0;
}
void ExtendPacketStatus::get_string(char* s)
{
    ExtendPacketBase::get_string(s);
    memcpy(s+8,player_one_name,32);
    memcpy(s+40,player_two_name,32);
}
char* ExtendPacketStatus::get_player_one_name()
{
    return player_one_name;
}
char* ExtendPacketStatus::get_player_two_name()
{
    return player_two_name;
}
void ExtendPacketStatus::set_string(const PacketHead& ph,const char* s)
{
    ExtendPacketBase::set_string(ph,s);
    memcpy(player_one_name,s,32);
    memcpy(player_two_name,s+32,32);
    player_one_name[32]=0;
    player_two_name[32]=0;
}  
/* ExtendPacketBuildAndDestroy */
ExtendPacketBuildAndDestroy::ExtendPacketBuildAndDestroy():ExtendPacketBase(){}
ExtendPacketBuildAndDestroy::ExtendPacketBuildAndDestroy(const PacketHead& ph,const char*sname,const char* rname):ExtendPacketBase(ph)
{
    strcpy(send_name,sname);
    strcpy(receive_name,rname);
    send_name[32]=0;
    receive_name[32]=0;
}
void ExtendPacketBuildAndDestroy::get_string(char* s)
{
    ExtendPacketBase::get_string(s);
    memcpy(s+8,send_name,32);
    memcpy(s+40,receive_name,32);
}
char* ExtendPacketBuildAndDestroy::get_send_name()
{
    return send_name;
}
char* ExtendPacketBuildAndDestroy::get_receive_name()
{
    return receive_name;
}
void ExtendPacketBuildAndDestroy::set_string(const PacketHead& ph,const char* s)
{
    ExtendPacketBase::set_string(ph,s);
    memcpy(send_name,s,32);
    memcpy(receive_name,s+32,32);
    send_name[32]=0;
    receive_name[32]=0;
}  
/* ExtendPacketPlaying */
ExtendPacketPlaying::ExtendPacketPlaying():ExtendPacketBase(){}
ExtendPacketPlaying::ExtendPacketPlaying(const PacketHead& ph,const unsigned short locs,const unsigned short locb):ExtendPacketBase(ph)
{
    loc_small=locs;
    loc_big=locb;
}
void ExtendPacketPlaying::get_string(char* s)
{
    ExtendPacketBase::get_string(s);
    unsigned short tmps_1=htons(loc_small);
    unsigned short tmps_2=htons(loc_big);
    memcpy(s+8, (char*)(&tmps_1) ,2);
    memcpy(s+10, (char*)(&tmps_2),2);
}
unsigned short ExtendPacketPlaying::get_loc_small()
{
    return loc_small;
}
unsigned short ExtendPacketPlaying::get_loc_big()
{
    return loc_big;
}
void ExtendPacketPlaying::set_string(const PacketHead& ph,const char* s)
{
    ExtendPacketBase::set_string(ph,s);
    loc_small=ntohs((*((unsigned short*)s))); 
    loc_big=ntohs((*((unsigned short*)(s+2)))); 
}  
