
#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif __linux__
#include <dlfcn.h>
#endif

#include <iostream>
#include <cstring>

#include "Enode.h"
#include "Custom.h"
#include "Debug.h"
#include "PointerMath.h"
#include "Graph.h"

using namespace std;

EnodeCustom EnodeCustomLoad(char *custom_dll_path);
void EnodeCustomUnload(EnodeCustom *custom);

struct ParsedFile
{
    EnodeNode *m_pRoot;
    char *m_strFilename;
    char *strLanguage;
};

struct Settings
{
    ParsedFile m_pParsedFile;
    char *m_strNameOutFile;
    std::string m_strLanguage;
};

static char* LoadEntireFile(Allocator& oAllocator, char *filename)
{
    char *result = nullptr;
    
    FILE* file = fopen(filename, "rb");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        unsigned int file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        result = NewArray<char>(oAllocator, file_size + 1);
        if(result)
        {
            fread(result, 1, file_size, file);
            result[file_size] = 0;
        }
        fclose(file);
    }
    
    return result;
}

void PrintAndResetParserErrors(Parser& oParser)
{
    for(int i = 0; i < oParser.m_iErrorStackSize; i++)
    {
        fprintf(stderr, "ERROR (%s:%i): %s\n",
                oParser.m_pErrorStack[i].m_strFilename,
                oParser.m_pErrorStack[i].m_iLine,
                oParser.m_pErrorStack[i].m_strText);
    }
    // oParser.m_vErrorStack.clear();
    oParser.m_iErrorStackSize = 0;
}

void GenerateGraphFromLanguage(FILE* pOutFile, Settings& oSettings)
{
    if (oSettings.m_strLanguage == "cpp")
        GenerateEnodeGraphAsCpp(pOutFile, oSettings.m_pParsedFile.m_pRoot, true);
    else if (oSettings.m_strLanguage == "c")
        GenerateEnodeGraphAsC(pOutFile, oSettings.m_pParsedFile.m_pRoot, true);
}

void ProcessParsedGraph(FILE* pOutFile, Settings& oSettings, Parser& oParser, EnodeCustom oCustom)
{
    // if (oCustom.InitCallback)
    //     oCustom.InitCallback(/*filename, oCustom.user_data*/);
    GenerateGraphFromLanguage(pOutFile, oSettings);
    if (oCustom.ParseCallback && oSettings.m_pParsedFile.m_pRoot)
    {
        for (EnodeNode* pNode = oSettings.m_pParsedFile.m_pRoot; pNode != nullptr; pNode = pNode->m_pNext) {
            if (pNode->m_eType == NodeType::NodeEntityDeclaration && oCustom.ParseEntityCallback)
                oCustom.ParseEntityCallback(pNode, oParser.GetLexer().m_strFilename);
            else if (pNode->m_eType == NodeType::NodeDeclaration && oCustom.ParseDeclarationCallback)
                oCustom.ParseDeclarationCallback(pNode, oParser.GetLexer().m_strFilename);
            else if (pNode->m_eType == NodeType::NodeNamespace && oCustom.ParseNamespaceCallback)
                oCustom.ParseNamespaceCallback(pNode, oParser.GetLexer().m_strFilename);
            else if (pNode->m_eType == NodeType::NodeProcedureHeader && oCustom.ParseProcedureCallback)
                oCustom.ParseProcedureCallback(pNode, oParser.GetLexer().m_strFilename);
            else
                oCustom.ParseCallback(pNode, oParser.GetLexer().m_strFilename);
        }
    }
    PrintAndResetParserErrors(oParser);
}

EnodeNode* ParseFile(Parser& oParser, char *strFileContent, char *strFilename)
{
    oParser.SetFilename(strFilename);
    oParser.Define("Whitespace","[ \t\r\b]+", true);
    oParser.Define("New Line","\n", true);
    oParser.Define("Number","[0-9]+");
    oParser.Define("Identifier","[A-Za-z_]+[_0-9]*");
    oParser.Define("Double Colon","::");
    oParser.Define("Single Colons",":");
    oParser.Define("Open Parenthesis","\\(");
    oParser.Define("Close Parenthesis","\\)");
    oParser.Define("Open Bracket","\\{");
    oParser.Define("Close Bracket","\\}");
    oParser.Define("Semi Colon",";");
    oParser.Define("Comma",",");
    oParser.Define("Star","\\*");
    oParser.Define("Tag","@");
    oParser.Define("Open Square Bracket","\\[");
    oParser.Define("Close Square Bracket","\\]");
    oParser.Define("Arrow","->");
    oParser.Define("String","\".*\"");
    oParser.Define("SingleComment","//.*\n");
    oParser.Define("MultiComment","/\\*.*\\*/");
    EnodeNode* pRroot = oParser.Process(strFileContent);
    // helper::Dump(oParser.GetLexer());
    PrintAndResetParserErrors(oParser);
    return pRroot;
}

int main(int ac, char *av[])
{
	if(ac < 2) {
        Log("USAGE: enode [OPTION] <path-to-enode-file>");
        Log("  OPTION:");
        Log("      [-c | --custom <path to custom layer DLL>]");
        Log("      [-o | --out] <path of the output file>");
        Log("      [--lang] <language code> (c: ANSI-C, cpp: C++)");
        return 84;
    }
    if(!strcmp(av[1], "help") || !strcmp(av[1], "-help") ||
       !strcmp(av[1], "--help") || !strcmp(av[1], "-h") || !strcmp(av[1], "--h"))
    {
        cout << "Enode Flags" << endl;
        cout << "--custom    (-c)        Specify the path to a custom layer to which parsed information is to be sent." << endl;
        cout << "--out       (-o)        The path and name of the output file who will contains the Enode Code" << endl;
        cout << "--lang       ()         Set the language that the code will be converted" << endl;
        return 0;
    }

    EnodeCustom custom = EnodeCustom();
    char *custom_layer_dll_path = 0;
    int expected_number_of_files = 0;
    Settings oSettings;

    // NOTE(rjf): Load command line arguments and set all non-file arguments
    // to zero, so that we know the arguments to process in the file-processing
    // loop.
    {

         for(int i = 1; i < ac; ++i)
         {
             if(av[i]) {
                 if (!strcmp(av[i], "-o") || !strcmp(av[i], "--out")) {
                     // global_log_enabled = 1;
                     oSettings.m_strNameOutFile = av[i + 1];
                     av[i] = 0;
                     av[i + 1] = 0;
                 }

                 else if (!strcmp(av[i], "--lang")) {
                     // global_log_enabled = 1;
                     oSettings.m_strLanguage = av[i + 1];
                     av[i] = 0;
                     av[i + 1] = 0;
                 }
             }
         }

        // NOTE(rjf): Print out command line.
        // if(global_log_enabled)
        // {
        //     for(int i = 0; i < ac; ++i)
        //     {
        //         fprintf(stdout, "%s ", arguments[i]);
        //     }
        // }

        // NOTE(rjf): Find all files and custom DLL stuff.
        {
            int argument_read_mode = 0;
            enum
            {
                ArgumentReadMode_Files,
                ArgumentReadMode_CustomLayerDLL,
            };

            for(int i = 1; i < ac; ++i)
            {
                if(av[i])
                {
                    if(argument_read_mode == ArgumentReadMode_Files)
                    {
                        if(av[i] && (!strcmp(av[i], "-c") || !strcmp(av[i], "--custom")))
                        {
                            argument_read_mode = ArgumentReadMode_CustomLayerDLL;
                            av[i] = nullptr;
                        }
                        if(av[i] && (!strcmp(av[i], "-l") || !strcmp(av[i], "--log")))
                        {
                            av[i] = 0;
                        }
                        else
                        {
                            expected_number_of_files++;
                        }
                    }
                    else if(argument_read_mode == ArgumentReadMode_CustomLayerDLL)
                    {
                        custom_layer_dll_path = av[i];
                        av[i] = nullptr;
                        argument_read_mode = ArgumentReadMode_Files;
                    }
                }
            }
        }
    }

    LogColor(Bold(Blue), "Enode v1.0\n");

    // Load custom code DLL if needed.
    if(custom_layer_dll_path)
    {
        LogColor(Bold(White), "Loading custom layer from ");
        LogColor(Bold(Yellow), custom_layer_dll_path << ".\n");
        custom = EnodeCustomLoad(custom_layer_dll_path);
    }
    else
        LogColor("\033[0;33m", "WARNING: No custom layer loaded.\n");

    if(custom.InitCallback)
        custom.InitCallback();

    // size_t lMemorySize = 1024ULL * 1024 * 1024; //1GB
    size_t lMemorySize = 107374182; //107MB
    void* pProgramMemory = malloc(lMemorySize);
    LinearAllocator* pLinearAllocator;

    if(pProgramMemory == nullptr)
    {
        cout << "Allocation Memory Failed" << endl;
        return (84);
    }

    pLinearAllocator = new (pProgramMemory)LinearAllocator(lMemorySize - sizeof(LinearAllocator),
                                                    PointerMath::Add(pProgramMemory, sizeof(LinearAllocator)));
    Parser oParser = Parser(*pLinearAllocator);
    int iNumberOfParsedFiles = 0;

    ParsedFile* pParsedFiles = NewArray<ParsedFile>(*pLinearAllocator, expected_number_of_files);

    assert(pParsedFiles != nullptr);

    for(int i = 1; i < ac; ++i)
    {
        if(av[i] != nullptr)
        {
            char *strFilename = av[i];
            LogColor(Bold(Blue), "Processing file at ");
            LogColor(Bold(Yellow), strFilename << ".\n");
            char *strFileContent = LoadEntireFile(*pLinearAllocator, strFilename);

            if(strFileContent)
            {
                EnodeNode* pRoot = ParseFile(oParser, strFileContent, strFilename);
                pParsedFiles[iNumberOfParsedFiles].m_pRoot = pRoot;
                pParsedFiles[iNumberOfParsedFiles].m_strFilename = strFilename;
                ++iNumberOfParsedFiles;
            }
            else
                LogError("ERROR: Could not load " << strFilename <<".");
        }
    }
    FILE* pOut = fopen(oSettings.m_strNameOutFile, "w");
    for(int i = 0; i < iNumberOfParsedFiles; ++i) {
        oSettings.m_pParsedFile = pParsedFiles[i];
        ProcessParsedGraph(pOut, oSettings, oParser, custom);
    }

    if(custom.CleanUpCallback)
        custom.CleanUpCallback();

    if(custom_layer_dll_path)
        EnodeCustomUnload(&custom);

    free(pProgramMemory);
    return 0;
}