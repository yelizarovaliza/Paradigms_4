#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

extern "C" {
    DLL_EXPORT char* encrypt(const char* rawText, int key);
    DLL_EXPORT char* decrypt(const char* encryptedText, int key);
}