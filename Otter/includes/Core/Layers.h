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
