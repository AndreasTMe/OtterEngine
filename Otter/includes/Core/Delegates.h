#ifndef OTTERENGINE_DELEGATES_H
#define OTTERENGINE_DELEGATES_H

#include "Core/Defines.h"
#include "Core/Function.h"
#include "Core/Collections/Deque.h"

namespace Otter
{
    template<typename TSignature>
    class Delegate;

    /**
     * @brief Delegate class
     *
     * @tparam TOut The return type of the delegate
     * @tparam TArgs The arguments of the delegate
     */
    template<typename TOut, typename... TArgs>
    class Delegate<TOut(TArgs...)>
    {
    protected:
        using Function = Function<TOut(TArgs...)>;
        Deque<Function> m_Functions{ };

    public:
        /**
         * @brief Default constructor
         */
        Delegate() = default;

        /**
         * @brief Destructor
         */
        ~Delegate() = default;

        /**
         * @brief Copy constructor
         *
         * @param other The delegate to copy
         */
        Delegate(const Delegate& other) noexcept { m_Functions = other.m_Functions; }

        /**
         * @brief Move constructor
         *
         * @param other The delegate to move
         */
        Delegate(Delegate&& other) noexcept { m_Functions = std::move(other.m_Functions); }

        /**
         * @brief Copy assignment operator
         *
         * @param other The delegate to copy
         *
         * @return The copied delegate
         */
        Delegate& operator=(const Delegate& other)
        {
            m_Functions = other.m_Functions;
            return *this;
        }

        /**
         * @brief Move assignment operator
         *
         * @param other The delegate to move
         *
         * @return The moved delegate
         */
        Delegate& operator=(Delegate&& other) noexcept
        {
            m_Functions = std::move(other.m_Functions);
            return *this;
        }

        /**
         * @brief Operator used to insert a function at the end of the invocation list.
         *
         * @param function The function to insert
         */
        void operator<=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.PushBack(function);
        }

        /**
         * @brief Operator used to insert a function at the beginning of the invocation list.
         *
         * @param function The function to insert
         */
        void operator>=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.PushFront(function);
        }

        /**
         * @brief Operator used to remove a function from the invocation list.
         *
         * @param function The function to remove
         */
        void operator-=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.TryRemove(function);
        }

        /**
         * @brief Equality operator
         *
         * @param other The delegate to compare
         *
         * @return True if the delegates are equal, false otherwise
         */
        bool operator==(const Delegate<TOut(TArgs...)>& other) const
        {
            if (m_Functions.GetCount() != other.m_Functions.GetCount())
                return false;

            for (auto func1 = m_Functions.begin(), func2 = other.m_Functions.begin();
                 func1 != m_Functions.end() && func2 != other.m_Functions.end();
                 ++func1, ++func2)
                if (func1->target_type() != func2->target_type())
                    return false;

            return true;
        }

        /**
         * @brief Inequality operator
         *
         * @param other The delegate to compare
         *
         * @return True if the delegates are not equal, false otherwise
         */
        OTR_INLINE bool operator!=(const Delegate<TOut(TArgs...)>& other) const { return !(operator==(other)); }

        /**
         * @brief Clears the invocation list.
         */
        OTR_INLINE void Clear() { m_Functions.Clear(); }

        /**
         * @brief Clears and destroys the invocation list.
         */
        OTR_INLINE void ClearDestructive() { m_Functions.ClearDestructive(); }
    };

    /**
     * @brief Action delegate class (void return type)
     *
     * @tparam TArgs The arguments of the delegate
     */
    template<typename... TArgs>
    class Action final : public Delegate<void(TArgs...)>
    {
        OTR_USING_BASE(Delegate<void(TArgs...)>)

    public:
        /**
         * @brief Default constructor
         */
        Action() = default;

        /**
         * @brief Destructor
         */
        ~Action() = default;

        /**
         * @brief Operator used to invoke the delegate.
         *
         * @param args The arguments to pass to the delegate
         */
        OTR_INLINE void operator()(TArgs... args) { Invoke(args...); }

        /**
         * @brief Invokes the delegate.
         *
         * @param args The arguments to pass to the delegate
         */
        void Invoke(TArgs... args)
        {
            if (base::m_Functions.IsEmpty())
                return;

            for (const auto& function: base::m_Functions)
                function(args...);
        }

        /**
         * @brief Invokes the delegate in reverse order.
         *
         * @param args The arguments to pass to the delegate
         */
        void ReverseInvoke(TArgs... args)
        {
            if (base::m_Functions.IsEmpty())
                return;

            for (const auto& function = base::m_Functions.rbegin(); function != base::m_Functions.rend(); ++function)
                function(args...);
        }
    };

    /**
     * @brief Func delegate class (non-void return type)
     *
     * @tparam TResult The return type of the delegate
     * @tparam TArgs The arguments of the delegate
     */
    template<typename TResult, typename... TArgs>
    class Func final : public Delegate<TResult(TArgs...)>
    {
        OTR_USING_BASE(Delegate<TResult(TArgs...)>)

    public:
        /**
         * @brief Default constructor
         */
        Func() = default;

        /**
         * @brief Destructor
         */
        ~Func() = default;

        /**
         * @brief Operator used to invoke the delegate.
         *
         * @param args The arguments to pass to the delegate
         *
         * @return The result of the invocation
         *
         * @note If the delegate is empty, the default value of TResult is returned.
         * @note If the delegate is not empty, the result of the first invocation is returned.
         */
        OTR_INLINE TResult operator()(TArgs... args) { return Invoke(args...); }

        /**
         * @brief Invokes the delegate.
         *
         * @param args The arguments to pass to the delegate
         *
         * @return The result of the invocation
         *
         * @note If the delegate is empty, the default value of TResult is returned.
         * @note If the delegate is not empty, the result of the first invocation is returned.
         */
        TResult Invoke(TArgs... args)
        {
            TResult result = TResult();

            if (base::m_Functions.IsEmpty())
                return result;

            auto count = base::m_Functions.GetCount();

            while (count > 0)
            {
                typename base::Function function;
                if (count == base::m_Functions.GetCount())
                {
                    base::m_Functions.TryPeekBack(function);

                    result = function(args...);
                    count--;
                    continue;
                }

                base::m_Functions.TryPeekBack(function);

                function(args...);
                count--;
            }

            return result;
        }

        /**
         * @brief Invokes the delegate in reverse order.
         *
         * @param args The arguments to pass to the delegate
         *
         * @return The result of the invocation
         *
         * @note If the delegate is empty, the default value of TResult is returned.
         * @note If the delegate is not empty, the result of the first invocation is returned.
         */
        TResult ReverseInvoke(TArgs... args)
        {
            TResult result = TResult();

            if (base::m_Functions.IsEmpty())
                return result;

            auto count = base::m_Functions.GetCount();

            while (count > 0)
            {
                typename base::Function function;
                if (count == base::m_Functions.GetCount())
                {
                    base::m_Functions.TryPeekFront(&function);

                    result = function(args...);
                    count--;
                    continue;
                }

                base::m_Functions.TryPeekFront(&function);

                function(args...);
                count--;
            }

            return result;
        }
    };
}

#endif //OTTERENGINE_DELEGATES_H
