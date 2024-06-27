#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

extern "C" {
    DLL_EXPORT char* encrypt(char* rawText, int key);
    DLL_EXPORT char* decrypt(char* encryptedText, int key);
}