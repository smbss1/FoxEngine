
#pragma once

#if MSVC2013_C
#define ENODE_HEADER_PROC static
#else
#define ENODE_HEADER_PROC static inline
#endif

ENODE_HEADER_PROC void GenerateEnodeGraphAsCpp(FILE* fp, EnodeNode* pRoot, bool follow_next);
ENODE_HEADER_PROC void GenerateEnodeGraphAsCpp_(FILE* fp, EnodeNode* pRoot, bool follow_next, int iNest);

// ------------------------------------
//      Generate Tag for Debug
// ------------------------------------

ENODE_HEADER_PROC void GenerateEnodeTagGraphAsCpp(FILE* fp, EnodeNode* pRoot, int iNest)
{
    for (EnodeNode* pTag = pRoot->m_pFirstTag; pTag; pTag = pTag->m_pNext) {
        fprintf(fp, "%s", pTag->m_strText.c_str());
        if (pTag->Tag.m_pFirstTagParam) {
            fprintf(fp, "(");
            // Generate Tag Parameters
            for (EnodeNode* pTagArg = pTag->Tag.m_pFirstTagParam; pTagArg; pTagArg = pTagArg->m_pNext) {
                GenerateEnodeGraphAsCpp_(fp, pTagArg, 0, iNest + 1);
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


ENODE_HEADER_PROC void GenerateEnodeUnaryAsCpp(FILE* fp, EnodeNode* pRoot, int iNest)
{
    fprintf(fp, "(");
    char *u_op_str = EnodeGetUnaryOpString((int) pRoot->UnaryOperator.m_eUnaryOpType);
    fprintf(fp, "%s", u_op_str);
    fprintf(fp, "(");
    GenerateEnodeGraphAsCpp_(fp, pRoot->UnaryOperator.m_pOperand, 0, iNest + 1);
    fprintf(fp, "))");
}

// ------------------------------------
//      Generate Binary
// ------------------------------------

ENODE_HEADER_PROC void GenerateEnodeBinaryAsCpp(FILE* fp, EnodeNode* pRoot, int iNest)
{
    fprintf(fp, "(");
    GenerateEnodeGraphAsCpp_(fp, pRoot->BinaryOperator.m_pLeft, 0, iNest);
    char *op_str = EnodeGetBinaryOpString((int) pRoot->BinaryOperator.m_eBinaryOpType);
    fprintf(fp, " %s ", op_str);
    GenerateEnodeGraphAsCpp_(fp, pRoot->BinaryOperator.m_pRight, 0, iNest + 1);
    fprintf(fp, ")");
}

// ------------------------------------
//      Generate Type Usage
// ------------------------------------

ENODE_HEADER_PROC void GenerateEnodeTypeUsageAsCpp(FILE* fp, EnodeNode* pRoot, int iNest)
{
    string strSpaces;

    if (pRoot->TypeUsage.m_pStructDeclaration) {
        GenerateEnodeGraphAsCpp_(fp, pRoot->TypeUsage.m_pStructDeclaration, 0, iNest + 1);
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

ENODE_HEADER_PROC void GenerateEnodeDeclarationAsCpp(FILE* fp, EnodeNode* pRoot, int iNest)
{
    GenerateEnodeGraphAsCpp_(fp, pRoot->Declaration.m_pType, false, iNest + 1);
    fprintf(fp, "%s", pRoot->m_strText.c_str());
    for (EnodeNode* pArray = pRoot->Declaration.m_pType->TypeUsage.m_pFirstArraySizeExpression; pArray; pArray = pArray->m_pNext) {
        fprintf(fp, "[");
        GenerateEnodeGraphAsCpp_(fp, pArray, 0, iNest);
        fprintf(fp, "]");
    }
}

ENODE_HEADER_PROC void GenerateEnodeEntityDeclarationAsCpp(FILE* fp, EnodeNode* pRoot, int iNest)
{
    std::string strSpaces = string(m_iIndentLevel * 4, ' ');

    fprintf(fp, "\n");
    if (!pRoot->m_strText.empty()) {
        char* strNamespace = (char *) pRoot->m_strNamespace.c_str();
        fprintf(fp, "%s%s ", strSpaces.c_str(), pRoot->EntityDeclaration.m_strClass.c_str());
        fprintf(fp, "%s", pRoot->m_strText.c_str());
        if (!pRoot->EntityDeclaration.m_strInherit.empty())
            fprintf(fp, " : %s", pRoot->EntityDeclaration.m_strInherit.c_str());
        fprintf(fp, "\n");
    } else
        fprintf(fp, "%s%s\n", strSpaces.c_str(), pRoot->EntityDeclaration.m_strClass.c_str());
    fprintf(fp, "%s{\n", strSpaces.c_str());
    m_iIndentLevel++;
    fprintf(fp, "public:\n");
    for (EnodeNode* pMember = pRoot->EntityDeclaration.m_pFirstMember; pMember; pMember = pMember->m_pNext) {
        GenerateEnodeGraphAsCpp_(fp, pMember, false, iNest + 1);
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

ENODE_HEADER_PROC void GenerateEnodeGraphAsCpp_(FILE* fp, EnodeNode* pRoot, bool follow_next, int iNest)
{
    if (pRoot->m_pFirstTag) {
#ifdef DEBUG
        fprintf(fp, "// ");
        GenerateEnodeTagGraphAsCpp(fp, pRoot, iNest);
        fprintf(fp, "\n");
#endif
    }
    if (pRoot->m_eType == NodeType::NodeIdentifier || pRoot->m_eType == NodeType::NodeString ||
        pRoot->m_eType == NodeType::NodeNumber || pRoot->m_eType == NodeType::NodeChar) {
        fprintf(fp, "%s", pRoot->m_strText.c_str());
    }
    else if (pRoot->m_eType == NodeType::NodeEnumIdentifier)
    {
        std::string strSpaces = string(m_iIndentLevel * 4, ' ');
        fprintf(fp, "%s%s,\n", strSpaces.c_str(), pRoot->m_strText.c_str());
    }
    else if(pRoot->m_eType == NodeType::NodeUnaryOperator)
        GenerateEnodeUnaryAsCpp(fp, pRoot, iNest);
    else if(pRoot->m_eType == NodeType::NodeBinaryOperator)
        GenerateEnodeBinaryAsCpp(fp, pRoot, iNest);
    else if(pRoot->m_eType == NodeType::NodeEntityDeclaration)
        GenerateEnodeEntityDeclarationAsCpp(fp, pRoot, iNest);
    else if(pRoot->m_eType == NodeType::NodeDeclaration)
        GenerateEnodeDeclarationAsCpp(fp, pRoot, iNest);
    else if(pRoot->m_eType == NodeType::NodeTypeUsage)
        GenerateEnodeTypeUsageAsCpp(fp, pRoot, iNest);
    else if(pRoot->m_eType == NodeType::NodeNamespace) {
        fprintf(fp, "\nnamespace %s\n{", pRoot->m_strText.c_str());
        m_iIndentLevel++;
        EnodeNode* pNode;
        for (pNode = pRoot->m_pNext; pNode && pNode->m_eType != NodeType::NodeNamespace; pNode = pNode->m_pNext)
            GenerateEnodeGraphAsCpp(fp, pNode, false);
        fprintf(fp, "} // namespace %s\n", pRoot->m_strText.c_str());
        m_iIndentLevel--;
        if (pNode != nullptr)
            GenerateEnodeGraphAsCpp(fp, pNode, true);
        return;
    }
    else if(pRoot->m_eType == NodeType::NodeConstantDefinition) {
        fprintf(fp, "#define %s (", pRoot->m_strText.c_str());
        GenerateEnodeGraphAsCpp(fp, pRoot->ConstantDefinition.m_pExpression, 0);
        fprintf(fp, ")\n");
    }
    else if (pRoot->m_eType == NodeType::NodeProcedureHeader)
    {
        std::string strSpaces = string(m_iIndentLevel * 4, ' ');
        fprintf(fp, "\n");
        if(pRoot->ProcedureHeader.m_pReturnType)
            GenerateEnodeGraphAsCpp_(fp, pRoot->ProcedureHeader.m_pReturnType, 0, iNest);
        else
            fprintf(fp, "%s", strSpaces.c_str());
        

        fprintf(fp, "%s(", pRoot->m_strText.c_str());
        if(pRoot->ProcedureHeader.m_pFirstParam)
        {
            for(EnodeNode* pParameter = pRoot->ProcedureHeader.m_pFirstParam; pParameter; pParameter = pParameter->m_pNext)
            {
                GenerateEnodeGraphAsCpp_(fp, pParameter, 0, -1);
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
    if (pRoot->m_pNext && follow_next)
        GenerateEnodeGraphAsCpp_(fp, pRoot->m_pNext, follow_next, iNest);
}

ENODE_HEADER_PROC void GenerateEnodeGraphAsCpp(FILE* fp, EnodeNode* pRoot, bool follow_next)
{
    GenerateEnodeGraphAsCpp_(fp, pRoot, follow_next, 0);
}