#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

class User
{
    protected:
    string username;
    string password;
    string fullName;
    int accessLevel;

    public:
    User(const string& username, const string& password, 
         const string& fullName="", int accessLevel = 0)
        : username(username), password(password), 
          fullName(fullName), accessLevel(accessLevel) {}

    // Виртуальный деструктор для правильного удаления объектов производных классов
    virtual ~User() {}

    virtual bool authenticate(const string& inputPassword) const 
    {
        return password == inputPassword;
    }

     // Геттеры
    string getUsername() const { return username; }
    string getFullName() const { return fullName; }
    int getAccessLevel() const { return accessLevel; }

    void showMenu()
    {
        ifstream menu("menu.txt");
        if (!menu)
        {
            cout << "Не удалось открыть файл меню" << endl;
            return;
        }

        cout << "Меню:" << endl;
        string line;
        while (getline(menu, line))
        {
            cout << line << endl;
        }
    }

    virtual void showInfo() = 0;
};

class Customer : public User
{
    private:
    vector<string> order;
    int orderIsPayed = 0;
    string menuFile = "menu.txt";

    public:
    Customer(const string& username, const string& password, 
             const string& fullName="", int accessLevel = 0)
        : User(username, password, fullName, accessLevel) {}

    void orderFood(const string& menuFile, const string& foodName)
    {

        ofstream file("orders.txt", ios::app);
        if (!file)
        {
            cout << "Ошибка при открытии файла заказов" << endl;
            return;
        }

        ifstream menu(menuFile);
        if (!menu)
        {
            cout << "Не удалось открыть файл меню" << endl;
            return;
        }

        string line;
        while (getline(menu, line))
        {
            istringstream iss(line);
            string name;
            int price;
            if (iss >> name >> price && name == foodName)
            {
                order.push_back(foodName);
                file << foodName << " " << getUsername() << endl;
                cout << "Блюдо успешно заказано, ожидайте принятия заказа" << endl;
                orderIsPayed = -1;
                return;
            }
        }

        cout << "Блюдо " << foodName << " не найдено в меню" << endl;
    }

    void pay(const string& menuFile, const string& paymentMethod)
    {
        ifstream orderFile("orders.txt", ios::app);
        if (!orderFile)
        {
            cout << "Ошибка при открытии файла заказов" << endl;
            return;
        }

        fstream file("checks.txt", ios::app);
        if (!file)
        {
            cout << "На сервере произошла ошибка, заказ будет бесплатным" << endl;
            return;
        }

        int totalCost = 0;
        string lineOrder;

        while (getline(orderFile, lineOrder))
        {
            istringstream iss(lineOrder);
            string usernameFile;
            string foodName;
            if (iss >> foodName >> usernameFile && usernameFile == getUsername())
            {
                ifstream menu(menuFile);
                if (!menu)
                {
                    cout << "Не удалось открыть файл меню" << endl;
                    return;
                }

                string line;
                while (getline(menu, line))
                {
                    istringstream iss(line);
                    string name;
                    int price;
                    if (iss >> name >> price && name == foodName)
                    {
                        totalCost += price;
                        break;
                    }
                }
                
                if (totalCost == 0)
                {
                    cout << "Заказ пуст" << endl;
                    return;
                }
                if (paymentMethod == "card")
                {
                    file << getUsername() << " оплатил " << totalCost << " с помощью карты" << endl;
                }
                else if (paymentMethod == "cash")
                {
                    file << getUsername() << " оплатил " << totalCost << " наличными" << endl;
                }
                else
                {
                    cout << "Недопустимый метод оплаты" << endl;
                }
                orderIsPayed = 0;
            }     
        }
            
    }

    void showInfo() override
    {
        cout << "Добро пожаловать, " << getFullName() << "!" << endl;
        while (true)
        {
            cout << "-----------------------------------" << endl;
            cout << "1. Заказать еду" << endl;
            cout << "2. Посмотреть меню" << endl;
            cout << "3. Оплатить заказ" << endl;
            cout << "4. Выход" << endl;
            cout << "-----------------------------------" << endl;
            cout << "Введите номер пункта меню: ";
            int choice;
            string foodName;
            string paymentMethod;
            cin >> choice;
            switch (choice)
            {
                case 1:
                    cout << "Введите название блюда: ";
                    cin >> foodName;
                    orderFood(menuFile, foodName);
                    break;
                case 2:
                    showMenu();
                    break;
                case 3:
                    cout << "Введите метод оплаты (card или cash): ";
                    cin >> paymentMethod;
                    pay(menuFile, paymentMethod);
                    cout << "Спасибо за заказ, до свидания!" << endl;
                    return;
                case 4:
                    if (orderIsPayed == 1)
                    {
                        cout << "Ваш заказ не оплачен!" << endl;
                        break;
                    }
                    return;
                default:
                    cout << "Неверный выбор пункта меню" << endl;
                    break;
            }
        }
    }
};

class Waiter : public User
{
    private:
    vector<string> orders;
    vector<string> users;
    // string menuFile = "menu.txt";

    public:
    Waiter(const string& username, const string& password, 
           const string& fullName="", int accessLevel = 0)
        : User(username, password, fullName, accessLevel) {}
    
    
    void takeOrder(const string& foodName, const string& usernameOrder)
    {
        ifstream fileOrders("orders.txt");
        if (!fileOrders)
        {
            cout << "Не удалось открыть файл заказов" << endl;
            return;
        }

        if (fileOrders.peek() == ifstream::traits_type::eof())
        {
            cout << "Файл заказов пуст" << endl;
            return;
        }

        ofstream temp("temp.txt");
        if (!temp)
        {
            cout << "Не удалось открыть временный файл" << endl;
            return;
        }

        string line;
        bool found = false;
        while (getline(fileOrders, line))
        {
            istringstream iss(line);
            string order, usernameFile;
            if (iss >> order >> usernameFile
                && order == foodName && usernameFile == usernameOrder && !found)
            {
                found = true;
                continue;
            }

            temp << line << endl;
        }

        fileOrders.close();
        temp.close();

        if (found == false)
        {
            cout << "Официант не нашел заказ на " << foodName << " от " << usernameOrder << endl;
            return;
        }

        if (remove("orders.txt") != 0)
        {
            cout << "Не удалось удалить исходный файл" << endl;
            return;
        }

        if (rename("temp.txt", "orders.txt") != 0)
        {
            cout << "Не удалось переименовать временный файл" << endl;
            return;
        }

        orders.push_back(foodName);
        users.push_back(usernameOrder);
        cout << "Официант принял заказ на " << foodName << " от " << usernameOrder << endl;
    }


    void sendOrdersToKitchen()
    {
        if (orders.empty())
        {
            cout << "Нет заказов для передачи в кухню" << endl;
            return;
        }

        fstream file("kitchen.txt", ios::app);
        if (!file)
        {
            cout << "На сервере произошла ошибка" << endl;
            return;
        }
        
        cout << "Официант передал следующие заказы в кухню:" << endl;

        int i = 0;
        for (const auto& order : orders)
        {
            cout << order << endl;
            file << order <<  " " << users[i] << endl;
            ++i;
        }

        orders.clear();
        users.clear();
    }

    void showInfo() override
    {
        cout << "Добро пожаловать, " << getFullName() << "!" << endl;
        while (true)
        {
            cout << "-----------------------------------" << endl;
            cout << "1. Принять заказ" << endl;
            cout << "2. Посмотреть заказы" << endl;
            cout << "3. Отдать заказы на кухню" << endl;
            cout << "4. Посмотреть меню" << endl;
            cout << "5. Выход" << endl;
            cout << "-----------------------------------" << endl;
            cout << "Введите номер пункта меню: ";
            int choice;
            string usernameOrd, orderName;
            cin >> choice;
            switch (choice)
            {
                case 1:
                    cout << "Введите название заказа: ";
                    cin >> orderName;
                    cout << "Введите имя пользователя: ";
                    cin >> usernameOrd;

                    takeOrder(orderName, usernameOrd);
                    break;
                case 2:
                    showOrders();
                    break;
                case 3:
                    sendOrdersToKitchen();
                    break;
                case 4:
                    showMenu();
                    break;
                case 5:
                    return;
                default:
                    cout << "Неверный выбор пункта меню" << endl;
                    break;
            }
        }
    }

    void showOrders()
    {
        ifstream file("orders.txt");
        if (!file)
        {
            cout << "Не удалось открыть файл заказов" << endl;
            return;
        }

        if (file.peek() == ifstream::traits_type::eof())
        {
            cout << "Файл заказов пуст" << endl;
            return;
        }

        cout << "Заказы:" << endl;
        string line;
        while (getline(file, line))
        {
            cout << line << endl;
        }
    }
};

class Chef : public User
{
    public:
    Chef(const string& username, const string& password, 
           const string& fullName="", int accessLevel = 0)
        : User(username, password, fullName, accessLevel) {}
    

    void placeOrder(const string& foodName, const string& usernameOrder)
    {
        ifstream fileOrders("kitchen.txt");
        if (!fileOrders)
        {
            cout << "Не удалось открыть файл заказов" << endl;
            return;
        }

        if (fileOrders.peek() == ifstream::traits_type::eof())
        {
            cout << "Файл заказов пуст" << endl;
            return;
        }

        ofstream temp("temp.txt");
        if (!temp)
        {
            cout << "Не удалось открыть временный файл" << endl;
            return;
        }

        string line;
        bool found = false;
        while (getline(fileOrders, line))
        {
            istringstream iss(line);
            string order, usernameFile;
            if (iss >> order >> usernameFile
                && order == foodName && usernameFile == usernameOrder && !found)
            {
                found = true;
                continue;
            }

            temp << line << endl;
        }

        fileOrders.close();
        temp.close();

        if (found == false)
        {
            cout << "Повар не нашел заказ на " << foodName << " от " << usernameOrder << endl;
            return;
        }

        if (remove("kitchen.txt") != 0)
        {
            cout << "Не удалось удалить исходный файл" << endl;
            return;
        }

        if (rename("temp.txt", "kitchen.txt") != 0)
        {
            cout << "Не удалось переименовать временный файл" << endl;
            return;
        }

        cout << "Повар отдал заказ " << usernameOrder << " на " << foodName << endl;
    }

    void deleteFood(const string& foodName)
{
    ifstream file("menu.txt");
    if (!file)
    {
        cout << "Не удалось открыть файл меню" << endl;
        return;
    }

    ofstream temp("temp.txt");
    if (!temp)
    {
        cout << "Не удалось открыть временный файл" << endl;
        return;
    }

    string line;
    bool found = false;
    while (getline(file, line))
    {
        istringstream iss(line);
        string food;
        int orderPrice;
        if (iss >> food >> orderPrice
            && food == foodName && !found)
        {
            found = true;
            continue;
        }

        temp << line << endl;
    }

    file.close();  // Закрываем файл
    temp.close();  // Закрываем файл

    if (found == false)
    {
        cout << "Нет блюда " << foodName << " в меню " << endl;
        return;
    }

    if (remove("menu.txt") != 0)
    {
        cout << "Не удалось удалить исходный файл" << endl;
        return;
    }

    if (rename("temp.txt", "menu.txt") != 0)
    {
        cout << "Не удалось переименовать временный файл" << endl;
        return;
    }

    cout << "Повар удалил блюдо " << foodName << " из меню" << endl;
}


    void addFood(const string& foodName, const int price)
    {
        ofstream file("menu.txt", ios::app);
        if (!file)
        {
            cout << "Не удалось открыть файл меню" << endl;
            return;
        }

        file << foodName << " " << price << endl;
        cout << "Блюдо " << foodName << " добавлено в меню за " << price << " рублей" << endl;
    }

    void showInfo() override
    {
        cout << "Добро пожаловать, " << getFullName() << "!" << endl;
        while (true)
        {
            cout << "-----------------------------------" << endl;
            cout << "1. Отдать заказ" << endl;
            cout << "2. Посмотреть заказы" << endl;
            cout << "3. Убрать блюдо из меню" << endl;
            cout << "4. Добавить блюдо в меню" << endl;
            cout << "5. Посмотреть меню" << endl;
            cout << "6. Выход" << endl;
            cout << "-----------------------------------" << endl;
            cout << "Введите номер пункта меню: ";
            int choice;
            string usernameOrd, orderName;
            int price;
            cin >> choice;
            switch (choice)
            {
                case 1:
                    cout << "Введите название заказа: ";
                    cin >> orderName;
                    cout << "Введите имя пользователя: ";
                    cin >> usernameOrd;

                    placeOrder(orderName, usernameOrd);
                    break;
                case 2:
                    showOrdersOnKitchen();
                    break;
                case 3:
                    cout << "Введите название блюда: ";
                    cin >> orderName;

                    deleteFood(orderName);
                    break;
                case 4:
                    cout << "Введите название блюда: ";
                    cin >> usernameOrd;
                    cout << "Введите цену блюда: ";
                    cin >> price;

                    addFood(usernameOrd, price);
                    break;
                case 5:
                    showMenu();
                    break;
                case 6:
                    return;
                default:
                    cout << "Неверный выбор пункта меню" << endl;
                    break;
            }
        }
    }

    void showOrdersOnKitchen()
    {
        ifstream file("kitchen.txt");
        if (!file)
        {
            cout << "Не удалось открыть файл заказов" << endl;
            return;
        }

        if (file.peek() == ifstream::traits_type::eof())
        {
            cout << "Файл заказов пуст" << endl;
            return;
        }

        cout << "Заказы:" << endl;
        string line;
        while (getline(file, line))
        {
            cout << line << endl;
        }
    }

};

class Administrator : public Chef
{
    public:
    Administrator(const string& username, const string& password, 
           const string& fullName="", int accessLevel = 0)
        : Chef(username, password, fullName, accessLevel) {}
    

    void deleteEmployee(const string& employeeUsername)
    {
        
        ifstream file("users.txt");
        if (!file)
        {
            cout << "Не удалось открыть файл меню" << endl;
            return;
        }

        ofstream temp("temp.txt");
        if (!temp)
        {
            cout << "Не удалось открыть временный файл" << endl;
            return;
        }

        if (employeeUsername == "admin")
        {
            cout << "Нельзя уволить главного администратора!" << endl;
            return;
        }

        string line;
        bool found = false;
        while (getline(file, line))
        {
            istringstream iss(line);
            string usernameFile, passwordFile, userIdFile;
            if (iss >> usernameFile >> passwordFile >> userIdFile
                && usernameFile == employeeUsername && !found)
            {
                found = true;
                continue;
            }

            temp << line << endl;
        }

        file.close();  // Закрываем файл
        temp.close();  // Закрываем файл

        if (found == false)
        {
            cout << "Нет сотрудника с логином " << employeeUsername << endl;
            return;
        }

        if (remove("users.txt") != 0)
        {
            cout << "Не удалось удалить исходный файл" << endl;
            return;
        }

        if (rename("temp.txt", "users.txt") != 0)
        {
            cout << "Не удалось переименовать временный файл" << endl;
            return;
        }

        cout << "Вы уволили сотрудника с логином " << employeeUsername << endl;
    }

    void addEmployee(const string& usernameToFile, const string& passwordToFile,
                     const string& fullNameToFile, const string& userIDToFile)
    {
        ofstream file("users.txt", ios::app);
        if (!file)
        {
            cout << "Не удалось открыть файл меню" << endl;
            return;
        }

        file << usernameToFile << " " << passwordToFile << 
        " " << fullNameToFile << " " << userIDToFile << endl;
        cout << "Сотрудник добален!" << endl;
    }

    void showEmployees()
    {
        ifstream menu("users.txt");
        if (!menu)
        {
            cout << "Не удалось открыть файл меню" << endl;
            return;
        }

        string line;
        while (getline(menu, line))
        {
            cout << line << endl;
        }
    }

    void showChecks()
    {
        ifstream menu("checks.txt");
        if (!menu)
        {
            cout << "Не удалось открыть файл меню" << endl;
            return;
        }

        string line;
        while (getline(menu, line))
        {
            cout << line << endl;
        }
    }

    void showInfo() override
    {
        cout << "Добро пожаловать, " << getFullName() << "!" << endl;
        while (true)
        {
            cout << "-----------------------------------" << endl;
            cout << "1. Уволить сотрудника" << endl;
            cout << "2. Добавить сотрудника" << endl;
            cout << "3. Список сотрудников" << endl;
            cout << "4. Убрать блюдо из меню" << endl;
            cout << "5. Добавить блюдо в меню" << endl;
            cout << "6. Посмотреть меню" << endl;
            cout << "7. Псмотреть чеки" << endl;
            cout << "8. Выход" << endl;
            cout << "-----------------------------------" << endl;
            cout << "Введите номер пункта меню: ";
            int choice;
            string usernameOrd, orderName, paswordToFile, userIDToFile, fullNameToFile;
            int price;
            cin >> choice;
            switch (choice)
            {
                case 1:
                    cout << "Введите имя пользователя: ";
                    cin >> usernameOrd;

                    deleteEmployee(usernameOrd);
                    break;
                case 2:
                    cout << "Введите имя пользователя: ";
                    cin >> usernameOrd;
                    cout << "Введите пароль: ";
                    cin >> paswordToFile;
                    cout << "Введите полное имя: ";
                    cin >> fullNameToFile;
                    cout << "Введите уровень доступа: ";
                    cin >> userIDToFile;

                    addEmployee(usernameOrd, paswordToFile, fullNameToFile, userIDToFile);
                    break;
                case 3:
                    showEmployees();
                    break;
                case 4:
                    cout << "Введите название блюда: ";
                    cin >> orderName;

                    deleteFood(orderName);
                    break;
                case 5:
                    cout << "Введите название блюда: ";
                    cin >> usernameOrd;
                    cout << "Введите цену блюда: ";
                    cin >> price;

                    addFood(usernameOrd, price);
                    break;
                case 6:
                    showMenu();
                    break;
                case 7:
                    showChecks();
                    break;
                case 8:
                    return;
                default:
                    cout << "Неверный выбор пункта меню" << endl;
                    break;
            }
        }
    }
};

class FileManager
{
    public:
    string fullName;

    int checkPassword(const string& username, const string& password)
    {
        ifstream file("users.txt");
        if (!file)
        {
            cout << "Не удалось открыть файл пользователей" << endl;
            return false;
        }

        string line;
        while (getline(file, line))
        {
            istringstream iss(line);
            string fileUsername, filePassword, fileFullName, userID;
            if (iss >> fileUsername >> filePassword >> fileFullName >> userID 
                && fileUsername == username)
            {
                if (userID == "costomer" && filePassword == password)
                    {
                        fullName = fileFullName;
                        return 1;
                    }
                else if (userID == "waiter" && filePassword == password)
                {
                    fullName = fileFullName;
                    return 2;
                }
                else if (userID == "chef" && filePassword == password)
                {
                    fullName = fileFullName;
                    return 3;
                }
                else if (userID == "admin" && filePassword == password)
                {
                    fullName = fileFullName;
                    return 4;
                }
                else if (filePassword != password) 
                {
                    cout << "Неверный пароль" << endl;
                    return -1;
                }
            }
        }

        return 0;
    }

    void registerUser(const string& username, const string& password, const string& fullName, const string& userID)
    {
        ofstream file("users.txt", ios::app);
        if (!file)
        {
            cout << "Не удалось открыть файл пользователей" << endl;
            return;
        }

        file << username << " " << password << " " << fullName << " " << userID << endl;
        cout << "Пользователь успешно зарегистрирован" << endl;
    }
};



int main()
{
    cout << "Добро пожаловать в ресторан \"Барбекю\"!" << endl;

    FileManager fileManager;

    while (true)
    {
        cout << "Введите имя пользователя (exit для выхода): ";
        string username;
        cin >> username;

        if (username == "exit") break;
        
        cout << "Введите пароль: ";
        string password;
        cin >> password;

        int loginStatus = fileManager.checkPassword(username, password);

        if (loginStatus == 0)
        {
            cout << "Пользователь не найден, хотите зарегистрироваться (да или нет)?" << endl;
            string choice;
            cin >> choice;
            if (choice == "да")
            {
                cout << "Введите полное имя: ";
                string fullName;
                cin >> fullName;

                fileManager.registerUser(username, password, fullName, "costomer");
            }
            else
            {
                cout << "До свидания!" << endl;
            }
        }
        else if (loginStatus == 1)
        {
            Customer customer(username, password, fileManager.fullName, 1);
            customer.showInfo();
        }
        else if (loginStatus == 2)
        {
            Waiter waiter(username, password, fileManager.fullName, 2);
            waiter.showInfo();
        }
        else if (loginStatus == 3)
        {
            Chef chef(username, password, fileManager.fullName, 3);
            chef.showInfo();
        }
        else if (loginStatus == 4)
        {
            Administrator admin(username, password, fileManager.fullName, 4);
            admin.showInfo();
        }
    }
}