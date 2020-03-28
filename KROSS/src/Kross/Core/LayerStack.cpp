#include "Kross_pch.h"
#include "LayerStack.h"

namespace Kross {
	LayerStack::LayerStack()
	{
		KROSS_CORE_INFO("[ {0} ] |||| LayerStack created", __FUNCTION__);
	}

	LayerStack::~LayerStack()
	{
		KROSS_PROFILE_FUNC();
		for (Ref<Layer>& layer : m_vecLayers)
		{
			const std::string name = layer->GetName();
			KROSS_CORE_INFO("[ {0} ] |||| Deleting layer '{1}'", __FUNCTION__, name);
			//layer->~Layer();
			layer.~shared_ptr();
			KROSS_CORE_INFO("[ {0} ] |||| Layer '{1}' deleted", __FUNCTION__, name);
		}
		m_vecLayers.clear();
	}

	void LayerStack::PushLayer(const Ref<Layer>& layer)
	{
		m_vecLayers.emplace(m_vecLayers.begin() + m_unLayersInsertIndex, layer);
		m_unLayersInsertIndex++;
	}

	void LayerStack::PushOverlay(const Ref<Layer>& overlay)
	{
		m_vecLayers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(const Ref<Layer>& layer)
	{
		auto it = std::find(m_vecLayers.begin(), m_vecLayers.end() + m_unLayersInsertIndex, layer);
		if (it != m_vecLayers.end() + m_unLayersInsertIndex)
		{
			m_vecLayers.erase(it);
			m_unLayersInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(const Ref<Layer>& overlay)
	{
		auto it = std::find(m_vecLayers.begin() + m_unLayersInsertIndex, m_vecLayers.end(), overlay);
		if (it != m_vecLayers.end())
		{
			m_vecLayers.erase(it);
		}
	}
}