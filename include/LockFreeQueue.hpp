
#ifndef FOX_LOCK_FREEm_vQueue_HPP_
#define FOX_LOCK_FREEm_vQueue_HPP_

#include <atomic>
#include <cstddef>
#include <vector>
#include <cassert>
#include "Option.hpp"

namespace fox
{
    template <typename T>
    class LockFreeQueue
    {
        struct Array
        {
            size_t m_lCapacity;
            size_t m_lCapacityMask;
            std::atomic<T>* m_vData;

            explicit Array(size_t lCapacity) : m_lCapacity(lCapacity),
                                            m_lCapacityMask(lCapacity - 1),
                                            m_vData(new std::atomic<T>[m_lCapacity])
            {
            }

            ~Array()
            {
                delete[] m_vData;
            }

            size_t capacity() const noexcept
            {
                return m_lCapacity;
            }

            template <typename O>
            void push(size_t i, O&& item) noexcept
            {
                m_vData[i & m_lCapacityMask].store(std::forward<O>(item), std::memory_order_relaxed);
            }

            T pop(size_t i) noexcept
            {
                return m_vData[i & m_lCapacityMask].load(std::memory_order_relaxed);
            }

            void resize(size_t b, size_t t)
            {
                Array* ptr = new Array(2 * m_lCapacity);
                for (size_t i = t; i != b; ++i)
                {
                    ptr->push(i, pop(i));
                }
                std::swap(m_lCapacity, ptr->m_lCapacity);
                std::swap(m_lCapacityMask, ptr->m_lCapacityMask);
                std::swap(m_vData, ptr->m_vData);
                delete ptr;
            }
        };

        std::atomic<size_t> m_lTop;
        std::atomic<size_t> m_lBottom;
        std::atomic<Array *> m_pArray;

    public:
        /**
            @brief constructs the queue with a given capacity
            @param capacity the capacity of the queue (must be power of 2)
        */
        explicit LockFreeQueue(size_t capacity = 1024)
        {
            assert(capacity && (!(capacity & (capacity - 1))));
            m_lTop.store(0, std::memory_order_relaxed);
            m_lBottom.store(0, std::memory_order_relaxed);
            m_pArray.store(new Array(capacity), std::memory_order_relaxed);
        }

        /**
            @brief destructs the queue
        */
        ~LockFreeQueue()
        {
            delete m_pArray.load();
        }

        /**
            @brief queries if the queue is empty at the time of this call
        */
        bool empty() const noexcept
        {
            return size() == 0;
        }

        /**
        @brief queries the number of items at the time of this call
        */
        size_t size() const noexcept
        {
            size_t bot = m_lBottom.load(std::memory_order_relaxed);
            size_t top = m_lTop.load(std::memory_order_relaxed);
            return bot >= top ? bot - top : 0;
        }

        /**
        @brief queries the capacity of the queue
        */
        size_t capacity() const noexcept
        {
            return m_pArray.load(std::memory_order_relaxed)->capacity();
        }

        /**
        @brief inserts an item to the queue
        Only the owner thread can insert an item to the queue. 
        The operation can trigger the queue to resize its capacity 
        if more space is required.
        @tparam O data type 
        @param item the item to perfect-forward to the queue
        */
        template <typename O>
        void push(O &&item)
        {
            size_t bot = m_lBottom.load(std::memory_order_relaxed);
            size_t top = m_lTop.load(std::memory_order_acquire);
            Array *a = m_pArray.load(std::memory_order_relaxed);

            // queue is full
            if (a->capacity() - 1 < (bot - top))
            {
                a->resize(bot, top);
                m_pArray.store(a, std::memory_order_relaxed);
            }

            a->push(bot, std::forward<O>(item));
            std::atomic_thread_fence(std::memory_order_release);
            m_lBottom.store(bot + 1, std::memory_order_relaxed);
        }

        /**
            @brief pops out an item from the queue
            Only the owner thread can pop out an item from the queue. 
            The return can be a @fox::nullopt if this operation failed (empty queue).
        */
        Option<T> pop()
        {
            size_t bot = m_lBottom.load(std::memory_order_relaxed) - 1;
            Array* a = m_pArray.load(std::memory_order_relaxed);

            m_lBottom.store(bot, std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_seq_cst);

            size_t top = m_lTop.load(std::memory_order_relaxed);
            Option<T> item;

            if (top <= bot)
            {
                item = a->pop(bot);
                if (top == bot) {
                    // the last item just got stolen
                    if (!m_lTop.compare_exchange_strong(top, top + 1,
                                                    std::memory_order_seq_cst,
                                                    std::memory_order_relaxed))
                    {
                        item = fox::nullopt;
                    }
                    m_lBottom.store(bot + 1, std::memory_order_relaxed);
                }
            }
            else {
                m_lBottom.store(bot + 1, std::memory_order_relaxed);
            }
            return item;
        }

        /**
        @brief steals an item from the queue
        Any threads can try to steal an item from the queue.
        The return can be a @std_nullopt if this operation failed (not necessary empty).
        */
        Option<T> steal()
        {
            size_t top = m_lTop.load(std::memory_order_acquire);
            std::atomic_thread_fence(std::memory_order_seq_cst);
            size_t bot = m_lBottom.load(std::memory_order_acquire);

            Option<T> item;

            if (top < bot) {
                Array *a = m_pArray.load(std::memory_order_consume);
                item = a->pop(top);
                if (!m_lTop.compare_exchange_strong(top, top + 1,
                                                std::memory_order_seq_cst,
                                                std::memory_order_relaxed))
                {
                    return {};
                }
            }
            return item;
        }
    };
}

#endif