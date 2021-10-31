#include "Engine.h"
#include "Engine/Core/EntryPoint.h"

#include "ExampleLayer.hpp"
#include "Sample2D.hpp"

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

class ExampleOverLay : public Engine::Layer
{
public:
	ExampleOverLay() : Layer("Example") {}

	void OnUpdate(Engine::Timestep ts) {
		if (viewFPS) { ENG_TRACE("%.0f FPS", (1.0f / ts)); }
	}
};

class Game : public Engine::Application
{
public:
	Game(Engine::ApplicationCommandLineArgs args) {
		PushLayer(new ClearLayer({ 0.2f, 0.2f, 0.2f, 1.0f }));
		PushOverlay(new ExampleOverLay());
		//PushLayer(new ExampleLayer());
		PushLayer(new Sample2D());
	};
	~Game() {};
};

Engine::Application* Engine::CreateApplication(Engine::ApplicationCommandLineArgs args) {
	return new Game(args);
}