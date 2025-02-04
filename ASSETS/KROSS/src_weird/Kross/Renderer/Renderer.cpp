#include "Kross_pch.h"
#include "Renderer.h"

#include "GFXAPI/OpenGL/RenderersAPI.h"
//#include "GFXAPI/DirectX/RenderersAPI.h"

#include "Renderer2D.h"
#include "Renderer3D.h"

namespace Kross {
	RendererAPI* RenderCommand::s_RendererAPI = new GraphicsAPI::RendererAPI;
	Renderer::Dimentions Renderer::s_dDims = Renderer::Dimentions::D2;

	void Renderer::Init(Renderer::Dimentions dims)
	{
		KROSS_PROFILE_FUNC();
		s_dDims = dims;

		RenderCommand::Init();
		switch (s_dDims) {
		case Dimentions::D2: Renderer2D::Init(); break;
		case Dimentions::D3: Renderer3D::Init(); break;
		default: { KROSS_CORE_ERROR("[{0}] Renderer::Dimentions not supported", __FUNCTION__); return; }
		}

		KROSS_CORE_TRACE("[Kross::Renderer] Renderer Initiated");
	}

	void Renderer::Shutdown()
	{
		KROSS_PROFILE_FUNC();

		RenderCommand::Shutdown();

		switch (s_dDims) {
		case Dimentions::D2: Renderer2D::Shutdown(); break;
		case Dimentions::D3: Renderer3D::Shutdown(); break;
		default: { KROSS_CORE_ERROR("[{0}] Renderer::Dimentions not supported", __FUNCTION__); return; }
		}

		KROSS_CORE_TRACE("[{0}] Renderer Finished", __FUNCTION__);
	}

	//void Renderer::Scene::Begin(Camera::Camera& camera)
	//{
	//	Scene::m_Data->ViewProjectionMatrix = camera.GetVPM();
	//}

	//void Renderer::Scene::End()
	//{
	//}

	//void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4 transform)
	//{
	//	shader->Bind();

	//	shader->SetMat4("u_ViewProjection", Scene::m_Data->ViewProjectionMatrix);
	//	shader->SetMat4("u_Transform", transform);

	//	va->Bind();
	//	Renderer::Command::DrawIndexed(va);
	//}

	//void Renderer::Submit(const Ref<Shader>& shader, const Scope<VertexArray>& va, const glm::mat4 transform)
	//{
	//	shader->Bind();

	//	shader->SetMat4("u_ViewProjection", Scene::m_Data->ViewProjectionMatrix);
	//	shader->SetMat4("u_Transform", transform);

	//	va->Bind();
	//	Renderer::Command::DrawIndexed(va);
	//}

	//Ref<Renderer::ShaderLibrary> Renderer::GetShaderLibrary()
	//{
	//	return shaderLibrary;
	//}

	//void Renderer::ShaderLibrary::Clear()
	//{
	//	shaders.clear();
	//}

	//void Renderer::ShaderLibrary::Add(const Ref<Shader>& shader)
	//{
	//	const std::string& name = shader->GetName();
	//	Add(name, shader);
	//}

	//void Renderer::ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	//{
	//	if (exist(name)) { KROSS_CORE_WARN("[{0}] Shader '{1}' already in library. ", __FUNCTION__, name); }
	//	shaders[name] = shader;
	//}

	//Ref<Shader> Renderer::ShaderLibrary::Load(const Ref<Shader>& shader)
	//{
	//	Add(shader);
	//	return shader;
	//}

	//Ref<Shader> Renderer::ShaderLibrary::Load(const std::string & filepath)
	//{
	//	Ref<Shader> shader = Shader::CreateRef(filepath);
	//	Add(shader);
	//	return shader;
	//}

	//Ref<Shader> Renderer::ShaderLibrary::Load(const std::string & name, const std::string & filepath)
	//{
	//	Ref<Shader> shader = Shader::CreateRef(name, filepath);
	//	Add(name, shader);
	//	return shader;
	//}

	//Ref<Shader> Renderer::ShaderLibrary::Get(const std::string& name)
	//{
	//	if (!exist(name)) { KROSS_CORE_WARN("[{0}] Shader '{1}' NOT FOUND in library. ", __FUNCTION__, name); }
	//	return shaders[name];
	//}
}
