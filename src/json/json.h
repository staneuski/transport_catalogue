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
    explicit Node() = default;

    explicit Node(std::nullptr_t) : Node() {};

    explicit Node(Array array) : content_(std::move(array)) {};

    explicit Node(Dict map) : content_(std::move(map)) {};

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
        return std::get<Array>(content_);
    }

    inline bool IsMap() const {
        return std::holds_alternative<Dict>(content_);
    }

    inline const Dict& AsMap() const {
        return std::get<Dict>(content_);
    }

    inline bool IsBool() const {
        return std::holds_alternative<bool>(content_);
    }

    inline bool AsBool() const {
        return std::get<bool>(content_);
    }

    inline bool IsInt() const {
        return std::holds_alternative<int>(content_);
    }

    inline int AsInt() const {
        return std::get<int>(content_);
    }

    inline bool IsDouble() const {
        return std::holds_alternative<double>(content_) || IsInt();
    }

    inline bool IsPureDouble() const {
        return std::holds_alternative<double>(content_);
    }

    inline double AsDouble() const {
        if (IsInt())
            return AsInt();
        return std::get<double>(content_);
    }

    inline const std::string& AsString() const {
        return std::get<std::string>(content_);
    }

    inline bool IsString() const {
        return std::holds_alternative<std::string>(content_);
    }

private:
    Value content_;
};

bool operator==(const Node& lhs, const Node& rhs);

class Document {
public:
    explicit Document(Node root);

    const Node& Document::GetRoot();

private:
    Node root_;
};

bool operator==(const Document& lhs, const Document& rhs);

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

} // end namespace json