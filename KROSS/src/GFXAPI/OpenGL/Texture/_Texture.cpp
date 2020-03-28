#include "Kross_pch.h"
#include "_Texture.h"
#include "stb_image.h"
#include "GFXAPI/OpenGL/GLErrors.h"
#include "GFXAPI/OpenGL/Context.h"

namespace Kross::OpenGL::Texture {
	const int Base::QueryMaxSlots()
	{
		GLerror();
		static int query = -1;
		if (query != -1) return query;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &query);
		return query;
	}

	T2D::T2D(uint32_t width, uint32_t height, const std::string& name, bool alpha, void* data)
		:
		m_strName(name),
		m_unWidth(width),
		m_unHeight(height),
		m_CurrentSlot(IncSlot()),
		m_unInternalFormat(alpha ? GL_RGBA8 : GL_RGB8),
		m_unDataFormat(alpha ? GL_RGBA : GL_RGB)
	{
		KROSS_PROFILE_FUNC();
		GLerror();
		if (Context::GetVersion() < 4.5)
		{
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_unInternalFormat, m_unWidth, m_unHeight);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, m_unInternalFormat, m_unWidth, m_unHeight, 0, m_unDataFormat, GL_UNSIGNED_BYTE, (const void*)data);
		KROSS_CORE_INFO("[ {0} ] |||| Texture '{1}' Created", __FUNCTION__, m_strName);
	}
	T2D::T2D(uint32_t width, uint32_t height, bool alpha, void* data)
		: m_unWidth(width),
		m_unHeight(height),
		m_CurrentSlot(IncSlot()),
		m_unInternalFormat(alpha ? GL_RGBA8 : GL_RGB8),
		m_unDataFormat(alpha ? GL_RGBA : GL_RGB)
	{
		KROSS_PROFILE_FUNC();
		GLerror();
		if (Context::GetVersion() < 4.5f)
		{
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else
		{	 //------------OpenGL 4.5------------
			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

			glTextureStorage2D(m_RendererID, 1, m_unInternalFormat, m_unWidth, m_unHeight);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, m_unInternalFormat, m_unWidth, m_unHeight, 0, m_unDataFormat, GL_UNSIGNED_BYTE, (const void*)data);
		KROSS_CORE_INFO("[ {0} ] |||| Texture '{1}' Created", __FUNCTION__, m_strName);
	}
	T2D::T2D(uint32_t width, uint32_t height, void* data)
		: m_unWidth(width),
		m_unHeight(height),
		m_CurrentSlot(IncSlot()),
		m_unInternalFormat(GL_RGBA8),
		m_unDataFormat(GL_RGBA)
	{
		KROSS_PROFILE_FUNC();
		GLerror();
		if (Context::GetVersion() < 4.5f)
		{
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else
		{	 //------------OpenGL 4.5------------
			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

			glTextureStorage2D(m_RendererID, 1, m_unInternalFormat, m_unWidth, m_unHeight);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, m_unInternalFormat, m_unWidth, m_unHeight, 0, m_unDataFormat, GL_UNSIGNED_BYTE, (const void*)data);
		KROSS_CORE_INFO("[ {0} ] |||| Texture '{1}' Created", __FUNCTION__, m_strName);
	}
	T2D::T2D(const std::string& path, const std::string& name)
		:
		m_strPath(path),
		m_strName(name),
		m_unDataFormat(0),
		m_unInternalFormat(0),
		m_CurrentSlot(IncSlot())
	{
		KROSS_PROFILE_FUNC();
		GLerror();
		if (name != "")
		{
			m_strName = FileName(path);
		}

		stbi_set_flip_vertically_on_load(true);

		int width, height, channels;
		stbi_uc* data = nullptr;
		{
			KROSS_PROFILE_SCOPE("T2D::T2D(const std::string&) - stbi_load");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		if (!data) { KROSS_MSGBOX("Failed to load image!\nFILE: " + path, __FUNCTION__, _ERROR_); }

		m_unWidth = width;
		m_unHeight = height;

		if (channels == 3)
		{
			m_unDataFormat = GL_RGB;
			m_unInternalFormat = GL_RGB8;
		}
		else if (channels == 4)
		{
			m_unDataFormat = GL_RGBA;
			m_unInternalFormat = GL_RGBA8;
		}
		else
		{
			KROSS_CORE_WARN("[ {0} ] |||| Image format not suported!\nFILE: {1}", __FUNCTION__, path);
		}

		if (Context::GetVersion() < 4.5f)
		{
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			if (data) glTexImage2D(GL_TEXTURE_2D, 0, m_unInternalFormat, m_unWidth, m_unHeight, 0, m_unDataFormat, GL_UNSIGNED_BYTE, (const void*)data);
		}
		else
		{	 //------------OpenGL 4.5------------
			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

			glTextureStorage2D(m_RendererID, 1, GL_RGB8, m_unWidth, m_unHeight);
			glTextureStorage2D(m_RendererID, 1, GL_RGB16, m_unWidth, m_unHeight);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_unWidth, m_unHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
		}

		stbi_image_free(data);
		KROSS_CORE_INFO("[ {0} ] |||| Texture '{1}' Created", __FUNCTION__, m_strName);
	}
	T2D::~T2D()
	{
		KROSS_PROFILE_FUNC();
		GLerror();
		glCall(glDeleteTextures(1, &m_RendererID));
		KROSS_CORE_INFO("[ {0} ] |||| Texture '{1}' Destructed", __FUNCTION__, m_strName);
	}
	void T2D::SetData(void* data, uint32_t size)
	{
		KROSS_PROFILE_FUNC();
		GLerror();
		if (size > m_unWidth* m_unHeight* (m_unDataFormat == GL_RGBA ? 4 : 3)) {
			KROSS_CORE_WARN("[ {0} ] |||| Texture Overflow.", __FUNCTION__); }
		Bind();
		glCall(glTexImage2D(GL_TEXTURE_2D, 0, m_unInternalFormat, m_unWidth, m_unHeight, 0, m_unDataFormat, GL_UNSIGNED_BYTE, (const void*)data));
	}
	void T2D::Bind(uint32_t slot) const
	{
		KROSS_PROFILE_FUNC();
		GLerror();
		if (m_RendererID)
		{
			if (Context::GetVersion() < 4.5)
			{
				if (slot) { glActiveTexture(GL_TEXTURE0 + slot); }
				else { glActiveTexture(GL_TEXTURE0 + 0); }
				glBindTexture(GL_TEXTURE_2D, m_RendererID);
			}
			else
			{
				if (slot) { glBindTextureUnit(slot, m_RendererID); }
				else { glBindTextureUnit(m_CurrentSlot, m_RendererID); }
			}
		}
	}
}