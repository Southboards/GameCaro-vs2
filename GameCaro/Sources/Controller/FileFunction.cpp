#include "Headers/Controller/FileFunction.h"

FileFunction::FileFunction()
{

}



string FileFunction::getStringName(string name)
{
    string str = name;
    for (int i=0;i<(20-name.length());i++)
    {
        str += " ";
    }
    return str;
}



/*
 * @brief stran int to string 4 char
 * @param num: int num
 * @return 4 char of number
 */
string FileFunction::getStringNumber(int num)
{
    string str = to_string(num);
    for (int i=0;str.length()<4;i++)
    {
        str += ' ';
    }
    return str;
}

/*
 * @brief stran float to string 4 char
 * @param num: float num
 * @return 4 char of number
 */
string FileFunction::getStringNumber(float num)
{
    string str = to_string(num);
    for (int i=4;i<str.length();i++)
    {
        str.erase(i);
    }

    if (str[3]=='.')
    {
        str[3] = ' ';
    }
    return str;
}




/*
 * @brief check existed account
 * @param link: link of account
 * @return true if account existed
 */
bool FileFunction::checkExistAccount(string link)
{
    QFile dataBaseFile(link.c_str());
    if( dataBaseFile.open( QIODevice::ReadOnly | QIODevice::Text) == 0 )
    {
        dataBaseFile.close();
        return 0;
    }
    dataBaseFile.close();
    return 1;
}
