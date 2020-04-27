#include <Kross_pch.h>

#include "VertexArray.h"
#include "Shaders.h"
#include "Renderer.h"
#include "Kross/Core/Application.h"
#include "Kross/Core/Stack.h"

#include "Textures/Array.h"
#include "Textures/Atlas.h"

#include <glm/gtc/type_ptr.hpp>

namespace Kross {
	struct R3DData {
		// Basic 3D Shader
		Ref<Shader> sho;

		//Basic data
		Ref<VertexArray> vao;
		Ref<Buffer::Vertex> vbo;
		Ref<Buffer::Index> ibo;

		// FrameBuffer
		Ref<Texture::FrameBuffer> fbo;

		static constexpr glm::mat4 mat4_ = glm::mat4(1.0f);
	};
	// Renderer2D data
	static R3DData* data;

	const Ref<Texture::FrameBuffer> Renderer3D::GetFrameBuffer()
	{
		if (!data)
		{
			KROSS_CORE_ERROR("[ {0} ] |||| Renderer has not yet been initialized.", __FUNCTION__);
			return nullptr;
		}
		return data->fbo;
	}
	bool Renderer3D::Is_Initiated()
	{
		return data;
	}
	void Renderer3D::Init()
	{
		KROSS_PROFILE_FUNC();
		KROSS_CORE_INFO("[ {0} ] |||| Renderer 3D Initiating...", __FUNCTION__);

		if (!data) {
			data = new R3DData();

			float vertices[] = {  // index
				0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f, 1.0f, // 0 - Front	Bottom	Left
				0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f, 1.0f, // 1 - Front	Top		Left
				1.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f, 1.0f, // 2 - Front	Top		Right
				1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f, 1.0f, // 3 - Front	Bottom	Right

				0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f, // 4 - Back		Bottom	Left
				0.0f, 1.0f, 1.0f,	0.0f, 1.0f, 1.0f, 1.0f, // 5 - Back		Top		Left
				1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f, 1.0f, // 6 - Back		Top		Right
				1.0f, 0.0f, 1.0f,	1.0f, 0.0f, 1.0f, 1.0f  // 7 - Back		Bottom	Right
			};
			uint32_t indices[] = {
				4, 0, 3,	3, 7, 4, // Bottom Face
				0, 1, 2,	2, 3, 0, // Front  Face
				3, 2, 6,	6, 7, 3, // Right  Face
				7, 6, 5,	5, 4, 7, // Back   Face
				4, 5, 1,	1, 0, 4, // Left   Face
				1, 5, 6,	6, 2, 1, // Top    Face
			};

			//Mesh data::base;
			//data->base.addData()
			//Model cube;
			

			data->vao = VertexArray::CreateScope();
			data->vbo = Buffer::Vertex::Create(vertices, sizeof(vertices), false);
			data->vbo->SetLayout({
				{ Buffer::ShaderDataType::Float3, "a_Position" },
				{ Buffer::ShaderDataType::Float4, "a_Color"}
				});
			data->vao->AddVertex(data->vbo);

			data->ibo = Buffer::Index::Create(indices, sizeof(indices) / sizeof(uint32_t));
			data->vao->SetIndex(data->ibo);

			Stack<Shader>::get().Add(data->sho = Shader::CreateRef("assets/shaders/OpenGL/Shader3D"));
			data->sho->SetFloat("u_Repeat", 1);
			data->sho->SetFloat4("u_Color", glm::vec4(0.75f));

			data->fbo = Texture::FrameBuffer::Create("Renderer3D");
		}
		else
		{
			KROSS_MSGBOX("Renderer3D is already initialized. Cannot call Renderer3D::Init(void) twice. Forget to call Renderer3D::Shutdown(void)?", __FUNCTION__, _ERROR_);
		}
	}
	void Renderer3D::Shutdown()
	{
		KROSS_PROFILE_FUNC();
		KROSS_CORE_INFO("[ {0} ] |||| Renderer 3D Shutting Down...", __FUNCTION__);

		if (data) {
			Stack<Shader>::get().Del("Shader3D");
			delete data;
		}
		else
		{
			KROSS_MSGBOX("Renderer3D is not initialized. Cannot call Renderer3D::Shutdown(void) while Renderer3D::Init(void) is not called.", __FUNCTION__, _ERROR_);
		}
	}
	void Renderer3D::Begin(Camera::Camera& camera)
	{
		if (!data)
		{
			KROSS_CORE_ERROR("[ {0} ] |||| Renderer has not yet been initialized.", __FUNCTION__);
			return;
		}
		data->sho->Bind();
		data->sho->SetMat4("u_ViewProjection", camera.GetVPM());
		data->fbo->Bind();
	}
	void Renderer3D::End()
	{
		if (!data)
		{
			KROSS_CORE_ERROR("[ {0} ] |||| Renderer has not yet been initialized.", __FUNCTION__);
			return;
		}
		data->fbo->unBind();
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color)
	{
		KROSS_PROFILE_FUNC();
		if (!data)
		{
			KROSS_CORE_ERROR("[ {0} ] |||| Renderer has not yet been initialized.", __FUNCTION__);
			return;
		}
		glm::mat4 transform;
		{
			KROSS_PROFILE_SCOPE("transform");
			transform = glm::translate(data->mat4_, position)
				* glm::scale(data->mat4_, scale);
		}
		data->sho->Bind();
		data->sho->SetMat4("u_Transform", transform);
		data->sho->SetFloat4("u_Color", color);
		data->vao->Bind();
		Renderer::Command::DrawIndexed(data->vao);
	}
	void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& scale, float angle, const glm::vec3& rotation_axis, const glm::vec4& color)
	{
		KROSS_PROFILE_FUNC();
		if (!data)
		{
			KROSS_CORE_ERROR("[ {0} ] |||| Renderer has not yet been initialized.", __FUNCTION__);
			return;
		}
		glm::mat4 transform;
		{
			KROSS_PROFILE_SCOPE("transform");
			transform = glm::translate(data->mat4_, position)
				* glm::rotate(data->mat4_, angle, rotation_axis)
				* glm::scale(data->mat4_, scale);
		}
		data->sho->Bind();
		data->sho->SetMat4("u_Transform", transform);
		data->sho->SetFloat4("u_Color", color);
		data->vao->Bind();
		Renderer::Command::DrawIndexed(data->vao);
	}
	void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& scale, const glm::mat4& rotation, const glm::vec4& color)
	{
		KROSS_PROFILE_FUNC();
		if (!data)
		{
			KROSS_CORE_ERROR("[ {0} ] |||| Renderer has not yet been initialized.", __FUNCTION__);
			return;
		}
		glm::mat4 transform;
		{
			KROSS_PROFILE_SCOPE("transform");
			transform = glm::translate(data->mat4_, position)
				* rotation
				* glm::scale(data->mat4_, scale);
		}
		data->sho->Bind();
		data->sho->SetMat4("u_Transform", transform);
		data->sho->SetFloat4("u_Color", color);
		data->vao->Bind();
		Renderer::Command::DrawIndexed(data->vao);
	}


}