#include "Headers/Model/AccountList.h"


/*
 * @brief contructor set and get database
 */
AccountList::AccountList()
{

    QFile dataBaseFile("D:/QT/GameCaro/Databases/AccountList.json");

    if( dataBaseFile.open( QIODevice::ReadOnly | QIODevice::Text) )
    {
        /* file exist */
        QJsonParseError JsonParseError;
        QJsonDocument JsonDocument = QJsonDocument::fromJson(dataBaseFile.readAll(), &JsonParseError);
        if (JsonParseError.error != QJsonParseError::NoError)
        {
            dataBaseFile.close();
            // throw
            return;
        }
        dataBaseFile.close();

        QJsonObject rootObj = JsonDocument.object();
        this->numAccount = rootObj.value(QString("Number of Account")).toInt();

    }

    else
    {
        /* create file*/
        this->numAccount = 0;
        dataBaseFile.open( QIODevice::WriteOnly | QIODevice::Text);

        QJsonObject rootObj;
        rootObj["Number of Account"] = this->numAccount ;


        QJsonArray list;
        rootObj["List Account"] = list ;

        auto writeContent = QJsonDocument(rootObj).toJson();
        dataBaseFile.write(writeContent);

        dataBaseFile.close();

    }

}


/*
 * @brief add new account
 * @param account: string name of account
 */
void AccountList::addAccount(string accountNew, string passWord)
{

    string linkPlayerFile = "D:/QT/GameCaro/Databases/" + accountNew + ".json";
    if (checkAccountExist(linkPlayerFile))
    {
        return;
    }


    /* file exist */
    QFile dataBaseFile("D:/QT/GameCaro/Databases/AccountList.json");

    if( dataBaseFile.open( QIODevice::ReadOnly | QIODevice::Text) == 0 )
    {
        //throw
        return;
    }

    else
    {
        QJsonParseError JsonParseError;
        QJsonDocument JsonDocument = QJsonDocument::fromJson(dataBaseFile.readAll(), &JsonParseError);
        if (JsonParseError.error != QJsonParseError::NoError)
        {
            dataBaseFile.close();
            return;
        }

        QJsonObject rootObj = JsonDocument.object();
        this->numAccount = rootObj.value("Number of Account").toInt() + 1;
        rootObj["Number of Account"] = this->numAccount;

        QJsonArray listAccount = rootObj["List Account"].toArray();
        listAccount.push_back(accountNew.c_str());
        rootObj["List Account"] = listAccount;
        dataBaseFile.close();

        QJsonObject newRootObj = rootObj;

        dataBaseFile.open( QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        auto writeContent = QJsonDocument(newRootObj).toJson();
        dataBaseFile.write(writeContent);

        dataBaseFile.close();
    }




    /* create player file */
    QFile playerFile(linkPlayerFile.c_str());
    if( playerFile.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate) == 0 )
    {
        //throw
        return;
    }
    else
    {
        QJsonObject rootObj;
        rootObj.insert("Name Player", accountNew.c_str() );

        rootObj.insert("Password", passWord.c_str() );

        rootObj.insert("Win", 0);

        rootObj.insert("Draw", 0);

        rootObj.insert("Lose", 0);

        rootObj.insert("Elo", 0);

        QJsonObject listHistory;
        listHistory.insert("Number of History Game", 0);

        rootObj.insert("History Game", listHistory);

        auto writeContent = QJsonDocument(rootObj).toJson();
        playerFile.write(writeContent);
        playerFile.close();

    }

    playerFile.close();
}



/*
 * @brief sort player rank
 * @return list of player pro to nood
 */
multimap<float, string, greater<float>> AccountList::sortRank()
{
    multimap<float, string, greater<float>> sortList;
    QFile dataBaseFile("D:/QT/GameCaro/Databases/AccountList.json");
    if( dataBaseFile.open( QIODevice::ReadOnly | QIODevice::Text) == 0 )
    {
        // throw
    }
    else
    {
        QJsonParseError JsonParseError;
        QJsonDocument JsonDocument = QJsonDocument::fromJson(dataBaseFile.readAll(), &JsonParseError);
        if (JsonParseError.error != QJsonParseError::NoError)
        {
            dataBaseFile.close();
            // throw
        }

        QJsonObject rootObj = JsonDocument.object();
        QJsonArray listAccount = rootObj["List Account"].toArray();

        float elo;
        for (auto&& account : listAccount)
        {
            auto acc = account.toString().toStdString();
            elo = getElo(acc);
            sortList.emplace(elo, acc);
        }
    }

    dataBaseFile.close();

    return sortList;
}


/*
 * @brief get elo of account
 * @param accout: string name of account
 * @return elo of account
 */
float AccountList::getElo(string account)
{
    string linkPlayerFile = "D:/QT/GameCaro/Databases/" + account + ".json";
    QFile playerFile(linkPlayerFile.c_str());
    if( playerFile.open( QIODevice::ReadOnly | QIODevice::Text) == 0 )
    {
        //throw
        return -1;
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

        playerFile.close();
        return rootObj.value("Elo").toDouble();

    }
}


/*
 * @brief get elo of account
 * @param accout: account input
 * @param password: password input
 * @return true if true account and pass
 */
bool AccountList::checkLogin(string account, string password)
{
    string linkPlayerFile = "D:/QT/GameCaro/Databases/" + account + ".json";
    QFile playerFile(linkPlayerFile.c_str());
    if( playerFile.open( QIODevice::ReadOnly | QIODevice::Text) == 0 )
    {
        //throw
        return 0;
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

        if (rootObj["Password"].toString().toStdString() == password)
        {
            playerFile.close();
            return 1;
        }
        playerFile.close();
        return 0;
    }
}


/*
 * @brief check existed account
 * @param accout: account input
 * @return true if account existed
 */
bool AccountList::checkAccountExist(string account)
{
    string linkPlayerFile = "D:/QT/GameCaro/Databases/" + account + ".json";
    if (FileFunction::checkExistAccount(linkPlayerFile))
    {
        return 1;
    }
    return 0;

}


