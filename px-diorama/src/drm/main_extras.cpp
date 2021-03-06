// name: main_extras.cpp

// no debug for external dependencies

// for those who can't into headers

#ifndef NDEBUG
#define NDEBUG
#endif

#define _USE_MATH_DEFINES // M_PI in cmath before it's included by anyone else
#include <cmath>

#pragma warning(push) // avoid comlile warnings
#pragma warning(disable:4334) // warning C4334: '<<': result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
#pragma warning(disable:4267) // warning C4267: '=': conversion from 'size_t' to 'int', possible loss of data
#include <lodepng.cpp>
#pragma warning(pop)

//#include <msdf/core/Bitmap.cpp>
//#include <msdf/core/Contour.cpp>
//#include <msdf/core/edge-coloring.cpp>
//#include <msdf/core/edgeHolder.cpp>
//#include <msdf/core/edge-segments.cpp>
//#include <msdf/core/equation-solver.cpp>
//#include <msdf/core/msdfgen.cpp>
//#include <msdf/core/render-sdf.cpp>
//#include <msdf/core/save-bmp.cpp>
//#include <msdf/core/Shape.cpp>
//#include <msdf/core/shape-description.cpp>
//#include <msdf/core/SignedDistance.cpp>
//#include <msdf/core/Vector2.cpp>
//#include <msdf/ext/import-font.cpp>