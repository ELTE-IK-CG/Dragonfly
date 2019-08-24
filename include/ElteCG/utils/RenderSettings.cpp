#include "RenderSettings.h"


#include <imgui/imgui.h>

void RenderSettings::SetUniforms(GLWRAPPER_NS::ShaderProgram& program)
{
	program.SetUniform("gLightColor", gLightColor);
	program.SetUniform("gLightPos", gLightPos);
	program.SetUniform("gLightSize", gLightSize);
	program.SetUniform("gSphereLightStrength", gSphereLightStrength);

	program.SetUniform("gAOMaxIters", gAOMaxIters);
	program.SetUniform("gAOStepSize", gAOStepSize);
	program.SetUniform("gAOStrength", gAOStrength);

	program.SetUniform("gAmbient", gAmbient);
	program.SetUniform("gDiffuse", gDiffuse);
	program.SetUniform("gCookRoughness", gCookRoughness);
	program.SetUniform("gCookIOR", gCookIOR);

	program.SetUniform("gShadowMaxiter", gShadowMaxiter);
	program.SetUniform("gShadowEpsilon", gShadowEpsilon);
}

bool RenderSettings::RenderUI()
{
	if (ImGui::Begin("Render Settings", &uiIsOpen))
	{
		ImGui::Text("Sphere Light");
		ImGui::ColorEdit3("Color##Sphere light", &gLightColor.x);
		ImGui::DragFloat3("Position##Sphere light", &gLightPos.x);
		ImGui::DragFloat("Radius##Sphere light", &gLightSize, 0.001f, 0.0f);
		ImGui::DragFloat("Strength##Sphere light", &gSphereLightStrength, 0.001f, 0.1f);

		ImGui::Text("\n\nAmbient Occlusion");
		ImGui::InputInt("Max iter.##Ambient Occlusion", &gAOMaxIters);
		ImGui::DragFloat("Step size##Ambient Occlusion", &gAOStepSize, 0.001f, 0.0f);
		ImGui::DragFloat("Strength##Ambient Occlusion", &gAOStrength, 0.001f, 0.0f);

		ImGui::Text("\n\nBRDF");
		ImGui::ColorEdit3("Ambient##BRDF", &gAmbient.x);
		ImGui::ColorEdit3("Diffuse##BRDF", &gDiffuse.x);
		ImGui::ColorEdit3("Roughness##BRDF", &gCookRoughness.x);
		ImGui::DragFloat3("IOR##BRDF", &gCookIOR.x);

		ImGui::Text("\n\nSoft Shadow");
		ImGui::InputInt("Max iter.##Soft shadow", &gShadowMaxiter);
		ImGui::DragFloat("Epsilon##Soft shadow", &gShadowEpsilon, 0.001f, 0.0001f, 1.0f, "%.5f");
	}
	ImGui::End();
	return uiIsOpen;
}
