#include <array>
#include <algorithm>

#include "geometry.h"
#include "Instrumentor.h"
#include "tgaimage.h"
#include "line_drawing.h"
#include "colors.h"
#include "line_drawing_test.h"
#include "triangle_drawing_test.h"




int main(int argc, char** argv)
{
	bEngine::Instrumentor::Get().BeginSession("TinyRenderer");

	DrawTriangle_Model();

	//MatrixInverseTest();

	//DrawTriangleTest();
	//DrawModel_wireframe();
	//DrawLineTest3();
	//DrawLineTest2();
	

	bEngine::Instrumentor::Get().EndSession();

	return 0;
}