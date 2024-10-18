/** @file work.cpp
* @author Фарафонов А.Е.
* @version 1.0
* @date 10.10.2024
* @copyright ИБСТ ПГУ
* @brief Файл с функциями сервера
*/

#include "head.h"
/** @brief Функция отправки сообщения клиенту
* @details Создаёт буфер для сообщения и отправляет сообщение 
*/
void msgsend(int work_sock, string mess){ //Отправка сообщения
    char *buffer = new char[4096];
    strcpy(buffer, mess.c_str());
    send(work_sock, buffer, mess.length(), 0);
}
/** @brief Обработка командной строки
* @details Обработка параметров командной строки и вывод справки в случае параметра -h
* @warning Указание параметров -f(файл базы данных) и -e(файл с логами) обязательно!
*/
string* Server::comstr(int argc, char *argv[]) {
    string* com = new string[3];
    if(argc == 1){ //Запуск без аргументов
        cout << "Kalculator"  << endl;
        cout << "-h info" << endl;
        cout << "-f file name" << endl;
        cout << "-p port" << endl;
        cout << "-e error file" << endl;
        exit(0);
    }
    int opt;
    int port = 33333;
    string file_name;
    string file_error = "error.txt";
    while ((opt = getopt(argc, argv, "hf:p:e:")) != -1) {
        switch(opt) {
            case 'h': //Вызов справки
                cout << "Векторный калькулятор выполняющий действие произведения вектора"  << endl;
                cout << "-f БД пользователей -p порт -e файл ошибок" << endl;
                cout << "-h info" << endl;
                cout << "-f file name" << endl;
                cout << "-p port" << endl;
                cout << "-e error file" << endl;
                exit(0);
                break;
            case 'f': //Указание файла с паролями
                file_name = string(optarg);
                com[0] = file_name;
                break;
            case 'p': //Указание порта
                port = stoi(string(optarg));
                com[1] = to_string(port);
                break;
            case 'e': //Указание файла с ошибками
                file_error = string(optarg);
                com[2] = file_error;
                break;
        }
    }
    return com;
}
/** @brief Функция создания хэша
* @details Принимает на вход текстовую строку и возвращает строку шестнадцатиричного хэша 
*/
string MD(string str){ 
    Weak::MD5 hash;
    string digest;
    StringSource(str, true,  new HashFilter(hash, new HexEncoder(new StringSink(digest))));  // строка-приемник
    return digest;
}
/** @brief Обработка ошибок
* @details Открывает файл с логами(параметр -e) и записывает туда ошибку в формате: Ошибка:Время. 
*/
void errors(string error, string name){ //Запись ошибки в файл
    ofstream file;
    file.open(name, ios::app);
    if(file.is_open()){
        time_t seconds = time(NULL);
        tm* timeinfo = localtime(&seconds);
        file<<error<<':'<<asctime(timeinfo)<<endl;
        cout << "error: " << error << endl;
    }
}
/** @brief Структура сервера
* @details Для соединения с клиентом используется порт 127.0.0.1. Функция создает сокет и включает режим ожидания клиента
*/
int Server::self_addr(string file_error, int port){ 
    int s = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in * self_addr = new (sockaddr_in); //Структура сервера
    self_addr->sin_family = AF_INET;
    self_addr->sin_port = htons(port);
    self_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
    
    cout << "Wait for connect client...\n";
    int b = bind(s,(const sockaddr*) self_addr,sizeof(sockaddr_in));
    if(b == -1) {
        cout << "Binding error\n";
        string error = "error binding";
        errors(error, file_error);
        throw BindingError(std::string("Binding error"));
        return false; //
    }
    listen(s, SOMAXCONN);
    return s;
}
/** @brief Структура клиента
* @details Подключается к серверу по уже созданному в структуре сервера сокету. В случае ошибки делает запись в лог.
*/
int Server::client_addr(int s, string file_error){
    sockaddr_in * client_addr = new sockaddr_in; //Структура клиента
    socklen_t len = sizeof (sockaddr_in);
    int work_sock = accept(s, (sockaddr*)(client_addr), &len);
    if(work_sock == -1) {
        cout << "Connection error\n";
        string error = "Connection error";
        errors(error, file_error);
        return 1;
    }
    else {
        //Успешное подключение к серверу
        cout << "Successfull client connection!\n";
        return work_sock;
    }
}
/** @brief Функция авторизации пользователя на сервере.
* @details Поочередно получает логин и пароль клиента, замешивает в пароль соль для шифрования и сравнивает правильность полученных данных.
* В случае ошибок выбрасывается исключение и ведется запись в лог.
*/
int autorized(int work_sock, string file_name, string file_error){ //Авторизация

    string ok = "OK";
    string salt = "2D2D2D2D2D2D2D22";
    string err = "ERROR";
    string error;
    char msg[255];
    
    //Получение данных о пароле и логине
    recv(work_sock, &msg, sizeof(msg), 0);
    string message = msg;
    string login, hashq;
    fstream file;
    file.open(file_name);
    getline (file, login, ':');
    getline (file, hashq);

    //СВЕРКА ЛОГИНОВ
    //std::cout << "message: " << message << endl;
    //std::cout << "login: " << login << endl;
    if(message != login){
        msgsend(work_sock,  err);
        error = "Ошибка логина";
        errors(error, file_error);
        close(work_sock);
        throw AuthError(std::string("Login error"));
        return 1;
    }
    else{
        //Отправка соли клиенту
        msgsend(work_sock,  salt);
        char msg[255] = "";
        recv(work_sock, msg, sizeof(msg), 0);
        string salted = salt + hashq;
        string digest;
        digest = MD(salted);
    
    //СВЕРКА ПАРОЛЕЙ
    //std::cout << "Digest: " << digest << endl;
    //std::cout << "msg: " << msg << endl;
    if(digest != msg){
        cout << digest << endl;
        cout << msg << endl;
        msgsend(work_sock,  err);
        error = "Ошибка пароля";
        errors(error, file_error);
        close(work_sock);
        throw AuthError(std::string("Password error"));
        return 1;
    }else{
        msgsend(work_sock,  ok);
    }
}
return 1;
}
/** @brief Обработка принимаемых векторов
* @details На вход принимается количество векторов kolvo, длина векторов numb и значения в каждом векторе.
* @warning На вход принимаются значения типа uint16_t. При переполнении возвращается значение 65535.
*/
int math(int work_sock) {
    int kolvo;
    int numb;
    uint16_t vect;

    // Прием количества векторов
    recv(work_sock, &kolvo, sizeof(kolvo), 0);

    // Цикл по векторам
    for (int j = 0; j < kolvo; j++) {
        // Прием длины вектора
        recv(work_sock, &numb, sizeof(numb), 0);
        
        uint16_t sum = 1; 

        // Цикл по значениям вектора
        for (int i = 0; i < numb; i++) {
            recv(work_sock, &vect, sizeof(vect), 0);
            if (sum == 65535){
                continue;
            }
            //std::cout << "Значение вектора (" << j+1 << ") длинной (" << i+1 << " / " << numb << ") получено: " << vect << std::endl;
            uint16_t sm = sum;
            sum *= vect; 
            /* Проверка правильности расчётов
            std::cout << "Сумма: " << sum << endl;
            std::cout << "SM: " << sm << endl;
            std::cout << "sum%vect: " << sum%vect << endl;
             */
            if (sum%vect !=0){
                sum = 65535;
                //std::cout << "New sum: " << sum << endl;
            }
        }
        
        uint16_t mfc = static_cast<uint16_t>(sum);

        send(work_sock, &mfc, sizeof(mfc), 0); // Отправка результата
    }

    cout << "Program finish!" << endl;
    close(work_sock);
    return 1;
}
