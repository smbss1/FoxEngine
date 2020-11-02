#include "Custom.h"
#include "Debug.h"

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif __linux__
#include <dlfcn.h>
#endif

EnodeCustom EnodeCustomLoad(char *custom_dll_path)
{
    EnodeCustom custom = EnodeCustom();
    
#if _WIN32
    custom.custom_dll = LoadLibraryA(custom_dll_path);
    if(custom.custom_dll)
    {
        Log("Custom layer successfully loaded from " << custom_dll_path << ".");
        custom.InitCallback      = (void *)GetProcAddress(custom.custom_dll, "EnodeCustomInitCallback");
        custom.ParseCallback     = (void *)GetProcAddress(custom.custom_dll, "EnodeCustomParseCallback");
        custom.CleanUpCallback   = (void *)GetProcAddress(custom.custom_dll, "EnodeCustomCleanUpCallback");
    }
#elif __linux__
    custom.custom_dll = dlopen(custom_dll_path, RTLD_NOW);
    if(custom.custom_dll)
    {
        LogColor(Bold(Green), "Custom layer successfully loaded from ");
        LogColor(Bold(White), custom_dll_path << ".\n");
        custom.InitCallback      = (EnodeInitCallback *) dlsym(custom.custom_dll, "EnodeCustomInitCallback");
        custom.ParseCallback     = (EnodeParseCallback *) dlsym(custom.custom_dll, "EnodeCustomParseCallback");
        custom.ParseEntityCallback     = (EnodeParseEntityCallback *) dlsym(custom.custom_dll, "EnodeCustomParseEntityCallback");
        custom.ParseNamespaceCallback     = (EnodeParseEntityCallback *) dlsym(custom.custom_dll, "EnodeCustomParseNamespaceCallback");
        custom.ParseDeclarationCallback     = (EnodeParseEntityCallback *) dlsym(custom.custom_dll, "EnodeCustomParseDeclarationCallback");
        custom.ParseProcedureCallback     = (EnodeParseEntityCallback *) dlsym(custom.custom_dll, "EnodeCustomParseProcedureCallback");
        custom.CleanUpCallback   = (EnodeCleanUpCallback *) dlsym(custom.custom_dll, "EnodeCustomCleanUpCallback");
    }
#endif
    
    if(!custom.InitCallback && !custom.ParseCallback && !custom.CleanUpCallback)
    {
        LogError("ERROR: No callbacks successfully loaded in custom layer.");
    }
    
    return custom;
}

void EnodeCustomUnload(EnodeCustom *custom)
{
    
#if _WIN32
    if(custom->custom_dll)
    {
        FreeLibrary(custom->custom_dll);
    }
#elif __linux__
    if(custom->custom_dll)
    {
        dlclose(custom->custom_dll);
    }
#endif
    
    custom->InitCallback = 0;
    custom->ParseCallback = 0;
    custom->ParseEntityCallback = 0;
    custom->ParseDeclarationCallback = 0;
    custom->ParseNamespaceCallback = 0;
    custom->ParseProcedureCallback = 0;
    custom->CleanUpCallback = 0;
    custom->custom_dll = 0;
}