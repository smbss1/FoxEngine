
#include <Components/NativeScript.hpp>

class Test : public ScriptableBehaviour
{
public:
protected:
    void OnStart() override;
    void on_update() override;
private:

    fox::TransformComponent* transform = nullptr;
};

REGISTER_SCRIPT(Test);
