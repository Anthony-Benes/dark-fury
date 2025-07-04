#include <core/logger.hpp>
#include <core/asserts.hpp>

int main(void) {
    DF_FATAL("Test Fatal!");
    DF_WARN("Test format %f", 3.14f);
    DF_INFO("Hello World");

    DF_ASSERT(1 == 1);
	return 0;
}
