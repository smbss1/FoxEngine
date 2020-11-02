#if defined(_MSC_VER)
#define ENODE_EXPORT __declspec(dllexport)
#if _MSC_VER < 1900
#define MSVC2013_C 1
#endif
#else
#define ENODE_EXPORT
#endif

#if defined(__cplusplus)
#define ENODE_EXTERN_C extern "C"
#else
#define ENODE_EXTERN_C
#endif

#define ENODE_FUNC ENODE_EXPORT ENODE_EXTERN_C
#define ENODE_PROC ENODE_FUNC

#if MSVC2013_C
#define ENODE_HEADER_PROC static
#else
#define ENODE_HEADER_PROC static inline
#endif

#include "Parser.h"
#include <cstring>

struct EnodeNode;


// static int
// second_check(struct ast_data data, int str_i, int substr_i)
// {
//     if (!strSubstring[substr_i]) {
//         if (substr_i > 0 &&
//             (my_char_isalpha(strSubstring[substr_i - 1]) ||
//              my_char_isnum(strSubstring[substr_i - 1]) ||
//              strSubstring[substr_i - 1] == '_')) {
//             IF_THEN(my_char_isalpha(strString[str_i]) ||
//                     my_char_isnum(strString[str_i]) ||
//                     strString[str_i] == '_', matches = 0);
//         }
//         return (1);
//     }
//     if (strString[str_i] != strSubstring[substr_i]) {
//         matches = 0;
//         return (1);
//     }
//     return (0);
// }

// static int
// first_check(char *str, char *substr, int i)
// {
//     int matches = 0;

//     if (str[i] == substr[0]) {
//         matches = 1;
//         for (int str_i = i + 1, substr_i = 1;; ++str_i, ++substr_i) {
//             struct ast_data data = { str, substr, i, &matches };
//             IF_THEN(second_check(data, str_i, substr_i), break);
//         }
//     }
//     return (matches);
// }

ENODE_HEADER_PROC bool EnodeStringHasAlphanumericBlock(char* strString, char* strSubstring)
{
    bool matches = false;

    if (strString && strSubstring) {
        for (int i = 0; strString[i] && matches != false; i++) {

            if (strString[i] == strSubstring[0]) {
                matches = true;
                for (int str_i = i + 1, substr_i = 1;; ++str_i, ++substr_i) {

                    if (!strSubstring[substr_i]) {
                        if (substr_i > 0 && (isalpha(strSubstring[substr_i - 1]) ||
                            isdigit(strSubstring[substr_i - 1]) || strSubstring[substr_i - 1] == '_')) {
                            if (isalpha(strString[str_i]) || isdigit(strString[str_i]) || strString[str_i] == '_')
                                    matches = false;
                        }
                        break;
                    }
                    if (strString[str_i] != strSubstring[substr_i]) {
                        matches = false;
                        break;
                    }
                }
            }
        }
    }
    return matches;
}

ENODE_HEADER_PROC bool EnodeStringHasSubstring(char* strString, char* strSubstring)
{
    return EnodeStringHasAlphanumericBlock(strString, strSubstring);
}

/**
 * @brief Recherche dans les Tags du node passé pour en trouver une qui correspond à la chaîne passée,
 *          et renvoie un pointeur vers celle-ci, ou 0 si elle n'a pas été trouvée sur le node passé.
 * @param pRoot Le node à vérifier
 * @param strTag le nom associé au Tag, sans le caractère "@".
 */
ENODE_HEADER_PROC EnodeNode* GetEnodeNodeTag(EnodeNode* pRoot, char* strTag)
{
    EnodeNode* pFoundTagNode = 0;

    for (EnodeNode* pTagNode = pRoot->m_pFirstTag; pTagNode; pTagNode = pTagNode->m_pNext) {
        if (EnodeStringHasAlphanumericBlock((char *) pTagNode->m_strText.c_str(), strTag)) {
            pFoundTagNode = pTagNode;
            break;
        }
    }
    return pFoundTagNode;
}

ENODE_HEADER_PROC bool EnodeNodeHasTag(EnodeNode* pNode, char* strTag)
{
    EnodeNode* pTagNode = GetEnodeNodeTag(pNode, strTag);
    return pTagNode != 0;
}

ENODE_HEADER_PROC EnodeNode* EnodeGetTagParamater(EnodeNode* pTag, int iParameterIndex)
{
    EnodeNode *result = 0;
    if (pTag && pTag->m_eType == NodeType::NodeTag) {
        int i = 0;
        for (EnodeNode* pParameter = pTag->Tag.m_pFirstTagParam; pParameter; pParameter = pParameter->m_pNext) {
            if (i == iParameterIndex) {
                result = pParameter;
                break;
            }
            i++;
        }
    }
    return result;
}

// ENODE_HEADER_PROC int count_pointers(char *type, char **type_name)
// {
//     int ptr = 0;

//     for (int i = 0; type[i]; ++i) {
//         if (type[i] == '*') {
//             ++ptr;
//         } else {
//             *type_name = type + i;
//             break;
//         }
//     }
//     return (ptr);
// }

ENODE_HEADER_PROC bool EnodeDeclarationIsType(EnodeNode* pRoot, char* strType)
{
    int matches = 0, iPtrCount = 0;

    if (pRoot->m_eType == NodeType::NodeDeclaration) {
        char* strTypeName = strType;
        for (int i = 0; strType[i]; ++i) {
            if (strType[i] == '*') {
                ++iPtrCount;
            } else {
                strTypeName = strType + i;
                break;
            }
        }
        matches = iPtrCount == pRoot->Declaration.m_pType->TypeUsage.m_iPtrCount;
        if (!matches)
            return (matches);
        matches = 0;
        if (pRoot->Declaration.m_pType->m_strText.empty())
            return (matches);
        matches = 1;
        matches = !strcmp(strTypeName, pRoot->Declaration.m_pType->m_strText.c_str()) ? 1 : 0;
    }
    return matches;
}

ENODE_HEADER_PROC bool EnodeStructMemberIsType(EnodeNode* pRoot, char *strType)
{
    return EnodeDeclarationIsType(pRoot, strType);
}