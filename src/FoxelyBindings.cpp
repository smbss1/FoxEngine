
#include "FoxEngine.hpp"
#include "foxely.h"
#include "FoxelyBindings.h"
#include "Components/Transform.hpp"

using namespace Fox;

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
                Fox_SetField(pVM, args[-1], "x", Fox_NumberToValue(0));
                Fox_SetField(pVM, args[-1], "y", Fox_NumberToValue(0));
            }
            else
            {
                Fox_FixArity(pVM, argc, 2);
                Fox_SetField(pVM, args[-1], "x", args[0]);
                Fox_SetField(pVM, args[-1], "y", args[1]);
            }

            Fox_Getter(pVM, AS_INSTANCE(args[-1]), "x", [](VM* pVM, int argc, Value* args)
            {
                Vec2* pPosition = (Vec2 *) Fox_GetUserData(args[-1]);
                return Fox_NumberToValue(pPosition->x);
            });

            Fox_Getter(pVM, AS_INSTANCE(args[-1]), "y", [](VM* pVM, int argc, Value* args)
            {
                Vec2* pPosition = (Vec2 *) Fox_GetUserData(args[-1]);
                return Fox_NumberToValue(pPosition->y);
            });

            Fox_Setter(pVM, AS_INSTANCE(args[-1]), "x", [](VM* pVM, int argc, Value* args)
            {
                Vec2* pPosition = (Vec2 *) Fox_GetUserData(args[-1]);
                pPosition->x = Fox_ValueToNumber(args[0]);
                return Fox_NumberToValue(pPosition->x);
            });

            Fox_Setter(pVM, AS_INSTANCE(args[-1]), "y", [](VM* pVM, int argc, Value* args)
            {
                Vec2* pPosition = (Vec2 *) Fox_GetUserData(args[-1]);
                pPosition->y = Fox_ValueToNumber(args[0]);
                return Fox_NumberToValue(pPosition->y);
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

            Fox_SetField(pVM, args[-1], "position", Fox_NumberToValue(0));
            Fox_SetField(pVM, args[-1], "rotation", Fox_NumberToValue(0));
            Fox_SetField(pVM, args[-1], "scale", Fox_NumberToValue(0));

            Fox_Getter(pVM, AS_INSTANCE(args[-1]), "position", [](VM* pVM, int argc, Value* args)
            {
                Transform* pTransform = (Transform *) Fox_GetUserData(args[-1]);
                return Fox_DefineInstanceOfCStruct(pVM, "fox", "Vec2", &pTransform->position);
            });

            Fox_Getter(pVM, AS_INSTANCE(args[-1]), "rotation", [](VM* pVM, int argc, Value* args)
            {
                Transform* pTransform = (Transform *) Fox_GetUserData(args[-1]);
                return Fox_DefineInstanceOfCStruct(pVM, "fox", "Vec2", &pTransform->rotation);
            });

            Fox_Getter(pVM, AS_INSTANCE(args[-1]), "scale", [](VM* pVM, int argc, Value* args)
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
            Fox_SetField(pVM, args[-1], "m_iId", Fox_NumberToValue(iEntity));
            if (argc == 0)
                Fox_SetField(pVM, args[-1], "m_strName", Fox_String(pVM, "GameObject"));
            else
                Fox_SetField(pVM, args[-1], "m_strName", Fox_String(pVM, Fox_ValueToCString(args[0])));
            return args[-1];
        }),

        std::make_pair<std::string, NativeFn>("add", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Entity iEntity = Fox_ValueToInteger(Fox_GetField(pVM, args[-1], "m_iId"));
            ObjectInstance* pComponent = Fox_ValueToInstance(args[0]);
            if (pComponent->klass->name->string == "Transform")
            {
                oWorld.AddComponent<Transform>(iEntity, {.position = Vec2(0, 0)});
            }
            return NIL_VAL;
        }),

        std::make_pair<std::string, NativeFn>("get", [&oWorld] (VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Entity iEntity = Fox_ValueToInteger(Fox_GetField(pVM, args[-1], "m_iId"));

            if (!strcmp(Fox_ValueToCString(args[0]), "Transform"))
            {
                return Fox_DefineInstanceOfCStruct(pVM, "fox", "Transform", &oWorld.GetComponent<Transform>(iEntity));
            }
            return NIL_VAL;
        }),

        std::make_pair<std::string, NativeFn>("printName", PrintGO),
        
    };

    pVM->DefineModule("fox");
    pVM->DefineClass("fox", "GameObject", oGameObjectMethods);

    AddVec2Class(oWorld, pVM);
    AddTransformClass(oWorld, pVM);
}