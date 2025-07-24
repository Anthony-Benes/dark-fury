#include <core/application.hpp>

#include <core/df_memory.hpp>
#include <core/event.hpp>
#include <core/input.hpp>
#include <core/logger.hpp>
#include <platform/platform.hpp>

namespace Engine {

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
    mRenderer = new Renderer::Frontend(mProps.Title, &mPlatform);
}

void Window::Shutdown() {
    delete mRenderer;
    platform_shutdown(&mPlatform);
}

b8 Window::OnUpdate() {
    return platform_pump_message(&mPlatform);
}

void Window::OnDraw(Renderer::render_packet* packet) {
    mRenderer->DrawFrame(packet);
}

b8 application_on_event(Event::Code::System code, void* sender, void* listener_inst, Event::Context context);
b8 application_on_key(Event::Code::System code, void* sender, void* listener_inst, Event::Context context);

Application::Application(const AppSpecification& specification) : mSpecification(specification) {
    if (sInstance) {
        Log::Error("Application already exists!");
        return;
    }
    sInstance = this;
    // Initialize Sub-systems
    Log::Initialize();
    Input::Initialize();
    Log::Fatal("Test format %f", 3.14f);
    Log::Error("Test format %f", 3.14f);
    Log::Warn("Test format %f", 3.14f);
    Log::Info("Test format %f", 3.14f);
    Log::Debug("Test format %f", 3.14f);
    Log::Trace("Test format %f", 3.14f);
    if (!Event::Initialize()) {
        Log::Error("Event system failed initialization. Application cannot continue.");
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
    Log::Shutdown();
}

void Application::Run() {
    mClock.Start();
    mClock.Update();
    mLastTime = mClock.TElapse();
    f64 running_time = 0;
    u8 frame_count = 0;
    f64 target_frame_rate = 1.0f / 144;
    while (mRunning) {
        if (!mWindow->OnUpdate()) {
            mRunning = false;
        }
        if (!mSuspended) {
            mClock.Update();
            f64 current_time = mClock.TElapse();
            f64 delta = (current_time - mLastTime);
            f64 frame_start_time = platform_get_absolute_time();
            if (!Update((f32)delta)) {
                Log::Fatal("Game update failed, shutting down.");
                Close();
                break;
            }
            if (!Render((f32)delta)) {
                Log::Fatal("Game render failed, shutting down.");
                Close();
                break;
            }
            Renderer::render_packet packet;
            packet.delta_time = delta;
            mWindow->OnDraw(&packet);
            f64 frame_end_time = platform_get_absolute_time();
            f64 frame_elapsed_time = frame_end_time - frame_start_time;
            running_time += frame_elapsed_time;
            f64 remaining_seconds = target_frame_rate - frame_elapsed_time;
            if (remaining_seconds > 0) {
                u64 remaining_ms = (remaining_seconds * 1000);
                b8 limit_frames = false;
                if (remaining_ms > 0 && limit_frames) {
                    platform_sleep(remaining_ms - 1);
                }
                frame_count++;
            }

            // Input update/state copying should always be handled after any input
            // should be recorded; I.E. before this line. As a safety, input is the
            // last thing to be updated before this frame ends.
            Input::Update(delta);
            mLastTime = current_time;
        }
    }
}

b8 application_on_event(Event::Code::System code, [[maybe_unused]]void* sender, [[maybe_unused]]void* listener_inst, [[maybe_unused]]Event::Context context) {
    switch (code) {
        case Event::Code::System::ApplicationQuit: {
            Log::Info("EVENT_CODE_APPLICATION_QUIT received, shutting down.\n");
            Application::Get().Close();
            return true;
        }
        default: return false;
    }
}

b8 application_on_key(Event::Code::System code, [[maybe_unused]]void* sender, [[maybe_unused]]void* listener_inst, Event::Context context) {
    switch (code) {
        case Event::Code::System::KeyPressed: {
            u16 key_code = context.data.d_u16[0];
            if (key_code == Input::Keys::Escape) {
                Event::Context data = {};
                Event::Fire(Event::Code::System::ApplicationQuit, 0, data);
                return true;
            } else if (key_code == Input::Keys::A) {
                Log::Debug("Explicit - A key pressed!");
            } else {
                Log::Debug("'%c' key pressed in a window.", key_code);
            }
        } break;
        case Event::Code::System::KeyReleased: {
            u16 key_code = context.data.d_u16[0];
            if (key_code == Input::Keys::B) {
                Log::Debug("Explicit - B key released!");
            }
            else {
                Log::Debug("'%c' key released in a window.", key_code);
            }
        } break;
        default: break;
    }
    return false;
}

} // namespace Engine
