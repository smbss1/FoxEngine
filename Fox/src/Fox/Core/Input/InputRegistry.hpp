////
//// Created by samuel on 22/06/2021.
////
//
//#ifndef ECS_INPUTREGISTRY_HPP
//#define ECS_INPUTREGISTRY_HPP
//
//#include <unordered_set>
//
//namespace fox
//{
//    enum class InputState
//    {
//        RELEASED,
//        PRESSED,
//    };
//
//    template<typename T>
//    class InputRegistry
//    {
//    private:
//        std::unordered_set<T> m_vPressed;
//        std::unordered_set<T> m_vDown;
//        std::unordered_set<T> m_vReleased;
//
//    public:
//        virtual ~InputRegistry() = default;
//
//        virtual void Clear()
//        {
//            m_vPressed.clear();
//            m_vDown.clear();
//            m_vReleased.clear();
//        }
//
//        void SetState(T key, InputState state)
//        {
//            switch (state)
//            {
//                case InputState::PRESSED:
//                    m_vPressed.insert(key);
//                    m_vDown.insert(key);
//                    break;
//                case InputState::RELEASED:
//                    m_vDown.erase(key);
//                    m_vReleased.insert(key);
//                    break;
//            }
//        }
//
//        bool IsPressed(const T &key) const
//        {
//            return m_vPressed.count(key) == 1;
//        }
//
//        bool IsDown(const T &key) const
//        {
//            return m_vDown.count(key) == 1;
//        }
//
//        bool IsReleased(const T &key) const
//        {
//            return m_vReleased.count(key) == 1;
//        }
//    };
//}
//
//#endif //ECS_INPUTREGISTRY_HPP
