
#define DEBUGINATOR_IMPLEMENTATION
#include <the_debuginator_wrapper.h>


struct TheDebuginator* alloc_debuginator() {
    static struct TheDebuginator debuginator;
    memset(&debuginator, 0, sizeof(debuginator));
    return &debuginator;
}
