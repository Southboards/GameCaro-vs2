#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include "PlayerModel.h"
#include <iostream>
#include "windows.h"
#include "vector"
#include <string>
#include <fstream>
#include "../Controller/FileFunction.h"
#include <QtCore>
#include <QIODevice>


/**********
 * Define
**********/
#define PLAYER_1 true
#define PLAYER_2 false

enum TYPE_GAME
{
    WIN,
    DRAW,
    LOSE
};



/**********
 * API
**********/

class Game
{
private:
    Player player1;
    Player player2;
    vector<COORD> movePlayer1;
    vector<COORD> movePlayer2;
    int boardWidth;
    int boardHeight;


    /*
     * @brief check file exist and create file data player
     * @param player: this player (ref)
     */
    void setupDataPlayer(Player& player);


    /*
     * @brief write data game into database
     * @param game: this game player play
     */
    void updateDataPlayer(Player player);


    /*
     * @brief write data player into database
     * @param player: this player
     */
    void writeDataPlayer(Player player);

    /*
     * @brief write history move 2 player to database
     * @param player: player to account
     */
    void writeHistoryGame(Player player);




public:
    Game();
    Game(Player player1, Player player2);



    /*
     * @brief get player
     * @retrun this player
     */
    Player getPlayerGame(bool player);


    /*
     * @brief get history move of player
     * @param player: numberPlayer player1 or player2
     */
    vector<COORD> getMovePlayer(bool player);

    /*
     * @brief add 1 move for 1 player
     * @param move: move of player
     *        player: this player moved (player1 or player2)
     */
    void addMove(COORD move, bool player);

    /*
     * @brief add 1 game win for player and 1 lose for other player and update database
     * @param player: this player winner
     */
    void updateDataGame(bool player, TYPE_GAME type, string account);



    /*
     * @brief reset history move
     */
    void resetMove();

    /*
     * @brief set board
     */
    void setBoard(int width, int height);





};

#endif // GAMEMODEL_H
