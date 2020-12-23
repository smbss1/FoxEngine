#include <iostream>
#include "Enode.h"
#include "Graph.h"

using namespace std;

FILE* global_source;
FILE* global_header;

void GenerateSerialize(EnodeNode * root);
void GenerateSerializeCode(FILE* fp, EnodeNode* pNode, char *acc_str);

void GenerateTekHeader(FILE* fp, char *fdname)
{
    fprintf(fp, "/*\n** EPITECH PROJECT, 2019\n");
    fprintf(fp, "** %s\n", fdname);
    fprintf(fp, "** File description:\n** %s\n*/\n\n", fdname);
}

ENODE_FUNC void EnodeCustomInitCallback(void)
{
    global_source = fopen("src/Serialize.cpp", "w");
    global_header = fopen("include/Serialize.hpp", "w");
    GenerateTekHeader(global_source, "Serialize");
    GenerateTekHeader(global_header, "Serialize");
    fprintf(global_header, "#pragma once\n\n");
    fprintf(global_header, "#include <SFML/Graphics.hpp>\n");
    fprintf(global_header, "using namespace sf;\n\n");
    fprintf(global_source, "#include \"Serialize.hpp\"\n");
    fprintf(global_source, "#include \"jsonxx.h\"\n\n");
}

ENODE_FUNC void EnodeCustomParseCallback(EnodeNode* pRoot, char *strFilename)
{
    // GenerateEnodeGraphAsCpp(global_header, pRoot, false);
}

ENODE_FUNC void EnodeCustomParseNamespaceCallback(EnodeNode* pRoot, char *strFilename)
{
    GenerateEnodeGraphAsCpp(global_header, pRoot, false);
}

ENODE_FUNC void EnodeCustomParseDeclarationCallback(EnodeNode* pRoot, char *strFilename)
{
    // GenerateEnodeGraphAsCpp(global_header, pRoot, false);
}

ENODE_FUNC void EnodeCustomParseEntityCallback(EnodeNode* pRoot, char *strFilename)
{
    GenerateEnodeGraphAsCpp(global_header, pRoot, false);
    if (EnodeNodeHasTag(pRoot, "Serialize"))
        GenerateSerialize(pRoot);
}

ENODE_FUNC void EnodeCustomCleanUpCallback(void)
{
    fclose(global_header);
    fclose(global_source);
}


// ----------------------------------
// Generate Printable Functions
// ----------------------------------

static bool IsSimpleType(FILE* fp, EnodeNode* pNode, char *acc_str)
{
    if (EnodeDeclarationIsType(pNode, "int") || EnodeDeclarationIsType(pNode, "uint")) {
        fprintf(fp, "    oSerializer << \"%s\" << %s%s;\n", pNode->m_strText.c_str(), acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeDeclarationIsType(pNode, "double") || EnodeDeclarationIsType(pNode, "float") || EnodeDeclarationIsType(pNode, "f64")) {
        fprintf(fp, "    oSerializer << \"%s\" << %s%s;\n", pNode->m_strText.c_str(), acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeStringHasSubstring("char", (char *) pNode->Declaration.m_pType->m_strText.c_str())
        && pNode->Declaration.m_pType->TypeUsage.m_pFirstArraySizeExpression) {
        fprintf(fp, "    oSerializer << \"%s\" << %s%s;\n", pNode->m_strText.c_str(), acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeStringHasSubstring("char", (char *) pNode->Declaration.m_pType->m_strText.c_str())
        && pNode->Declaration.m_pType->TypeUsage.m_pFirstArraySizeExpression) {
        fprintf(fp, "    oSerializer << \"%s\" << %s%s;\n", pNode->m_strText.c_str(), acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeDeclarationIsType(pNode, "char")) {
        fprintf(fp, "    oSerializer << \"%s\" << %s%s;\n", pNode->m_strText.c_str(), acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeDeclarationIsType(pNode, "*char")) {
        fprintf(fp, "    oSerializer << \"%s\" << %s%s;\n", pNode->m_strText.c_str(), acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeDeclarationIsType(pNode, "*void")) {
        fprintf(fp, "    oSerializer << \"%s\" << %s%s;\n", pNode->m_strText.c_str(), acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (pNode->Declaration.m_pType->TypeUsage.m_pFirstArraySizeExpression)
    {
        EnodeNode *pFirst = pNode->Declaration.m_pType->TypeUsage.m_pFirstArraySizeExpression;
        char next_acc_str[255] = { 0 };
        int cond = pNode->Declaration.m_pType->TypeUsage.m_iPtrCount > 1;
        sprintf(next_acc_str, "%s%s%s", acc_str, pNode->m_strText.c_str(), cond ? "->" : ".");
        fprintf(fp, "    for (int i = 0; i < sizeof(%s%s)/sizeof(%s%s[0]); ++i){\n", acc_str, pNode->m_strText.c_str(), acc_str, pNode->m_strText.c_str());
        for (EnodeNode* node = pNode->Declaration.m_pType->TypeUsage.m_pFirstArraySizeExpression; node; node = node->m_pNext)
            GenerateSerializeCode(global_source, node, next_acc_str);
        fprintf(fp, "    }\n");
        return (true);
    }
    return (false);
}

void GenerateSerializeCode(FILE* fp, EnodeNode* pNode, char *acc_str)
{
    // If it's not a simple type so it's object (struct or class)
    if (!IsSimpleType(fp, pNode, acc_str)) {
        char next_acc_str[255] = { 0 };
        int cond = pNode->Declaration.m_pType->TypeUsage.m_iPtrCount == 1;
        if (pNode->Declaration.m_pType->TypeUsage.m_pTypeDefinition &&
            pNode->Declaration.m_pType->TypeUsage.m_iPtrCount <= 1) {
            EnodeNode *pFirstMember = pNode->Declaration.m_pType->TypeUsage.m_pTypeDefinition->EntityDeclaration.m_pFirstMember;
            sprintf(next_acc_str, "%s%s%s", acc_str, pNode->m_strText.c_str(), cond ? "->" : ".");
            GenerateSerializeCode(fp, pFirstMember, next_acc_str);
        } else if (pNode->Declaration.m_pType->TypeUsage.m_pStructDeclaration &&
                   pNode->Declaration.m_pType->TypeUsage.m_iPtrCount <= 1) {
            EnodeNode *pFirstMember = pNode->Declaration.m_pType->TypeUsage.m_pStructDeclaration->EntityDeclaration.m_pFirstMember;
            sprintf(next_acc_str, "%s%s%s", acc_str, pNode->m_strText.c_str(), cond ? "->" : ".");
            GenerateSerializeCode(fp, pFirstMember, next_acc_str);
        }
    }
    fprintf(fp, pNode->m_pNext ? "    printf(\", \");\n" : "");
}

void GenerateSerialize(EnodeNode * root)
{
    fprintf(global_header, "void WriteToFile%s(%s *obj);\n", root->m_strText.c_str(), root->m_strText.c_str());
    fprintf(global_source, "void WriteToFile%s(%s *obj)\n{\n", root->m_strText.c_str(), root->m_strText.c_str());
    fprintf(global_source, "    jsonxx::Object oSerializer;\n");

    for (EnodeNode* node = root; node; node = node->m_pNext)
    {
        if (!EnodeNodeHasTag(node, "NoSerialize")) {
            GenerateSerializeCode(global_source, node, "obj->");
        }
    }
    fprintf(global_source, "    printf(\" }\");\n");
    fprintf(global_source, "}\n\n");
}