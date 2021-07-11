#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
    using variant::variant;
    using Value = variant;

    inline bool IsInt() const {
        return std::holds_alternative<int>(*this);
    }

    inline int AsInt() const {
        if (!IsInt())
            throw std::logic_error("not an int");
        return std::get<int>(*this);
    }

    inline bool IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }

    inline bool IsDouble() const {
        return IsInt() || IsPureDouble();
    }

    inline double AsDouble() const {
        if (!IsDouble())
            throw std::logic_error("not a double");
        return IsPureDouble() ? std::get<double>(*this) : AsInt();
    }

    inline bool IsBool() const {
        return std::holds_alternative<bool>(*this);
    }

    inline bool AsBool() const {
        if (!IsBool())
            throw std::logic_error("not a bool");
        return std::get<bool>(*this);
    }

    inline bool IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    inline bool IsArray() const {
        return std::holds_alternative<Array>(*this);
    }

    inline const Array& AsArray() const {
        if (!IsArray())
            throw std::logic_error("not an array");
        return std::get<Array>(*this);
    }

    inline bool IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    inline const std::string& AsString() const {
        if (!IsString())
            throw std::logic_error("not a string");

        return std::get<std::string>(*this);
    }

    inline bool IsDict() const {
        return std::holds_alternative<Dict>(*this);
    }

    inline const Dict& AsDict() const {
        if (!IsDict())
            throw std::logic_error("not a dict");
        return std::get<Dict>(*this);
    }

    inline bool operator==(const Node& rhs) const {
        return GetValue() == rhs.GetValue();
    }

    inline bool operator==(const std::string& rhs) {
        if (!IsString())
            throw std::logic_error("not a string");

        return AsString() == rhs;
    }

    inline const Value& GetValue() const {
        return *this;
    }
};

inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

inline bool operator==(const std::string& lhs, const Node& rhs) {
    return rhs == lhs;
}

class NodeGetter {
public:
    inline Node operator() (std::nullptr_t) const {
        return Node();
    }

    inline Node operator() (bool value) const {
        return Node(value);
    }

    inline Node operator() (int value) const {
        return Node(value);
    }

    inline Node operator() (double value) const {
        return Node(value);
    }

    inline Node operator() (std::string value) const {
        return Node(std::move(value));
    }

    inline Node operator() (Array value) const {
        return Node(std::move(value));
    }

    inline Node operator() (Dict value) const {
        return Node(std::move(value));
    }
};

class Document {
public:
    explicit Document(Node root) : root_(std::move(root)) {}

    const Node& GetRoot() const {
        return root_;
    }

private:
    Node root_;
};

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

std::ostream& operator<<(std::ostream& out, const Document& doc);

}  // namespace json