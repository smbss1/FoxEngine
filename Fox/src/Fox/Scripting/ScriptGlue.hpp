//
// Created by samuel on 03/10/22.
//

#ifndef FOXENGINE_SCRIPTGLUE_HPP
#define FOXENGINE_SCRIPTGLUE_HPP

namespace fox
{
    class ScriptGlue
    {
    public:
        static void RegisterComponents();
        static void RegisterFunctions();

        static void ClearTypes();
    };
}

#endif //FOXENGINE_SCRIPTGLUE_HPP
