#include "register.h"
#include <iostream>
#include <QDebug>
#include "database.h"

void cregister::run()
{
    std::string strUsername = "";
    std::string strPassword = "";
    std::string strConfigmPassword = "";

    std::cout << "please input username: ";
    std::cin >> strUsername;
    std::cout << "please input password: ";
    std::cin >> strPassword;
    std::cout << "please confim password: ";
    std::cin >> strConfigmPassword;

    if(strUsername.empty())
    {
        std::cout << "error: username is empty"<< std::endl;
        emit exit();
        return;
    }

    if(strUsername.length() > 16)
    {
        std::cout << "error: username should be less than 16"<<std::endl;
        emit exit();
        return;
    }

    if(strPassword.empty())
    {
        std::cout<<"error: password is empty" << std::endl;
        emit exit();
        return;
    }

    if(strPassword.length() > 32)
    {
        std::cout << "error: password should be less than 32"<<std::endl;
        emit exit();
        return;
    }

    if(strConfigmPassword.empty())
    {
        std::cout<<"error: confirm password is empty"<<std::endl;
        emit exit();
        return;
    }

    if(strConfigmPassword.length() > 32)
    {
        std::cout<<"error: confirm password should be less than 32"<<std::endl;
        emit exit();
        return;
    }

    if(strConfigmPassword != strPassword)
    {
        std::cout << "error: password is not equal";
        emit exit();
        return;
    }

    if(0 != Database::getInstance()->setUser(strUsername.c_str(),strPassword.c_str()))
    {
        std::cout << "register fail";
        emit exit();
        return;
    }
}
