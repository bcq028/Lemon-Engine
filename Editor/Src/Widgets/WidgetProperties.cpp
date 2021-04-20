#include "WidgetProperties.h"

#include "WidgetSceneHierachy.h"
#include "World/Components/StaticMeshComponent.h"
#include "World/Components/TransformComponent.h"
#include "WidgetHelpers.h"
#include "World/Components/DirectionalLightComponent.h"

WidgetProperties::WidgetProperties(Lemon::Engine* engine)
	:Widget(engine)
{
	m_WidgetProp.Title = WidgetGlobal::g_WidgetPropertiesTitle;
	m_WidgetProp.MinSize = glm::vec2(370.0f, -1.0f);

}

void WidgetProperties::Tick(float deltaTime)
{
	if(WidgetSceneHierachy::SelectEntity)
	{
		DrawEntity(WidgetSceneHierachy::SelectEntity);
	}
}

void WidgetProperties::DrawEntity(Lemon::Entity entity) const
{
	// Draw Name
	{
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		std::string& entityName = entity.GetName();
		std::strncpy(buffer, entityName.c_str(), sizeof(buffer));
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
		{
			entityName = std::string(buffer);
		}
	}
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");
	
	if (ImGui::BeginPopup("AddComponent"))
	{
		if (ImGui::MenuItem("StaticMeshComponent"))
		{
			if (!entity.HasComponent<Lemon::StaticMeshComponent>())
				entity.AddComponent<Lemon::StaticMeshComponent>();
			else
				LEMON_CORE_WARN("This entity already has the StaticMeshComponent!");
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::PopItemWidth();

	if (entity.HasComponent<Lemon::TransformComponent>())
	{
		bool open = ImGui::TreeNodeEx((void*)typeid(Lemon::TransformComponent).hash_code(), treeNodeFlags, "Transform");

		if (open)
		{
			auto& tc = entity.GetComponent<Lemon::TransformComponent>();
			WidgetHelpers::DrawVec3Control("Translation", tc.Position);
			WidgetHelpers::DrawVec3Control("Rotation", tc.Rotation);
			WidgetHelpers::DrawVec3Control("Scale", tc.Scale, 1.0f);
			ImGui::TreePop();
		}
	}

	if(entity.HasComponent<Lemon::StaticMeshComponent>())
	{
		bool open = ImGui::TreeNodeEx((void*)typeid(Lemon::TransformComponent).hash_code(), treeNodeFlags, "StaticMeshComponent");
		if(open)
		{
			Lemon::StaticMeshComponent& staticMeshComp = entity.GetComponent<Lemon::StaticMeshComponent>();
			Lemon::Ref<Lemon::Material>& meshMaterial = staticMeshComp.GetRenderMesh()->GetMaterial();
			if (meshMaterial)
			{
				// 
				ImGui::Text("Material");

				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 100.0f);
				ImGui::Text("Albedo");
				ImGui::NextColumn();
				ImGui::ColorEdit3("##1", (float*)&meshMaterial->Albedo.x);

				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 100.0f);
				ImGui::Text("Metallic");
				ImGui::NextColumn();
				ImGui::DragFloat("##2", (float*)&meshMaterial->Metallic);

				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 100.0f);
				ImGui::Text("Roughness");
				ImGui::NextColumn();
				ImGui::DragFloat("##3", (float*)&meshMaterial->Roughness);

				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 100.0f);
				ImGui::Text("AO");
				ImGui::NextColumn();
				ImGui::DragFloat("##4", (float*)&meshMaterial->AO);
			}

			ImGui::TreePop();
		}
	}

	if(entity.HasComponent<Lemon::DirectionalLightComponent>())
	{
		bool open = ImGui::TreeNodeEx((void*)typeid(Lemon::TransformComponent).hash_code(), treeNodeFlags, "StaticMeshComponent");
		if(open)
		{
			Lemon::DirectionalLightComponent& directionalLightComp = entity.GetComponent<Lemon::DirectionalLightComponent>();
			glm::vec3 LightColor = directionalLightComp.GetLightColor();
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("LightColor");
			ImGui::NextColumn();
			static bool alpha_preview = true;
			static bool alpha_half_preview = false;
			static bool drag_and_drop = true;
			static bool options_menu = true;
			static bool hdr = false;
			ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0)
							| (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop)
							| (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0))
							| (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

			ImGui::ColorEdit3("##1", (float*)&LightColor.x, misc_flags);
			directionalLightComp.SetLightColor(LightColor);
			
			ImGui::TreePop();
		}
	}
}