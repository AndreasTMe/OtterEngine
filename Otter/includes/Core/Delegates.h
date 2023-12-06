#ifndef OTTERENGINE_DELEGATES_H
#define OTTERENGINE_DELEGATES_H

#include "Core/Defines.h"
#include "Core/Function.h"
#include "Core/Collections/Deque.h"

namespace Otter
{
    template<typename TSignature>
    class Delegate;

    template<typename TOut, typename... TArgs>
    class Delegate<TOut(TArgs...)>
    {
    protected:
        using Function = Function<TOut(TArgs...)>;
        Deque<Function> m_Functions{ };

    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Delegate)

        Delegate(const Delegate& other) noexcept { m_Functions = other.m_Functions; }
        Delegate(Delegate&& other) noexcept { m_Functions = std::move(other.m_Functions); }

        Delegate& operator=(const Delegate& other)
        {
            m_Functions = other.m_Functions;
            return *this;
        }

        Delegate& operator=(Delegate&& other) noexcept
        {
            m_Functions = std::move(other.m_Functions);
            return *this;
        }

        void operator<=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.PushBack(function);
        }

        void operator>=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.PushFront(function);
        }

        void operator-=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.TryRemove(function);
        }

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

        OTR_INLINE bool operator!=(const Delegate<TOut(TArgs...)>& other) const { return !(operator==(other)); }

        OTR_INLINE void Clear() { m_Functions.Clear(); }
        OTR_INLINE void ClearDestructive() { m_Functions.ClearDestructive(); }
    };

    template<typename... TArgs>
    class Action final : public Delegate<void(TArgs...)>
    {
        OTR_USING_BASE(Delegate<void(TArgs...)>)

    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Action)

        OTR_INLINE void operator()(TArgs... args) { Invoke(args...); }

        void Invoke(TArgs... args)
        {
            if (base::m_Functions.IsEmpty())
                return;

            for (const auto& function: base::m_Functions)
                function(args...);
        }

        void ReverseInvoke(TArgs... args)
        {
            if (base::m_Functions.IsEmpty())
                return;

            for (const auto& function = base::m_Functions.rbegin(); function != base::m_Functions.rend(); ++function)
                function(args...);
        }
    };

    template<typename TResult, typename... TArgs>
    class Func final : public Delegate<TResult(TArgs...)>
    {
        OTR_USING_BASE(Delegate<TResult(TArgs...)>)

    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Func)

        OTR_INLINE TResult operator()(TArgs... args) { return Invoke(args...); }

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
