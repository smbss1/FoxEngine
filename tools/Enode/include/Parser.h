
#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "MyAllocator.h"
#include "LinearAllocator.h"
#include "ParserHelper.h"

using namespace std;

class Lexer;
class Token;

enum class EnodeBinaryOpType
{
    Invalid,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulus,
    BitshiftLeft,
    BitshiftRight,
    BitwiseAnd,
    BitwiseOr,
    BooleanAnd,
    BooleanOr,
    Max
};

enum class EnodeUnaryOpType
{
    Invalid,
    Negative,
    Not,
    BitwiseNegate,
    Max
};

enum class NodeType {
    NodeIdentifier,
    NodeEnumIdentifier,
    NodeDeclaration,
    NodeTypeUsage,
    NodeEntityDeclaration,
    NodeTag,
    NodeConstantDefinition,
    NodeNamespace,
    NodeNumber,
    NodeString,
    NodeChar,
    NodeBinaryOperator,
    NodeUnaryOperator,
    NodeProcedureHeader,
};

struct EnodeNode
{
public:
    EnodeNode();
    ~EnodeNode();

    EnodeNode* m_pNext;
    NodeType m_eType;
    int m_iLength;
    EnodeNode* m_pFirstTag;
    EnodeNode* m_pNamespaceNode;
    std::string m_strNamespace;
    std::string m_strText;

    union {
        struct {
            EnodeNode* m_pDeclaration;
        } Identifier;

        struct {
            EnodeUnaryOpType m_eUnaryOpType;
            EnodeNode* m_pOperand;
        } UnaryOperator;

        struct {
            EnodeBinaryOpType m_eBinaryOpType;
            EnodeNode* m_pLeft;
            EnodeNode* m_pRight;
        } BinaryOperator;

        struct {
            std::string m_strClass;
            std::string m_strInherit;
            EnodeNode* m_pFirstMember;
        } EntityDeclaration;

        struct {
            EnodeNode* m_pFirstMember;
        } UnionDeclaration;

        struct {
            EnodeNode* m_pFirstConst;
        } EnumDeclaration;

        struct {
            EnodeNode* m_pFirstFlag;
        } FlagsDeclaration;

        struct {
            EnodeNode* m_pType;
            EnodeNode* m_pinitialization;
        } Declaration;

        struct {
            int m_iPtrCount;
            EnodeNode* m_pFirstArraySizeExpression;
            EnodeNode* m_pStructDeclaration;
            EnodeNode* m_pUnionDeclaration;
            EnodeNode* m_pTypeDefinition;
        } TypeUsage;

        struct {
            EnodeNode* m_pFirstTagParam;
        } Tag;

        struct {
            EnodeNode* m_pExpression;
        } ConstantDefinition;

        struct {
            EnodeNode* m_pReturnType;
            EnodeNode* m_pFirstParam;
        } ProcedureHeader;
    };
};

struct ParseError
{
    ParseError() {};
    ParseError(char* strText, char* strFilename, int iLine) { m_strText = strText; m_strFilename = strFilename; m_iLine = iLine; };
    char* m_strText;
    int m_iLine;
    char* m_strFilename;
};

class Parser : public ParserHelper
{
public:
    Allocator& m_oAllocator;
    int m_iErrorStackSize;
    int m_iErrorStackMax;
    vector<ParseError> m_vErrorStack;
    ParseError* m_pErrorStack;
    EnodeNode* m_pTagStackHead;

    Parser(Allocator& oAllocator);
    ~Parser();

private:
    EnodeNode* ParseCode();
    EnodeNode* ParseDeclarationBody(const Token& oName);
    EnodeNode* ParseDeclarationList();
    EnodeNode* ParseStructDeclarationList();
    EnodeNode* ParseStructBody(const Token& oName);
    EnodeNode* ParseTypeUsage();
    EnodeNode* ParseUnionBody(const Token& oName);
    EnodeNode* ParseEnumBody(const Token& oName);
    EnodeNode* ParseIdentifierList();
    int BinaryOpPrecedence(int iType);
    EnodeNode* ParseExpression_p(int iPrecedenceIn);
    EnodeNode* ParseExpression();
    int UnaryOpPrecedence(int iType);
    EnodeNode* ParseUnarySetExpr(Token oToken, NodeType eType);
    EnodeNode* ParseUnaryExpression();
    void PushTag(EnodeNode* pTagNode);
    EnodeNode *PopAllTag();
    void ParseTagParams(EnodeNode* pTagNode);
    void ParseTagList();
    EnodeNode* ParseNamespace(const Token& oName);
    EnodeNode* ParseProcedureHeaderBody(const Token& oName);
    EnodeNode* AllocateNode(NodeType eType, Token oToken);
    EnodeNode* AllocateNode(NodeType eType);
    EnodeNode* ParseConstBody(const Token& name);
    void PushError(char *msg, ...);

public:
    EnodeNode *Process(const string &strText);

    void SetFilename(const string &strFiilename);

    void SetFilename(char *strFilename);

    EnodeNode *ParseEntityDeclarationList();

    EnodeNode *ParseEntityBody(const Token &oClass, const Token &oName, const Token& oInherit = Token());
};