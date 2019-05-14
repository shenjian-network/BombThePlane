#ifndef XMLLOG
#define XMLLOG
#include <stdio.h>
#include "tinyxml.h"
#include <iostream>
#include <cstring>
#include <ctime>
using namespace std;

class XmlLog{
    char xmlfile[30];//xml�ļ���

public:
    XmlLog();
    XmlLog(const char* s);

    void xml_init();
    void appendlog_sql_select(const char *table_name, const char *operation, int sql_key);        //������ݿ������־
    void appendlog_sql_update(const char *table_name, const char *biaoxiang, const char *yuanzu); //������ݿ������־
    void appendlog_sql_insert(const char *table_name, int sql_key);  //������ݿ������־
    void appendlog_sql_create(const char *table_name);                                      //������ݿ������־
    void appendlog_sendText(const char *from, const int unum, char **to);
    void appendlog_sendFileInfo(const char *from, const int unum, char **to);
    void appendlog_send(const char *to, const char *packethead);               //���server����������־
    void appendlog_recv(const char *from, const char *packethead);             //���server����������־
    void appendlog_user_askfor_text(const char *username, const char *result, const int ret);       //����û���¼��־
    void appendlog_user_login(const char *username, const char *result);       //����û���¼��־
    void appendlog_user_logout(const char *username, const char *result);                                       //����û��˳���־
    void appendlog_user_add(const char *username , const char *result);                    //����½��û���־
    void appendlog_passwd_change(const char *username, const char *result);                //��������޸���־
    void appendlog_userconfig_change(const char *username, const char *result);            //����û������޸���־
    void appendlog_others(const char *data);                                                //��������¼���־
    void appendlog_onlinefile(const char *sname, const char *rname, const int bnum);
    void appendlog_onlinefilenotify(const char *sname, const char *rname,const int bnum, const char *msg);
    void appendlog_game(const char* event_name,const char *player_one_name,const char* plyer_two_name, const char* add_node,const char* add_content,bool is_add);
};

extern XmlLog xl;

#endif

