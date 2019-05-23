#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv)
{
    int socks = -1, opt = 1;
    struct sockaddr_in server;
    char buffer[1024] = {0};
    string msg;

    //----------------------------- SOCKET KÉSZÍTÉSE --------------------------------------//
    if((socks=socket(AF_INET,SOCK_STREAM,0)) == -1){
        cout << "Socket létrehozás sikertelen" << endl;
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(2222);
    if(argc==1) server.sin_addr.s_addr = INADDR_ANY;
    else server.sin_addr.s_addr = inet_addr(argv[1]);
    //server.sin_addr.s_addr = INADDR_ANY;
    setsockopt(socks,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT|SO_KEEPALIVE,&opt,sizeof(opt));

    //----------------------------- CSATLAKOZÁS --------------------------------------//
    cout << endl << "Csatlakozás a szerverhez... ";
    if(connect(socks,(struct sockaddr *)&server,sizeof(server)) < 0){
        cout << "Csatlakozási hiba" << endl;
        return -1;
    }
    cout << "Sikeres csatlakozás!" << endl;

    //----------------------------- JÁTÉK --------------------------------------//
    while(recv(socks,buffer,1024,0)>0)
    {
        msg = buffer;
        if(msg == "YOU"){
            cout << "Te tippelsz: ";
            getline(cin,msg);
            send(socks,msg.c_str(),1024,0);
        }
        else if(msg == "NOTYOU"){
            cout << "A másik játékos tippel. Várj!" << endl;
        }
        else cout << msg << endl;
    }

    shutdown(socks,2);
    close(socks);
    return 0;
}