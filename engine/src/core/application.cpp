#include <core/application.hpp>

#include <core/df_memory.hpp>
#include <core/event.hpp>
#include <core/input.hpp>
#include <core/logger.hpp>
#include <platform/platform.hpp>

Application* Application::sInstance = nullptr;

Window* Window::Create(const WindowProps& props) {
    return new Window(props);
}

Window::Window(const WindowProps& props) {
    Init(props);
}

Window::~Window() {
    Shutdown();
}

void Window::Init(const WindowProps& props) {
    mProps = props;
    static const Point startPos = { 100, 100 };
    if (!platform_startup(&mPlatform, mProps.Title, startPos.x,
        startPos.y, mProps.Width, mProps.Height)) {
        return;
    }
}

void Window::Shutdown() {
    platform_shutdown(&mPlatform);
}

b8 Window::OnUpdate() {
    return platform_pump_message(&mPlatform);
}

b8 application_on_event(Event::Code::System code, void* sender, void* listener_inst, Event::Context context);
b8 application_on_key(Event::Code::System code, void* sender, void* listener_inst, Event::Context context);

Application::Application(const AppSpecification& specification) : mSpecification(specification) {
    if (sInstance) {
        Logger::Error("Application already exists!");
        return;
    }
    sInstance = this;
    // Initialize Sub-systems
    Logger::Initialize();
    Input::Initialize();
    Logger::Fatal("Test format %f", 3.14f);
    Logger::Error("Test format %f", 3.14f);
    Logger::Warn("Test format %f", 3.14f);
    Logger::Info("Test format %f", 3.14f);
    Logger::Debug("Test format %f", 3.14f);
    Logger::Trace("Test format %f", 3.14f);
    if (!Event::Initialize()) {
        Logger::Error("Event system failed initialization. Application cannot continue.");
        return;
    }
    Event::Register(Event::Code::System::ApplicationQuit, 0, application_on_event);
    Event::Register(Event::Code::System::KeyPressed, 0, application_on_key);
    Event::Register(Event::Code::System::KeyReleased, 0, application_on_key);
    mWindow = Window::Create(WindowProps(mSpecification.Name));
}

Application::~Application() {
    Event::Unregister(Event::Code::System::ApplicationQuit, 0, application_on_event);
    Event::Unregister(Event::Code::System::KeyPressed, 0, application_on_key);
    Event::Unregister(Event::Code::System::KeyReleased, 0, application_on_key);
    Event::Shutdown();
    Input::Shutdown();
    Logger::Shutdown();
}

void Application::Run() {
    while (mRunning) {
        if (!mWindow->OnUpdate()) {
            mRunning = false;
        }
        if (!mSuspended) {
            if (!Update((f32)0)) {
                Logger::Fatal("Game update failed, shutting down.");
                Close();
                break;
            }
            if (!Render((f32)0)) {
                Logger::Fatal("Game render failed, shutting down.");
                Close();
                break;
            }

            // Input update/state copying should always be handled after any input
            // should be recorded; I.E. before this line. As a safety, input is the
            // last thing to be updated before this frame ends.
            Input::Update(0);
        }
    }
}

b8 application_on_event(Event::Code::System code, void* sender, void* listener_inst, Event::Context context) {
    switch (code) {
        case Event::Code::System::ApplicationQuit: {
            Logger::Info("EVENT_CODE_APPLICATION_QUIT received, shutting down.\n");
            Application::Get().Close();
            return true;
        }
        default: return false;
    }
}

b8 application_on_key(Event::Code::System code, void* sender, void* listener_inst, Event::Context context) {
    switch (code) {
        case Event::Code::System::KeyPressed: {
            u16 key_code = context.data.d_u16[0];
            if (key_code == Input::Keys::Escape) {
                Event::Context data = {};
                Event::Fire(Event::Code::System::ApplicationQuit, 0, data);
                return true;
            } else if (key_code == Input::Keys::A) {
                Logger::Debug("Explicit - A key pressed!");
            } else {
                Logger::Debug("'%c' key pressed in a window.", key_code);
            }
        } break;
        case Event::Code::System::KeyReleased: {
            u16 key_code = context.data.d_u16[0];
            if (key_code == Input::Keys::B) {
                Logger::Debug("Explicit - B key released!");
            }
            else {
                Logger::Debug("'%c' key released in a window.", key_code);
            }
        } break;
    }
    return false;
}
