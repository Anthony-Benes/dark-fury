#include "Engine.h"
#include "Engine/Core/EntryPoint.h"

#include "EditorLayer.hpp"
	
static bool viewFPS = false;

class ClearLayer : public Engine::Layer
{
public:
	ClearLayer() : Layer("Clear"), m_ClearColor({ 0.1f, 0.3f, 0.1f, 1.0f }) {}
	ClearLayer(glm::vec4 clearColor) : Layer("Clear"), m_ClearColor(clearColor) {}
	void OnUpdate(Engine::Timestep ts) {
		Engine::RenderCommand::SetClearColor(m_ClearColor);
		Engine::RenderCommand::Clear();
	}
private:
	glm::vec4 m_ClearColor;
};

class Editor : public Engine::Application
{
public:
	Editor(Engine::ApplicationCommandLineArgs args) {
		PushLayer(new ClearLayer({ 0.2f, 0.2f, 0.2f, 1.0f }));
		PushLayer(new EditorLayer());
	};
	~Editor() {};
};

Engine::Application* Engine::CreateApplication(Engine::ApplicationCommandLineArgs args) {
	return new Editor(args);
}