#include <iostream>
#include "Enode.h"
#include "Graph.h"

using namespace std;

FILE* global_source;
FILE* global_header;

void generate_print_code(FILE* fp, EnodeNode * root, char *acc_str);
void GeneratePrint(EnodeNode * root);
void GenerateSetter(EnodeNode* pRoot);

void GenerateTekHeader(FILE* fp, char *fdname)
{
    fprintf(fp, "/*\n** EPITECH PROJECT, 2019\n");
    fprintf(fp, "** %s\n", fdname);
    fprintf(fp, "** File description:\n** %s\n*/\n\n", fdname);
}

ENODE_FUNC void EnodeCustomInitCallback(void)
{
    global_source = fopen("src/FoxEngine.cpp", "w");
    global_header = fopen("include/FoxEngine.hpp", "w");
    GenerateTekHeader(global_source, "FoxEngine");
    GenerateTekHeader(global_header, "FoxEngine");
    fprintf(global_header, "#pragma once\n\n");
    // fprintf(global_source, "#include \"FoxEngine.hpp\"\n\n");
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
    if (EnodeNodeHasTag(pRoot, "printable"))
        GeneratePrint(pRoot);
    if (EnodeNodeHasTag(pRoot, "set"))
        GenerateSetter(pRoot);
    
    if (EnodeNodeHasTag(pRoot, "Public"))
    {
        std::cout << "jnbhb" << std::endl;
        // fprintf(global_header, "public:\n");
    }
}

ENODE_FUNC void EnodeCustomCleanUpCallback(void)
{
    fclose(global_header);
    fclose(global_source);
}


// ----------------------------------
// Generate Printable Functions
// ----------------------------------


static bool second_check_type(FILE* fp, EnodeNode* pNode, char *acc_str)
{
    if (EnodeStringHasSubstring("char", (char *) pNode->Declaration.m_pType->m_strText.c_str())
        && pNode->Declaration.m_pType->TypeUsage.m_pFirstArraySizeExpression) {
        fprintf(fp, "    printf(\"%%s\", %s%s);\n", acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeDeclarationIsType(pNode, "char")) {
        fprintf(fp, "    printf(\"%%c\", %s%s);\n", acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeDeclarationIsType(pNode, "*char")) {
        fprintf(fp, "    printf(\"%%s\", %s%s);\n", acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeDeclarationIsType(pNode, "*void")) {
        fprintf(fp, "    printf(\"%%p\", %s%s);\n", acc_str, pNode->m_strText.c_str());
        return (true);
    }
    return (false);
}

static bool
first_check_type(FILE* fp, EnodeNode* pNode, char *acc_str)
{
    if (EnodeDeclarationIsType(pNode, "int") || EnodeDeclarationIsType(pNode, "uint")) {
        fprintf(fp, "    printf(\"%%d\", %s%s);\n", acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeDeclarationIsType(pNode, "double") || EnodeDeclarationIsType(pNode, "float") || EnodeDeclarationIsType(pNode, "f64")) {
        fprintf(fp, "    printf(\"%%f\", %s%s);\n", acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (EnodeStringHasSubstring("char", (char *) pNode->Declaration.m_pType->m_strText.c_str())
        && pNode->Declaration.m_pType->TypeUsage.m_pFirstArraySizeExpression) {
        fprintf(fp, "    printf(\"%%s\", %s%s);\n", acc_str, pNode->m_strText.c_str());
        return (true);
    }
    if (second_check_type(fp, pNode, acc_str))
        return (true);
    return (false);
}

void
generate__print_code(FILE* fp, EnodeNode* pNode, char *acc_str)
{
    if (!first_check_type(fp, pNode, acc_str)) {
        char next_acc_str[255] = { 0 };
        int cond = pNode->Declaration.m_pType->TypeUsage.m_iPtrCount == 1;
        if (pNode->Declaration.m_pType->TypeUsage.m_pTypeDefinition &&
            pNode->Declaration.m_pType->TypeUsage.m_iPtrCount <= 1) {
            EnodeNode *pFirst_member =
                pNode->Declaration.m_pType->TypeUsage.m_pTypeDefinition->EntityDeclaration.m_pFirstMember;
            sprintf(next_acc_str, "%s%s%s", acc_str, pNode->m_strText,
                       cond ? "->" : ".");
            generate_print_code(fp, pFirst_member, next_acc_str);
        } else if (pNode->Declaration.m_pType->TypeUsage.m_pStructDeclaration &&
                   pNode->Declaration.m_pType->TypeUsage.m_iPtrCount <= 1) {
            EnodeNode *first_memb =
                pNode->Declaration.m_pType->TypeUsage.m_pStructDeclaration->EntityDeclaration.m_pFirstMember;
            sprintf(next_acc_str, "%s%s%s", acc_str, pNode->m_strText, cond ? "->" : ".");
            generate_print_code(fp, first_memb, next_acc_str);
        }
    }
    fprintf(fp, pNode->m_pNext ? "    printf(\", \");\n" : "");
}

void generate_print_code(FILE* fp, EnodeNode * root, char *acc_str)
{
    fprintf(fp, "    printf(\"{ \");\n");
    for (EnodeNode * node = root; node; node = node->m_pNext) {
        if (!EnodeNodeHasTag(node, "noprint") &&
            node->m_eType == NodeType::NodeDeclaration) {
            generate__print_code(fp, node, acc_str);
        }
    }
    fprintf(fp, "    printf(\" }\");\n");
}

void GeneratePrint(EnodeNode * root)
{
    fprintf(global_header, "void print_%s_%s", root->m_strNamespace.c_str(), root->m_strText.c_str());
    fprintf(global_header, "(%s_%s *obj);\n", root->m_strNamespace.c_str(), root->m_strText.c_str());
    fprintf(global_source, "void print_%s_%s", root->m_strNamespace.c_str(), root->m_strText.c_str());
    fprintf(global_source, "(%s_%s *obj)\n{\n", root->m_strNamespace.c_str(), root->m_strText.c_str());
    generate_print_code(global_source, root->EntityDeclaration.m_pFirstMember, "obj->");
    fprintf(global_source, "}\n\n");
}

// ---------------------------------------
//      Setter Code
// ---------------------------------------


void
generate__setter_code(EnodeNode* pNode, char *acc_str)
{
    char space[255] = "    ";
    if (pNode->Declaration.m_pType->TypeUsage.m_iPtrCount <= 1) {
        EnodeNode *param = GetEnodeNodeTag(pNode, "param");
        EnodeNode *tag = GetEnodeNodeTag(pNode, "set");
        EnodeNode *e = EnodeGetTagParamater(tag, 0);
        char *name = (char *) pNode->m_strText.c_str();
        fprintf(global_source, "%s(*obj)->%s = malloc(sizeof((*obj)->%s) * %s);\n", space, name, name, e ? e->m_strText : "1");
        fprintf(global_source, "%sif ((*obj)->%s)\n", space, name);
        if (!param)
            fprintf(global_source, "%s%smy_memset((*obj)->%s, 0, %s((*obj)->%s));\n", space, space, name, "sizeof", name);
        else
            fprintf(global_source, "%s%smy_memset((*obj)->%s, %s, %s((*obj)->%s));\n", space, space, name, name, "sizeof", name);
    }
}

void
generate_setter_code(EnodeNode* pRoot, char *acc_str)
{
    for (EnodeNode* pNode = pRoot; pNode; pNode = pNode->m_pNext) {
        if (EnodeNodeHasTag(pNode, "Set") && pNode->m_eType == NodeType::NodeDeclaration) {
            generate__setter_code(pNode, acc_str);
        } else if (EnodeNodeHasTag(pNode, "param") && pNode->m_eType == NodeType::NodeDeclaration) {
            fprintf(global_source, "    (*obj)->%s = %s;\n", pNode->m_strText.c_str(), pNode->m_strText.c_str());
        }
    }
    fprintf(global_source, "    return (true);\n}\n\n");
}

static void
generate__procedure(EnodeNode* pNode)
{
    int ptr_count = pNode->Declaration.m_pType->TypeUsage.m_iPtrCount;
    fprintf(global_source, ", %s ", pNode->Declaration.m_pType->m_strText.c_str());
    fprintf(global_header, ", %s ", pNode->Declaration.m_pType->m_strText.c_str());
    for (int i = 0; i < ptr_count; i++) {
        fprintf(global_source, "*");
        fprintf(global_header, "*");
    }
    fprintf(global_source, "%s", pNode->m_strText.c_str());
    fprintf(global_header, "%s", pNode->m_strText.c_str());
}

void
generate_procedure(EnodeNode* pRoot)
{
    for (EnodeNode* pNode = pRoot; pNode; pNode = pNode->m_pNext) {
        if (EnodeNodeHasTag(pNode, "param") && pNode->m_eType == NodeType::NodeDeclaration) {
            generate__procedure(pNode);
        }
    }
    fprintf(global_source, "){\n");
    fprintf(global_header, ");\n");
}


void
generate__destroyer_code(EnodeNode* pNode)
{
    char space[255] = "    ";
    if (pNode->Declaration.m_pType->TypeUsage.m_iPtrCount <= 1) {
        fprintf(global_source, "%sfree(obj->%s);\n", space, pNode->m_strText.c_str());
    }
}

void
generate_destroyer_code(EnodeNode* root)
{
    for (EnodeNode* pNode = root; pNode; pNode = pNode->m_pNext) {
        if (EnodeNodeHasTag(pNode, "set") && pNode->m_eType == NodeType::NodeDeclaration) {
            generate__destroyer_code(pNode);
        }
    }
    fprintf(global_source, "    free(obj);\n}\n\n");
}


void GenerateSetter(EnodeNode* pRoot)
{

    fprintf(global_header, "bool init_%s_%s(%s_%s **obj",
             pRoot->m_strNamespace.c_str(), pRoot->m_strText.c_str(), pRoot->m_strNamespace.c_str(), pRoot->m_strText.c_str());
    fprintf(global_source, "bool init_%s_%s(%s_%s **obj",
             pRoot->m_strNamespace.c_str(), pRoot->m_strText.c_str(), pRoot->m_strNamespace.c_str(), pRoot->m_strText.c_str());
    generate_procedure(pRoot->EntityDeclaration.m_pFirstMember);
    fprintf(global_source, "    *obj = malloc(sizeof(**obj));\n");
    fprintf(global_source, "    if (!*obj)\n        return (false);\n");
    fprintf(global_source, "    my_memset(*obj, 0, sizeof(**obj));\n");
    generate_setter_code(pRoot->EntityDeclaration.m_pFirstMember, "obj->");
    fprintf(global_header, "void destroy_%s_%s", pRoot->m_strNamespace.c_str(), pRoot->m_strText.c_str());
    fprintf(global_header, "(%s_%s *obj);\n", pRoot->m_strNamespace.c_str(), pRoot->m_strText.c_str());
    fprintf(global_source, "void destroy_%s_%s", pRoot->m_strNamespace.c_str(), pRoot->m_strText.c_str());
    fprintf(global_source, "(%s_%s *obj)\n{\n", pRoot->m_strNamespace.c_str(), pRoot->m_strText.c_str());
    generate_destroyer_code(pRoot->EntityDeclaration.m_pFirstMember);
}

// ---------------------------------------
//      Constructor Code
// ---------------------------------------
