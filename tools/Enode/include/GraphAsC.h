
#pragma once

#if MSVC2013_C
#define ENODE_HEADER_PROC static
#else
#define ENODE_HEADER_PROC static inline
#endif

ENODE_HEADER_PROC void GenerateEnodeGraphAsC(FILE* fp, EnodeNode* pRoot, bool follow_next);
ENODE_HEADER_PROC void GenerateEnodeGraphAsC_(FILE* fp, EnodeNode* pRoot, bool follow_next, int iNest);
ENODE_HEADER_PROC void GenerateEnodeGraphAsC__(FILE* fp, EnodeNode* pRoot, bool follow_next, int iNest);

// ------------------------------------
//      Generate Tag for Debug
// ------------------------------------

ENODE_HEADER_PROC void GenerateEnodeTagGraph(FILE* fp, EnodeNode* pRoot, int iNest)
{
    for (EnodeNode* pTag = pRoot->m_pFirstTag; pTag; pTag = pTag->m_pNext) {
        fprintf(fp, "%s", pTag->m_strText.c_str());
        if (pTag->Tag.m_pFirstTagParam) {
            fprintf(fp, "(");
            // Generate Tag Parameters
            for (EnodeNode* pTagArg = pTag->Tag.m_pFirstTagParam; pTagArg; pTagArg = pTagArg->m_pNext) {
                GenerateEnodeGraphAsC_(fp, pTagArg, 0, iNest + 1);
                if (pTagArg->m_pNext)
                    fprintf(fp, ", ");
            }
            fprintf(fp, ")");
        }
        fprintf(fp, " ");
    }
}

// ------------------------------------
//      Generate Unary
// ------------------------------------

char* EnodeGetUnaryOpString(int type)
{
    char *strings[] = {
        "(invalid)",
        "-",
        "!",
        "~",
    };
    return strings[type];
}


ENODE_HEADER_PROC void GenerateEnodeUnary(FILE* fp, EnodeNode* pRoot, int iNest)
{
    fprintf(fp, "(");
    char *u_op_str = EnodeGetUnaryOpString((int) pRoot->UnaryOperator.m_eUnaryOpType);
    fprintf(fp, "%s", u_op_str);
    fprintf(fp, "(");
    GenerateEnodeGraphAsC_(fp, pRoot->UnaryOperator.m_pOperand, 0, iNest + 1);
    fprintf(fp, "))");
}

// ------------------------------------
//      Generate Binary
// ------------------------------------

ENODE_HEADER_PROC void GenerateEnodeBinary(FILE* fp, EnodeNode* pRoot, int iNest)
{
    fprintf(fp, "(");
    GenerateEnodeGraphAsC_(fp, pRoot->BinaryOperator.m_pLeft, 0, iNest);
    char *op_str = EnodeGetBinaryOpString((int) pRoot->BinaryOperator.m_eBinaryOpType);
    fprintf(fp, " %s ", op_str);
    GenerateEnodeGraphAsC_(fp, pRoot->BinaryOperator.m_pRight, 0, iNest + 1);
    fprintf(fp, ")");
}

// ------------------------------------
//      Generate Type Usage
// ------------------------------------

ENODE_HEADER_PROC void GenerateEnodeTypeUsage(FILE* fp, EnodeNode* pRoot, int iNest)
{
    string strSpaces;
    // static int iBracketCount = 1;

    if (pRoot->TypeUsage.m_pStructDeclaration) {
        GenerateEnodeGraphAsC_(fp, pRoot->TypeUsage.m_pStructDeclaration, 0, iNest + 1);
    } else {
        if (iNest > 0)
            strSpaces = string(m_iIndentLevel * 4, ' ');
        else
            strSpaces = string("");
        fprintf(fp, "%s%s ", strSpaces.c_str(), pRoot->m_strText.c_str());
    }
    for (int i = 0; i < pRoot->TypeUsage.m_iPtrCount; ++i)
        fprintf(fp, "*");
}

// ------------------------------------
//      Generate Declaration
// ------------------------------------

ENODE_HEADER_PROC void GenerateEnodeDeclaration(FILE* fp, EnodeNode* pRoot, int iNest)
{
    GenerateEnodeGraphAsC_(fp, pRoot->Declaration.m_pType, false, iNest + 1);
    fprintf(fp, "%s", pRoot->m_strText.c_str());
    for (EnodeNode* pArray = pRoot->Declaration.m_pType->TypeUsage.m_pFirstArraySizeExpression; pArray; pArray = pArray->m_pNext) {
        fprintf(fp, "[");
        GenerateEnodeGraphAsC_(fp, pArray, 0, iNest);
        fprintf(fp, "]");
    }
}

ENODE_HEADER_PROC void GenerateEnodeEntityDeclaration(FILE* fp, EnodeNode* pRoot, int iNest)
{
    string strSpaces;
    // static int iBracketCount = 0;

//    if (iNest == 0 && !pRoot->m_strText.empty()) {
//        char* strNamespace = (char *) pRoot->m_strNamespace.c_str();
//        if (!pRoot->m_strNamespace.empty())
//            fprintf(fp, "\ntypedef struct %s_%s %s_%s;\n", strNamespace, pRoot->m_strText.c_str(), strNamespace, pRoot->m_strText.c_str());
//        else
//            fprintf(fp, "\ntypedef struct %s %s;\n", pRoot->m_strText.c_str(), pRoot->m_strText.c_str());
//    }
    strSpaces = string(m_iIndentLevel * 4, ' ');
    fprintf(fp, "\n");
    if (!pRoot->m_strText.empty()) {
        char* strNamespace = (char *) pRoot->m_strNamespace.c_str();
        fprintf(fp, "%s%s ", strSpaces.c_str(), pRoot->EntityDeclaration.m_strClass.c_str());
        if (!pRoot->m_strNamespace.empty())
            fprintf(fp, "%s_", strNamespace);
        fprintf(fp, "%s", pRoot->m_strText.c_str());
        fprintf(fp, "\n");
    } else
        fprintf(fp, "%s%s\n", strSpaces.c_str(), pRoot->EntityDeclaration.m_strClass.c_str());
    fprintf(fp, "%s{\n", strSpaces.c_str());
    m_iIndentLevel++;
    for (EnodeNode* pMember = pRoot->EntityDeclaration.m_pFirstMember; pMember; pMember = pMember->m_pNext) {
        GenerateEnodeGraphAsC_(fp, pMember, false, iNest + 1);
        if (pMember->m_eType != NodeType::NodeEnumIdentifier)
            fprintf(fp, ";\n");
    }
    if (iNest == 0) {
        fprintf(fp, "%s}", strSpaces.c_str());
        fprintf(fp, ";\n");
    } else
        fprintf(fp, "%s} ", strSpaces.c_str());
    m_iIndentLevel--;
}

// ------------------------------------
//      Generate Graph
// ------------------------------------

ENODE_HEADER_PROC void GenerateEnodeGraphAsC__(FILE* fp, EnodeNode* pRoot, bool follow_next, int iNest)
{
    if (!pRoot)
        return;
    if(pRoot->m_eType == NodeType::NodeEntityDeclaration)
        GenerateEnodeEntityDeclaration(fp, pRoot, iNest);
    else if(pRoot->m_eType == NodeType::NodeDeclaration)
        GenerateEnodeDeclaration(fp, pRoot, iNest);
    else if(pRoot->m_eType == NodeType::NodeTypeUsage)
        GenerateEnodeTypeUsage(fp, pRoot, iNest);
    else if(pRoot->m_eType == NodeType::NodeConstantDefinition) {
        fprintf(fp, "#define %s (", pRoot->m_strText.c_str());
        GenerateEnodeGraphAsC(fp, pRoot->ConstantDefinition.m_pExpression, 0);
        fprintf(fp, ")\n");
    }
    else if (pRoot->m_eType == NodeType::NodeProcedureHeader)
    {
        std::string strSpaces = string(m_iIndentLevel * 4, ' ');
        fprintf(fp, "\n%s", strSpaces.c_str());
        if(pRoot->ProcedureHeader.m_pReturnType)
            GenerateEnodeGraphAsC_(fp, pRoot->ProcedureHeader.m_pReturnType, 0, iNest);
        else
            fprintf(fp, "void ");
        fprintf(fp, "%s(", pRoot->m_strText.c_str());
        if(pRoot->ProcedureHeader.m_pFirstParam)
        {
            for(EnodeNode* pParameter = pRoot->ProcedureHeader.m_pFirstParam; pParameter; pParameter = pParameter->m_pNext)
            {
                GenerateEnodeGraphAsC_(fp, pParameter, 0, -1);
                if(pParameter->m_pNext)
                    fprintf(fp, ", ");
            }
        }
        else
            fprintf(fp, "void");
        fprintf(fp, ")");
        if (iNest == 0)
            fprintf(fp, ";\n");
    }
}

ENODE_HEADER_PROC void GenerateEnodeGraphAsC_(FILE* fp, EnodeNode* pRoot, bool follow_next, int iNest)
{
    if (pRoot->m_pFirstTag) {
#ifdef DEBUG
        fprintf(fp, "// ");
        GenerateEnodeTagGraph(fp, pRoot, iNest);
        fprintf(fp, "\n");
#endif
    }
    if (pRoot->m_eType == NodeType::NodeIdentifier || pRoot->m_eType == NodeType::NodeString ||
        pRoot->m_eType == NodeType::NodeNumber || pRoot->m_eType == NodeType::NodeChar)
        fprintf(fp, "%s", pRoot->m_strText.c_str());
    else if (pRoot->m_eType == NodeType::NodeEnumIdentifier)
    {
        std::string strSpaces = string(m_iIndentLevel * 4, ' ');
        fprintf(fp, "%s%s,\n", strSpaces.c_str(), pRoot->m_strText.c_str());
    }
    else if(pRoot->m_eType == NodeType::NodeUnaryOperator)
        GenerateEnodeUnary(fp, pRoot, iNest);
    else if(pRoot->m_eType == NodeType::NodeBinaryOperator)
        GenerateEnodeBinary(fp, pRoot, iNest);
    GenerateEnodeGraphAsC__(fp, pRoot, follow_next, iNest);
    if (pRoot->m_pNext && follow_next)
        GenerateEnodeGraphAsC_(fp, pRoot->m_pNext, follow_next, iNest);
}

ENODE_HEADER_PROC void GenerateEnodeGraphAsC(FILE* fp, EnodeNode* pRoot, bool follow_next)
{
    GenerateEnodeGraphAsC_(fp, pRoot, follow_next, 0);
}