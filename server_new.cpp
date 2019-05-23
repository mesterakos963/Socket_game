#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <ctime>
#define SZAVAK_DB 13

using namespace std;

bool vege = false;
int socks = -1, sockc1 = -1, sockc2 = -1;
char buffer[1024] = {0};
int szohossz;
int index = 0;
int random_szo;
int eletero = 10;
string szo;
string szavak[SZAVAK_DB];
string tippek;
string msg;
string jelenlegi;
bool talalt = false;
bool volt = false;

void minden_bezar();
void info_kuldes();
void send_to_client(int, string);
void send_to_clients(string);
void jatek(int);
void szo_general();
bool volt_mar();

int main(int argc, char** argv)
{
    int opt = 1;
    struct sockaddr_in server;
    struct sockaddr_in client1;
    struct sockaddr_in client2;
    socklen_t client1_size = sizeof(client1);
    socklen_t client2_size = sizeof(client2);

    //Socket létrehozás
    if((socks=socket(AF_INET,SOCK_STREAM,0)) == -1){
        cout << "Socket creation error" << endl;
        return -1;
    }
    cout << endl << "A szerver elindult!";
    server.sin_family = AF_INET;
    server.sin_port = htons(2222);
    server.sin_addr.s_addr = INADDR_ANY;
    setsockopt(socks,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT|SO_KEEPALIVE,&opt,sizeof(opt));

    if(bind(socks,(struct sockaddr *)&server,sizeof(server)) < 0){
        cout << "Bind error" << endl;
        return -1;
    }
    if(listen(socks,10) < 0){
        cout << "Listen error" << endl;
        return -1;
    }
    cout << " Várakozás játékosokra..." << endl;

    //Player I
    if((sockc1 = accept(socks,(struct sockaddr *)&client1,&client1_size)) == -1){
        cout << "Accept error 1" << endl;
        return -1;
    }
    cout << "Csatlakozott az első játékos (" << inet_ntoa(client1.sin_addr)
    << ":" << ntohs(client1.sin_port) << ")" << endl;

    msg = "Várakozás a másik játékosra...";
    send_to_client(sockc1,msg);

    //Player II
    if((sockc2 = accept(socks,(struct sockaddr *)&client2,&client2_size)) == -1){
        cout << "Accept error 2" << endl;
        return -1;
    }
    cout << "Csatlakozott a második játékos (" << inet_ntoa(client2.sin_addr)
    << ":" << ntohs(client2.sin_port) << ")" << endl;

    msg = "Másik játékos online. A játék hamarosan (3 mp) kezdődik!";
    send_to_clients(msg);

    sleep(3);

    //Játék kezdete
    szo_general();
    info_kuldes();
    for(int i = 0; i < eletero; i++)
    {
        tippek[i] = '|';
    }
    while(!vege){
        msg = "NOTYOU";
        send_to_client(sockc2,msg);
        jatek(1);

        if(vege) break;
        msg = "NOTYOU";
        send_to_client(sockc1,msg);
        jatek(2);
    }

    minden_bezar();
    return 0;
}
void minden_bezar()
{
    shutdown(socks,2);
    shutdown(sockc1,2);
    shutdown(sockc2,2);
    close(socks);
    close(sockc1);
    close(sockc2);
}
void send_to_client(int sockc, string msg){
    send(sockc,msg.c_str(),1024,0);
}
void send_to_clients(string msg){
    send(sockc1,msg.c_str(),1024,0);
    send(sockc2,msg.c_str(),1024,0);
}
void szo_general()
{
    srand (time(NULL));
    ifstream infile ("szavak.txt");
    while(infile >> szavak[index])
    {
        index++;
    }
    random_szo = rand() % SZAVAK_DB;
    szo = szavak[random_szo];
    szohossz = szo.length();
    for(int i = 0; i < szohossz; i++)
    {
        jelenlegi += "_";
    }
    cout << "A játékosoknak a(z) " << szo << " szót kell kitalálni." << endl;
}
void info_kuldes()
{
    msg = "Akasztófa játék.\nKészítette: Mester Ákos\n";
    msg += "Találjátok ki a szót!\n";
    msg += jelenlegi;
    msg += "\nFelváltva tippeltek, 10 életerőtök van.\nHa hibás a tipp, veszítetek egyet.\n";
    msg += "Ha elfogy az életerő, veszítetek.\n";
    msg += "Feladásért: giveup\n";
    msg += "Életerő: vigor\n";
    //msg += "Már használt tippekért: tips\n";
    msg += "Ékezet NEM használható! Helyettesítsd a megfelelő karakterrel!\n";
    send_to_clients(msg);
}
/*bool volt_mar()
{
    for(int i = 0; i < tippek.length(); i++)
    {
        if(tippek[i] == msg[0])
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}*/
void jatek(int client)
{
    int tmp,sockc,socke;
    if(client == 1){
        tmp = 2;
        sockc = sockc1;
        socke = sockc2;
    }else{
        tmp = 1;
        sockc = sockc2;
        socke = sockc1;
    }
    msg.clear();
    msg = "YOU";
    send_to_client(sockc,msg);

    recv(sockc,buffer,sizeof(buffer),0);
    msg = buffer;
    cout << "játékos " << client << ": " << msg << endl;
    if(msg.find("vigor")!=string::npos)
    {
        msg = "Életerő: ";
        msg += to_string(eletero);
        send_to_client(sockc,msg);
        return jatek(client);
    }
    /*if(msg.find("tips")!=string::npos)
    {
        if (tippek.length() == 0)
        {
            msg = "Még nincsenek hibás tippeitek.";
            send_to_client(sockc,msg);
            return jatek(client);
        }
        if (tippek.length() > 0)
        {
            msg = "Hibás tippek: ";
            send_to_client(sockc,tippek);
            return jatek(client);
        }
    }*/
    else if(msg.find("giveup")!=string::npos)
    {
        msg = "Feladtad a játékot, vesztettetek.";
        send_to_client(sockc,msg);
        msg = "A másik játékos feladta a játékot, vesztettetek.";
        send_to_client(socke,msg);
        msg = "A kitalálandó szó a(z) ";
        msg += szo;
        msg += " szó volt.\n";
        send_to_clients(msg);
        vege = true;
        minden_bezar();
    }
    //Tipp vizsgalata
    for(int i = 0; i < szo.length(); i++)
    {
        if (szo[i] == msg[0])
        {
            jelenlegi[i] = msg[0];
            talalt = true;
        }
    }
    //Rossz tipp
    if (talalt == false)
    {   
        //Volt-e már a tipp
        /*if(volt_mar())
        {
            msg = "Ez már szerepel a tippeitek között.\n";
            send_to_client(sockc, msg);
            msg.clear();
            return jatek(client);
        }
        //Nem volt
        if (!volt_mar())
        {
            eletero--;
            msg = "Helytelen tipp!\nVesztettetek 1 életerőt.";
            tippek += msg;
            send_to_client(sockc,msg);
            msg = "A társad helytelenül tippelt!\nVesztettetek 1 életerőt.";
            send_to_client(socke,msg);
        }*/
        eletero--;
        msg = "Helytelen tipp!\nVesztettetek 1 életerőt.";
        send_to_client(sockc,msg);
        msg = "A társad helytelenül tippelt!\nVesztettetek 1 életerőt.";
        send_to_client(socke,msg);
    }
    //Jó tipp
    if (talalt == true)
    {
        talalt = false;
        msg = "Helyes tipp!\n";
        send_to_client(sockc,msg);
        msg = "A társad helyesen tippelt!\n";
        send_to_client(socke,msg);
        send_to_clients(jelenlegi);
    }
    //Elfogyott életerő
    if (eletero == 0)
        {
            vege = true;
            msg = "Elfogyott az életerő, vesztettetek!\n";
            send_to_clients(msg);
        }
    //Kitalált szó
    if (szo.find(jelenlegi)!=string::npos)
    {
        msg = "Kitaláltátok a szót, nyertetek!\n";
        sleep(1);
        send_to_clients(msg);
        vege = true;
    }
}