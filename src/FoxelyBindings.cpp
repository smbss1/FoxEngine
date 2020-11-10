
#include "FoxEngine.hpp"
#include "foxely.h"
#include "FoxelyBindings.h"

using namespace Fox;

// -----------------------
//      Bind ECS
// -----------------------

Value PrintGO(VM* pVM, int argc, Value* args)
{
    Fox_FixArity(pVM, argc, 0);
    Value oName = Fox_GetInstanceField(pVM, args[-1], "m_strName");
    std::cout << "GameObject Name: ";
    PrintValue(oName);
    std::cout << std::endl;
    return NIL_VAL;
}

void InitBindings(World& oWorld, VM* pVM)
{
    NativeMethods oGameObjectMethods = 
    {
        std::make_pair<std::string, NativeFn>("init", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            Fox_Arity(pVM, argc, 0, 1);
            Entity iEntity = oWorld.CreateEntity();
            Fox_SetInstanceField(pVM, args[-1], "m_iId", Fox_NumberToValue(iEntity));
            if (argc == 0)
                Fox_SetInstanceField(pVM, args[-1], "m_strName", Fox_StringToValue(pVM, "GameObject"));
            else
                Fox_SetInstanceField(pVM, args[-1], "m_strName", Fox_StringToValue(pVM, Fox_ValueToCString(args[0])));
            return args[-1];
        }),

        std::make_pair<std::string, NativeFn>("add", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Entity iEntity = Fox_ValueToInteger(Fox_GetInstanceField(pVM, args[-1], "m_iId"));
            ObjectInstance* pComponent = Fox_ValueToInstance(args[0]);
            oWorld.AddComponent(iEntity, pComponent->klass->name->string.c_str(), pComponent);
            return NIL_VAL;
        }),

        std::make_pair<std::string, NativeFn>("get", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Entity iEntity = Fox_ValueToInteger(Fox_GetInstanceField(pVM, args[-1], "m_iId"));
            return oWorld.GetComponent(iEntity, Fox_ValueToCString(args[0]));
        }),

        std::make_pair<std::string, NativeFn>("printName", PrintGO),
        
    };

    pVM->DefineModule("entity");
    pVM->DefineClass("entity", "GameObject", oGameObjectMethods);
}