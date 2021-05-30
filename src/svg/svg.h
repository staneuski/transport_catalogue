#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace svg {

using Color = std::string;

inline const Color NoneColor{"none"};

struct Point {
    Point() = default;

    Point(double x, double y) : x(x), y(y) {}

    double x{}, y{};
};

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

    uint8_t r{}, g{}, b{};
};

struct Rgba : public Rgb {
    Rgba() = default;
    Rgba(uint8_t r, uint8_t g, uint8_t b) : Rgb(r, g, b) {}
    Rgba(uint8_t r, uint8_t g, uint8_t b, double a) : Rgb(r, g, b), a(a) {}

    double a = 1.;
};

std::ostream& operator<<(std::ostream& out, const Rgb& rgb);

std::ostream& operator<<(std::ostream& out, const Rgba& rgba);

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

    inline void RenderIndent() const {
        for (int i = 0; i < indent; ++i)
            out.put(' ');
    }

    std::ostream& out;
    int indent_step{};
    int indent{};
};

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

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object {
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
class Polyline final : public Object {
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
class Text final : public Object {
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

} // end namespace svg