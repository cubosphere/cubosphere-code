// FIXME: This define should not be needed, as it is also defined by Glew's CMakeLists
#define GLEW_STATIC

extern "C" {
#include <GL/glew.h>
// This is a workaround for mesa drivers that now use __gl_glext_h_ instead of
// __glext_h_ in header file which is not defined in current glew version
#define __gl_glext_h_
}
