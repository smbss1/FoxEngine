
#include <Components/NativeScript.hpp>

class Player_Controller : public ScriptableBehaviour
{
public:
protected:
    void OnStart() override;
    void on_update() override;
private:

    fox::TransformComponent* transform;
    float m_fSpeed = 10.f;
};

REGISTER_SCRIPT(Player_Controller);