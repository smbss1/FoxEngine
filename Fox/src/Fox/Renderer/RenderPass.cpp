//
// Created by samuel on 29/10/22.
//

#include "RenderPass.hpp"

namespace fox
{
    RenderPass::RenderPass(const RenderPassSpecification& specification) : m_Specs(specification)
    {
    }

    Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& specification)
    {
        return new_ref<RenderPass>(specification);
    }
}
