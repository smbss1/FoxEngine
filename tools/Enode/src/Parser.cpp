
#include <iostream>
#include <stdarg.h>

#include "Parser.h"
#include "PointerMath.h"

using namespace std;

// -------------------------------------------
//      Constructeur / Destructeur
// -------------------------------------------

/**
 * @brief EnodeNode Constructeur
 */
EnodeNode::EnodeNode() { m_pNext = NULL; }

/**
 * @brief EnodeNode Destructeur
 */
EnodeNode::~EnodeNode() { }

/**
 * @brief Parser Constructeur
 */
Parser::Parser(Allocator& oAllocator) : m_oAllocator(oAllocator)
{
    m_iErrorStackMax = 16;
    m_vErrorStack = vector<ParseError>(m_iErrorStackMax);
    m_pErrorStack = NULL;
    m_iErrorStackSize = 0;
    m_iErrorStackMax = 16;
}

/**
 * @brief Destructeur
 */
Parser::~Parser() { }


// -------------------------------------------
//      Memory
// -------------------------------------------

/**
 * @brief Cette méthode alloue de la mémoire pour un node
 * @return un node alloué dans la mémoire dynamique
 */
EnodeNode* Parser::AllocateNode(NodeType eType, Token oToken)
{
    EnodeNode* node = New<EnodeNode>(m_oAllocator);
    node->m_eType = eType;
    node->m_strText = oToken.GetText();
    node->m_iLength = oToken.m_iLength;
    return node;
}

/**
 * @brief Cette méthode alloue de la mémoire pour un node
 * @return un node alloué dans la mémoire dynamique
 */
EnodeNode* Parser::AllocateNode(NodeType eType)
{
    EnodeNode* node = New<EnodeNode>(m_oAllocator);
    node->m_eType = eType;
    return node;
}

/**
 * @brief Cette méthode ajoute une erreur dans la stack avec le message correspond à l'erreur
 */
void Parser::PushError(char *msg, ...)
{
    if(!m_pErrorStack)
    {
        m_pErrorStack = NewArray<ParseError>(m_oAllocator, m_iErrorStackMax);
    }

    if (m_iErrorStackSize < m_iErrorStackMax) {
        va_list args;
        va_start(args, msg);
        unsigned int msg_bytes = vsnprintf(0, 0, msg, args) + 1;
        va_end(args);
        va_start(args, msg);
        char *stored_msg = NewArray<char>(m_oAllocator, msg_bytes);
        assert(stored_msg != nullptr);
        vsnprintf(stored_msg, msg_bytes, msg, args);
        va_end(args);

        ParseError oError;

        Token oPrevToken = *(GetLexer().oTokenIterator - 2);
        oError = ParseError(stored_msg, oLexer.m_strFilename, oPrevToken.m_iLinesTraversed);
        m_pErrorStack[m_iErrorStackSize++] = oError;
    }
}



// -------------------------------------------
//      Parsing
// -------------------------------------------

/**
 * @brief Cette méthode permet de parser les constantes
 * @return une node contenant les données de la Constante Parsé
 * @note <CONSTANTE_NAME>:<TYPE>
 * Example: SOME_CONSTANT::16
 * Dans le fichier C : #define SOME_CONSTANT(16)
 */
EnodeNode* Parser::ParseConstBody(const Token& oName)
{
    EnodeNode* newNode = AllocateNode(NodeType::NodeConstantDefinition, oName);
    newNode->ConstantDefinition.m_pExpression = ParseExpression();
    if (!IsToken(";"))
        PushError("Expected ';'.");
    return (newNode);
}

/**
 * @brief Cette méthode permet de parser les déclarations
 * @return une node contenant les données de la Declaration Parsé
 * @note <DECLARATION_NAME>:<TYPE>
 * Example: MyVar : float;
 * Dans le fichier C : float MyVar;
 */
EnodeNode* Parser::ParseDeclarationBody(const Token& oName)
{
    EnodeNode *pRoot = AllocateNode(NodeType::NodeDeclaration, oName);
    pRoot->Declaration.m_pType = ParseTypeUsage();
    pRoot->m_pFirstTag = PopAllTag();
    return pRoot;
}

/**
 * @brief Cette méthode permet de parser une liste des déclarations
 * @return une liste de node contenant les données de chaques Declaration parsé
 */
EnodeNode* Parser::ParseDeclarationList()
{
    EnodeNode* pRoot = nullptr, **target = &pRoot;
    Token name = Token();

    for (Token curr = CurrentToken(); ; curr = CurrentToken()) {
        if (oLexer.TokenMatch(curr, "}") || oLexer.TokenMatch(curr, ")"))
            break;
        ParseTagList();
        if (IsTokenThenAssign("Identifier", name) && IsToken(":")) {
            EnodeNode* declaration = ParseDeclarationBody(name);
            *target = declaration;
            target = &(*target)->m_pNext;
            if (!oLexer.TokenMatch(CurrentToken(), "}") && !oLexer.TokenMatch(CurrentToken(), ")"))
            {
                if (!IsToken(";") && !IsToken(","))
                {
                    PushError("Expected ';' or ',' after declaration.");
                    break;
                }
            }
        }
        else {
            PushError("Expected declaration (<name> : <type>).");
            break;
        }
    }
    return pRoot;
}

/**
 * @brief Cette méthode permet de parser un type comme le type du variable
 * @return une node contenant les données du type Parsé
 */
EnodeNode* Parser::ParseTypeUsage()
{
    EnodeNode *pType = nullptr, *pStructDeclaration = nullptr, *pUnionDeclaration = nullptr;
    int iPtrCount = 0;
    Token oTypeName;

    for (; IsToken("*"); ++iPtrCount);
    Token oClass;
    if (IsToken("Identifier", "as") && IsTokenThenAssign("Identifier", oClass)) {
        pStructDeclaration = ParseEntityBody(oClass, Token());
    } else {
        if (!IsTokenThenAssign("Identifier", oTypeName)) {
            PushError("Missing type name.\n");
            return nullptr;
        }
    }
    pType = AllocateNode(NodeType::NodeTypeUsage, oTypeName);
    pType->TypeUsage.m_iPtrCount = iPtrCount;
    pType->TypeUsage.m_pStructDeclaration = pStructDeclaration;
    pType->TypeUsage.m_pUnionDeclaration = pUnionDeclaration;
    pType->TypeUsage.m_pTypeDefinition = nullptr;

    EnodeNode** pArraySizeTarget = &pType->TypeUsage.m_pFirstArraySizeExpression;

    for(;;)
    {
        if(IsToken("["))
        {
            *pArraySizeTarget = ParseExpression();
            pArraySizeTarget = &(*pArraySizeTarget)->m_pNext;

            if(!IsToken("]"))
            {
                PushError("Missing ]");
                break;
            }
        }
        else
            break;
    }
    return pType;
}

/**
 * @brief Cette méthode permet de recuperer la precedance(groupe) du type Binaire
 * @return un chiffre représentant le groupe du type Binaire qu'il appartient
 */
int Parser::BinaryOpPrecedence(int iType)
{
    int iPrecedence = 0;
    int iPrecedenceTable[(int) EnodeBinaryOpType::Max] = {
        0,
        1, 1,
        2, 2, 2,
        3, 3,
        4, 4,
        5, 5,
    };
    if (iType >= 0 && iType < (int) EnodeBinaryOpType::Max) {
        iPrecedence = iPrecedenceTable[iType];
    }
    return iPrecedence;
}

char* GetBinaryOpStrFromType(int iType)
{
    char* pBinaryOperatorStrings[] = {
        "INVALID_BINARY_OPERATOR",
        "+", "-", "*", "/", "%",
        "<<", ">>",
        "&", "|", "&&", "||",
    };

    return pBinaryOperatorStrings[iType];
}

int GetBinaryOpTypeFromToken(Lexer& oLexer, Token& oToken)
{
    int iType = (int) EnodeBinaryOpType::Invalid;

    for (int i = 1; i < (int) EnodeBinaryOpType::Max; ++i) {
        if (oLexer.TokenMatch(oToken, GetBinaryOpStrFromType(i))) {
            iType = i;
            break;
        }
    }
    return iType;
}

/**
 * @brief Cette méthode permet de parser une expression
 * @return une node contenant l'expression parsee
 */
EnodeNode* Parser::ParseExpression_p(int iPrecedenceIn)
{
    EnodeNode *pExpression = ParseUnaryExpression();
    if (!pExpression)
        return pExpression;
    Token oToken = CurrentToken();
    EnodeBinaryOpType eOpType = (EnodeBinaryOpType) GetBinaryOpTypeFromToken(oLexer, oToken);
    if (eOpType != EnodeBinaryOpType::Invalid) {
        for (int iPrecedence = BinaryOpPrecedence((int) eOpType); iPrecedence >= iPrecedenceIn; --iPrecedence) {
            for (Token oToken = CurrentToken(); ; oToken = CurrentToken()) {
                eOpType = (EnodeBinaryOpType) GetBinaryOpTypeFromToken(oLexer, oToken);
                if (BinaryOpPrecedence((int) eOpType) != iPrecedence)
                    break;
                if (eOpType == EnodeBinaryOpType::Invalid)
                    break;
                NextToken();
                EnodeNode *right = ParseExpression_p(iPrecedence + 1);
                if (!right) {
                    PushError("Expected right-hand-side of binary expression.");
                    return (pExpression);
                }
                EnodeNode *pExistingExpression = pExpression;
                pExpression = AllocateNode(NodeType::NodeBinaryOperator);
                pExpression->BinaryOperator.m_eBinaryOpType = eOpType;
                pExpression->BinaryOperator.m_pLeft = pExistingExpression;
                pExpression->BinaryOperator.m_pRight = right;
            }
        }
    }
    return pExpression;
}

/**
 * @brief Cette méthode permet de parser une expression
 * @return une node contenant l'expression parsee
 * @note Exemple d'expression: (6 + 6), (5 - 7) * 4, 255 << 1, etc...
 */
EnodeNode* Parser::ParseExpression()
{
    return ParseExpression_p(1);
}

/**
 * @brief Cette méthode permet de recuperer la precedance(groupe) du type Unary
 * @return un chiffre représentant le groupe du type Unary qu'il appartient
 */
int Parser::UnaryOpPrecedence(int iType)
{
    int iPrecedence = 0;

    int iPrecedenceTable[(int) EnodeUnaryOpType::Max] = {
        0,
        6,
        6,
        6,
    };
    if (iType >= 0 && iType < (int) EnodeUnaryOpType::Max) {
        iPrecedence = iPrecedenceTable[iType];
    }
    return iPrecedence;
}

/**
 * @brief Cette méthode permet de initialiser le node de l'expression Unary
 * @return une node contenant l'expression initialiser
 */
EnodeNode* Parser::ParseUnarySetExpr(Token oToken, NodeType eType)
{
    NextToken();
    EnodeNode* pExpression = AllocateNode(eType, oToken);
    return pExpression;
}

/**
 * @brief Cette méthode permet de parser une expression unary
 * @return une node contenant l'expression parsee
 * @note Exemple d'expression unary: (6 + 6), (5 - 7) * 4
 */
EnodeNode* Parser::ParseUnaryExpression()
{
    EnodeNode* pExpression = 0;
    Token oToken = CurrentToken();

    if (oLexer.TokenMatch(oToken, "(")) {
        NextToken();
        pExpression = ParseExpression();
        if (!IsToken(")"))
            PushError("Missing ')'.");
    }
    else if (oToken.m_oType == "Number") {
        pExpression = ParseUnarySetExpr(oToken, NodeType::NodeNumber);
    }
    else if (oToken.m_oType == "Identifier") {
        pExpression =
            ParseUnarySetExpr(oToken, NodeType::NodeIdentifier);
    }
    else if (oToken.m_oType == "String") {
        pExpression = ParseUnarySetExpr(oToken, NodeType::NodeString);
    }
    else if (oToken.m_oType == "Char") {
        pExpression = ParseUnarySetExpr(oToken, NodeType::NodeChar);
    }
    return pExpression;
}

/**
 * @brief Cette méthode permet d'ajouter un tag dans la liste
 * @param pTagNode C'est le tag qui sera ajouter
 */
void Parser::PushTag(EnodeNode* pTagNode)
{
    pTagNode->m_pNext = m_pTagStackHead;
    m_pTagStackHead = pTagNode;
}

/**
 * @brief Cette méthode permet de recuperer toute la liste de tag
 * @return une liste de tag
 */
EnodeNode* Parser::PopAllTag()
{
    EnodeNode* pTagsHead = m_pTagStackHead;
    m_pTagStackHead = nullptr;
    return pTagsHead;
}

/**
 * @brief Cette méthode permet de parser les parametres d'un tag
 * @param pTagNode C'est le tag qui contient les parametres
 * @note Example: @Parameter(P1, P2, P3, ...)
 */
void Parser::ParseTagParams(EnodeNode* pTagNode)
{
    if (IsToken("(")) {
        EnodeNode **pParamStore = &pTagNode->Tag.m_pFirstTagParam;
        for (;;) {
            EnodeNode *pParam = ParseExpression();
            pParam ? *pParamStore = pParam : 0;
            pParam ? pParamStore = &(*pParamStore)->m_pNext : 0;
            if (IsToken(")"))
                break;
            if (!IsToken(","))
                PushError("Expected ','.");
        }
    }
}

/**
 * @brief Cette méthode permet de parser une liste de tag
 */
void Parser::ParseTagList()
{
    for (;;) {
        Token oTag;
        EnodeNode *pTagNode = nullptr;

        if (IsToken("Tag", "@")) {
            if (IsTokenThenAssign("Identifier", oTag)) {
                pTagNode = AllocateNode(NodeType::NodeTag, oTag);
                ParseTagParams(pTagNode);
                PushTag(pTagNode);
            } else {
                PushError("Expected Tag Name");
            }
        } else
            break;
    }
}

/**
 * @brief Cette méthode permet de parser un namespace
 * @return une node contenant le namespace parsee
 */
EnodeNode* Parser::ParseNamespace(const Token& oName)
{
    EnodeNode *pRoot = AllocateNode(NodeType::NodeNamespace, oName);
    pRoot->m_pFirstTag = PopAllTag();

    if (!IsToken(";")) {
        PushError("Expected ';'.");
        return pRoot;
    }
    return pRoot;
}

/**
 * @brief Cette méthode permet de parser une procedure
 * @return une node contenant la procedure parsee
 * @note <FUNC_NAME>::(<PARAMETER(S)>) -> <RETURN_TYPE>;
 */
EnodeNode* Parser::ParseProcedureHeaderBody(const Token& oName)
{
    EnodeNode* pRoot = AllocateNode(NodeType::NodeProcedureHeader, oName);

    EnodeNode* pDeclarationList = 0;
    EnodeNode* pReturnType = 0;

    if(!IsToken("("))
    {
        PushError("Expected '('.");
        goto end_parse;
    }

    pDeclarationList = ParseDeclarationList();

    if(!IsToken(")"))
    {
        PushError("Expected ')'.");
        goto end_parse;
    }

    if(IsToken("->"))
        pReturnType = ParseTypeUsage();

    if(pReturnType)
        pRoot->ProcedureHeader.m_pReturnType = pReturnType;

    pRoot->ProcedureHeader.m_pFirstParam = pDeclarationList;

    end_parse:;
    return pRoot;
}

/**
 * @brief Cette méthode permet de parser une liste des valeurs contenus dans une Entity
 * @return une liste de node contenant les données de chaques membres parsé de l'Entity
 */
EnodeNode* Parser::ParseEntityDeclarationList()
{
    EnodeNode* pRoot = nullptr, **target = &pRoot;
    Token name;

    for (Token curr = CurrentToken(); ; curr = CurrentToken()) {
        if (oLexer.TokenMatch(curr, "}") || oLexer.TokenMatch(curr, ")"))
            break;
        ParseTagList();
        if (IsTokenThenAssign("Identifier", name)) {
            if (IsToken(":"))
            {
                EnodeNode* declaration = ParseDeclarationBody(name);
                *target = declaration;
                target = &(*target)->m_pNext;
                if (CurrentToken().m_oType == "Identifier")
                    continue;
                if (!oLexer.TokenMatch(CurrentToken(), "}") && !oLexer.TokenMatch(CurrentToken(), ")"))
                {
                    if (!IsToken(";") && !IsToken(","))
                    {
                        PushError("Expected ';' or ',' after declaration.");
                        break;
                    }
                }
            }
            else if (IsToken("::") && IsToken("func"))
            {
                EnodeNode* procedure = ParseProcedureHeaderBody(name);
                *target = procedure;
                target = &(*target)->m_pNext;
                if (!oLexer.TokenMatch(CurrentToken(), "}") && !oLexer.TokenMatch(CurrentToken(), ")"))
                {
                    if (!IsToken(";") && !IsToken(","))
                    {
                        PushError("Expected ';' or ',' after procedure.");
                        break;
                    }
                }
            }
            else if (IsToken(","))
            {
                EnodeNode* pIdentifier = AllocateNode(NodeType::NodeEnumIdentifier, name);
                pIdentifier->m_pFirstTag = PopAllTag();
                *target = pIdentifier;
                target = &(*target)->m_pNext;
            }
        }
        else {
            PushError("Expected declaration (<name> : <type>).");
            break;
        }
    }
    return pRoot;
}

/**
 * @brief Cette méthode permet de parser une Entity
 * @return une node contenant les données de l'Entity Parsé
 * @note <ENTITY_NAME> as <ENTITY_TYPE>
 * Example 1: MyStructB as struct { ... }
 * Dans le fichier C : struct MyStructB { ... }
 * @note <ENTITY_TYPE>: the keyword of the object like in C++ is 'class' and in C is 'struct' or Java is 'class'
 */
EnodeNode* Parser::ParseEntityBody(const Token& oClass, const Token& oName, const Token& oInherit)
{
    EnodeNode *pRoot = AllocateNode(NodeType::NodeEntityDeclaration, oName);
    pRoot->EntityDeclaration.m_strClass = oClass.GetText().c_str();
    if (!oInherit.GetText().empty())
        pRoot->EntityDeclaration.m_strInherit = oInherit.GetText().c_str();
    pRoot->m_pFirstTag = PopAllTag();

    if (!IsToken("{")) {
        PushError("Expected '{'.");
        return pRoot;
    }
    pRoot->EntityDeclaration.m_pFirstMember = ParseEntityDeclarationList();
    if (!IsToken("}")) {
        PushError("Expected '}'.");
        return pRoot;
    }
    if (!IsToken(";")) {
        PushError("Expected ';'.");
        return pRoot;
    }
    return pRoot;
}

/**
 * @brief Cette méthode permet de parser le code
 * @return une node selon la donnee(struct, constant, etc..) parsee
 */
EnodeNode* Parser::ParseCode()
{
    EnodeNode* newNode = nullptr;
    Token oName;

    if (IsToken("namespace")) {
        IsTokenThenAssign("Identifier", oName);
        newNode = ParseNamespace(oName);
    }
    else if (IsTokenThenAssign("Identifier", oName)) {
        if (IsToken("::")) {
            if(IsToken("func"))
                newNode = ParseProcedureHeaderBody(oName);
            else
                newNode = ParseConstBody(oName);
        }
        else if (IsToken(":"))
            newNode = ParseDeclarationBody(oName);
        else if (IsToken("Identifier", "as"))
        {
            Token oClass;
            Token oInherit;
            if (!IsTokenThenAssign("Identifier", oClass)) {
                PushError("Expected Class Type.");
                return (newNode);
            }
            if (IsToken("inherit")) {
                if (!IsTokenThenAssign("Identifier", oInherit)) {
                    PushError("Expected Inherit Class Name.");
                    return (newNode);
                }
            }
            newNode = ParseEntityBody(oClass, oName, oInherit);
        }
    }
    return (newNode);
}

/**
 * @brief Cette méthode est la boucle principale
 * @return le code parsé sous forme d'AST(Abstract Syntax Tree)
 */
EnodeNode* Parser::Process(const std::string& strText)
{
    EnodeNode* pRoot = nullptr;
    EnodeNode** pNodeStoreTarget = &pRoot;
    EnodeNode* pNamespaceNode = nullptr;

    if (!Init(strText))
        return nullptr;
    while (GetLexer().oTokenIterator != GetLexer().oTokenList.end())
    {
        Token oToken = CurrentToken();
        EnodeNode* newNode;

        ParseTagList();
        newNode = ParseCode();
        if (newNode && newNode->m_eType == NodeType::NodeNamespace)
            pNamespaceNode = newNode;
        if (newNode && pNamespaceNode && newNode->m_eType != NodeType::NodeNamespace) {
            newNode->m_pNamespaceNode = pNamespaceNode;
            newNode->m_strNamespace = pNamespaceNode->m_strText;
        }
        if (newNode != nullptr) {
            *pNodeStoreTarget = newNode;
            pNodeStoreTarget = &(*pNodeStoreTarget)->m_pNext;
        } else
            break;
    }
    return (pRoot);
}

void Parser::SetFilename(const std::string& strFilename)
{
    oLexer.m_strFilename = (char *) strFilename.c_str();
}

void Parser::SetFilename(char* strFilename)
{
    oLexer.m_strFilename = strFilename;
}