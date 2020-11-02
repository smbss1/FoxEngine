
typedef struct EnodeNode EnodeNode;

typedef void EnodeInitCallback();
typedef void EnodeParseCallback(EnodeNode *root, char *filename);
typedef void EnodeParseEntityCallback(EnodeNode *root, char *filename);
typedef void EnodeCleanUpCallback();

struct EnodeCustom
{
    EnodeCustom()
    {
        InitCallback = nullptr;
        ParseCallback = nullptr;
        ParseEntityCallback = nullptr;
        CleanUpCallback = nullptr;
    }

    EnodeInitCallback*          InitCallback;
    EnodeParseCallback*         ParseCallback;
    EnodeParseEntityCallback*   ParseEntityCallback;
    EnodeParseEntityCallback*   ParseNamespaceCallback;
    EnodeParseEntityCallback*   ParseDeclarationCallback;
    EnodeParseEntityCallback*   ParseProcedureCallback;
    EnodeCleanUpCallback*       CleanUpCallback;

    
#if _WIN32
    HANDLE custom_dll;
#elif __linux__
    void* custom_dll;
#endif
    
};