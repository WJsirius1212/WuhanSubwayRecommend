#include"crypt.h"
#include<iostream>
#include<fstream>
#include"login.h"

#define KEY "HUST1037"
bool userLogin();
bool userRegister();
using namespace std;

bool loginInterface()
{
	std::cout << "Welcome to Subway Transfer System wrote by wj!\n" << std::endl;

	start:
	int choice;
	std::cout<< "Please log in or register for an account." << std::endl;
	std::cout << "0.Login\n1.Register\n2.Quit" << std::endl;
	std::cin >> choice;
	bool success;
	switch (choice)
	{
	case 0:
		success = userLogin();
		if (success) break;
		else goto start;
	case 1:
		success = userRegister();
		goto start;
	case 2:
		return false;
	default:
		std::cout << "Please try again." << std::endl;
		goto start;
		break;
	}
	return success;
}

bool userLogin() {
	bool success = false;
	std::string userName;
	std::string userPW;
	std::cout << "Please enter ur name: " << std::endl;
	std::cin >> userName;
	std::cout << "Please enter ur password: " << std::endl;
	std::cin >> userPW;
	std::ifstream fin;
	fin.open(("./users.txt"), std::ios::in);
	if (!fin.is_open()) {
		std::cout << "Error.";
		return false;
	}

	std::string aUserName;
	std::string aUserPW;
	while (!fin.eof()) {
		std::getline(fin, aUserName);
		std::getline(fin, aUserPW);
		// std::cout << aUserName << "\n";
		if (aUserName == userName) {
			// std::cout << aUserName << ":UserName Correct\n";
			aUserPW = des_decrypt(aUserPW, KEY);
			// std::cout << aUserName << ":" << aUserPW << "end";
			if (aUserPW == userPW) {
				success = true;
				std::cout << "Success.\n";
				break;
			}
		}
	}
	fin.close();

	if (!success) std::cout << "User name does not exist or password is incorrect.";
	return success;
}

bool userRegister() {
	bool success = true;
	std::string userName;
	std::string userPW;
	do{
		std::cout << "Please enter ur name(<=20), recommend to enter student ID): " << std::endl;
		std::cin >> userName;
	} while (userName.length() > 20);
	while(userPW.length()!=8){
		std::cout << "Please enter ur password(==8):(a-Z/A-Z/0-9)" << std::endl;
		std::cin >> userPW; 
	}

	std::ifstream fin;
	fin.open(("./users.txt"), std::ios::in);
	if (!fin.is_open()) {
		std::cout << "Error.";
		return false;
	}
	std::string aUserName;
	std::string aUserPW;
	while (!fin.eof()) {
		std::getline(fin, aUserName);
		std::getline(fin, aUserPW);
		if (aUserName == userName) {
			success = false;
			std::cout << "UserName already been used.";
			break;
		}
	}
	fin.close();


	if (success) {
		std::string eUserPW;
		eUserPW = des_encrypt(userPW, KEY);

		std::ofstream ofs;

		ofs.open(("./users.txt"), std::ofstream::app);
		if (!ofs.is_open()) {
			std::cout << "Error.";
			return false;
		}
		ofs << userName << "\n";
		ofs << eUserPW << "\n";
		std::cout << "Success.";

		ofs.close();

	}
	

	return success;

}

