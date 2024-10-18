/** @file head.h
* @author Фарафонов А.Е.
* @version 1.0
* @date 10.10.2024
* @copyright ИБСТ ПГУ
* @brief Заголовочный файл
*/

/** @brief Используемые библиотеки
*/
#include <netinet/in.h>
#include <iostream>
#include <cassert>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>
#include <cryptopp/cryptlib.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <getopt.h>
#include <cryptopp/hex.h> // HexEncoder
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
using namespace CryptoPP;
using namespace std;
#include <cryptopp/md5.h>
/** @brief Структура сервера
* @details Структура сервера с обозначением функций программы и требуемых параметров функций.
*/
std::string MD(std::string sah);
void errors(std::string error, std::string name);
void msgsend(int work_sock, string mess);
int autorized(int work_sock, string file_name, string file_error);
int math(int work_sock);
//Класс сервера
class Server{
private:

public:
    string* comstr(int argc, char *argv[]);
    int self_addr(string file_error, int port);
    int client_addr(int s, string file_error);
};

/** @brief Классы ошибок
* @details Данный блок нужен для выброса исключений и обработки ошибок.
*/
class BindingError: public std::invalid_argument
{
public:
    BindingError(const std::string& s) : std::invalid_argument(s) {}
    BindingError(const char * s) : std::invalid_argument(s) {}
};

class AuthError: public std::invalid_argument
{
public:
    AuthError(const std::string& s) : std::invalid_argument(s) {}
    AuthError(const char * s) : std::invalid_argument(s) {}
};
