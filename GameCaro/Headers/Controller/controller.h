#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../Controller/client.h"
#include <iostream>
#include "Windows.h"
#include <string>
#include "../View/view.h"
#include "../Model/GameModel.h"
#include "../Model/AccountList.h"
#include "../Controller/ProtocolFunction.h"


using namespace std;

/**********
 * Define
**********/

enum inputLogin
{
    LOG_IN = 1,
    REGISTER = 2,
    SHOW_RANK = 3,
    END
};

enum inputScreenMain
{
    NEW_GAME = 1,
    INFOMATION = 2,
    REPLAY_GAME = 3,
    LOG_OUT = 4
};

enum inputReplay
{
    CONTINOUS = 1,
    EXIT = 2
};

#define MOVE_WIN 4U




/**********
 * API
**********/

class Controller
{
private:
    View UI;
    Game game;
    AccountList accountList;
    Client client;


    /*
     * @brief choose login
     */
    void loginAccount();

    /*
     * @brief choose register
     */
    void registerAccount();


    /*
     * @brief choose start game
     */
    void mainCreen(const string& account);



    /*
     * @brief choose start game
     */
    void startNewGame(const string& account);

    /*
     * @brief play a game
     */
    void playeInGame(const string& account);


    /*
     * @brief check winner in last move
     * @param turn: player go last move
     */
    bool checkWin(bool turn);

    /*
     * @brief check horizontal line winner
     * @param turn: player go last move
     * @return true if have 5 sequential move in a line
     */
    bool checkHoriLine(bool turn);

    /*
     * @brief check increment line winner
     * @param turn: player go last move
     * @return true if have 5 sequential move in a line
     */
    bool checkIncreLine(bool turn);


    /*
     * @brief check decrement line winner
     * @param turn: player go last move
     * @return true if have 5 sequential move in a line
     */
    bool checkDecreLine(bool turn);


    /*
     * @brief check straight line winner
     * @param turn: player go last move
     * @return true if have 5 sequential move in a line
     */
    bool checkStraightLine(bool turn);



    /*
     * @brief check draw, fill full board
     * @return true if fill full board
     */
    bool checkDraw();







public:
    Controller();

    /*
     * @brief main program
     */
    void run();
};

#endif // CONTROLLER_H
