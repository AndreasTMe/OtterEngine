#ifndef OTTERENGINE_KEYVALUEPAIR_H
#define OTTERENGINE_KEYVALUEPAIR_H

#include <utility>

namespace Otter
{
    /**
     * @brief A structure that represents a key-value pair.
     *
     * @tparam TKey The type of the key.
     * @tparam TValue The type of the value.
     */
    template<typename TKey, typename TValue>
    struct KeyValuePair final
    {
    public:
        TKey   Key;
        TValue Value;

        /**
         * @brief Constructor.
         */
        KeyValuePair() = default;

        /**
         * @brief Destructor.
         */
        ~KeyValuePair()
        {
            if constexpr (!std::is_trivially_destructible_v<TKey>)
                Key.~TKey();

            if constexpr (!std::is_trivially_destructible_v<TValue>)
                Value.~TValue();
        }

        /**
         * @brief Constructor.
         *
         * @param key The key.
         * @param value The value.
         */
        KeyValuePair(const TKey& key, const TValue& value)
        {
            Key   = key;
            Value = value;
        }

        /**
         * @brief Constructor.
         *
         * @param key The key.
         * @param value The value.
         */
        KeyValuePair(TKey&& key, TValue&& value)
        {
            Key   = std::move(key);
            Value = std::move(value);
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The key-value pair to copy.
         */
        KeyValuePair(const KeyValuePair& other)
        {
            Key   = other.Key;
            Value = other.Value;
        }

        /**
         * @brief Move constructor.
         *
         * @param other The key-value pair to move.
         */
        KeyValuePair(KeyValuePair&& other) noexcept
        {
            Key   = std::move(other.Key);
            Value = std::move(other.Value);
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other The key-value pair to copy.
         *
         * @return A reference to this key-value pair.
         */
        KeyValuePair& operator=(const KeyValuePair& other)
        {
            if (this == &other)
                return *this;

            Key   = other.Key;
            Value = other.Value;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The key-value pair to move.
         *
         * @return A reference to this key-value pair.
         */
        KeyValuePair& operator=(KeyValuePair&& other) noexcept
        {
            if (this == &other)
                return *this;

            Key   = std::move(other.Key);
            Value = std::move(other.Value);

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other The key-value pair to compare.
         *
         * @return True if the key-value pairs are equal, false otherwise.
         */
        bool operator==(const KeyValuePair& other) const
        {
            return Key == other.Key && Value == other.Value;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The key-value pair to compare.
         *
         * @return True if the key-value pairs are not equal, false otherwise.
         */
        bool operator!=(const KeyValuePair& other) const
        {
            return Key != other.Key || Value != other.Value;
        }
    };
}

#endif //OTTERENGINE_KEYVALUEPAIR_H
