


/*
 * psandkill.c
 *
 *  Created on: Mar 26, 2011
 *      Author: zzh
 *
 *  20110715
 *  add kill -9
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

//#define false 0
//#define true !false

//#  ifndef TRUE
//#   define TRUE true
//#   define FALSE false
//#  endif

#define dResultPath "/tmp/zzh/psandkill"
#define dSignalLineSize 64
#define dMaxIdNum 10

#if 0
void customMsgHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch(type){
    case QtDebugMsg:
        txt = QString("%1Debug:%2").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).arg(msg);
        break;
    default:break;
    }
    QFile dplFile("debugpsandkillLog.txt");
    dplFile.open(QIODevice::ReadWrite| QIODevice::Append);
    QTextStream ts(&dplFile);
    ts<<txt<<endl;
}
#endif

void init()
{
        system("mkdir /tmp/zzh 2>/dev/null");
        system("chmod -R 777 /tmp/zzh");
}

int zzh_ps(char *name, int *id)
{
        char *cmd1 = "ps -A | grep ";
        char *cmd2 = " 1>/tmp/zzh/psandkill 2>/dev/null";
        char *cmd = NULL;
        FILE *p = NULL;
        char buf[dSignalLineSize];
        int ret = 0;

        if(name==NULL)
                return -1;
        cmd = (char *)malloc( strlen(cmd1)+strlen(cmd2)+strlen(name)+1 );
        if(cmd==NULL)
                return -1;
        memset(cmd,0,sizeof(cmd));
        strcpy(cmd,cmd1);
        strcat(cmd,name);
        strcat(cmd,cmd2);
        system(cmd);
        system("chmod -R 777 /tmp/zzh/psandkill");
        free(cmd);
        cmd = NULL;

        p = fopen(dResultPath,"r");
        if(p==NULL){
                printf("Open %s Fail!\n",dResultPath);
                return -1;
        }
        memset(buf,0,sizeof(buf));
        while(fgets(buf,dSignalLineSize,p)){
                //get id
                char temp[6];
                memset(temp,0,sizeof(temp));
                memcpy(temp,buf,5);
                id[ret] = atoi(temp);
                ret++;
                memset(buf,0,sizeof(buf));
        }
        fclose(p);
        if(ret==0 ){
                fprintf(stdout,"%s","No this process!\n");
                return -1;
        }
        if(ret>dMaxIdNum){
                fprintf(stdout,"%s","More than MaxValue Id!\n");
                return -1;
        }
        return ret;
}

void zzh_kill(int id)
{
        char *cmd = NULL;
        char *cmd1 = "kill -9 ";
        char cmd2[6];
        char *cmd3 = "\n";
        memset(cmd2,0,sizeof(cmd2));
        snprintf(cmd2,sizeof(cmd2),"%d",id);
        cmd = (char*)malloc(strlen(cmd1)+strlen(cmd2)+strlen(cmd3)+1);
        strcpy(cmd,cmd1);
        strcat(cmd,cmd2);
        strcat(cmd,cmd3);
        system(cmd);
        free(cmd);

        return;
}

int psandkill(char *name)
{
        int id[dMaxIdNum];
        int ret = 0, i = 0;
        memset(id,0,sizeof(id));
        ret =  zzh_ps(name,id);
        for(i=0;i<ret;i++){
            qDebug()<<QString("%1 process ID: %2").arg(name).arg(id[i]);
            //fprintf(stdout,"%s process ID: %d\n",name,id[i]);
            zzh_kill(id[i]);
            qDebug()<<QString("Kill process ID: %1").arg(id[i]);
            //    fprintf(stdout,"Kill process ID: %d\n",id[i]);
        }
        return true;
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int i=0;
    init();
    if(argc<=1){
        qDebug()<<"Arguments error!";
        //fprintf(stdout,"%s","Arguments error!\n");
        exit(1);
    }
    for(i=0;i<(argc-1);i++){
        psandkill(argv[i+1]);
    }
    return true;

    return a.exec();
}
