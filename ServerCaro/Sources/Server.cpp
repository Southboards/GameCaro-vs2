#include "Headers/Server.h"

Server::Server(string ipAddress, int port, MessageReceivedHandler handler)
{
    this->ipAddress = ipAddress;
    this->port = port;
    this->msgReceived = handler;

}

Server::~Server()
{
    cleanUp();
}




/*
 * @brief main run
 */
void Server::run()
{
    char buf[MAX_BUFFER_SIZE];
    SOCKET listening = createSocket();
    if (listening == INVALID_SOCKET)
    {
        return;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(listening, &master);
    cout << "Server run"  << endl;


    while (true)
    {
        fd_set copy = master;

        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

        for (int i=0;i<socketCount;i++)
        {
            SOCKET sock = copy.fd_array[i];
            if (sock == listening)
            {
                // accept new connection
                SOCKET client = accept(listening, nullptr, nullptr);

                // add new connect to list client
                FD_SET(client, &master);

                // check
                cout << "Account active: " << master.fd_count-1 << endl;

            }

            else
            {
                int bytesReceived = 0;
                ZeroMemory(buf, MAX_BUFFER_SIZE);
                bytesReceived = recv(sock, buf, MAX_BUFFER_SIZE, 0);

                if (bytesReceived <= 0)
                {
                    closesocket(sock);
                    FD_CLR(sock, &master);
                    cout << "Account active: " << master.fd_count-1 << endl;
                }
                else
                {
                    controller(master, listening, sock, buf);
                }
            }
        }
    }
}


/*
 * @brief case controller to send command to 2 client
 */
void Server::controller(fd_set master, SOCKET listening, SOCKET sock, char* buf)
{
    switch (buf[0])
    {
        case FIND_GAME_TYPE:
            cout << sock << " find game" << endl;
            findGame(master, listening, sock, buf);
            break;
        case SIZE_BOARD_TYPE:
            cout << sock << " set size" << endl;
            setupBoard(sock, buf);
            break;
        case POS_MOVE_TYPE:
            cout << sock << " move" << endl;
            playerMove(sock, buf);
            break;
        case REPLAY_TYPE:
            cout << sock << " replay" << endl;
            replayGame(sock, buf);
            break;
        default:
            break;
    }
}


/*
 * @brief case find game
 */
void Server::findGame(fd_set master, SOCKET listening, SOCKET sock, char* buf)
{
    string nameSock = string(buf);
    nameSock.replace(0, 1, "");
    Account account(sock, nameSock);
    int numPlayer;
    if (listAccount.size()==0)
    {
        numPlayer = 1;
    }
    else
    {
        numPlayer = listAccount.rbegin()->first + 1 ;
    }

    pair<int, Account> pairAccount(numPlayer, account);
    listAccount.insert(pairAccount);
    for (int i=0;i<master.fd_count;i++)
    {
        SOCKET outSock = master.fd_array[i];
        if (outSock != listening && outSock == sock)
        {
            sendMsg(outSock, string(buf));
        }
    }

    if (numPlayer!=0 && numPlayer%2==0)
    {
        sendMsg(listAccount.at(numPlayer).socket, ProtocolFunction::reponseFindGame(listAccount.at(numPlayer-1).account, BOOL_FALSE));
        sendMsg(listAccount.at(numPlayer-1).socket, ProtocolFunction::reponseFindGame(listAccount.at(numPlayer).account, BOOL_TRUE));
        InfoGame game;
        pair<int, InfoGame> pairGame(numPlayer, game);
        listGame.insert(pairGame);
    }
}


/*
 * @brief case set up  board
 */
void Server::setupBoard(SOCKET sock, char* buf)
{
    int numPlayer;
    bool checkExisted=0;
    for (auto&& acc : listAccount)
    {
        if (acc.second.socket == sock)
        {
            if (acc.first%2==0)
            {
                numPlayer = acc.first;
            }
            else
            {
                numPlayer = acc.first+1;
            }
            checkExisted=1;
            break;
        }
    }
    if (checkExisted==0)
    {
        return;
    }

    if (listGame.at(numPlayer).argWidth==0 && listGame.at(numPlayer).argHeight==0)
    {
        listGame.at(numPlayer).argWidth = buf[1];
        listGame.at(numPlayer).argHeight = buf[2];
    }
    else if (listGame.at(numPlayer).argWidth!=0 && listGame.at(numPlayer).argHeight!=0 )
    {
        listGame.at(numPlayer).argWidth = (buf[1]+listGame.at(numPlayer).argWidth)/2;
        listGame.at(numPlayer).argHeight = (buf[2]+listGame.at(numPlayer).argHeight)/2;

        sendMsg(listAccount.at(numPlayer).socket, ProtocolFunction::msgSizeBoard(listGame.at(numPlayer).argWidth, listGame.at(numPlayer).argHeight));
        sendMsg(listAccount.at(numPlayer-1).socket, ProtocolFunction::msgSizeBoard(listGame.at(numPlayer).argWidth, listGame.at(numPlayer).argHeight));

    }

}

void Server::playerMove(SOCKET sock, char* buf)
{
    uint8_t move_x = buf[1];
    uint8_t move_y = buf[2];
    bool checkExisted=0;
    int numPlayer;
    for (auto&& acc : listAccount)
    {
        if (acc.second.socket == sock)
        {
            numPlayer = acc.first;
            checkExisted=1;
            break;
        }
    }
    if (checkExisted==0)
    {
        return;
    }

    if (numPlayer%2==0)
    {
        sendMsg(listAccount.at(numPlayer-1).socket, ProtocolFunction::msgMove(move_x, move_y));
    }
    else
    {
        sendMsg(listAccount.at(numPlayer+1).socket, ProtocolFunction::msgMove(move_x, move_y));
    }
}

void Server::replayGame(SOCKET sock, char* buf)
{
    int numPlayer;
    bool checkExisted=0;
    for (auto&& acc : listAccount)
    {
        if (acc.second.socket == sock)
        {
            if (acc.first%2==0)
            {
                numPlayer = acc.first;
            }
            else
            {
                numPlayer = acc.first+1;
            }
            checkExisted=1;
            break;
        }
    }
    if (checkExisted==0)
    {
        return;
    }

    if (buf[1]==BOOL_TRUE)
    {
        listGame.at(numPlayer).replay = listGame.at(numPlayer).replay && 1;
    }
    else
    {
        listGame.at(numPlayer).replay = 0;
    }


    if (listGame.at(numPlayer).turn==0)
    {
        listGame.at(numPlayer).turn = 1;
    }

    else
    {
        if (listGame.at(numPlayer).replay == 1)
        {
            sendMsg(listAccount.at(numPlayer).socket, ProtocolFunction::msgReplay(BOOL_TRUE));
            sendMsg(listAccount.at(numPlayer-1).socket, ProtocolFunction::msgReplay(BOOL_TRUE));
        }
        else
        {
            sendMsg(listAccount.at(numPlayer).socket, ProtocolFunction::msgReplay(BOOL_FALSE));
            sendMsg(listAccount.at(numPlayer-1).socket, ProtocolFunction::msgReplay(BOOL_FALSE));
            listAccount.erase(numPlayer);
            listAccount.erase(numPlayer-1);
            listGame.erase(numPlayer);
        }

    }
}


/*
 * @brief send data
 * @param clientSocket: client received
 *        msg: msg data
 */
void Server::sendMsg(int clientSocket, string msg)
{
    send(clientSocket, msg.c_str(), msg.size()+1, 0);
}



/*
 * @brief init server
 * @return true: completed
 *         false: init fail
 */
bool Server::initServer()
{
    /* Init winsock */
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsCheck = WSAStartup(ver, &wsData);
    if(wsCheck != 0)
    {
        //cout << "Init fail !" << endl;
        return 0;
    }
    return 1;
}


/*
 * @brief clean up wsa
 */
void Server::cleanUp()
{
    WSACleanup();
}



/*
 * @brief create new socket listener
 * @return new socket
 */
SOCKET Server::createSocket()
{
    /* create socket */
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening != INVALID_SOCKET)
    {
        /* set up socket to an ip port */
        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

        int bindCheck = bind(listening, (sockaddr*)&hint, sizeof(hint));
        if (bindCheck != SOCKET_ERROR)
        {
            int listenCheck = listen(listening, SOMAXCONN);
            if (listenCheck == SOCKET_ERROR)
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
        return listening;
    }
    return -1;

}



/*
 * @brief create new socket client
 * @return new socket client
 */
SOCKET Server::waitConnection(SOCKET listening)
{
    SOCKET clientSocket = accept(listening, NULL, NULL);
    return clientSocket;
}
