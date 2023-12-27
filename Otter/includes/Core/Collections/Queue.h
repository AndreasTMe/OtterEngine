#ifndef OTTERENGINE_QUEUE_H
#define OTTERENGINE_QUEUE_H

#include "Core/Memory.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    /**
     * @brief A FIFO (First In First Out) collection. The items are stored in a contiguous memory block on the heap.
     * It does not have a fixed capacity and will expand as needed.
     *
     * @tparam T The type of the items in the queue.
     *
     * @note The queue is circular in order to preserve the O(1) complexity of the operations. The expansion and
     * shrinking of the queue is not as efficient but it is not supposed to be a common operation.
     */
    template<typename T>
    class Queue final
    {
    public:
        /**
         * @brief Constructor.
         */
        Queue() = default;

        /**
         * @brief Destructor.
         */
        ~Queue()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);
        }

        /**
         * @brief Creates a queue from an initialiser list.
         *
         * @param list The initialiser list.
         */
        Queue(InitialiserList<T> list)
            : Queue()
        {
            m_Capacity   = list.size();
            m_Data       = Buffer::New<T>(m_Capacity);
            m_Count      = 0;
            m_StartIndex = 0;

            for (const T& item: list)
                m_Data[m_Count++] = item;
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The queue to copy.
         */
        Queue(const Queue<T>& other)
        {
            m_Capacity   = other.m_Capacity;
            m_Count      = other.m_Count;
            m_StartIndex = other.m_StartIndex;
            m_Data       = Buffer::New<T>(m_Capacity);

            if (m_Count > 0)
                MemorySystem::MemoryCopy(m_Data, other.m_Data, m_Capacity * sizeof(T));
        }

        /**
         * @brief Move constructor.
         *
         * @param other The queue to move.
         */
        Queue(Queue<T>&& other) noexcept
        {
            m_Data       = std::move(other.m_Data);
            m_Capacity   = std::move(other.m_Capacity);
            m_Count      = std::move(other.m_Count);
            m_StartIndex = std::move(other.m_StartIndex);

            other.m_Data       = nullptr;
            other.m_Capacity   = 0;
            other.m_Count      = 0;
            other.m_StartIndex = 0;
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other The queue to copy.
         *
         * @return A reference to this queue.
         */
        Queue<T>& operator=(const Queue<T>& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Capacity   = other.m_Capacity;
            m_Count      = other.m_Count;
            m_StartIndex = other.m_StartIndex;
            m_Data       = Buffer::New<T>(m_Capacity);

            if (m_Count > 0)
                MemorySystem::MemoryCopy(m_Data, other.m_Data, m_Capacity * sizeof(T));

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The queue to move.
         *
         * @return A reference to this queue.
         */
        Queue<T>& operator=(Queue<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = std::move(other.m_Data);
            m_Capacity   = std::move(other.m_Capacity);
            m_Count      = std::move(other.m_Count);
            m_StartIndex = std::move(other.m_StartIndex);

            other.m_Data       = nullptr;
            other.m_Capacity   = 0;
            other.m_Count      = 0;
            other.m_StartIndex = 0;

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other The queue to compare to.
         *
         * @return True if the queues are equal, false otherwise.
         */
        [[nodiscard]] bool operator==(const Queue<T>& other) const
        {
            return m_Data == other.m_Data
                   && m_Capacity == other.m_Capacity
                   && m_Count == other.m_Count
                   && m_StartIndex == other.m_StartIndex;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The queue to compare to.
         *
         * @return True if the queues are not equal, false otherwise.
         */
        [[nodiscard]] bool operator!=(const Queue<T>& other) const { return !(*this == other); }

        /**
         * @brief Tries to enqueue an item into the queue.
         *
         * @param item The item to enqueue.
         *
         * @return True if the item was enqueued, false otherwise.
         */
        bool TryEnqueue(const T& item)
        {
            if (m_Count >= m_Capacity)
                Expand();

            auto endIndex = m_StartIndex + m_Count;

            if (endIndex >= m_Capacity)
                endIndex -= m_Capacity;

            m_Data[endIndex] = item;

            m_Count++;

            return true;
        }

        /**
         * @brief Tries to enqueue an item into the queue.
         *
         * @param item The item to enqueue.
         *
         * @return True if the item was enqueued, false otherwise.
         */
        bool TryEnqueue(T&& item) noexcept
        {
            if (m_Count >= m_Capacity)
                Expand();

            auto endIndex = m_StartIndex + m_Count;

            if (endIndex >= m_Capacity)
                endIndex -= m_Capacity;

            m_Data[endIndex] = std::move(item);

            m_Count++;

            return true;
        }

        /**
         * @brief Tries to dequeue an item from the queue.
         *
         * @return True if an item was dequeued, false otherwise.
         */
        bool TryDequeue()
        {
            if (IsEmpty())
                return false;

            m_StartIndex++;

            if (m_StartIndex >= m_Capacity)
                m_StartIndex = 0;

            m_Count--;

            return true;
        }

        /**
         * @brief Tries to dequeue an item from the queue.
         *
         * @param outItem The item that was dequeued.
         *
         * @return True if an item was dequeued, false otherwise.
         */
        bool TryDequeue(T* outItem)
        {
            if (IsEmpty())
                return false;

            *outItem = m_Data[m_StartIndex];
            m_StartIndex++;

            if (m_StartIndex >= m_Capacity)
                m_StartIndex = 0;

            m_Count--;

            return true;
        }

        /**
         * @brief Tries to peek at the first item in the queue.
         *
         * @param outItem The item at the front of the queue.
         *
         * @return True if an item was peeked at, false otherwise.
         */
        bool TryPeek(T* outItem)
        {
            if (IsEmpty())
                return false;

            *outItem = m_Data[m_StartIndex];

            return true;
        }

        /**
         * @brief Used to reserve space for the queue.
         *
         * @param capacity The capacity to reserve.
         *
         * @note This operation is destructive and will delete any existing data.
         */
        void Reserve(const UInt64 capacity)
        {
            RecreateEmpty(capacity);
        }

        /**
         * @brief Used to expand the size of the queue by a given amount.
         *
         * @param amount The amount to expand the queue by.
         */
        void Expand(const UInt64 amount = 0)
        {
            UInt64 newCapacity = CalculateExpandCapacity(amount);

            if (IsEmpty())
            {
                RecreateEmpty(newCapacity);
                return;
            }

            T* newData = Buffer::New<T>(newCapacity);

            if (m_StartIndex + m_Count < m_Capacity)
            {
                for (UInt64 i = 0; i < m_Count; i++)
                    newData[i] = m_Data[i + m_StartIndex];
            }
            else
            {
                const auto endIndex = m_StartIndex + m_Count - m_Capacity;

                for (UInt64 i = 0; i < m_Capacity - m_StartIndex; i++)
                    newData[i] = m_Data[i + m_StartIndex];

                for (UInt64 i = 0; i < endIndex; i++)
                    newData[i + m_Capacity - m_StartIndex] = m_Data[i];
            }

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = newData;
            m_Capacity   = newCapacity;
            m_StartIndex = 0;
        }

        /**
         * @brief Used to shrink the size of the queue by a given amount.
         *
         * @param amount The amount to shrink the queue by.
         * @param isDestructive Whether or not the shrink is destructive. If true, some data may be lost.
         */
        void Shrink(const UInt64 amount = 0, const bool isDestructive = false)
        {
            UInt64 newCapacity = CalculateShrinkCapacity(amount, isDestructive);

            if (IsEmpty() || newCapacity == 0)
            {
                RecreateEmpty(newCapacity);
                return;
            }

            T* newData = Buffer::New<T>(newCapacity);

            if (m_StartIndex + m_Count < m_Capacity)
            {
                for (UInt64 i = 0; i < m_Count && i < newCapacity; i++)
                    newData[i] = m_Data[i + m_StartIndex];
            }
            else
            {
                const auto endIndex = m_StartIndex + m_Count - m_Capacity;

                for (UInt64 i = 0; i < m_Capacity - m_StartIndex && i < newCapacity; i++)
                    newData[i] = m_Data[i + m_StartIndex];

                for (UInt64 i = 0; i < endIndex && i < newCapacity; i++)
                    newData[i + m_Capacity - m_StartIndex] = m_Data[i];
            }

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = newData;
            m_Capacity   = newCapacity;
            m_Count      = m_Count < newCapacity ? m_Count : newCapacity;
            m_StartIndex = 0;
        }

        /**
         * @brief Checks if the queue contains a given item.
         *
         * @param item The item to check for.
         *
         * @return True if the queue contains the item, false otherwise.
         */
        bool Contains(const T& item) const
        {
            if (m_StartIndex + m_Count <= m_Capacity)
            {
                for (UInt64 i = m_StartIndex; i < m_Count; i++)
                    if (m_Data[i] == item)
                        return true;
            }
            else
            {
                const auto endIndex = m_StartIndex + m_Count - m_Capacity;

                for (UInt64 i = m_StartIndex; i < m_Capacity; i++)
                    if (m_Data[i] == item)
                        return true;

                for (UInt64 i = 0; i <= endIndex; i++)
                    if (m_Data[i] == item)
                        return true;
            }

            return false;
        }

        /**
         * @brief Checks if the queue contains a given item.
         *
         * @param item The item to check for.
         *
         * @return True if the queue contains the item, false otherwise.
         */
        bool Contains(T&& item) const noexcept
        {
            if (m_StartIndex + m_Count <= m_Capacity)
            {
                for (UInt64 i = m_StartIndex; i < m_Count; i++)
                    if (m_Data[i] == item)
                        return true;
            }
            else
            {
                const auto endIndex = m_StartIndex + m_Count - m_Capacity;

                for (UInt64 i = m_StartIndex; i < m_Capacity; i++)
                    if (m_Data[i] == item)
                        return true;

                for (UInt64 i = 0; i <= endIndex; i++)
                    if (m_Data[i] == item)
                        return true;
            }

            return false;
        }

        /**
         * @brief Clears the queue.
         */
        OTR_INLINE void Clear()
        {
            m_StartIndex = 0;
            m_Count      = 0;
        }

        /**
         * @brief Clears the queue and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = nullptr;
            m_Capacity   = 0;
            m_Count      = 0;
            m_StartIndex = 0;
        }

#if !OTR_RUNTIME
        /**
         * @brief Gets the memory footprint of the queue.
         *
         * @param debugName The name of the queue for debugging purposes.
         *
         * @return The memory footprint of the queue.
         */
        ReadOnlySpan<MemoryFootprint, 1> GetMemoryFootprint(const char* const debugName) const
        {
            MemoryFootprint footprint = { };
            MemorySystem::CheckMemoryFootprint([&]()
                                               {
                                                   MemoryDebugPair pair[1];
                                                   pair[0] = { debugName, m_Data };

                                                   return MemoryDebugHandle{ pair, 1 };
                                               },
                                               &footprint,
                                               nullptr);

            return ReadOnlySpan<MemoryFootprint, 1>{ footprint };
        }
#endif

        /**
         * @brief Gets the capacity of the queue.
         *
         * @return The capacity of the queue.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCapacity() const noexcept { return m_Capacity; }

        /**
         * @brief Gets the item count of the queue.
         *
         * @return The item count of the queue.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

        /**
         * @brief Checks whether the queue has been created. A queue is created when it has been initialised
         * with a valid capacity and has not been destroyed.
         *
         * @return True if the queue has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Data && m_Capacity > 0; }

        /**
         * @brief Checks whether the queue is empty.
         *
         * @return True if the queue is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

    private:
        T* m_Data = nullptr;
        UInt64 m_Capacity   = 0;
        UInt64 m_Count      = 0;
        UInt64 m_StartIndex = 0;

        /**
         * @brief Recreates the queue with a given capacity. Deletes any existing data.
         *
         * @param capacity The capacity to recreate the queue with.
         */
        void RecreateEmpty(const UInt64 capacity)
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = capacity > 0 ? Buffer::New<T>(capacity) : nullptr;
            m_Capacity   = capacity;
            m_Count      = 0;
            m_StartIndex = 0;
        }

        /**
         * @brief Calculates the new capacity when expanding the queue.
         *
         * @param expandAmount The amount to expand the queue by.
         *
         * @return The new capacity.
         */
        [[nodiscard]] UInt64 CalculateExpandCapacity(const UInt64 expandAmount) const
        {
            UInt64 newCapacity;

            if (expandAmount == 0)
                newCapacity = m_Capacity == 0 ? 2 : m_Capacity * 1.5;
            else
                newCapacity = m_Capacity + expandAmount;

            return newCapacity;
        }

        /**
         * @brief Calculates the new capacity when shrinking the queue.
         *
         * @param shrinkAmount The amount to shrink the queue by.
         * @param isDestructive Whether or not the shrink is destructive. If true, some data may be lost.
         *
         * @return The new capacity.
         */
        [[nodiscard]] UInt64 CalculateShrinkCapacity(const UInt64 shrinkAmount, const bool isDestructive) const
        {
            if (m_Capacity == 0)
                return 0;

            UInt64 newCapacity;

            if (shrinkAmount == 0)
                newCapacity = m_Capacity * 0.75;
            else if (shrinkAmount > m_Capacity)
                newCapacity = 0;
            else
                newCapacity = m_Capacity - shrinkAmount;

            if (!isDestructive && newCapacity < m_Count)
                newCapacity = m_Count;

            return newCapacity;
        }
    };
}

#endif //OTTERENGINE_QUEUE_H
