#include "Headers/Controller/controller.h"

Controller::Controller()
{

}

/*
 * @brief main program
 */
void Controller::run()
{
    bool run = 1;
    int command;

    while (run)
    {
        UI.screenMenuMain();
        UI.screenGetCommandMain(command);
        switch (command)
        {
            case LOG_IN:
                loginAccount();
                break;
            case REGISTER:
                registerAccount();
                break;
            case SHOW_RANK:
                UI.screenRank(accountList);
                break;
            case END:
                run = 0;
                break;
            default:
                break;
        }
    }
}

/*
 * @brief choose login
 */
void Controller::loginAccount()
{
    string account, passWord;
    UI.screenLogin(account, passWord);
    if (accountList.checkLogin(account, passWord)==0)
    {
        UI.screenLoginFail();
    }

    else
    {
        client.run();

        mainCreen(account);
    }


}

/*
 * @brief choose register
 */
void Controller::registerAccount()
{
    string account, passWord;
    UI.screenRegister(account, passWord);
    accountList.addAccount(account, passWord);
    UI.screenRegisterDone();
}


/*
 * @brief choose start game
 */
void Controller::mainCreen(const string& account)
{
    bool run = 1;
    int command;

    while (run)
    {
        UI.screenMenuStart();
        UI.screenGetCommandGame(command);
        switch (command)
        {
            case NEW_GAME:
                startNewGame(account);
                break;
            case INFOMATION:
                UI.screenInfoPlayer(account);
                break;
            case REPLAY_GAME :
                UI.screenReplayGame(account);
                break;
            case LOG_OUT:
                client.closeSocket();
                run = 0;
                break;
            default:
                break;
        }
    }
}


/*
 * @brief choose start game
 */
void Controller::startNewGame(const string& account)
{
    bool replay;
    int command;

    string feedback_command ;
    do
    {
        client.sendMsg(ProtocolFunction::msgFindGame(account));
        client.listenReponse(feedback_command);

    }while (ProtocolFunction::msgFindGame(account) != feedback_command );

    UI.waitGame();
    string responseNamePlayer2;
    client.listenReponse(responseNamePlayer2);

    responseNamePlayer2.replace(0,1,"");
    if (responseNamePlayer2[0]==BOOL_TRUE)
    {
        responseNamePlayer2.replace(0,1,"");
        game = Game(account, responseNamePlayer2);
    }
    else
    {
        responseNamePlayer2.replace(0,1,"");
        game = Game(responseNamePlayer2, account);
    }



    do
    {
        game.resetMove();
        playeInGame(account);

        UI.screenGetCommandReplay(command);

        if (command == 1)
        {
            client.sendMsg(ProtocolFunction::msgReplay(BOOL_TRUE));
        }
        else
        {
            client.sendMsg(ProtocolFunction::msgReplay(BOOL_FALSE));
        }


        string responseReplay;
        client.listenReponse(responseReplay);

        int newCommand ;

        if (responseReplay[1] == BOOL_TRUE)
        {
            newCommand = CONTINOUS;
        }
        else
        {
            newCommand = EXIT;
        }

        switch (newCommand)
        {
            case CONTINOUS:
                replay = 1;
                break;
            case EXIT:
                replay = 0;
                break;
            default:
                break;
        }

    }
    while(replay);

}



/*
 * @brief play a game
 */
void Controller::playeInGame(const string& account)
{
    bool run = 1;
    COORD move;
    bool turn = PLAYER_1;
    UI.screenInfoGame(game);
    int width, height;
    UI.screenGetSizeBoard(width, height);

    client.sendMsg(ProtocolFunction::msgSizeBoard(width, height));

    string resposeSizeBoard;
    client.listenReponse(resposeSizeBoard);

    int newWidth = resposeSizeBoard[1];
    int newHeight = resposeSizeBoard[2];
    game.setBoard(newWidth, newHeight);
    string responseMove;
    UI.setBoard(newWidth, newHeight);

    while (run)
    {
        UI.screenTurn(game, turn);
        if (game.getPlayerGame(turn).getName() == account)
        {

            move = UI.getPositionMouseClick();
            move = UI.transCoordConsoleToBoard(move);

            game.addMove(move, turn);

            UI.drawMove(turn, move);

            client.sendMsg(ProtocolFunction::msgMove(move.X, move.Y));

        }
        else
        {

            client.listenReponse(responseMove);

            move.X = responseMove[1];
            move.Y = responseMove[2];

            game.addMove(move, turn);

            UI.drawMove(turn, move);
        }

        if (checkWin(turn)==1)
        {
            UI.screenShowEndGame(game, turn);
            game.updateDataGame(turn, WIN, account);
            run = 0;
        }

        if (checkDraw()==1)
        {
            UI.screenShowEndGame();
            game.updateDataGame(turn, DRAW, account);
            run = 0;
        }

        turn = !turn;

    }

}



/*
 * @brief check winner in last move
 * @param turn: player go last move
 */
bool Controller::checkWin(bool turn)
{
    bool checkWin;
    checkWin = checkHoriLine(turn) || checkIncreLine(turn)
                || checkDecreLine(turn) || checkStraightLine(turn);
    return checkWin;
}


/*
 * @brief check horizontal line winner
 * @param turn: player go last move
 * @return true if have 5 sequential move in a line
 */
bool Controller::checkHoriLine(bool turn)
{
    int count=1;

    vector<COORD> historyMove = game.getMovePlayer(turn);

    /* check empty */
    if(historyMove.empty())
    {
        return 0;
    }

    COORD lastMove = historyMove.back();

    /* check line 0 do */

    /* duyet trai */
    for(int i=1; i<MOVE_WIN ;i++)
    {
        bool check=0;
        /* Out board */
        if ( (lastMove.X-i)<0 )
        {
            break;
        }

        for (auto t : historyMove)
        {
            if ((lastMove.X-i)==t.X && lastMove.Y==t.Y)
            {
                count++;
                check =1;
                break;
            }
        }
        if (!check)
        {
            break;
        }
    }
    /* duyet phai */
    for(int i=1; i<MOVE_WIN ;i++)
    {
        bool check=0;
        /* Out board */
        if ( (lastMove.X+i)>=UI.getBoard().getWidth() )
        {
            break;
        }

        for (auto t : historyMove)
        {
            if ((lastMove.X+i)==t.X && lastMove.Y==t.Y)
            {
                count++;
                check =1;
                break;
            }
        }
        if (!check)
        {
            break;
        }
    }

    if (count>=MOVE_WIN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*
 * @brief check increment line winner
 * @param turn: player go last move
 * @return true if have 5 sequential move in a line
 */
bool Controller::checkIncreLine(bool turn)
{
    int count=1;

    vector<COORD> historyMove = game.getMovePlayer(turn);
    /* check empty */
    if(historyMove.empty())
    {
        return 0;
    }

    COORD lastMove = historyMove.back();


    /* duyet xeo xuong */
    for(int i=1; i<MOVE_WIN ;i++)
    {
        bool check=0;
        /* Out board */
        if ( (lastMove.X-i)<0 && (lastMove.Y+i)>=UI.getBoard().getHeight() )
        {
            break;
        }

        for (auto t : historyMove)
        {
            if ( (lastMove.X-i)==t.X && (lastMove.Y+i)==t.Y )
            {
                count++;
                check =1;
                break;
            }
        }
        if (!check)
        {
            break;
        }
    }

    /* duyet xeo len */
    for(int i=1; i<MOVE_WIN ;i++)
    {
        bool check=0;
        /* Out board */
        if ( (lastMove.X+i)>=UI.getBoard().getWidth() && (lastMove.Y-i)<0 )
        {
            break;
        }

        for (auto t : historyMove)
        {
            if ( (lastMove.X+i)==t.X && (lastMove.Y-i)==t.Y )
            {
                count++;
                check =1;
                break;
            }
        }
        if (!check)
        {
            break;
        }
    }

    if (count>=MOVE_WIN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
 * @brief check decrement line winner
 * @param turn: player go last move
 * @return true if have 5 sequential move in a line
 */
bool Controller::checkDecreLine(bool turn)
{
    int count=1;

    vector<COORD> historyMove = game.getMovePlayer(turn);
    /* check empty */
    if(historyMove.empty())
    {
        return 0;
    }

    COORD lastMove = historyMove.back();


    /* duyet xeo len */
    for(int i=1; i<MOVE_WIN ;i++)
    {
        bool check=0;
        /* Out board */
        if ( (lastMove.X-i)<0 && (lastMove.Y-i)<0 )
        {
            break;
        }

        for (auto t : historyMove)
        {
            if ( (lastMove.X-i)==t.X && (lastMove.Y-i)==t.Y )
            {
                count++;
                check =1;
                break;
            }
        }
        if (!check)
        {
            break;
        }
    }

    /* duyet xeo xuong */
    for(int i=1; i<MOVE_WIN ;i++)
    {
        bool check=0;
        /* Out board */
        if ( (lastMove.X+i)>=UI.getBoard().getWidth() && (lastMove.Y+i)>=UI.getBoard().getHeight() )
        {
            break;
        }

        for (auto t : historyMove)
        {
            if ( (lastMove.X+i)==t.X && (lastMove.Y+i)==t.Y )
            {
                count++;
                check =1;
                break;
            }
        }
        if (!check)
        {
            break;
        }
    }

    if (count>=MOVE_WIN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*
 * @brief check straight line winner
 * @param turn: player go last move
 * @return true if have 5 sequential move in a line
 */
bool Controller::checkStraightLine(bool turn)
{
    int count=1;

    vector<COORD> historyMove = game.getMovePlayer(turn);
    /* check empty */
    if(historyMove.empty())
    {
        return 0;
    }

    COORD lastMove = historyMove.back();


    /* duyet len */
    for(int i=1; i<MOVE_WIN ;i++)
    {
        bool check=0;
        /* Out board */
        if ( (lastMove.Y-i)<0 )
        {
            break;
        }

        for (auto t : historyMove)
        {
            if ( (lastMove.X)==t.X && (lastMove.Y-i)==t.Y )
            {
                count++;
                check =1;
                break;
            }
        }
        if (!check)
        {
            break;
        }
    }

    /* duyet xuong */
    for(int i=1; i<MOVE_WIN ;i++)
    {
        bool check=0;
        /* Out board */
        if ( (lastMove.Y+i)>=UI.getBoard().getHeight() )
        {
            break;
        }

        for (auto t : historyMove)
        {
            if ( (lastMove.X)==t.X && (lastMove.Y+i)==t.Y )
            {
                count++;
                check = 1;
                break;
            }
        }
        if (!check)
        {
            break;
        }
    }

    if (count>=MOVE_WIN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



/*
 * @brief check draw, fill full board
 * @return true if fill full board
 */
bool Controller::checkDraw()
{
    size_t numMove = game.getMovePlayer(PLAYER_1).size();
    numMove += game.getMovePlayer(PLAYER_2).size();

    int numBoard = UI.getBoard().getWidth() * UI.getBoard().getHeight();

    if (numMove < numBoard)
    {
        return 0;
    }
    else
    {
        return 1;
    }

}




