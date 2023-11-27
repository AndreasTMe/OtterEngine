#ifndef OTTERENGINE_DELEGATES_H
#define OTTERENGINE_DELEGATES_H

#include <functional>

#include "Core/Defines.h"
#include "Core/Collections/Deque.h"

namespace Otter
{
    template<typename... TArgs>
    class Action final
    {
        using Function = std::function<void(TArgs...)>;

    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Action)

        Action(const Action& other) noexcept { m_Functions = other.m_Functions; }
        Action(Action&& other) noexcept { m_Functions = std::move(other.m_Functions); }

        Action& operator=(const Action& other)
        {
            m_Functions = other.m_Functions;
            return *this;
        }

        Action& operator=(Action&& other) noexcept
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

        bool operator==(const Action<TArgs...>& other) const
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

        OTR_INLINE bool operator!=(const Action<TArgs...>& other) const { return !(operator==(other)); }

        OTR_INLINE void operator()(TArgs... args) { Invoke(args...); }

        void Invoke(TArgs... args)
        {
            if (m_Functions.IsEmpty())
                return;

            for (const auto& function: m_Functions)
                function(args...);
        }

        void ReverseInvoke(TArgs... args)
        {
            if (m_Functions.IsEmpty())
                return;

            for (const auto& function = m_Functions.rbegin(); function != m_Functions.rend(); ++function)
                function(args...);
        }

        OTR_INLINE void Clear() { m_Functions.Clear(); }

        OTR_INLINE void ClearDestructive() { m_Functions.ClearDestructive(); }

    private:
        Deque<Function> m_Functions{ };
    };

    template<typename TResult, typename... TArgs>
    class Func final
    {
        using Function = std::function<TResult(TArgs...)>;

    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Func)

        Func(const Func& other) noexcept { m_Functions = other.m_Functions; }
        Func(Func&& other) noexcept { m_Functions = std::move(other.m_Functions); }

        Func& operator=(const Func& other)
        {
            m_Functions = other.m_Functions;
            return *this;
        }

        Func& operator=(Func&& other) noexcept
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

        bool operator==(const Func<TResult, TArgs...>& other) const
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

        OTR_INLINE bool operator!=(const Func<TResult, TArgs...>& other) const { return !(operator==(other)); }

        OTR_INLINE virtual TResult operator()(TArgs... args) { return Invoke(args...); }

        TResult Invoke(TArgs... args)
        {
            TResult result = TResult();

            if (m_Functions.IsEmpty())
                return result;

            auto count = m_Functions.GetCount();

            while (count > 0)
            {
                Function function;
                if (count == m_Functions.GetCount())
                {
                    m_Functions.TryPeekBack(function);

                    result = function(args...);
                    count--;
                    continue;
                }

                m_Functions.TryPeekBack(function);

                function(args...);
                count--;
            }

            return result;
        }

        TResult ReverseInvoke(TArgs... args)
        {
            TResult result = TResult();

            if (m_Functions.IsEmpty())
                return result;

            auto count = m_Functions.GetCount();

            while (count > 0)
            {
                Function function;
                if (count == m_Functions.GetCount())
                {
                    m_Functions.TryPeekFront(function);

                    result = function(args...);
                    count--;
                    continue;
                }

                m_Functions.TryPeekFront(function);

                function(args...);
                count--;
            }

            return result;
        }

        OTR_INLINE void Clear() { m_Functions.Clear(); }

        OTR_INLINE void ClearDestructive() { m_Functions.ClearDestructive(); }

    private:
        Deque<Function> m_Functions{ };
    };
}

#endif //OTTERENGINE_DELEGATES_H
