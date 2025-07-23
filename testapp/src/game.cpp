#include <defines.hpp>
#include <entry.hpp>
#include <core/logger.hpp>
#include<core/df_memory.hpp>

using namespace Engine;

namespace TestApp {
class Test : public Application {
public:
    Test(const AppSpecification& spec) : Application(spec) { Log::Info("Game Started"); }
    b8 Update(f32 delta_time);
    b8 Render(f32 delta_time);
private:
    b8 OnResize(Point newSize);
};

b8 Test::Update([[maybe_unused]]f32 delta_time) { return true; }
b8 Test::Render([[maybe_unused]]f32 delta_time) { return true; }
b8 Test::OnResize([[maybe_unused]]Point newSize) { return true; }

}

Application* CreateApplication(AppCommandLineArgs args) {
    AppSpecification spec;
    spec.Name = "TestApp";
    spec.CommandLineArgs = args;
    return new TestApp::Test(spec);
}
