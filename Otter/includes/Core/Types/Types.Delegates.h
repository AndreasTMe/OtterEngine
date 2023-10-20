#ifndef OTTERENGINE_TYPES_DELEGATES_H
#define OTTERENGINE_TYPES_DELEGATES_H

#include <functional>
#include <vector>

#include "Core/Defines.h"

template<typename... TArgs>
class Action
{
    using Function = std::function<void(TArgs...)>;

public:
    OTR_WITH_DEFAULT_CONSTRUCTOR(Action)

    OTR_INLINE Action(const Action& other)
    {
        m_Functions = other.m_Functions;
    }

    OTR_INLINE Action(Action&& other) noexcept
    {
        m_Functions = std::move(other.m_Functions);
    }

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

    OTR_INLINE void operator+=(Function function)
    {
        if (!function)
            return;

        m_Functions.push_back(function);
    }

    OTR_INLINE void operator-=(Function function)
    {
        if (!function)
            return;

        m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
    }

    OTR_INLINE virtual void operator()(TArgs... args) const
    {
        if (m_Functions.empty())
            return;

        for (const auto& function: m_Functions)
            function(args...);
    }

    OTR_INLINE virtual void Invoke(TArgs... args) const
    {
        if (m_Functions.empty())
            return;

        for (const auto& function: m_Functions)
            function(args...);
    }

    OTR_INLINE void Clear()
    {
        m_Functions.clear();
    }

protected:
    // TODO: Replace with a custom list.
    std::vector<Function> m_Functions;
};

template<typename TResult, typename... TArgs>
class Func
{
    using Function = std::function<TResult(TArgs...)>;

public:
    OTR_WITH_DEFAULT_CONSTRUCTOR(Func)

    OTR_INLINE Func(const Func& other)
    {
        m_Functions = other.m_Functions;
    }

    OTR_INLINE Func(Func&& other) noexcept
    {
        m_Functions = std::move(other.m_Functions);
    }

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

    OTR_INLINE void operator+=(Function function)
    {
        if (!function)
            return;

        m_Functions.push_back(function);
    }

    OTR_INLINE void operator-=(Function function)
    {
        if (!function)
            return;

        m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
    }

    OTR_INLINE virtual const TResult* operator()(TArgs... args) const
    {
        if (m_Functions.empty())
            return nullptr;

        TResult result = m_Functions[0](args...);

        for (int i = 1; i < m_Functions.size(); i++)
            result = m_Functions[i](args...);

        return &result;
    }

    OTR_INLINE virtual const TResult Invoke(TArgs... args) const
    {
        if (m_Functions.empty())
            return nullptr;

        TResult result = m_Functions[0](args...);

        for (int i = 1; i < m_Functions.size(); i++)
            result = m_Functions[i](args...);

        return result;
    }

    OTR_INLINE void Clear()
    {
        m_Functions.clear();
    }

protected:
    // TODO: Replace with a custom list.
    std::vector<Function> m_Functions;
};

#endif //OTTERENGINE_TYPES_DELEGATES_H
