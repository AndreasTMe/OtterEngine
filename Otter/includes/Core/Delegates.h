#ifndef OTTERENGINE_DELEGATES_H
#define OTTERENGINE_DELEGATES_H

#include <functional>
#include <vector>

#include "Core/Defines.h"

namespace Otter
{
    template<typename... TArgs>
    class Action final
    {
        using Function = std::function<void(TArgs...)>;

    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Action)

        OTR_INLINE Action(const Action& other) noexcept { m_Functions = other.m_Functions; }
        OTR_INLINE Action(Action&& other) noexcept { m_Functions = std::move(other.m_Functions); }

        OTR_INLINE Action& operator=(const Action& other)
        {
            m_Functions = other.m_Functions;
            return *this;
        }

        OTR_INLINE Action& operator=(Action&& other) noexcept
        {
            m_Functions = std::move(other.m_Functions);
            return *this;
        }

        OTR_INLINE void operator+=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.push_back(function);
        }

//    OTR_INLINE void operator-=(const Function& function)
//    {
//        if (!function)
//            return;
//
//        /*
//         * BUG: no match for 'operator=='
//         * (operand types are 'std::function<bool(const Otter::Internal::Event&)* >' and
//         * 'const std::function<bool(const Otter::Internal::Event&)>')
//         */
//        m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
//    }

        OTR_INLINE bool operator==(const Action<TArgs...>& other) const
        {
            if (m_Functions.size() != other.m_Functions.size())
                return false;

            for (auto i = 0; i < m_Functions.size(); i++)
                if (m_Functions[i].target_type() != other.m_Functions[i].target_type())
                    return false;

            return true;
        }

        OTR_INLINE bool operator!=(const Action<TArgs...>& other) const { return !(operator==(other)); }
        OTR_INLINE void operator()(TArgs... args) { Invoke(args...); }

        OTR_INLINE virtual void Invoke(TArgs... args)
        {
            if (m_Functions.empty())
                return;

            for (const auto& function: m_Functions)
                function(args...);
        }

        OTR_INLINE void Clear() { m_Functions.clear(); }

    private:
        // TODO: Replace with a custom list.
        std::vector<Function> m_Functions;
    };

    template<typename TResult, typename... TArgs>
    class Func final
    {
        using Function = std::function<TResult(TArgs...)>;

    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Func)

        OTR_INLINE Func(const Func& other) noexcept { m_Functions = other.m_Functions; }
        OTR_INLINE Func(Func&& other) noexcept { m_Functions = std::move(other.m_Functions); }

        OTR_INLINE Func& operator=(const Func& other)
        {
            m_Functions = other.m_Functions;
            return *this;
        }

        OTR_INLINE Func& operator=(Func&& other) noexcept
        {
            m_Functions = std::move(other.m_Functions);
            return *this;
        }

        OTR_INLINE void operator+=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.push_back(function);
        }

//    OTR_INLINE void operator-=(Function& function)
//    {
//        if (!function)
//            return;
//
//        /*
//         * BUG: no match for 'operator=='
//         * (operand types are 'std::function<bool(const Otter::Internal::Event&)* >' and
//         * 'const std::function<bool(const Otter::Internal::Event&)>')
//         */
//        m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
//    }

        OTR_INLINE bool operator==(const Func<TResult, TArgs...>& other) const
        {
            if (m_Functions.size() != other.m_Functions.size())
                return false;

            for (auto i = 0; i < m_Functions.size(); i++)
                if (m_Functions[i].target_type() != other.m_Functions[i].target_type())
                    return false;

            return true;
        }

        OTR_INLINE bool operator!=(const Func<TResult, TArgs...>& other) const { return !(operator==(other)); }
        OTR_INLINE virtual TResult operator()(TArgs... args) { return Invoke(args...); }

        OTR_INLINE TResult Invoke(TArgs... args)
        {
            TResult result = TResult();

            if (m_Functions.empty())
                return result;

            result = m_Functions[0](args...);

            for (int i = 1; i < m_Functions.size(); i++)
                m_Functions[i](args...);

            return result;
        }

        OTR_INLINE TResult ReverseInvoke(TArgs... args)
        {
            TResult result = TResult();

            if (m_Functions.empty())
                return result;

            result = m_Functions[m_Functions.size() - 1](args...);

            for (int i = m_Functions.size() - 2; i >= 0; i--)
                m_Functions[i](args...);

            return result;
        }

        OTR_INLINE void Clear() { m_Functions.clear(); }

    private:
        // TODO: Replace with a custom list.
        std::vector<Function> m_Functions;
    };
}

#endif //OTTERENGINE_DELEGATES_H
