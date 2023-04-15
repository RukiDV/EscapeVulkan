#include "vk/RenderObject.hpp"

namespace ve
{
    RenderObject::RenderObject(const VulkanMainContext& vmc) : dsh(vmc), vmc(vmc), pipeline(vmc), mesh_view_pipeline(vmc)
    {}

    void RenderObject::self_destruct()
    {
        pipeline.self_destruct();
        mesh_view_pipeline.self_destruct();
        dsh.self_destruct();
    }

    void RenderObject::add_model(uint32_t idx)
    {
        model_indices.emplace_back(idx);
    }

    void RenderObject::add_bindings(std::vector<Model>& models)
    {
        for (auto& model : model_indices)
        {
            models[model].add_set_bindings(dsh);
        }
    }

    void RenderObject::construct(const RenderPass& render_pass, const std::vector<std::pair<std::string, vk::ShaderStageFlagBits>>& shader_names, vk::PolygonMode polygon_mode)
    {
        if (model_indices.empty()) return;
        dsh.construct();
        pipeline.construct(render_pass, dsh.get_layouts()[0], shader_names, polygon_mode);
        mesh_view_pipeline.construct(render_pass, dsh.get_layouts()[0], shader_names, vk::PolygonMode::eLine);
    }

    void RenderObject::draw(vk::CommandBuffer& cb, DrawInfo& di, std::vector<Model>& models)
    {
        if (model_indices.empty()) return;
        cb.bindPipeline(vk::PipelineBindPoint::eGraphics, di.mesh_view ? mesh_view_pipeline.get() : pipeline.get());
        for (auto& model : model_indices)
        {
            models[model].draw(di.current_frame, pipeline.get_layout(), dsh.get_sets(), di.vp);
        }
    }
} // namespace ve
