
#pragma once

ENODE_HEADER_PROC int m_iIndentLevel;

char* EnodeGetBinaryOpString(int type)
{
    char *strings[] = {
        "(invalid)",
        "+",
        "-",
        "*",
        "/",
        "%",
        "<<",
        ">>",
        "&",
        "|",
        "&&",
        "||",
    };
    return strings[type];
}

ENODE_HEADER_PROC void GenerateEpitechHeader(FILE* fp, char *fpname)
{
    fprintf(fp, "/*\n** EPITECH PROJECT, 2019\n");
    fprintf(fp, "** %s\n", fpname);
    fprintf(fp, "** File description:\n** %s\n*/\n\n", fpname);
}

#include "GraphAsC.h"
#include "GraphAsCpp.h"