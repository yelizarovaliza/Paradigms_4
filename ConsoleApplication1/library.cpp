#include "caesar.h"
#include <cstring>
#include <cctype>

// Encrypt function
char* encrypt(char* rawText, int key) {
    int len = std::strlen(rawText);
    char* encryptedText = new char[len + 1];
    key = key % 26;

    for (int i = 0; i < len; ++i) {
        char c = rawText[i];
        if (std::isalpha(c)) {
            char base = std::isupper(c) ? 'A' : 'a';
            encryptedText[i] = (c - base + key + 26) % 26 + base;
        }
        else {
            encryptedText[i] = c;
        }
    }
    encryptedText[len] = '\0';
    return encryptedText;
}

// Decrypt function
char* decrypt(char* encryptedText, int key) {
    return encrypt(encryptedText, 26 - (key % 26));
}