#ifndef OTTERENGINE_FUNCTION_H
#define OTTERENGINE_FUNCTION_H

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Otter
{
    template<typename TSignature>
    class Function;

    /**
     * @brief Function class
     *
     * @tparam TOut The return type of the function.
     * @tparam TArgs The parameter types of the function.
     */
    template<typename TOut, typename... TArgs>
    class Function<TOut(TArgs...)>
    {
        /**
         * @brief An alias for a template class used to enable a function if a condition is met.
         */
        template<typename TFunction>
        using DecayType = EnableIf<!IsSame<std::remove_cvref_t<TFunction>, Function>, std::decay_t<TFunction>>;

        /**
         * @brief An alias for the return type of a callable object.
         */
        template<typename TFunction>
        using CallableOut = std::invoke_result<DecayType<TFunction>&, TArgs...>;

        /**
         * @brief A template class representing a callable object, such as a function, functor, or lambda.
         * It provides a unified way to invoke these callable objects with a given set of arguments and obtain their
         * return value.
         *
         * @tparam TFunction The type of the callable object.
         * @tparam TCallableOut The return type of the callable object.
         */
        template<typename TFunction, typename TCallableOut = CallableOut<TFunction>>
        struct Callable : std::__is_invocable_impl<TCallableOut, TOut>::type
        {
        };

    public:
        /**
         * @brief Constructor.
         */
        Function() noexcept = default;

        /**
         * @brief Destructor.
         */
        ~Function() = default;

        /**
         * @brief Constructor.
         *
         * @param NullPtr Null pointer.
         */
        Function(NullPtr) noexcept // NOLINT(*-explicit-constructor)
        {
            m_Callback = nullptr;
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The other function to copy from.
         */
        Function(const Function& other)
        {
            ::new(m_Data.Ptr) std::decay_t<decltype(other.m_Data)>(other.m_Data);

            m_Callback = other.m_Callback;
        }

        /**
         * @brief Move constructor.
         *
         * @param other The other function to move from.
         */
        Function(Function&& other) noexcept
        {
            m_Data     = other.m_Data;
            m_Callback = other.m_Callback;

            other.m_Callback = nullptr;
        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"

        /**
         * @brief Constructor.
         *
         * @tparam TFunctor The type of the functor.
         * @tparam TConstraints The constraints.
         *
         * @param functor The functor to construct from.
         */
        template<typename TFunctor, typename TConstraints = EnableIf<Callable<TFunctor>::value, void>>
        requires std::is_copy_constructible_v<std::decay_t<TFunctor >> &&
                 std::is_constructible_v<std::decay_t<TFunctor>, TFunctor>
        Function(TFunctor&& functor) noexcept(std::is_nothrow_constructible_v<std::decay_t<TFunctor>, TFunctor>)
        {
            ::new(m_Data.Ptr) std::decay_t<TFunctor>(std::forward<TFunctor>(functor));

            m_Callback = &Invoke<std::decay_t<TFunctor>>;
        }

#pragma clang diagnostic pop

        /**
         * @brief Copy assignment operator.
         *
         * @param other The other function to copy from.
         *
         * @return A reference to this function.
         */
        Function& operator=(const Function& other)
        {
            Function(other).Swap(*this);
            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The other function to move from.
         *
         * @return A reference to this function.
         */
        Function& operator=(Function&& other) noexcept
        {
            Function(std::move(other)).Swap(*this);
            return *this;
        }

        /**
         * @brief Assignment operator.
         *
         * @param NullPtr Null pointer.
         *
         * @return A reference to this function.
         */
        Function& operator=(NullPtr) noexcept
        {
            m_Callback = nullptr;
            m_Data     = { };

            return *this;
        }

        /**
         * @brief Function call operator.
         *
         * @tparam TOut The return type of the function.
         * @tparam TArgs The argument types.
         *
         * @param args The arguments to pass to the function.
         *
         * @return The return value of the function.
         */
        TOut operator()(TArgs... args) const
        {
            return m_Callback(m_Data, std::forward<TArgs>(args)...);
        }

        /**
         * @brief Checks if the function has a callback.
         *
         * @return True if the function does not have a callback, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Callback == nullptr; }

        /**
         * @brief Checks if the function has a callback.
         *
         * @return True if the function has a callback, false otherwise.
         */
        [[nodiscard]] OTR_INLINE explicit operator bool() const noexcept { return !IsEmpty(); }

    private:
        /// @brief A placeholder class for the class type in a member function pointer.
        class ClassPlaceholder;

        /**
         * @brief A union used to hold the size of the largest callable object.
         */
        union DataSizePlaceholder
        {
            void      * ObjectPtr;
            const void* ConstObjectPtr;
            void (* FunctionPtr)();
            void (ClassPlaceholder::*MethodPtr)();
        };

        /**
         * @brief A union used to hold the callback data as a byte array.
         */
        union CallbackData
        {
            Byte Ptr[sizeof(DataSizePlaceholder)];
        };

        using Callback = TOut (*)(const CallbackData&, TArgs&& ...);
        Callback m_Callback = nullptr;

        CallbackData m_Data{ };

        /**
         * @brief Invokes the callback.
         *
         * @tparam TCallback The type of the callback.
         * @tparam TArgs The argument types.
         *
         * @param data The callback data.
         * @param args The arguments to pass to the callback.
         *
         * @return The return value of the callback.
         */
        template<typename TCallback>
        static TOut Invoke(const CallbackData& data, TArgs&& ... args)
        {
            return std::__invoke_r<TOut>(*(TCallback*) data.Ptr, std::forward<TArgs>(args)...);
        }

        /**
         * @brief Swaps the contents of this function with another function.
         *
         * @param other The other function to swap with.
         */
        void Swap(Function& other) noexcept
        {
            std::swap(m_Callback, other.m_Callback);
            std::swap(m_Data, other.m_Data);
        }
    };
}

#endif //OTTERENGINE_FUNCTION_H
