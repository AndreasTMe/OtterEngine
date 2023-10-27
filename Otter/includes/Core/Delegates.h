#ifndef OTTERENGINE_DELEGATES_H
#define OTTERENGINE_DELEGATES_H

#include <functional>

#include "Core/Defines.h"
#include "Core/Collections/List.h"

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
        explicit Action(Action* other)
            : Action(*other)
        {
        }

        explicit Action(const Function& function) { m_Functions.Add(function); }
        explicit Action(Function&& function) { m_Functions.Add(std::move(function)); }

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

        void operator+=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.Add(function);
        }

        void operator-=(const Function& function)
        {
            if (!function)
                return;

            /*
             * BUG: no match for 'operator=='
             * (operand types are 'std::function<bool(const Otter::Internal::Event&)* >' and
             * 'const std::function<bool(const Otter::Internal::Event&)>')
             */
            // m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
        }

        bool operator==(const Action<TArgs...>& other) const
        {
            if (m_Functions.GetCount() != other.m_Functions.GetCount())
                return false;

            for (auto i = 0; i < m_Functions.GetCount(); i++)
                if (m_Functions[i].target_type() != other.m_Functions[i].target_type())
                    return false;

            return true;
        }

        OTR_INLINE bool operator!=(const Action<TArgs...>& other) const { return !(operator==(other)); }

        OTR_INLINE void operator()(TArgs... args) { Invoke(args...); }

        virtual void Invoke(TArgs... args)
        {
            if (m_Functions.IsEmpty())
                return;

            for (const auto& function: m_Functions)
                function(args...);
        }

        OTR_INLINE void Clear() { m_Functions.Clear(); }

        OTR_INLINE void ClearDestructive() { m_Functions.ClearDestructive(); }

    private:
        List<Function> m_Functions{ };
    };

    template<typename TResult, typename... TArgs>
    class Func final
    {
        using Function = std::function<TResult(TArgs...)>;

    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Func)

        Func(const Func& other) noexcept { m_Functions = other.m_Functions; }
        Func(Func&& other) noexcept { m_Functions = std::move(other.m_Functions); }
        explicit Func(Func* other)
            : Func(*other)
        {
        }

        explicit Func(const Function& function) { m_Functions.Add(function); }
        explicit Func(Function&& function) { m_Functions.Add(std::move(function)); }

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

        void operator+=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.Add(function);
        }

        void operator-=(const Function& function)
        {
            if (!function)
                return;

            /*
             * BUG: no match for 'operator=='
             * (operand types are 'std::function<bool(const Otter::Internal::Event&)* >' and
             * 'const std::function<bool(const Otter::Internal::Event&)>')
             */
            // m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
        }

        bool operator==(const Func<TResult, TArgs...>& other) const
        {
            if (m_Functions.GetCount() != other.m_Functions.GetCount())
                return false;

            for (auto i = 0; i < m_Functions.GetCount(); i++)
                if (m_Functions[i].target_type() != other.m_Functions[i].target_type())
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

            result = m_Functions[0](args...);

            for (int i = 1; i < m_Functions.GetCount(); i++)
                m_Functions[i](args...);

            return result;
        }

        TResult ReverseInvoke(TArgs... args)
        {
            TResult result = TResult();

            if (m_Functions.IsEmpty())
                return result;

            result = m_Functions[m_Functions.GetCount() - 1](args...);

            for (int i = m_Functions.GetCount() - 2; i >= 0; i--)
                m_Functions[i](args...);

            return result;
        }

        OTR_INLINE void Clear() { m_Functions.Clear(); }

        OTR_INLINE void ClearDestructive() { m_Functions.ClearDestructive(); }

    private:
        List<Function> m_Functions{ };
    };
}

#endif //OTTERENGINE_DELEGATES_H
