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

class Node final
    : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
    // Making available all constructors of the parent class std::variant
    using variant::variant;

    inline const variant& AsVariant() const {
        return *this;
    }

    inline bool IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    inline bool IsArray() const {
        return std::holds_alternative<Array>(*this);
    }

    inline const Array& AsArray() const {
        if (!IsArray())
            throw std::logic_error("unable to convent content to array");
        return std::get<Array>(*this);
    }

    inline bool IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }

    inline const Dict& AsMap() const {
        if (!IsMap())
            throw std::logic_error("unable to convent content to map");
        return std::get<Dict>(*this);
    }

    inline bool IsBool() const {
        return std::holds_alternative<bool>(*this);
    }

    inline bool AsBool() const {
        if (!IsBool())
            throw std::logic_error("unable to convent content to boolean");
        return std::get<bool>(*this);
    }

    inline bool IsInt() const {
        return std::holds_alternative<int>(*this);
    }

    inline int AsInt() const {
        if (!IsInt())
            throw std::logic_error("unable to convent content to integer");
        return std::get<int>(*this);
    }

    inline bool IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }

    inline bool IsDouble() const {
        return IsPureDouble() || IsInt();
    }

    inline double AsDouble() const {
        if (!IsDouble())
            throw std::logic_error("unable to convent content to double");
        if (IsInt())
            return AsInt();
        return std::get<double>(*this);
    }

    inline bool IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    inline const std::string& AsString() const {
        if (!IsString())
            throw std::logic_error("unable to convent content to string");
        return std::get<std::string>(*this);
    }
};

inline bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.AsVariant() == rhs.AsVariant();
}

inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs.AsVariant() == rhs.AsVariant());
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

inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs.GetRoot() == rhs.GetRoot());
}

inline Document Load(std::istream& input) {
    return Document{LoadNode(input)};
}

// ---------- NodePrinter -------------

class NodePrinter {
public:
    explicit NodePrinter(std::ostream& out) : out_(out) {}

    inline void operator()(const std::nullptr_t) const {
        out_ << "null";
    }

    inline void operator()(const bool boolean) const {
        out_ << std::boolalpha << boolean;
    }

    inline void operator()(const int number) const {
        out_ << number;
    }

    inline void operator()(const double number) const {
        out_ << number;
    }

    void operator()(const std::string& s) const;

    void operator()(const Array& array) const;

    void operator()(const Dict& map) const;

private:
    std::ostream& out_;
};

inline std::ostream& operator<<(std::ostream& out, const Node& node) {
    std::visit(NodePrinter{out}, node.AsVariant());
    return out;
}

inline void Print(const Document& document, std::ostream& out) {
    out << document.GetRoot();
}

} // end namespace json