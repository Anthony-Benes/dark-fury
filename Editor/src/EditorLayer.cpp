#include "EditorLayer.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

EditorLayer::EditorLayer()
	: Engine::Layer("Editor"),
	m_CameraController(1280.0f / 720.0f, true, -10.0f),
	m_SquareColor({ 1.0f, 1.0f, 1.0f, 1.0f }) {}

void EditorLayer::OnAttach()
{
	ENG_PROFILE_FUNCTION();
	
	Engine::FramebufferSpecification fbSpec;
	fbSpec.Attachments = { Engine::FramebufferTextureFormat::RGBA8, Engine::FramebufferTextureFormat::DEPTH24STENCIL8 };
	fbSpec.Width = 1280.0f;
	fbSpec.Height = 720.0f;
	m_Framebuffer = Engine::Framebuffer::Create(fbSpec);

	m_TextureCB = Engine::Texture2D::Create("assets/textures/Checkerboard.png");

	m_ActiveScene = Engine::CreateRef<Engine::Scene>();

	m_SquareEntity = m_ActiveScene->CreateEntity("Green Square");
	m_SquareEntity.AddComponent<Engine::SpriteRendererComponent>(m_SquareColor);

	m_Camera = m_ActiveScene->CreateEntity("Main Camera");
	m_Camera.AddComponent<Engine::CameraComponent>(Engine::CameraProjection((uint32_t)16, (uint32_t)9, -10.0f, 0.0f));

	auto& squareTransform = m_SquareEntity.GetComponent<Engine::TransformComponent>();
	squareTransform.Scale = glm::vec3{ 5.0f, 5.0f, 1.0f };
	squareTransform.Translation = glm::vec3{ 0.0f, 0.0f, -0.5f };
	auto& squareSprite = m_SquareEntity.GetComponent<Engine::SpriteRendererComponent>();
	squareSprite.Texture = m_TextureCB;

	auto& cameraTransform = m_Camera.GetComponent<Engine::TransformComponent>();
	cameraTransform.Translation = glm::vec3{ 0.0f, 0.0f, 1.0f };

	class CameraController : public Engine::ScriptableEntity
	{
	public:
		virtual void OnCreate() override
		{
		}

		virtual void OnDestroy() override
		{
		}

		virtual void OnUpdate(Engine::Timestep ts) override
		{
			auto& translation = GetComponent<Engine::TransformComponent>().Translation;
			float speed = 5.0f;

			if (Engine::Input::IsKeyPressed(Engine::Key::A))
				translation.x -= speed * ts;
			if (Engine::Input::IsKeyPressed(Engine::Key::D))
				translation.x += speed * ts;
			if (Engine::Input::IsKeyPressed(Engine::Key::W))
				translation.y += speed * ts;
			if (Engine::Input::IsKeyPressed(Engine::Key::S))
				translation.y -= speed * ts;
		}
	};

	m_Camera.AddComponent<Engine::NativeScriptComponent>().Bind<CameraController>();

	//m_CameraController = Engine::OrthographicCameraController(Engine::Camera(m_Camera.GetComponent<Engine::CameraComponent>(), cameraTransform));
}

void EditorLayer::OnUpdate(Engine::Timestep ts)
{
	ENG_PROFILE_FUNCTION();

	// Resize
	{
		ENG_PROFILE_SCOPE("Resize");
		Engine::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
	}

	// Update
	if (m_ViewFocused) { m_CameraController.OnUpdate(ts); }
	
	// Render
	Engine::Renderer2D::ResetStats();
	{
		ENG_PROFILE_SCOPE("Renderer Prep");
		m_Framebuffer->Bind();
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();
	}

	{
		ENG_PROFILE_SCOPE("Renderer Draw");

		m_ActiveScene->OnUpdateRuntime(ts);

		m_Framebuffer->Unbind();
	}
}

void EditorLayer::OnImGuiRender()
{
	ENG_PROFILE_FUNCTION();
	
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("Exit")) Engine::Application::Get().Close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Begin("Settings");
	{
		auto stats = Engine::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	}
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
	ImGui::Begin("Viewport");
	{
		m_ViewFocused = ImGui::IsWindowFocused();
		m_ViewHovered = ImGui::IsWindowHovered();
		Engine::Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewFocused || !m_ViewHovered);
		ImVec2 vpSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { vpSize.x, vpSize.y };
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, { m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1, 0 });
	}
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

void EditorLayer::OnEvent(Engine::Event& event)
{
	m_CameraController.OnEvent(event);
}
