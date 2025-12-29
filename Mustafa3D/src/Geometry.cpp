#include "Geometry.h"

int edgeFunction(ScreenPosition point1, ScreenPosition point2, ScreenPosition point3) {
	return (point2.x - point1.x) * (point3.y - point1.y) - (point2.y - point1.y) * (point3.x - point1.x);
}
