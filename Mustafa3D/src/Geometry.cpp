#include "Geometry.h"

int edgeFunction(ScreenPosition A, ScreenPosition B, ScreenPosition C) {
	return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
}