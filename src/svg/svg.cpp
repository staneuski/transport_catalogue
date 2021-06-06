#include "svg.h"

namespace svg {

// ---------- Object ------------------

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\"" << center_.x << "\" cy=\"" << center_.y << "\" ";
    out << "r=\"" << radius_ << "\"";

    RenderAttrs(out);
    out << "/>";
}

// ---------- Polyline ----------------

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(std::move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\"";

    bool is_first = true;
    for (const Point& p : points_) {
        if (is_first)
            is_first = false;
        else
            out << ' ';
        out << p.x << ',' << p.y;
    }
    out << "\"";

    RenderAttrs(out);
    out << "/>";
}

// ---------- Text --------------------

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text" << " x=\"" << position_.x << "\""
                   << " y=\"" << position_.y << "\""
                   << " dx=\"" << offset_.x << "\""
                   << " dy=\"" << offset_.y << "\""
                   << " font-size=\"" << font_size_ << "\"";
    if (font_family_)
        out << " font-family=\"" << *font_family_ << "\"";
    if (font_weight_)
        out << " font-weight=\"" << *font_weight_ << "\"";

    RenderAttrs(out);
    out << ">";

    for (const char letter : content_)
        switch (letter) {
            case '\"':
                out << "&quot;";
                break;
            case '\'':
                out << "&apos;";
                break;
            case '<':
                out << "&lt;";
                break;
            case '>':
                out << "&gt;";
                break;
            case '&':
                out << "&amp";
                break;
            default:
                out << letter;
                break;
        }

    out << "</text>";
}

// ---------- Document ----------------

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";

    for (const std::unique_ptr<Object>& object : objects_)
        object->Render(RenderContext(out, 2, 2));

    out << "</svg>";
}

// ---------- helpers -----------------

std::ostream& operator<<(std::ostream& out, const Rgb& color) {
    out << "rgb(" << unsigned(color.red)
        << ',' << unsigned(color.green)
        << ',' << unsigned(color.blue)
        << ')';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Rgba& color) {
    out << "rgb(" << unsigned(color.red)
        << ',' << unsigned(color.green)
        << ',' << unsigned(color.blue)
        << ',' << unsigned(color.opacity)
        << ')';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Color& color) {
    std::visit(ColorPrinter{out}, color);
    return out;
}

std::ostream& operator<<(std::ostream& out, const StrokeLineCap& line_cap) {
    switch (line_cap) {
        case StrokeLineCap::BUTT:
            out << "butt";
            break;
        case StrokeLineCap::ROUND:
            out << "round";
            break;
        case StrokeLineCap::SQUARE:
            out << "square";
            break;
        default:
            break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& line_join) {
    switch (line_join) {
        case StrokeLineJoin::ARCS:
            out << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            out << "bevel";
            break;
        case StrokeLineJoin::MITER:
            out << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            out << "round";
            break;
        default:
            break;
    }
    return out;
}

} // end namespace svg