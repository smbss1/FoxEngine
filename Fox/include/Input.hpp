

#ifndef FOX_INPUT_HPP_
#define FOX_INPUT_HPP_

#include <unordered_map>
#include "KeyCodes.hpp"
#include "MouseCodes.hpp"
#include "Vec2.hpp"

namespace fox
{
	class Input
	{
	public:
        // explicit Input()
        // {
        //     for (int i = 0; i < Key::KeyCount; ++i)
        //     {
        //         m_vKeys[i] = false;
        //     }
        // }

		bool GetKeyDown(KeyCode key)
        {
            bool& curr = m_vKeys[key].first;
            bool& was = m_vKeys[key].second;
            bool ret = curr && !was;
            m_vKeys[key].second = m_vKeys[key].first;
            return ret;
        }

		bool GetKeyUp(KeyCode key)
        {
            return m_vKeys[key].first == false;
        }

        void SetKeyDown(KeyCode key)
        {
            m_vKeys[key].first = true;
            // std::cout << key << " " << m_vKeys[key] << std::endl;
        }

		void SetKeyUp(KeyCode key)
        {
            m_vKeys[key].first = false;
            // m_vKeys[key] = false;
            // std::cout << key << " " << m_vKeys[key] << std::endl;
        }

		// static bool IsMouseButtonPressed(MouseCode button);
		// static Vec2 GetMousePosition();
		// static float GetMouseX();
		// static float GetMouseY();
        std::unordered_map<KeyCode, std::pair<bool, bool>> m_vKeys;
	};
}

#endif