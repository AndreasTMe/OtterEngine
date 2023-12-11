#ifndef OTTERENGINE_KEYVALUEPAIR_H
#define OTTERENGINE_KEYVALUEPAIR_H

namespace Otter
{
    template<typename TKey, typename TValue>
    struct KeyValuePair final
    {
    public:
        TKey   Key;
        TValue Value;

        KeyValuePair() = default;
        ~KeyValuePair() = default;

        KeyValuePair(const TKey& key, const TValue& value)
        {
            Key   = key;
            Value = value;
        }

        KeyValuePair(const KeyValuePair& other)
        {
            Key   = other.Key;
            Value = other.Value;
        }

        KeyValuePair(KeyValuePair&& other) noexcept
        {
            Key   = std::move(other.Key);
            Value = std::move(other.Value);
        }

        KeyValuePair& operator=(const KeyValuePair& other)
        {
            if (this == &other)
                return *this;

            Key   = other.Key;
            Value = other.Value;

            return *this;
        }

        KeyValuePair& operator=(KeyValuePair&& other) noexcept
        {
            if (this == &other)
                return *this;

            Key   = std::move(other.Key);
            Value = std::move(other.Value);

            return *this;
        }
    };
}

#endif //OTTERENGINE_KEYVALUEPAIR_H
