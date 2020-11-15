
#include "Engine.hpp"
#include "foxely.h"
#include "FoxelyBindings.h"
#include "Components/Transform.hpp"

using namespace Fox;

// -----------------------
//   Engine Function
// -----------------------

void AddEngineBinding(VM* pVM)
{
    NativeMethods oDebugMethods = 
    {
        std::make_pair<std::string, NativeFn>("log", [] (VM* pVM, int argc, Value* args)
        {
            Fox_Arity(pVM, argc, 1, 255);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Debug.log require a string.");
            std::cout << "\033[0;37m" << "[LOG] " << Fox_AsCString(args[0]);
            for (int i = 1; i < argc; i++)
                PrintValue(args[i]);
            std::cout << "\033[0m" << std::endl;
            return Fox_Nil;
        }),

        std::make_pair<std::string, NativeFn>("logError", [] (VM* pVM, int argc, Value* args)
        {
            Fox_Arity(pVM, argc, 1, 255);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Debug.logError require a string.");
            std::cout << "\033[0;31m" << "[ERROR] " << Fox_AsCString(args[0]);
            for (int i = 1; i < argc; i++)
                PrintValue(args[i]);
            std::cout << "\033[0m" << std::endl;
            return Fox_Nil;
        }),

        std::make_pair<std::string, NativeFn>("logWarning", [] (VM* pVM, int argc, Value* args)
        {
            Fox_Arity(pVM, argc, 1, 255);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Debug.logWarning require a string.");
            std::cout << "\033[0;33m" << "[WARNING] " << Fox_AsCString(args[0]);
            for (int i = 1; i < argc; i++)
                PrintValue(args[i]);
            std::cout << "\033[0m" << std::endl;
            return Fox_Nil;
        }),

        std::make_pair<std::string, NativeFn>("logInfo", [] (VM* pVM, int argc, Value* args)
        {
            Fox_Arity(pVM, argc, 1, 255);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Debug.logInfo require a string.");
            std::cout << "\033[0;36m" << "[INFO] " << Fox_AsCString(args[0]);
            for (int i = 1; i < argc; i++)
                PrintValue(args[i]);
            std::cout << "\033[0m" << std::endl;
            return Fox_Nil;
        }),
    };

    pVM->DefineLib("fox", "Debug", oDebugMethods);
}


// -----------------------
//      Bind ECS
// -----------------------

void AddVec2Class(World& oWorld, VM* pVM)
{
    NativeMethods oVec2Methods = 
    {
        std::make_pair<std::string, NativeFn>("init", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            if (argc == 0)
            {
                Fox_SetField(pVM, args[-1], "x", Fox_Double(0));
                Fox_SetField(pVM, args[-1], "y", Fox_Double(0));
            }
            else
            {
                Fox_FixArity(pVM, argc, 2);
                Fox_SetField(pVM, args[-1], "x", args[0]);
                Fox_SetField(pVM, args[-1], "y", args[1]);
            }

            Fox_Getter(pVM, Fox_AsInstance(args[-1]), "x", [](VM* pVM, int argc, Value* args)
            {
                Vec2* pPosition = (Vec2 *) Fox_GetUserData(args[-1]);
                return Fox_Double(pPosition->x);
            });

            Fox_Getter(pVM, Fox_AsInstance(args[-1]), "y", [](VM* pVM, int argc, Value* args)
            {
                Vec2* pPosition = (Vec2 *) Fox_GetUserData(args[-1]);
                return Fox_Double(pPosition->y);
            });

            Fox_Setter(pVM, Fox_AsInstance(args[-1]), "x", [](VM* pVM, int argc, Value* args)
            {
                Vec2* pPosition = (Vec2 *) Fox_GetUserData(args[-1]);
                pPosition->x = Fox_AsDouble(args[0]);
                return Fox_Double(pPosition->x);
            });

            Fox_Setter(pVM, Fox_AsInstance(args[-1]), "y", [](VM* pVM, int argc, Value* args)
            {
                Vec2* pPosition = (Vec2 *) Fox_GetUserData(args[-1]);
                pPosition->y = Fox_AsDouble(args[0]);
                return Fox_Double(pPosition->y);
            });
            return args[-1];
        }),
    };

    pVM->DefineClass("fox", "Vec2", oVec2Methods);
}

// -----------------------
//   Transform Function
// -----------------------

void AddTransformClass(World& oWorld, VM* pVM)
{
    NativeMethods oTransformMethods = 
    {
        std::make_pair<std::string, NativeFn>("init", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 0);

            Fox_SetField(pVM, args[-1], "position", Fox_Double(0));
            Fox_SetField(pVM, args[-1], "rotation", Fox_Double(0));
            Fox_SetField(pVM, args[-1], "scale", Fox_Double(0));

            Fox_Getter(pVM, Fox_AsInstance(args[-1]), "position", [](VM* pVM, int argc, Value* args)
            {
                Transform* pTransform = (Transform *) Fox_GetUserData(args[-1]);
                return Fox_DefineInstanceOfCStruct(pVM, "fox", "Vec2", &pTransform->position);
            });

            Fox_Getter(pVM, Fox_AsInstance(args[-1]), "rotation", [](VM* pVM, int argc, Value* args)
            {
                Transform* pTransform = (Transform *) Fox_GetUserData(args[-1]);
                return Fox_DefineInstanceOfCStruct(pVM, "fox", "Vec2", &pTransform->rotation);
            });

            Fox_Getter(pVM, Fox_AsInstance(args[-1]), "scale", [](VM* pVM, int argc, Value* args)
            {
                Transform* pTransform = (Transform *) Fox_GetUserData(args[-1]);
                return Fox_DefineInstanceOfCStruct(pVM, "fox", "Vec2", &pTransform->scale);
            });
            return args[-1];
        }),
    };

    pVM->DefineClass("fox", "Transform", oTransformMethods);
}

// -----------------------
//   GameObject Function
// -----------------------

Value PrintGO(VM* pVM, int argc, Value* args)
{
    Fox_FixArity(pVM, argc, 0);
    Value oName = Fox_GetField(pVM, args[-1], "m_strName");
    std::cout << "GameObject Name: ";
    PrintValue(oName);
    std::cout << std::endl;
    return Fox_Nil;
}

void InitBindings(World& oWorld, VM* pVM)
{
    NativeMethods oGameObjectMethods = 
    {
        std::make_pair<std::string, NativeFn>("init", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            Fox_Arity(pVM, argc, 0, 1);
            Entity iEntity = oWorld.CreateEntity();
            Fox_SetField(pVM, args[-1], "m_iId", Fox_Double(iEntity));
            if (argc == 0)
                Fox_SetField(pVM, args[-1], "m_strName", Fox_String(pVM, "GameObject"));
            else
                Fox_SetField(pVM, args[-1], "m_strName", Fox_String(pVM, Fox_AsCString(args[0])));
            return args[-1];
        }),

        std::make_pair<std::string, NativeFn>("add", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Entity iEntity = Fox_AsInt(Fox_GetField(pVM, args[-1], "m_iId"));
            ObjectInstance* pComponent = Fox_AsInstance(args[0]);
            if (pComponent->klass->name->string == "Transform")
            {
                oWorld.AddComponent<Transform>(iEntity, {.position = Vec2(0, 0)});
            }
            return Fox_Nil;
        }),

        std::make_pair<std::string, NativeFn>("get", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Entity iEntity = Fox_AsInt(Fox_GetField(pVM, args[-1], "m_iId"));

            if (!strcmp(Fox_AsCString(args[0]), "Transform"))
            {
                return Fox_DefineInstanceOfCStruct(pVM, "fox", "Transform", &oWorld.GetComponent<Transform>(iEntity));
            }
            return Fox_Nil;
        }),

        std::make_pair<std::string, NativeFn>("printName", PrintGO),
        
    };

    pVM->DefineModule("fox");
    pVM->DefineClass("fox", "GameObject", oGameObjectMethods);

    AddVec2Class(oWorld, pVM);
    AddTransformClass(oWorld, pVM);
    AddEngineBinding(pVM);
}