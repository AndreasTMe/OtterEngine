#ifndef OTTERENGINE_FUNCTION_H
#define OTTERENGINE_FUNCTION_H

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Otter
{
    template<typename TSignature>
    class Function;

    template<typename TOut, typename... TArgs>
    class Function<TOut(TArgs...)>
    {
        template<typename TFunction,
            typename TDecayType = EnableIf<!IsSame<std::remove_cvref_t<TFunction>, Function>, std::decay_t<TFunction>>,
            typename TCallableOut = std::invoke_result<TDecayType&, TArgs...>>
        struct Callable
            : std::__is_invocable_impl<TCallableOut, TOut>::type
        {
        };

    public:
        Function() noexcept = default;
        ~Function() = default;

        Function(NullPtr) noexcept // NOLINT(*-explicit-constructor)
        {
            m_Callback = nullptr;
        }

        Function(const Function& other)
        {
            ::new(m_Data.Ptr) std::decay_t<decltype(other.m_Data)>(other.m_Data);

            m_Callback = other.m_Callback;
        }

        Function(Function&& other) noexcept
        {
            m_Data     = other.m_Data;
            m_Callback = other.m_Callback;

            other.m_Callback = nullptr;
        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"

        template<typename TFunctor, typename TConstraints = EnableIf<Callable<TFunctor>::value, void>>
        requires std::is_copy_constructible_v<std::decay_t<TFunctor >> &&
                 std::is_constructible_v<std::decay_t<TFunctor>, TFunctor>
        Function(TFunctor&& func) noexcept(std::is_nothrow_constructible<std::decay_t<TFunctor>, TFunctor>::value)
        {
            ::new(m_Data.Ptr) std::decay_t<TFunctor>(std::forward<TFunctor>(func));

            m_Callback = &Invoke<std::decay_t<TFunctor>>;
        }

#pragma clang diagnostic pop

        Function& operator=(const Function& other)
        {
            Function(other).Swap(*this);
            return *this;
        }

        Function& operator=(Function&& other) noexcept
        {
            Function(std::move(other)).Swap(*this);
            return *this;
        }

        Function& operator=(NullPtr) noexcept
        {
            m_Callback = nullptr;
            m_Data     = { };

            return *this;
        }

        TOut operator()(TArgs... args) const
        {
            return m_Callback(m_Data, std::forward<TArgs>(args)...);
        }

        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Callback == nullptr; }

        [[nodiscard]] OTR_INLINE explicit operator bool() const noexcept { return !IsEmpty(); }

    private:
        class ClassPlaceholder;

        union DataSizePlaceholder
        {
            void      * ObjectPtr;
            const void* ConstObjectPtr;
            void (* FunctionPtr)();
            void (ClassPlaceholder::*MethodPtr)();
        };

        union CallbackData
        {
            Byte Ptr[sizeof(DataSizePlaceholder)];
        };

        using Callback = TOut (*)(const CallbackData&, TArgs&& ...);
        Callback m_Callback = nullptr;

        CallbackData m_Data{ };

        template<typename TCallback>
        static TOut Invoke(const CallbackData& data, TArgs&& ... args)
        {
            return std::__invoke_r<TOut>(*(TCallback*) data.Ptr, std::forward<TArgs>(args)...);
        }

        void Swap(Function& other) noexcept
        {
            std::swap(m_Callback, other.m_Callback);
            std::swap(m_Data, other.m_Data);
        }
    };
}

#endif //OTTERENGINE_FUNCTION_H
