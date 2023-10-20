#ifndef OTTERENGINE_LAYERS_H
#define OTTERENGINE_LAYERS_H

#include <deque>

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Collections.h"

namespace Otter
{
    class Layer
    {
    public:
        OTR_INLINE explicit Layer(bool isEnabled)
            : m_IsEnabled(isEnabled)
        {
        }
        OTR_INLINE virtual ~Layer() = default;

        virtual void OnEnable() = 0;
        virtual void OnDisable() = 0;
        virtual void OnUpdate() = 0;

        [[nodiscard]] OTR_INLINE bool IsEnabled() const { return m_IsEnabled; }

    protected:
        bool m_IsEnabled = false;
    };

    class LayerContainer
    {
    public:
        LayerContainer() = default;
        ~LayerContainer()
        {
            for (auto& layer: m_Layers)
                delete layer;
        }

        OTR_INLINE void Enable()
        {
            if (m_IsEnabled)
                return;

            for (auto& layer: m_Layers)
                layer->OnEnable();

            m_IsEnabled = true;
        }

        OTR_INLINE void Disable()
        {
            if (!m_IsEnabled)
                return;

            for (auto& layer: m_Layers)
                layer->OnDisable();

            m_IsEnabled = false;
        }

        OTR_INLINE void PushFront(Layer* layer)
        {
            OTR_INTERNAL_ASSERT_MSG(!m_IsEnabled, "Cannot add a layer while the container is enabled!")
            m_Layers.push_front(layer);
        }

        OTR_INLINE void PushBack(Layer* layer)
        {
            OTR_INTERNAL_ASSERT_MSG(!m_IsEnabled, "Cannot add a layer while the container is enabled!")
            m_Layers.push_back(layer);
        }

        OTR_INLINE void PopFront()
        {
            OTR_INTERNAL_ASSERT_MSG(!m_IsEnabled, "Cannot remove a layer while the container is enabled!")
            m_Layers.pop_front();
        }

        OTR_INLINE void PopBack()
        {
            OTR_INTERNAL_ASSERT_MSG(!m_IsEnabled, "Cannot remove a layer while the container is enabled!")
            m_Layers.pop_back();
        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

        using LayersConstIterator = std::deque<Layer*>::const_iterator;
        using LayersConstReverseIterator = std::deque<Layer*>::const_reverse_iterator;

        [[nodiscard]] OTR_INLINE LayersConstIterator begin() const { return m_Layers.begin(); }
        [[nodiscard]] OTR_INLINE LayersConstIterator end() const { return m_Layers.end(); }
        [[nodiscard]] OTR_INLINE LayersConstReverseIterator rbegin() const { return m_Layers.rbegin(); }
        [[nodiscard]] OTR_INLINE LayersConstReverseIterator rend() const { return m_Layers.rend(); }

#pragma clang diagnostic pop

    private:
        // TODO: Replace with a custom deque.
        std::deque<Layer*> m_Layers;
        bool               m_IsEnabled = false;
    };
}

#endif //OTTERENGINE_LAYERS_H
