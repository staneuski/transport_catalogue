#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace svg {

struct Point {
    Point() = default;

    Point(double x, double y) : x(x), y(y) {}

    double x{}, y{};
};

inline std::ostream& operator<<(std::ostream& out, const Point& point) {
    out << point.x << ',' << point.y;
    return out;
}

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

    uint8_t red{}, green{}, blue{};
};

inline std::ostream& operator<<(std::ostream& out, const Rgb& color) {
    out << "rgb(" << unsigned(color.red)
        << ',' << unsigned(color.green)
        << ',' << unsigned(color.blue)
        << ')';
    return out;
}

struct Rgba : public Rgb {
    Rgba() = default;
    Rgba(uint8_t r, uint8_t g, uint8_t b) : Rgb(r, g, b) {}
    Rgba(uint8_t r, uint8_t g, uint8_t b, double a)
        : Rgb(r, g, b)
        , opacity(a)
    {}

    double opacity = 1.;
};

inline std::ostream& operator<<(std::ostream& out, const Rgba& color) {
    out << "rgba(" << unsigned(color.red)
        << ',' << unsigned(color.green)
        << ',' << unsigned(color.blue)
        << ',' << color.opacity
        << ')';
    return out;
}

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

inline const Color NoneColor{};

struct ColorPrinter {
    std::ostream& out;

    inline void operator()(std::monostate) const {
        out << "none";
    }

    inline void operator()(const std::string_view word) const {
        out << word;
    }

    inline void operator()(const Rgb& rgb) {
        out << rgb;
    }

    inline void operator()(const Rgba& rgba) {
        out << rgba;
    }
};

inline std::ostream& operator<<(std::ostream& out, const Color& color) {
    std::visit(ColorPrinter{out}, color);
    return out;
}

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out) : out(out) {}

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent)
    {
    }

    inline RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const;

    std::ostream& out;
    int indent_step{};
    int indent{};
};

// ---------- Object ------------------

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

enum class StrokeLineCap { BUTT, ROUND, SQUARE, };

enum class StrokeLineJoin { ARCS, BEVEL, MITER, MITER_CLIP, ROUND, };

std::ostream& operator<<(std::ostream& out, const StrokeLineCap& line_cap);

std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& line_join);

/*
 * Вспомогательный базовый класс, служит для хранения свойств,
 * управляющих параметрами заливки и контура
 */
template <typename T>
class PathProps {
public:
    inline T& SetFillColor(Color fill_color) {
        fill_color_ = std::move(fill_color);
        return static_cast<T&>(*this);
    }

    inline T& SetStrokeColor(Color stroke_color) {
        stroke_color_ = std::move(stroke_color);
        return static_cast<T&>(*this);
    }

    inline T& SetStrokeWidth(double width) {
        width_ = std::move(width);
        return static_cast<T&>(*this);
    }

    inline T& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = std::move(line_cap);
        return static_cast<T&>(*this);
    }

    inline T& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = std::move(line_join);
        return static_cast<T&>(*this);
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        if (fill_color_)
            out << " fill=\"" << *fill_color_ << "\"";
        if (stroke_color_)
            out << " stroke=\"" << *stroke_color_ << "\"";
        if (width_)
            out << " stroke-width=\"" << *width_ << "\"";
        if (line_cap_)
            out << " stroke-linecap=\"" << *line_cap_ << "\"";
        if (line_join_)
            out << " stroke-linejoin=\"" << *line_join_ << "\"";
    }

private:
    std::optional<Color> fill_color_, stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);

    Circle& SetRadius(double radius);

private:
    Point center_;
    double radius_ = 1.0;

    void RenderObject(const RenderContext& context) const override;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);

private:
    std::vector<Point> points_{};

    void RenderObject(const RenderContext& context) const override;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    inline Text& SetPosition(Point pos) {
        position_ = std::move(pos);
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    inline Text& SetOffset(Point offset) {
        offset_ = std::move(offset);
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    inline Text& SetFontSize(uint32_t size) {
        font_size_ = std::move(size);
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    inline Text& SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    inline Text& SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    inline Text& SetData(std::string data) {
        content_ = std::move(data);
        return *this;
    }

private:
    Point position_, offset_;
    uint32_t font_size_ = 1;
    std::optional<std::string> font_family_, font_weight_;
    std::string content_;

    void RenderObject(const RenderContext& context) const override;
};

class ObjectContainer {
public:
    virtual void AddPtr(std::unique_ptr<Object>&& object) = 0;

    // Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
    template <typename T>
    inline void Add(const T& object) {
        AddPtr(std::make_unique<T>(object));
    }
};

class Drawable {
public:
    virtual void Draw(ObjectContainer& g) const = 0;

    virtual ~Drawable() = default;
};

class Document : public ObjectContainer {
public:
    // Добавляет в svg-документ объект-наследник svg::Object
    inline void AddPtr(std::unique_ptr<Object>&& object) override {
        objects_.emplace_back(std::move(object));
    }

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
};

inline std::ostream& operator<<(std::ostream& out, const Document& document) {
    document.Render(out);
    return out;
}

} // end namespace svg