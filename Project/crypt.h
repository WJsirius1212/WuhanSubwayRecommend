#include <string>
#pragma once
std::string des_encrypt(const std::string& clearText, const std::string& key);
std::string des_decrypt(const std::string& cipherText, const std::string& key);
