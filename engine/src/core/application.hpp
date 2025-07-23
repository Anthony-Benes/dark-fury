#pragma once

#include <defines.hpp>
#include <core/asserts.hpp>
#include <core/clock.hpp>
#include <platform/platform.hpp>

int main(int argc, char** argv);

namespace Engine {

struct AppCommandLineArgs {
    int Count = 0;
    char** Args = nullptr;
    const char* operator[](int index) const {
        DF_ASSERT(index < Count);
        return Args[index];
    }
};

struct AppSpecification {
    const char* Name = "Application";
    AppCommandLineArgs CommandLineArgs;
};

struct WindowProps {
    const char* Title;
    u32 Width;
    u32 Height;
    WindowProps(const char* title = "Application", u32 width = 1280, u32 height = 720) : Title(title), Width(width), Height(height) {}
};

class Window {
public:
    Window(const WindowProps& props);
    ~Window();
    b8 OnUpdate();
    u32 GetWidth() const { return mProps.Width; }
    u32 GetHeight() const { return mProps.Height; }
    static Window* Create(const WindowProps& props = WindowProps());
private:
    void Init(const WindowProps& props);
    void Shutdown();
    WindowProps mProps;
    platform_state mPlatform;
};

class DF_API Application {
public:
  Application(const AppSpecification& specification);
  virtual ~Application();

  virtual b8 Update(f32 delta_time) = 0;
  virtual b8 Render(f32 delta_time) = 0;

  void Close() { mRunning = false; }

  static Application& Get() { return *sInstance; }
  const AppSpecification& GetSpecification() const { return mSpecification; }
private:
  void Run();
  b8 OnResize(Point newSize);
  AppSpecification mSpecification;
  Window* mWindow;
  Clock mClock;
  f64 mLastTime = 0;
  b8 mRunning = true;
  b8 mSuspended = false;

  static Application* sInstance;
  friend int ::main(int argc, char** argv);
};

} //namespace Engine

// Defined in client
Engine::Application* CreateApplication(Engine::AppCommandLineArgs args);
