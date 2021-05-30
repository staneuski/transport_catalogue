#define _USE_MATH_DEFINES
#include "svg.h"

#include <cmath>

/*
Пример использования библиотеки. Он будет компилироваться и работать, когда вы реализуете
все классы библиотеки.
*/

namespace {

using namespace svg;
using namespace std::literals;
    
Polyline CreateStar(Point center, double outer_rad, double inner_rad, int num_rays) {
    Polyline polyline;
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2*M_PI*(i % num_rays)/num_rays;
        polyline.AddPoint({
            center.x + outer_rad*sin(angle),
            center.y - outer_rad*cos(angle)
        });
        if (i == num_rays)
            break;
        angle += M_PI/num_rays;
        polyline.AddPoint({
            center.x + inner_rad*sin(angle),
            center.y - inner_rad*cos(angle)
        });
    }
    return polyline;
}

// Выводит приветствие, круг и звезду
void DrawPicture() {
    Document doc;
    doc.Add(Circle().SetCenter({20, 20}).SetRadius(10));
    doc.Add(Text()
                .SetFontFamily("Verdana"s)
                .SetPosition({35, 20})
                .SetOffset({0, 6})
                .SetFontSize(12)
                .SetFontWeight("bold"s)
                .SetData("Hello C++"s));
    doc.Add(CreateStar({20, 50}, 10, 5, 5));
    doc.Render(std::cout);
}

}  // namespace


int main() {
    DrawPicture();

    return 0;
}