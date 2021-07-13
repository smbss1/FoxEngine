//
// Created by samuel on 14/06/2021.
//

#ifndef TCPSERVER_OBSERVER_HPP
#define TCPSERVER_OBSERVER_HPP

namespace fox {
    namespace net
    {

    /**
     * This class is part of the Observer design pattern. It defines the interface
     * methods for the observer.
     */
        template<typename T>
        class Observer
        {
        public:
            /**
             * Notify the observer
             * @param[in] operation	Operation that notifies the observer
             */
            virtual void OnNotify(T& operation) = 0;

            /**
             * Class destructor
             */
            virtual ~Observer() {};
        };
    }
}
#endif //TCPSERVER_OBSERVER_HPP
