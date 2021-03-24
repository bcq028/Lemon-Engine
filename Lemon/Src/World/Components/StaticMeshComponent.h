﻿#pragma once
#include "Core/Core.h"
#include "IComponent.h"
#include <glm/glm.hpp>
#include "RenderCore/Mesh.h"

namespace Lemon
{
    class LEMON_API StaticMeshComponent : public IComponent
    {
	public:
		StaticMeshComponent() {}
        void SetMesh(Ref<Mesh> renderMesh);

        Ref<Mesh> GetRenderMesh() { return m_RenderMesh; }
        const Ref<Mesh> GetRenderMesh() const { return m_RenderMesh; }
        
    private:
        Ref<Mesh> m_RenderMesh;
    };
}