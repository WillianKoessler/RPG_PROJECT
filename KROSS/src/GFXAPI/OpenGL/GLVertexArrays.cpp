#include "Kross_pch.h"
#include "GLVertexArrays.h"
#include "GLErrors.h"

namespace Kross::OpenGL {

	unsigned int convertion_to_GLTYPE(const Kross::Buffer::ShaderDataType type)
	{
		if (type == Kross::Buffer::ShaderDataType::Float)		return GL_FLOAT;
		else if (type == Kross::Buffer::ShaderDataType::Float2)	return GL_FLOAT;
		else if (type == Kross::Buffer::ShaderDataType::Float3)	return GL_FLOAT;
		else if (type == Kross::Buffer::ShaderDataType::Float4)	return GL_FLOAT;
		else if (type == Kross::Buffer::ShaderDataType::Mat3)	return GL_FLOAT;
		else if (type == Kross::Buffer::ShaderDataType::Mat4)	return GL_FLOAT;
		else if (type == Kross::Buffer::ShaderDataType::Int)	return GL_INT;
		else if (type == Kross::Buffer::ShaderDataType::Int2)	return GL_INT;
		else if (type == Kross::Buffer::ShaderDataType::Int3)	return GL_INT;
		else if (type == Kross::Buffer::ShaderDataType::Int4)	return GL_INT;
		else if (type == Kross::Buffer::ShaderDataType::uInt)	return GL_UNSIGNED_INT;
		else if (type == Kross::Buffer::ShaderDataType::uInt2)	return GL_UNSIGNED_INT;
		else if (type == Kross::Buffer::ShaderDataType::uInt3)	return GL_UNSIGNED_INT;
		else if (type == Kross::Buffer::ShaderDataType::uInt4)	return GL_UNSIGNED_INT;
		else if (type == Kross::Buffer::ShaderDataType::Bool)	return GL_BOOL;
		return NULL;
	}

	VertexArray::VertexArray(const char* name)
	{
		KROSS_PROFILE_FUNC();
		glCall(glGenVertexArrays(1, &m_RendererID));
		Bind();
		SetName(name);
		KROSS_INFO("'{0}' Constructed", GetName());
	}

	VertexArray::~VertexArray()
	{
		KROSS_PROFILE_FUNC();
		glCall(glDeleteVertexArrays(1, &m_RendererID));
	}

	void VertexArray::Bind() const
	{
		KROSS_PROFILE_FUNC();
		glCall(glBindVertexArray(m_RendererID));
	}

	void VertexArray::unBind() const
	{
		KROSS_PROFILE_FUNC();
		glCall(glBindVertexArray(0));
	}

	void VertexArray::AddVoxel(const Ref<Kross::Buffer::Vertex>& voxel)
	{
		KROSS_PROFILE_FUNC();
		KROSS_WARN("CALLING UNFINISHED CODE. BUGS MAY HAPPEN");

		this->Bind();
		voxel->Bind();

		if (voxel->GetLayout().GetElements().size() == 0)
		{
			KROSS_WARN("Voxel Buffer has no layout. Aborting.");
			return;
		}

		uint32_t index = 0;
		const Buffer::Layout& layout = voxel->GetLayout();

		glCall(glEnableVertexAttribArray(0));
		glCall(glVertexAttribIPointer(0, 2, GL_UNSIGNED_INT, 8, nullptr));

		m_vecVertex.push_back(voxel);
		KROSS_TRACE("Voxel Buffer Added");
		//if ((layout.begin()->Type == Buffer::ShaderDataType::Int) ||
		//	(layout.begin()->Type == Buffer::ShaderDataType::Int2) ||
		//	(layout.begin()->Type == Buffer::ShaderDataType::Int3) ||
		//	(layout.begin()->Type == Buffer::ShaderDataType::uInt) ||
		//	(layout.begin()->Type == Buffer::ShaderDataType::uInt2) ||
		//	(layout.begin()->Type == Buffer::ShaderDataType::uInt3))
		//{
		//	for (const Buffer::Element& element : layout)
		//	{
		//		glCall(glEnableVertexAttribArray(index));
		//		glCall(glVertexAttribIPointer(
		//			index,
		//			element.GetComponentCount(),
		//			convertion_to_GLTYPE(element.Type),
		//			layout.GetStride(),
		//			(const void*)element.OffSet
		//		));
		//		index++;
		//	}
		//}
	}

	void VertexArray::AddVertex(const Ref<Kross::Buffer::Vertex>& vertex)
	{
		KROSS_ASSERT(vertex->GetLayout().GetElements().size(), "Vertex Buffer has no layout");
		KROSS_PROFILE_FUNC();
		
		Bind();
		vertex->Bind();
		const auto &layout = vertex->GetLayout();
		for(auto &element : layout) switch (element.Type) {
				case Buffer::ShaderDataType::Float:
				case Buffer::ShaderDataType::Float2:
				case Buffer::ShaderDataType::Float3:
				case Buffer::ShaderDataType::Float4:
					{
						glEnableVertexAttribArray(m_VertexBufferIndex);
						glVertexAttribPointer(
							m_VertexBufferIndex,
							element.GetComponentCount(),
							convertion_to_GLTYPE(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void *)element.OffSet
						);
						m_VertexBufferIndex++;
						break;
					}
				case Buffer::ShaderDataType::Int:
				case Buffer::ShaderDataType::Int2:
				case Buffer::ShaderDataType::Int3:
				case Buffer::ShaderDataType::Int4:
				case Buffer::ShaderDataType::Bool:
					{
						glEnableVertexAttribArray(m_VertexBufferIndex);
						glVertexAttribIPointer(m_VertexBufferIndex,
							element.GetComponentCount(),
							convertion_to_GLTYPE(element.Type),
							layout.GetStride(),
							(const void *)element.OffSet);
						m_VertexBufferIndex++;
						break;
					}
				case Buffer::ShaderDataType::Mat3:
				case Buffer::ShaderDataType::Mat4:
					{
						uint8_t count = element.GetComponentCount();
						for (uint8_t i = 0; i < count; i++)
						{
							glEnableVertexAttribArray(m_VertexBufferIndex);
							glVertexAttribPointer(m_VertexBufferIndex,
								count,
								convertion_to_GLTYPE(element.Type),
								element.Normalized ? GL_TRUE : GL_FALSE,
								layout.GetStride(),
								(const void *)(element.OffSet + sizeof(float) * count * i));
							glVertexAttribDivisor(m_VertexBufferIndex, 1);
							m_VertexBufferIndex++;
						}
						break;
					}
				default:
					KROSS_ASSERT(false, "Unknown ShaderDataType!");
			}
		m_vecVertex.push_back(vertex);
		KROSS_TRACE("Vertex Buffer Added");
	}

	void VertexArray::SetIndex(const Ref<Kross::Buffer::Index>& index)
	{
		KROSS_PROFILE_FUNC();
		this->Bind();
		index->Bind();
		m_Index = index;
		KROSS_TRACE("Index Buffer Setted");
	}
}
