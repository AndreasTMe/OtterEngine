#ifndef OTTERENGINE_LAYERS_H
#define OTTERENGINE_LAYERS_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Core/Time.h"

namespace Otter
{
    class Layer
    {
    public:
        virtual ~Layer() = default;

        OTR_DISABLE_OBJECT_COPIES(Layer)
        OTR_DISABLE_OBJECT_MOVES(Layer)

        template<typename T = Layer>
        [[nodiscard]] static Layer* Create(bool isEnabled)
        {
            return Otter::New<T>(isEnabled);
        }

        template<typename T = Layer>
        static void Delete(T* layer)
        {
            if (layer)
                Otter::Delete<T>(layer);
        }

        virtual void OnEnable() { m_IsEnabled = true; }
        virtual void OnDisable() { m_IsEnabled = false; }
        virtual void OnUpdate(const TimeStep& step) = 0;

        [[nodiscard]] OTR_INLINE bool IsEnabled() const { return m_IsEnabled; }

    protected:
        explicit Layer(bool isEnabled = true)
            : m_IsEnabled(isEnabled)
        {
        }

        bool m_IsEnabled = false;
    };
}

#endif //OTTERENGINE_LAYERS_H
