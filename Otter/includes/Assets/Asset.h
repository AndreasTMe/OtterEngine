#ifndef OTTERENGINE_ASSET_H
#define OTTERENGINE_ASSET_H

#include "Core/Assert.h"
#include "Graphics/Abstractions/Texture.h"
#include "Graphics/Abstractions/Shader.h"

#define ASSET_TYPE_LIST         \
    REPLACE_WITH(Metadata, 0x01)\
    REPLACE_WITH(Shader, 0x02)  \
    REPLACE_WITH(Texture, 0x03) \
    REPLACE_WITH(Mesh, 0x04)

namespace Otter
{
    enum class AssetType : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        ASSET_TYPE_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };

    class Asset
    {
    public:
        template<AssetType TType, typename... TArgs>
        [[nodiscard]] static auto Create(const char* const filePath, TArgs&& ... args)
        {
            OTR_INTERNAL_ASSERT_MSG(TType != AssetType::None && TType != AssetType::Max, "Invalid asset type")
            OTR_INTERNAL_ASSERT_MSG(filePath != nullptr, "File path is null")

            if constexpr (TType == AssetType::Shader)
                return Graphics::Shader::Create(filePath, std::forward<TArgs>(args)...);
            else if constexpr (TType == AssetType::Texture)
                return Graphics::Texture::Create(filePath, std::forward<TArgs>(args)...);

            OTR_INTERNAL_ASSERT_MSG(false, "Unimplemented asset type")
        }

        template<AssetType TType>
        static void Destroy(auto* outAsset)
        {
            OTR_INTERNAL_ASSERT_MSG(TType != AssetType::None && TType != AssetType::Max, "Invalid asset type")

            if constexpr (TType == AssetType::Shader)
                Graphics::Shader::Destroy(outAsset);
            else if constexpr (TType == AssetType::Texture)
                Graphics::Texture::Destroy(outAsset);

            OTR_INTERNAL_ASSERT_MSG(outAsset == nullptr, "Asset not destroyed")
        }
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::AssetType& bufferType)
{
    switch (bufferType)
    {
#define REPLACE_WITH(Item, Value) case Otter::AssetType::Item: os << "AssetType::" << #Item; break;
        ASSET_TYPE_LIST
#undef REPLACE_WITH
        default:
            os << "AssetType[Unknown]";
    }

    return os;
}
#undef ASSET_TYPE_LIST

#endif //OTTERENGINE_ASSET_H
