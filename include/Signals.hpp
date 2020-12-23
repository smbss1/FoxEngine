#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace Fox
{
    class Connection
    {
    public:
        Connection(std::shared_ptr<void>&& slotPtr)
            :m_slotptr(std::move(slotPtr))
        {}

        void Disconnect()
        {
            m_slotptr = nullptr;
        }
    private:
        std::shared_ptr<void> m_slotptr;
    };

    template <typename...Args>
    class Signal
    {
    public:
        using SlotFn = std::function<void(Args...)>;

        Connection Connect(const SlotFn& slot)
        {
            std::shared_ptr<SlotFn> sharedSlot = std::make_shared<SlotFn>(slot);
            const std::lock_guard<std::mutex> lg(m_mutex);
            m_slots.push_back(sharedSlot);
            return Connection(std::move(sharedSlot));
        }

        Connection Connect(SlotFn&& slot)
        {
            std::shared_ptr<SlotFn> sharedSlot = std::make_shared<SlotFn>(std::move(slot));
            const std::lock_guard<std::mutex> lg(m_mutex);
            m_slots.push_back(sharedSlot);
            return Connection(std::move(sharedSlot));
        }

        void Emit(Args... params)
        {
            std::vector<std::shared_ptr<SlotFn>> funcs;
            std::vector<std::weak_ptr<SlotFn>> validSlots;
            {
                const std::lock_guard<std::mutex> lg(m_mutex);
                validSlots.reserve(m_slots.size());
                for (auto& slot : m_slots)
                {
                    if (auto sharedFn = slot.lock())
                    {
                        funcs.push_back(std::move(sharedFn));
                        validSlots.push_back(std::move(slot));
                    }
                }
                m_slots = std::move(validSlots);
            }
            for (const auto& slotFun : funcs)
            {
                (*slotFun)(params...);
            }
        }

    private:

        std::vector<std::weak_ptr<SlotFn>> m_slots;
        std::mutex m_mutex;
    };
} // namespace Fox