#pragma once

#include <cctype>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <sstream>
#include <variant>
#include <vector>

namespace json {

// ---------- Node -------------------

class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
public:
    Node() = default;

    Node(std::nullptr_t) : Node() {};

    Node(Array array) : content_(std::move(array)) {};

    Node(Dict map) : content_(std::move(map)) {};

    Node(bool boolean) : content_(boolean) {};

    Node(int number) : content_(number) {};

    Node(double number) : content_(number) {};

    Node(std::string s) : content_(std::move(s)) {};

    inline const Value& AsVariant() const {
        return content_;
    }

    inline bool IsNull() const {
        return std::holds_alternative<std::nullptr_t>(content_);
    }

    inline bool IsArray() const {
        return std::holds_alternative<Array>(content_);
    }

    inline const Array& AsArray() const {
        if (!IsArray())
            throw std::logic_error("unable to convent content to array");
        return std::get<Array>(content_);
    }

    inline bool IsMap() const {
        return std::holds_alternative<Dict>(content_);
    }

    inline const Dict& AsMap() const {
        if (!IsMap())
            throw std::logic_error("unable to convent content to map");
        return std::get<Dict>(content_);
    }

    inline bool IsBool() const {
        return std::holds_alternative<bool>(content_);
    }

    inline bool AsBool() const {
        if (!IsBool())
            throw std::logic_error("unable to convent content to boolean");
        return std::get<bool>(content_);
    }

    inline bool IsInt() const {
        return std::holds_alternative<int>(content_);
    }

    inline int AsInt() const {
        if (!IsInt())
            throw std::logic_error("unable to convent content to integer");
        return std::get<int>(content_);
    }

    inline bool IsPureDouble() const {
        return std::holds_alternative<double>(content_);
    }

    inline bool IsDouble() const {
        return IsPureDouble() || IsInt();
    }

    inline double AsDouble() const {
        if (!IsDouble())
            throw std::logic_error("unable to convent content to double");
        if (IsInt())
            return AsInt();
        return std::get<double>(content_);
    }

    inline bool IsString() const {
        return std::holds_alternative<std::string>(content_);
    }

    inline const std::string& AsString() const {
        if (!IsString())
            throw std::logic_error("unable to convent content to string");
        return std::get<std::string>(content_);
    }

private:
    Value content_;
};

inline bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.AsVariant() == rhs.AsVariant();
}

Node LoadNode(std::istream& input);

// ---------- Document ----------------

class Document {
public:
    explicit Document(Node root) : root_(std::move(root)) {}

    inline const Node& GetRoot() const {
        return root_;
    }

private:
    Node root_;
};

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

inline Document Load(std::istream& input) {
    return Document{LoadNode(input)};
}

// ---------- NodePrinter -------------

struct NodePrinter {
    inline void operator()(const std::nullptr_t) const {
        out << "null";
    }

    inline void operator()(const bool boolean) const {
        out << std::boolalpha << boolean;
    }

    inline void operator()(const int number) const {
        out << number;
    }

    inline void operator()(const double number) const {
        out << number;
    }

    void operator()(const std::string& s) const;

    void operator()(const Array& array) const;

    void operator()(const Dict& map) const;

    std::ostream& out;
};

inline std::ostream& operator<<(std::ostream& out, const Node& node) {
    std::visit(NodePrinter{out}, node.AsVariant());
    return out;
}

inline void Print(const Document& document, std::ostream& out) {
    out << document.GetRoot();
}

} // end namespace json