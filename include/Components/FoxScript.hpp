#pragma once

#include <string>

class VM;

struct FoxScript
{
    std::string strFilename;
    VM m_pVm;
};