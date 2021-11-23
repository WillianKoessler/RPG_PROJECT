#include <Kross_pch.h>
#include "Shader.h"
#include "GLErrors.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "Kross/Util/FileUtil.h"

namespace Kross::OpenGL {
	void Shader::CreateShader(const char* cfilepath)
	{
		KROSS_PROFILE_FUNC();
		std::string filepath(cfilepath);
		KROSS_CORE_INFO("[{0}] Creating Shader '{1}'...", __FUNCTION__, m_strName);
		if (filepath.rfind('.') != std::string::npos)
		{
			const char* src = ReadFile(cfilepath);
			if (src != "")
			{
				if (Compile(PreProcess(src)))
				{
					KROSS_CORE_INFO("[{0}] Shader '{1}' Created Successfully", __FUNCTION__, m_strName);
					return;
				}
			}
		}
		KROSS_CORE_WARN("[{0}] Shader '{1}' Failed to Create", __FUNCTION__, m_strName);
	}
	Shader::Shader(const char* filepath)
		: m_RendererID(UINT32_MAX)
	{
		KROSS_PROFILE_FUNC();
		m_strName = FileName(filepath);
		CreateShader(filepath);
	}
	Shader::Shader(const char* name, const char* filepath)
		: m_RendererID(UINT32_MAX),
		m_strName(name)
	{
		KROSS_PROFILE_FUNC();
		CreateShader(filepath);
	}
	Shader::Shader(const char* name, const char* vertexSource, const char* fragmentSource)
		: m_RendererID(UINT32_MAX),
		m_strName(name)
	{
		KROSS_PROFILE_FUNC();
		KROSS_CORE_INFO("[{0}] Creating Shader '{1}'...", __FUNCTION__, m_strName);
		uint32_t program;
		glCall(program = glCreateProgram());
		uint32_t shaders[] = {
			Compile(program, GL_VERTEX_SHADER, vertexSource),
			Compile(program, GL_FRAGMENT_SHADER, fragmentSource)
		};

		if (shaders[0] && shaders[1] && Link(program, shaders, sizeof(shaders) / sizeof(uint32_t)))
			KROSS_CORE_INFO("[{0}] Shader '{1}' Created Successfully", __FUNCTION__, m_strName);
		else
			KROSS_CORE_WARN("[{0}] Shader '{1}' Failed to Create", __FUNCTION__, m_strName);
	}
	Shader::Shader(const char* name, const std::initializer_list<const char*>& sources)
		: m_RendererID(UINT32_MAX),
		m_strName(name)
	{
		KROSS_PROFILE_FUNC();
		KROSS_CORE_INFO("[{0}] Creating Shader '{1}'...", __FUNCTION__, m_strName);
		if (sources.size() < 1) { KROSS_CORE_ERROR("[{0}] List of Sources must have at least 1 source file."); return; }

		uint32_t programID, i = 0u;
		std::vector<uint32_t> shaders;

		glCall(programID = glCreateProgram());

		for (std::string path : sources)
			shaders.push_back(Compile(programID, GetGlType((path.substr(path.find_last_of('.'))).c_str()), ReadFile(path.c_str())));

		bool valid = true;
		for (int i = 0; i < sources.size(); i++) if (!shaders[i]) valid = false;

		if (valid && Link(programID, shaders.data(), (uint32_t)shaders.size())) KROSS_CORE_INFO("[{0}] Shader '{1}' Created Successfully", __FUNCTION__, m_strName);
		else KROSS_CORE_WARN("[{0}] Shader '{1}' Failed to Create", __FUNCTION__, m_strName);
	}
	Shader::~Shader()
	{
		KROSS_PROFILE_FUNC();
		if (m_RendererID != UINT32_MAX)
		{
			KROSS_CORE_INFO("[{0}] Deleting Shader '{1}' ...", __FUNCTION__, m_strName);
			unBind();
			glCall(glDeleteProgram(m_RendererID));
			UniformCache.clear();
			KROSS_CORE_INFO("[{0}] Shader '{1}' Deleted Successfully", __FUNCTION__, m_strName);
		}
		else
		{
			UniformCache.clear();
			KROSS_CORE_WARN("[{0}] Trying to delete a Invalid Shader '{1}'", __FUNCTION__, m_strName);
		}
	}
	const char* Shader::ReadFile(const char* filepath)
	{
		KROSS_PROFILE_FUNC();
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
			return result.c_str();
		}
		else
		{
			KROSS_MSGBOX("Could not open file " + std::string(filepath), __FUNCTION__, _WARN_);
			return "";
		}
	}
	std::unordered_map<unsigned int, const char*> Shader::PreProcess(const char* csource)
	{
		KROSS_PROFILE_FUNC();
		std::unordered_map<GLenum, const char*> sources;
		std::string source(csource);
		const char* maskType = "#type";
		size_t lengthMaskType = strlen(maskType);
		size_t pos = source.find(maskType, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			if (eol == std::string::npos) KROSS_MSGBOX("line : " + pos, "Syntax Error", _ERROR_);
			size_t begin = pos + lengthMaskType + 1;
			std::string type = source.substr(begin, eol - begin);
			if (!GetGlType(type.c_str()))
				KROSS_MSGBOX("Invalid shader type specification: " + type, __FUNCTION__, _ERROR_);
			size_t nextlinepos = source.find_first_not_of("\r\n", eol);
			pos = source.find(maskType, nextlinepos);
			sources[GetGlType(type.c_str())] =
				source.substr(
					nextlinepos,
					pos - (nextlinepos == std::string::npos ? source.size() - 1 : nextlinepos)
				).c_str();
		}
		return sources;
	}
	unsigned int Shader::GetGlType(const char* ctype)
	{
		std::string type(ctype);
		if (!type.compare("vertex") || !type.compare(".vert"))
			return GL_VERTEX_SHADER;
		if (!type.compare("fragment") || !type.compare("pixel") || !type.compare(".frag"))
			return GL_FRAGMENT_SHADER;
		if (!type.compare("geometry") || !type.compare(".geom"))
			return GL_GEOMETRY_SHADER;
		if (!type.compare("compute") || !type.compare(".comp"))
			return GL_COMPUTE_SHADER;
		if (!type.compare("tess_eval") || !type.compare(".tese"))
			return GL_TESS_EVALUATION_SHADER;
		if (!type.compare("tess_ctrl") || !type.compare(".tesc"))
			return GL_TESS_CONTROL_SHADER;

		KROSS_MSGBOX("Unknown shader type: " + type, __FUNCTION__, _ERROR_);
		return 0;
	}
	const char* Shader::GetShaderType(unsigned int type)
	{
		switch (type)
		{
		case GL_VERTEX_SHADER:			return "Vertex Shader";
		case GL_FRAGMENT_SHADER:		return "Fragment Shader";
		case GL_GEOMETRY_SHADER:		return "Geometry Shader";
		case GL_TESS_CONTROL_SHADER:	return "Tesselation Shader";
		case GL_TESS_EVALUATION_SHADER:	return "Evaluation Shader";
		case GL_COMPUTE_SHADER:			return "Compute Shader";
		}
		return "Unknown Shader";
	}
	bool Shader::Compile(const std::unordered_map<unsigned int, const char*>& sources)
	{
		KROSS_PROFILE_FUNC();
		KROSS_CORE_TRACE("[{0}] Compiling Shader '{1}'...", __FUNCTION__, m_strName);

		GLuint program;
		glCall(program = glCreateProgram());
		//if(sources.size() > 5) KROSS_CORE_WARN("[{0}] {1} Shader Sources it's not supported. Maximum = 2", __FUNCTION__, sources.size());
		//std::array<GLenum, 5> shaderIDs;
		std::vector<GLenum> shadersIDs;
		int index = 0;
		for (auto& kv : sources)
		{
			GLenum type = kv.first;
			const char* source = kv.second;

			GLuint shader;
			glCall(shader = glCreateShader(type));

			const GLchar* BinarySource = (const GLchar*)source;
			glCall(glShaderSource(shader, 1, &BinarySource, 0));

			glCall(glCompileShader(shader));

			GLint isCompiled = 0;
			glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled));
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength));
				char* infoLog = new char[maxLength];
				glCall(glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]));
				glCall(glDeleteShader(shader));
				KROSS_CORE_WARN("[{0}] Shader '{1}' failed to Compile", __FUNCTION__, m_strName);
				KROSS_CORE_ERROR("[{0}]\n{1}", __FUNCTION__, infoLog);
				return false;
			}
			glCall(glAttachShader(program, shader));
			//shaderIDs[index++] = shader;
			shadersIDs.push_back(shader);
		}

		KROSS_CORE_TRACE("[{0}] Shader '{1}' Compiled Successfully!", __FUNCTION__, m_strName);

		glCall(glLinkProgram(program));

		GLint isLinked = 0;
		glCall(glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked));
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));
			char* infoLog = new char[maxLength];
			glCall(glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]));
			glCall(glDeleteProgram(program));
			//if (shaderIDs.size() > 0)
				//for (GLenum id : shaderIDs)
			if (shadersIDs.size() > 0)
				for (GLenum id : shadersIDs)
				{
					glCall(glDeleteShader(id));
				}
			KROSS_CORE_WARN("[{0}] Shader Failed to Link", __FUNCTION__);
			KROSS_CORE_ERROR("[{0}]", infoLog);
			return false;
		}
		else
		{
			m_RendererID = program;

			//if (shaderIDs.size() > 0)
			//	for (GLenum id : shaderIDs)
			if (shadersIDs.size() > 0)
				for (GLenum id : shadersIDs)
					glCall(glDetachShader(program, id));
			Bind();
		}
		return true;
	}
	uint32_t Shader::Compile(uint32_t program, uint32_t type, const char* source)
//	uint32_t Shader::Compile(uint32_t program, int32_t type, const char* source);
	{
		GLuint shader;
		glCall(shader = glCreateShader(type));

		const char* shtype = GetShaderType(type);

		const GLchar* BinarySource = (const GLchar*)source;
		glCall(glShaderSource(shader, 1, &BinarySource, 0));

		glCall(glCompileShader(shader));

		GLint OK = 0;
		glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &OK));
		if (!OK)
		{
			GLint maxLength = 0;
			glCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength));
			char* log = new char[maxLength];
			glCall(glGetShaderInfoLog(shader, maxLength, &maxLength, &log[0]));
			glCall(glDeleteShader(shader));
			KROSS_CORE_WARN("[{0}] Shader '{1}' failed to Compile. ({2})", __FUNCTION__, m_strName, shtype);
			KROSS_CORE_ERROR("[{0}]{2}\n{1}", __FUNCTION__, log, shtype);
			return 0;
		}
		glCall(glAttachShader(program, shader));
		KROSS_CORE_TRACE("[{0}] Shader '{1}' Compiled Successifuly. ({2})", __FUNCTION__, m_strName, shtype);
		return shader;
	}
	bool Shader::Link(unsigned int program, const uint32_t* shaders, uint32_t uShadersPtrSize)
	{
		glCall(glLinkProgram(program));
		GLint OK = 0;
		glCall(glGetProgramiv(program, GL_LINK_STATUS, (int*)&OK));
		if (!OK)
		{
			GLint maxLength = 0;
			glCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));
			char* log = new char[maxLength];
			glCall(glGetProgramInfoLog(program, maxLength, &maxLength, &log[0]));
			glCall(glDeleteProgram(program));
			if (uShadersPtrSize > 0)
				for (GLenum id = 0u; id < uShadersPtrSize; id++)
				{
					glCall(glDeleteShader(id));
				}
			KROSS_CORE_WARN("[{0}] Shader '{1}' Failed to Link", __FUNCTION__, m_strName);
			KROSS_CORE_ERROR("[{0}]\n{1}", __FUNCTION__, log);
			return false;
		}
		else
		{
			m_RendererID = program;
			if (uShadersPtrSize > 0)
				for (GLenum id = 0u; id < uShadersPtrSize; id++)
				{
					glCall(glDetachShader(program, id));
				}
			Bind();
			return true;
		}
	}
	void Shader::Bind() const
	{
		KROSS_PROFILE_FUNC();
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUseProgram(m_RendererID));
		}
	}
	void Shader::unBind() const
	{
		KROSS_PROFILE_FUNC();
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUseProgram(0));
		}
	}
	void Shader::SetIntV(const char* name, size_t count, int* value) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformIntV(name, count, value);
	}
	void Shader::SetInt(const char* name, int value) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformInt(name, value);
	}
	void Shader::SetInt2(const char* name, const glm::vec2& values) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformInt2(name, values);
	}
	void Shader::SetInt3(const char* name, const glm::vec3& values) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformInt3(name, values);
	}
	void Shader::SetInt4(const char* name, const glm::vec4& values) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformInt4(name, values);
	}
	void Shader::SetFloat(const char* name, float value) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformFloat(name, value);
	}
	void Shader::SetFloat2(const char* name, const glm::vec2& values) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformFloat2(name, values);
	}
	void Shader::SetFloat3(const char* name, const glm::vec3& values) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformFloat3(name, values);
	}
	void Shader::SetFloat4(const char* name, const glm::vec4& values) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformFloat4(name, values);
	}
	void Shader::SetMat3(const char* name, const glm::mat3& matrix) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformMat3(name, matrix);
	}
	void Shader::SetMat4(const char* name, const glm::mat4& matrix) const
	{
		KROSS_PROFILE_FUNC();
		UploadUniformMat4(name, matrix);
	}
	void Shader::UploadUniformIntV(const char* name, size_t count, int* value) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			if (!value)
			{
				int* defaults = new int[count];
				for (size_t i = 0; i < count; i++) defaults[i] = (int)i;
				KROSS_CORE_WARN("[{0}] Nullptr value. Setting defaults.", __FUNCTION__);
				glCall(glUniform1iv(GetUniformLocation(name), (GLsizei)count, defaults));
			}
			else
			{
				glCall(glUniform1iv(GetUniformLocation(name), (GLsizei)count, value));
			}
		}
	}
	void Shader::UploadUniformInt(const char* name, int value) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniform1i(GetUniformLocation(name), value));
		}
	}
	void Shader::UploadUniformInt2(const char* name, const glm::vec2& values) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniform2i(GetUniformLocation(name), (int)values.x, (int)values.y));
		}
	}
	void Shader::UploadUniformInt3(const char* name, const glm::vec3& values) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniform3i(GetUniformLocation(name), (int)values.x, (int)values.y, (int)values.z));
		}
	}
	void Shader::UploadUniformInt4(const char* name, const glm::vec4& values) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniform4i(GetUniformLocation(name), (int)values.x, (int)values.y, (int)values.z, (int)values.w));
		}
	}
	void Shader::UploadUniformFloat(const char* name, float value) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniform1f(GetUniformLocation(name), value));
		}
	}
	void Shader::UploadUniformFloat2(const char* name, const glm::vec2& values) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniform2f(GetUniformLocation(name), values.x, values.y));
		}
	}
	void Shader::UploadUniformFloat3(const char* name, const glm::vec3& values) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniform3f(GetUniformLocation(name), values.x, values.y, values.z));
		}
	}
	void Shader::UploadUniformFloat4(const char* name, const glm::vec4& values) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniform4f(GetUniformLocation(name), values.x, values.y, values.z, values.w));
		}
	}
	void Shader::UploadUniformMat3(const char* name, const glm::mat3& matrix) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniformMatrix4fv(GetUniformLocation(name), 1, false, glm::value_ptr(matrix)));
		}
	}
	void Shader::UploadUniformMat4(const char* name, const glm::mat4& matrix) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			glCall(glUniformMatrix4fv(GetUniformLocation(name), 1, false, glm::value_ptr(matrix)));
		}
	}
	int Shader::GetUniformLocation(const char* name) const
	{
		if (m_RendererID != UINT32_MAX)
		{
			for (UniformLocation* uniform : UniformCache)
			{
				if (uniform->name == name) return uniform->location;
			}
			int location;
			glCall(location = glGetUniformLocation(m_RendererID, name));
			UniformCache.push_back(new UniformLocation(name, location));
			return location;
		}
		return -1;
	}
}