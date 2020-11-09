#pragma once

#include <string>

class VM;
class Callable;

struct FoxScript
{
    std::string m_strFilename;
    VM* m_pVm;
    Callable OnUpdate;
};