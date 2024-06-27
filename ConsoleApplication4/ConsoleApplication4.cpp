#include <iostream>
#include <fstream>
#include <cstring>
#include <stack>
#include <sstream>
#include <string>
#include <windows.h>
#include "caesar.h"

using namespace std;

typedef char* (*encrypt_ptr)(const char*, int);
typedef char* (*decrypt_ptr)(const char*, int);

class CaesarCipher {
public:
    static int encryptDecryptMenu(int UserChoice) {
        HINSTANCE hDll = LoadLibrary(TEXT("library.dll"));
        if (!hDll) {
            cerr << "Could not load the DLL!" << endl;
            return 1;
        }

        encrypt_ptr encrypt = (encrypt_ptr)GetProcAddress(hDll, "encrypt");
        decrypt_ptr decrypt = (decrypt_ptr)GetProcAddress(hDll, "decrypt");

        if (!encrypt || !decrypt) {
            cerr << "Could not locate the functions!" << endl;
            FreeLibrary(hDll);
            return 1;
        }

        const int BufferSize = 100;
        char inputText[BufferSize];
        int key;
        const int FileNameSize = 256;
        char inputFile[FileNameSize];
        char outputFile[FileNameSize];

        FILE* encryptDecrypt;
        FILE* outputFilePtr;

        switch (UserChoice) {
        case 15:
            cout << "Enter text to encrypt: ";
            cin.getline(inputText, BufferSize);
            cout << "Enter key to encrypt: ";
            cin >> key;
            {
                char* encryptedText = encrypt(inputText, key);
                if (encryptedText) {
                    cout << encryptedText << endl;
                    delete[] encryptedText;
                }
            }
            break;
        case 16:
            cout << "Enter text to decrypt: ";
            cin.getline(inputText, BufferSize);
            cout << "Enter key to decrypt: ";
            cin >> key;
            {
                char* decryptedText = decrypt(inputText, key);
                if (decryptedText) {
                    cout << decryptedText << endl;
                    delete[] decryptedText;
                }
            }
            break;
        case 17:
            cout << "Enter file name to encrypt: ";
            cin.getline(inputFile, FileNameSize);
            cout << "Enter key to encrypt: ";
            cin >> key;
            cout << "Enter output file name: ";
            cin.getline(outputFile, FileNameSize);
            {
                errno_t err = fopen_s(&encryptDecrypt, inputFile, "r");
                if (err != 0 || !encryptDecrypt) {
                    cerr << "Could not open the input file!" << endl;
                    FreeLibrary(hDll);
                    return 1;
                }
                err = fopen_s(&outputFilePtr, outputFile, "w");
                if (err != 0 || !outputFilePtr) {
                    cerr << "Could not open the output file!" << endl;
                    fclose(encryptDecrypt);
                    FreeLibrary(hDll);
                    return 1;
                }
                const int ChunkSize = 128;
                char chunk[ChunkSize];
                size_t charsRead;
                while ((charsRead = fread(chunk, 1, ChunkSize - 1, encryptDecrypt)) != 0) {
                    chunk[charsRead] = '\0';
                    char* encryptedChunk = encrypt(chunk, key);
                    if (encryptedChunk) {
                        fwrite(encryptedChunk, sizeof(char), strlen(encryptedChunk), outputFilePtr);
                        delete[] encryptedChunk;
                    }
                }
                fclose(encryptDecrypt);
                fclose(outputFilePtr);
            }
            break;
        case 18:
            cout << "Enter file name to decrypt: ";
            cin.getline(inputFile, FileNameSize);
            cout << "Enter key to decrypt: ";
            cin >> key;
            cout << "Enter output file name: ";
            cin.getline(outputFile, FileNameSize);
            {
                errno_t err = fopen_s(&encryptDecrypt, inputFile, "r");
                if (err != 0 || !encryptDecrypt) {
                    cerr << "Could not open the input file!" << endl;
                    FreeLibrary(hDll);
                    return 1;
                }
                err = fopen_s(&outputFilePtr, outputFile, "w");
                if (err != 0 || !outputFilePtr) {
                    cerr << "Could not open the output file!" << endl;
                    fclose(encryptDecrypt);
                    FreeLibrary(hDll);
                    return 1;
                }
                const int ChunkSize = 128;
                char chunk[ChunkSize];
                size_t charsRead;
                while ((charsRead = fread(chunk, 1, ChunkSize - 1, encryptDecrypt)) != 0) {
                    chunk[charsRead] = '\0';
                    char* decryptedChunk = decrypt(chunk, key);
                    if (decryptedChunk) {
                        fwrite(decryptedChunk, sizeof(char), strlen(decryptedChunk), outputFilePtr);
                        delete[] decryptedChunk;
                    }
                }
                fclose(encryptDecrypt);
                fclose(outputFilePtr);
            }
            break;
        default:
            cout << "Invalid choice." << endl;
            break;
        }

        FreeLibrary(hDll);
        return 0;
    }
};


class FileHandler {
public:
    void fileSave(const char** linesArray, int currLine) {
        char fileName[128];
        cout << "Enter the file name for saving: ";
        cin.getline(fileName, 128);

        std::ofstream file(fileName);
        if (!file) {
            cout << "Error opening file for writing." << endl;
            return;
        }

        for (int i = 0; i <= currLine; i++) {
            if (linesArray[i] != NULL) {
                file << linesArray[i] << endl;
            }
        }

        file.close();
        cout << "Text has been saved successfully." << endl;
    }

    void fileLoad(char** linesArray, size_t* lineSizes, int& currLine, int& maxLines) {
        char fileName[128];
        cout << "Enter the file name for loading: ";
        cin.getline(fileName, 128);

        std::ifstream file(fileName);
        if (!file) {
            cout << "Error opening file for reading." << endl;
            return;
        }

        char line[128];
        int lineIndex = currLine + 1;
        while (file.getline(line, 128)) {
            size_t len = strlen(line);
            if (lineIndex >= maxLines) {
                maxLines *= 2;
                linesArray = (char**)realloc(linesArray, maxLines * sizeof(char*));
                lineSizes = (size_t*)realloc(lineSizes, maxLines * sizeof(size_t));
                if (linesArray == NULL || lineSizes == NULL) {
                    cout << "Memory reallocation failed." << endl;
                    file.close();
                    exit(EXIT_FAILURE);
                }
                for (int i = lineIndex; i < maxLines; i++) {
                    linesArray[i] = NULL;
                    lineSizes[i] = 0;
                }
            }

            linesArray[lineIndex] = (char*)malloc((len + 1) * sizeof(char));
            if (linesArray[lineIndex] == NULL) {
                cout << "Memory allocation failed." << endl;
                file.close();
                return;
            }
            strcpy_s(linesArray[lineIndex], len + 1, line);
            lineSizes[lineIndex] = len + 1;
            lineIndex++;
        }

        file.close();
        currLine = lineIndex - 1;
        cout << "Text has been loaded successfully." << endl;
    }
};

class TextEditor {
private:
    char** linesArray;
    size_t* lineSizes;
    int currLine;
    int maxLines;
    char* cutCopySaver;
    stack<string> undoStack;
    stack<string> redoStack;

    void saveState() {
        string state;
        for (int i = 0; i <= currLine; i++) {
            if (linesArray[i] != NULL) {
                state += linesArray[i];
            }
            state += '\n';
        }
        undoStack.push(state);
        if (undoStack.size() > 3) {
            undoStack.pop();
        }
    }

    void restoreState(const string& state) {
        istringstream ss(state);
        string line;
        currLine = 0;
        while (getline(ss, line)) {
            if (linesArray[currLine] != NULL) {
                free(linesArray[currLine]);
            }
            linesArray[currLine] = (char*)malloc((line.length() + 1) * sizeof(char));
            if (linesArray[currLine] == NULL) {
                std::cout << "Memory allocation failed." << std::endl;
                return;
            }
            strcpy_s(linesArray[currLine], line.length() + 1, line.c_str());
            lineSizes[currLine] = line.length() + 1;
            currLine++;
        }
        currLine--;
    }


public:
    TextEditor() {
        maxLines = 10;
        currLine = 0;
        linesArray = (char**)malloc(maxLines * sizeof(char*));
        lineSizes = (size_t*)malloc(maxLines * sizeof(size_t));
        cutCopySaver = NULL;
        if (linesArray == NULL || lineSizes == NULL) {
            cout << "Memory allocation failed." << endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < maxLines; i++) {
            linesArray[i] = NULL;
            lineSizes[i] = 0;
        }
    }

    void addText() {
        saveState();
        char text[128];
        cout << "Enter text to append: ";
        cin.getline(text, 128);

        if (strlen(text) > 0) {
            if (linesArray[currLine] == NULL) {
                linesArray[currLine] = (char*)malloc((strlen(text) + 1) * sizeof(char));
                if (linesArray[currLine] == NULL) {
                    cout << "Memory allocation failed." << endl;
                    return;
                }
                strcpy_s(linesArray[currLine], strlen(text) + 1, text);
                lineSizes[currLine] = strlen(text) + 1;
            }
            else {
                size_t lineArraylen = strlen(linesArray[currLine]);
                lineSizes[currLine] += (strlen(text) + 1);
                linesArray[currLine] = (char*)realloc(linesArray[currLine], lineSizes[currLine] * sizeof(char));
                if (linesArray[currLine] == NULL) {
                    cout << "Memory reallocation failed." << endl;
                    return;
                }
                strcat_s(linesArray[currLine], lineSizes[currLine], " ");
                strcat_s(linesArray[currLine], lineSizes[currLine], text);
            }

            cout << "Current line content: " << linesArray[currLine] << endl;
        }
        else {
            cout << "Error reading input." << endl;
        }
    }

    void newLine() {
        saveState();
        currLine++;
        if (currLine >= maxLines) {
            maxLines *= 2;
            linesArray = (char**)realloc(linesArray, maxLines * sizeof(char*));
            lineSizes = (size_t*)realloc(lineSizes, maxLines * sizeof(size_t));
            if (linesArray == NULL || lineSizes == NULL) {
                cout << "Memory reallocation failed." << endl;
                exit(EXIT_FAILURE);
            }
            for (int i = currLine; i < maxLines; i++) {
                linesArray[i] = NULL;
                lineSizes[i] = 0;
            }
        }
        cout << "New line is started" << endl;
    }

    void outputAllText() {
        bool hasContent = false;
        cout << "Here is all your lines which were written down: " << endl;
        for (int i = 0; i <= currLine; i++) {
            if (linesArray[i] != NULL && strlen(linesArray[i]) > 0) {
                cout << linesArray[i] << endl;
                hasContent = true;
            }
        }

        if (!hasContent) {
            cout << "No non-empty lines found." << endl;
        }
    }

    void addTextCoordinates() {
        saveState();
        int lineIndex, charIndex;
        char insertText[128];

        cout << "Choose line and index: ";
        cin >> lineIndex >> charIndex;
        cin.ignore();

        if (lineIndex < 0 || lineIndex >= maxLines || linesArray[lineIndex] == NULL) {
            cout << "Invalid line index or line is empty." << endl;
            return;
        }

        cout << "Enter text to insert: ";
        cin.getline(insertText, 128);

        if (strlen(insertText) > 0) {
            size_t lenInsertText = strlen(insertText);

            size_t lineLen = strlen(linesArray[lineIndex]);

            if (charIndex < 0 || (size_t)charIndex > lineLen) {
                cout << "Invalid character index." << endl;
                return;
            }

            lineSizes[lineIndex] += lenInsertText;
            linesArray[lineIndex] = (char*)realloc(linesArray[lineIndex], lineSizes[lineIndex] * sizeof(char));
            if (linesArray[lineIndex] == NULL) {
                cout << "Memory reallocation failed." << endl;
                return;
            }

            memmove(&linesArray[lineIndex][charIndex + lenInsertText], &linesArray[lineIndex][charIndex], lineLen - charIndex + 1);
            memcpy(&linesArray[lineIndex][charIndex], insertText, lenInsertText);

            cout << "Text has been inserted successfully." << endl;
            cout << "Updated line content: " << linesArray[lineIndex] << endl;
        }
        else {
            cout << "Error reading input." << endl;
        }
    }

    void searchWord() {
        char searchTerm[128];
        cout << "Enter text to search: ";
        cin.ignore();
        cin.getline(searchTerm, 128);

        if (strlen(searchTerm) > 0) {
            bool found = false;
            for (int i = 0; i <= currLine; i++) {
                if (linesArray[i] != NULL) {
                    char* pos = strstr(linesArray[i], searchTerm);
                    while (pos != NULL) {
                        int index = (int)(pos - linesArray[i]);
                        if (found) {
                            std::cout << ", ";
                        }
                        cout << i << " " << index;
                        found = true;
                        pos = strstr(pos + 1, searchTerm);
                    }
                }
            }

            if (!found) {
                cout << "Has not found your phrase." << endl;
            }
            else {
                cout << endl;
            }
        }
        else {
            cout << "Error reading search." << endl;
        }
    }

    void deleteChars() {
        saveState();
        int lineIndex, charIndex, numChars;

        cout << "Choose line, index and number of symbols: ";
        cin >> lineIndex >> charIndex >> numChars;
        cin.ignore();

        if (lineIndex < 0 || lineIndex >= maxLines || linesArray[lineIndex] == NULL) {
            cout << "Invalid line index or line is empty." << endl;
            return;
        }

        size_t lineLen = strlen(linesArray[lineIndex]);

        if (charIndex < 0 || charIndex >= lineLen || numChars <= 0 || (size_t)(charIndex + numChars) > lineLen) {
            cout << "Invalid character index or number of symbols." << endl;
            return;
        }
        memmove(&linesArray[lineIndex][charIndex], &linesArray[lineIndex][charIndex + numChars], lineLen - charIndex - numChars + 1);
        lineSizes[lineIndex] -= numChars;
        linesArray[lineIndex] = (char*)realloc(linesArray[lineIndex], lineSizes[lineIndex] * sizeof(char));
        if (linesArray[lineIndex] == NULL) {
            cout << "Memory reallocation failed." << endl;
            return;
        }

        cout << "Characters deleted successfully." << endl;
    }

    void cutText() {
        saveState();
        int lineIndex, charIndex, numChars;

        cout << "Choose line, index and number of symbols: ";
        cin >> lineIndex >> charIndex >> numChars;
        cin.ignore();

        if (lineIndex < 0 || lineIndex >= maxLines || linesArray[lineIndex] == NULL) {
            cout << "Invalid line index or line is empty." << endl;
            return;
        }

        size_t lineLen = strlen(linesArray[lineIndex]);

        if (charIndex < 0 || charIndex >= lineLen || numChars <= 0 || (size_t)(charIndex + numChars) > lineLen) {
            cout << "Invalid character index or number of symbols." << endl;
            return;
        }

        cutCopySaver = (char*)realloc(cutCopySaver, (numChars + 1) * sizeof(char));
        if (cutCopySaver == NULL) {
            cout << "Memory allocation for clipboard failed." << endl;
            return;
        }

        strncpy_s(cutCopySaver, numChars + 1, &linesArray[lineIndex][charIndex], numChars);
        cutCopySaver[numChars] = '\0';

        memmove(&linesArray[lineIndex][charIndex], &linesArray[lineIndex][charIndex + numChars], lineLen - charIndex - numChars + 1);
        lineSizes[lineIndex] -= numChars;
        linesArray[lineIndex] = (char*)realloc(linesArray[lineIndex], lineSizes[lineIndex] * sizeof(char));
        if (linesArray[lineIndex] == NULL) {
            cout << "Memory reallocation failed." << endl;
            return;
        }

        cout << "Text cut successfully." << endl;
    }

    void copyText() {
        saveState();
        int lineIndex, charIndex, numChars;

        cout << "Choose line, index and number of symbols: ";
        cin >> lineIndex >> charIndex >> numChars;
        cin.ignore();

        if (lineIndex < 0 || lineIndex >= maxLines || linesArray[lineIndex] == NULL) {
            cout << "Invalid line index or line is empty." << endl;
            return;
        }

        size_t lineLen = strlen(linesArray[lineIndex]);

        if (charIndex < 0 || charIndex >= lineLen || numChars <= 0 || (size_t)(charIndex + numChars) > lineLen) {
            cout << "Invalid character index or number of symbols." << endl;
            return;
        }

        cutCopySaver = (char*)realloc(cutCopySaver, (numChars + 1) * sizeof(char));
        if (cutCopySaver == NULL) {
            cout << "Memory allocation for clipboard failed." << endl;
            return;
        }

        strncpy_s(cutCopySaver, numChars + 1, &linesArray[lineIndex][charIndex], numChars);
        cutCopySaver[numChars] = '\0';

        cout << "Text copied successfully." << endl;
    }

    void pasteText() {
        saveState();
        if (cutCopySaver == NULL) {
            cout << "Buffer is empty." << endl;
            return;
        }

        int lineIndex, charIndex;

        cout << "Choose line and index: ";
        cin >> lineIndex >> charIndex;
        cin.ignore();

        if (lineIndex < 0 || lineIndex >= maxLines || linesArray[lineIndex] == NULL) {
            cout << "Invalid line index or line is empty." << endl;
            return;
        }

        size_t lineLen = strlen(linesArray[lineIndex]);
        size_t cutCoryLen = strlen(cutCopySaver);

        if (charIndex < 0 || (size_t)charIndex > lineLen) {
            cout << "Invalid character index." << endl;
            return;
        }

        lineSizes[lineIndex] += cutCoryLen;
        linesArray[lineIndex] = (char*)realloc(linesArray[lineIndex], lineSizes[lineIndex] * sizeof(char));
        if (linesArray[lineIndex] == NULL) {
            cout << "Memory reallocation failed." << endl;
            return;
        }

        memmove(&linesArray[lineIndex][charIndex + cutCoryLen], &linesArray[lineIndex][charIndex], lineLen - charIndex + 1);
        memcpy(&linesArray[lineIndex][charIndex], cutCopySaver, cutCoryLen);

        cout << "Text pasted successfully." << endl;
    }

    void insertPlace() {
        saveState();
        int lineIndex, charIndex;
        char insertText[128];

        cout << "Choose line and index: ";
        cin >> lineIndex >> charIndex;
        cin.ignore();

        if (lineIndex < 0 || lineIndex >= maxLines || linesArray[lineIndex] == NULL) {
            cout << "Invalid line index or line is empty." << endl;
            return;
        }

        cout << "Write text: ";
        cin.getline(insertText, 128);

        if (strlen(insertText) > 0) {
            size_t lenInsertText = strlen(insertText);

            size_t lineLen = strlen(linesArray[lineIndex]);

            if (charIndex < 0 || (size_t)charIndex > lineLen) {
                cout << "Invalid character index." << endl;
                return;
            }

            lineSizes[lineIndex] += lenInsertText;
            linesArray[lineIndex] = (char*)realloc(linesArray[lineIndex], lineSizes[lineIndex] * sizeof(char));
            if (linesArray[lineIndex] == NULL) {
                cout << "Memory reallocation failed." << endl;
                return;
            }

            memmove(&linesArray[lineIndex][charIndex + lenInsertText], &linesArray[lineIndex][charIndex], lineLen - charIndex + 1);
            memcpy(&linesArray[lineIndex][charIndex], insertText, lenInsertText);

            cout << "Text has been inserted successfully." << endl;
            cout << "Updated line content: " << linesArray[lineIndex] << endl;
        }
    }

    void undo() {
        if (undoStack.empty()) {
            cout << "Nothing to undo." << endl;
            return;
        }

        string currentState;
        for (int i = 0; i <= currLine; i++) {
            if (linesArray[i] != NULL) {
                currentState += linesArray[i];
            }
            currentState += '\n';
        }
        redoStack.push(currentState);

        string lastState = undoStack.top();
        undoStack.pop();
        restoreState(lastState);

        cout << "Undo operation completed." << endl;
    }

    void redo() {
        if (redoStack.empty()) {
            cout << "Nothing to redo." << endl;
            return;
        }

        std::string currentState;
        for (int i = 0; i <= currLine; i++) {
            if (linesArray[i] != NULL) {
                currentState += linesArray[i];
            }
            currentState += '\n';
        }
        undoStack.push(currentState);

        std::string nextState = redoStack.top();
        redoStack.pop();
        restoreState(nextState);

        cout << "Redo operation completed." << endl;
    }

    void run() {
        int userChoice = -1;
        while (userChoice != 0) {
            cout << "Choose the command:" << endl;
            cin >> userChoice;
            cin.ignore();

            switch (userChoice) {
            case 0:
                cout << "Exiting program." << endl;
                break;
            case 1:
                addText();
                break;
            case 2:
                newLine();
                break;
            case 3:
                FileHandler().fileSave((const char**)linesArray, currLine);
                break;
            case 4:
                FileHandler().fileLoad(linesArray, lineSizes, currLine, maxLines);
                break;
            case 5:
                outputAllText();
                break;
            case 6:
                addTextCoordinates();
                break;
            case 7:
                searchWord();
                break;
            case 8:
                deleteChars();
                break;
            case 9:
                undo();
                break;
            case 10:
                redo();
                break;
            case 11:
                cutText();
                break;
            case 12:
                copyText();
                break;
            case 13:
                pasteText();
                break;
            case 14:
                insertPlace();
                break;
            case 15:
            case 16:
            case 17:
            case 18:
                CaesarCipher::encryptDecryptMenu(userChoice);
                break;
          
            default:
                cout << "Invalid choice, please try again." << endl;
            }
        }
    }

    ~TextEditor() {
        for (int i = 0; i < maxLines; i++) {
            free(linesArray[i]);
        }
        free(linesArray);
        free(lineSizes);
        if (cutCopySaver != NULL) {
            free(cutCopySaver);
        }
    }
};

int main() {
    TextEditor editor;
    editor.run();

    return 0;
}