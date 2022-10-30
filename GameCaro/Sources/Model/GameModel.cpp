#include "Headers/Model/GameModel.h"

Game::Game()
{

}

Game::Game(Player player1, Player player2 )
{
    this->player1 = player1;
    this->player2 = player2; 
    setupDataPlayer(this->player1);
    setupDataPlayer(this->player2);
}



/*
 * @brief get player
 * @retrun this player
 */
Player Game::getPlayerGame(bool player)
{
    if (player==PLAYER_1)
    {
        return player1;
    }
    else
    {
        return player2;
    }
}

/*
 * @brief get history move of player
 * @param player: numberPlayer player1 or player2
 */
vector<COORD> Game::getMovePlayer(bool player)
{
    if (player==PLAYER_1)
    {
        return movePlayer1;
    }
    else
    {
        return movePlayer2;
    }
}


/*
 * @brief add 1 move for 1 player
 * @param move: move of player
 *        player: this player moved (player1 or player2)
 */
void Game::addMove(COORD move, bool player)
{
    if (player==PLAYER_1)
    {
        movePlayer1.push_back(move);
    }
    else
    {
        movePlayer2.push_back(move);
    }
}

/*
 * @brief add 1 game win and 1 lose for other player and update database
 * @param player: this player winner
 */
void Game::updateDataGame(bool player, TYPE_GAME type, string account)
{
    if (type == WIN || type == LOSE)
    {
        if (player == PLAYER_1)
        {
            player1.setNumWins(player1.getNumWins()+1);
            player2.setNumLoses(player2.getNumLoses()+1);

        }
        else
        {
            player2.setNumWins(player2.getNumWins()+1);
            player1.setNumLoses(player1.getNumLoses()+1);
        }
    }
    else
    {
        player1.setNumDraws(player1.getNumDraws()+1);
        player2.setNumDraws(player2.getNumDraws()+1);
    }

    if (player1.getName()==account)
    {
        updateDataPlayer(player1);
    }
    else
    {
        updateDataPlayer(player2);
    }

}




/*
 * @brief reset history move
 */
void Game::resetMove()
{
    movePlayer1.clear();
    movePlayer2.clear();
}


/*
 * @brief check file exist and create file data player
 * @param player: this player
 */
void Game::setupDataPlayer(Player& player)
{
    string linkPlayerFile = "D:/QT/GameCaro/Databases/" + player.getName() + ".json";

    QFile playerFile(linkPlayerFile.c_str());
    if( playerFile.open( QIODevice::ReadOnly | QIODevice::Text) == 0 )
    {
        //throw
        return;
    }
    else
    {
        QJsonParseError JsonParseError;
        QJsonDocument JsonDocument = QJsonDocument::fromJson(playerFile.readAll(), &JsonParseError);
        if (JsonParseError.error != QJsonParseError::NoError)
        {
            playerFile.close();
            // throw
        }
        QJsonObject rootObj = JsonDocument.object();

        int win = rootObj["Win"].toInt();
        int draw = rootObj["Draw"].toInt();
        int lose = rootObj["Lose"].toInt();

        player.setNumWins(win);
        player.setNumDraws(draw);
        player.setNumLoses(lose);

        playerFile.close();
    }
}


/*
 * @brief write data game into database
 * @param game: this game player play
 */
void Game::updateDataPlayer(Player player)
{

    writeDataPlayer(player);
    writeHistoryGame(player);
}

/*
 * @brief write data player into database
 * @param game: this game player play
 * @param player: this player
 */
void Game::writeDataPlayer(Player player)
{
    string linkPlayerFile = "D:/QT/GameCaro/Databases/" + player.getName() + ".json";

    QFile playerFile(linkPlayerFile.c_str());
    if( playerFile.open( QIODevice::ReadOnly | QIODevice::Text) == 0 )
    {
        //throw
        return;
    }
    else
    {
        QJsonParseError JsonParseError;
        QJsonDocument JsonDocument = QJsonDocument::fromJson(playerFile.readAll(), &JsonParseError);
        if (JsonParseError.error != QJsonParseError::NoError)
        {
            playerFile.close();
            // throw
        }
        QJsonObject rootObj = JsonDocument.object();

        rootObj["Win"] = player.getNumWins();
        rootObj["Draw"] = player.getNumDraws();
        rootObj["Lose"] = player.getNumLoses();
        rootObj["Elo"] = player.getEloPlayer();
        playerFile.close();

        QJsonObject newRootObj = rootObj;

        playerFile.open( QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        auto writeContent = QJsonDocument(newRootObj).toJson();
        playerFile.write(writeContent);

        playerFile.close();
    }

}



/*
 * @brief write history move 2 player to database
 * @param player: player to account
 */
void Game::writeHistoryGame(Player player)
{
    string linkPlayerFile = "D:/QT/GameCaro/Databases/" + player.getName() + ".json";

    QFile playerFile(linkPlayerFile.c_str());
    if( playerFile.open( QIODevice::ReadOnly | QIODevice::Text) == 0 )
    {
        //throw
        return;
    }
    else
    {
        QJsonParseError JsonParseError;
        QJsonDocument JsonDocument = QJsonDocument::fromJson(playerFile.readAll(), &JsonParseError);
        if (JsonParseError.error != QJsonParseError::NoError)
        {
            playerFile.close();
            // throw
        }
        QJsonObject rootObj = JsonDocument.object();

        QJsonObject listGame = rootObj["History Game"].toObject();
        int numHis = listGame["Number of History Game"].toInt() + 1;
        listGame["Number of History Game"] = numHis;

        QJsonObject newGame;

        newGame.insert("Date", __DATE__);

        newGame.insert("Player1", player1.getName().c_str());

        newGame.insert("Player2", player2.getName().c_str());

        int sumMove = (int)movePlayer1.size() + (int)movePlayer2.size();
        QString winner;
        if (sumMove%2==1)
        {
            winner = player1.getName().c_str();
        }
        else
        {
            winner = player2.getName().c_str();
        }
        newGame.insert("Winner", winner);

        QJsonObject boardObj;
        boardObj.insert("X", boardWidth);       // wrong
        boardObj.insert("Y", boardHeight);

        newGame.insert("Board", boardObj);

        QJsonObject numMoveObj;
        newGame.insert("Number of Move", sumMove);


        QJsonArray listMoveArray;
        int i=0;
        while ( i<(int)movePlayer1.size() || i<(int)movePlayer2.size())
        {
            if (i<(int)movePlayer1.size())
            {
                listMoveArray.push_back(movePlayer1[i].X);
                listMoveArray.push_back(movePlayer1[i].Y);
            }
            if (i<(int)movePlayer2.size())
            {
                listMoveArray.push_back(movePlayer2[i].X);
                listMoveArray.push_back(movePlayer2[i].Y);
            }
            i++;
        }

        newGame.insert("List Move", listMoveArray);


        string nameGame = "Game " + to_string(numHis);
        listGame.insert(nameGame.c_str(), newGame);

        rootObj["History Game"] = listGame;
        playerFile.close();

        QJsonObject newRootObj = rootObj;
        playerFile.open( QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        auto writeContent = QJsonDocument(newRootObj).toJson();
        playerFile.write(writeContent);

        playerFile.close();

    }
}

/*
 * @brief set board
 */
void Game::setBoard(int width, int height)
{
    boardWidth = width;
    boardHeight = height;
}

