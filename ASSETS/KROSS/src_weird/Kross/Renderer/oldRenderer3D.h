#pragma once

#include "Cameras/Camera.h"
#include "Textures/Textures.h"
#include "Textures/Atlas.h"
#include "Shaders.h"

namespace Kross {
	struct CubeParams;
	class Renderer3D
	{
		static bool s_bSceneBegan;
		static bool s_bBatch;
		static bool s_bRendererInitialized;

	public:
		struct Stats
		{
			uint32_t DrawCount = 0;
			uint32_t QuadCount = 0;
		};

		Renderer3D() = delete;
		Renderer3D(Renderer3D&& other) = delete;
		Renderer3D& operator =(Renderer3D&& other) = delete;

		static const Stats& getStats();
		static void ResetStats();

		static void Init();
		static void Shutdown();

		static void Begin(Ref<Camera::Camera>& camera);
		static void End();

		static void SwitchShader(const Ref<Shader>& shader);

		static void DrawCube(const CubeParams& params);
	};

}
