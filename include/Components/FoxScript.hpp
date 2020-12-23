
#pragma once

#include <string>
#include "json_struct.h"
#include "Parser.h"
#include "vm.hpp"

class VM;
class Callable;

class FoxScript
{
public:
    explicit FoxScript() { }
    explicit FoxScript(const std::string& strFile)
    {
        m_strFilename = strFile;
    }

    std::string m_strFilename;
    VM* m_pVm;
    Callable OnUpdate;

    JS_OBJECT(JS_MEMBER(m_strFilename));
};