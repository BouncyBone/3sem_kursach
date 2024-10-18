/** @file main.cpp
* @author Фарафонов А.Е.
* @version 1.0
* @date 10.10.2024
* @copyright ИБСТ ПГУ
* @brief Главный файл программы
*/
#include "head.h"
/** @brief Функция запуска сервера
* @details Функция отвечает за запуск сервера с параметрами командной строки.
* Для работы использует заголовочный файл со структурой сервера и файл с функциями для определения поведения сервера.
*/
int main(int argc, char *argv[]) {
    Server Server;
    while (true) {
        string* com = Server.comstr(argc, argv);
        int s = Server.self_addr(com[2], stoi(com[1]));
        int work_sock = Server.client_addr(s, com[2]);
        // Вызов функций autorized и math
        autorized(work_sock, com[0], com[2]);
        math(work_sock);
        delete[] com; // Освобождаем выделенную память
        return 0;
    }
    return 0;
}