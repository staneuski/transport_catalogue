#include "json.h"

// using namespace std;

namespace json {

namespace {

Node LoadNode(std::istream& input);

Node LoadArray(std::istream& input) {
    Array result;

    for (char c; input >> c && c != ']';) {
        if (c != ',')
            input.putback(c);
        result.push_back(LoadNode(input));
    }

    return Node(move(result));
}

Node LoadInt(std::istream& input) {
    int result = 0;
    while (isdigit(input.peek())) {
        result *= 10;
        result += input.get() - '0';
    }
    return Node(result);
}

Node LoadString(std::istream& input) {
    std::string line;
    getline(input, line, '"');
    return Node(move(line));
}

Node LoadDict(std::istream& input) {
    Dict result;

    for (char c; input >> c && c != '}';) {
        if (c == ',')
            input >> c;

        std::string key = LoadString(input).AsString();
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }

    return Node(move(result));
}

Node LoadNode(std::istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else {
        input.putback(c);
        return LoadInt(input);
    }
}

} // end namespace

Node::Node(Array array) : as_array_(move(array)) {}

Node::Node(Dict map) : as_map_(move(map)) {}

Node::Node(int value) : as_int_(value) {}

Node::Node(std::string value) : as_string_(std::move(value)) {}

const Array& Node::AsArray() const {
    return as_array_;
}

const Dict& Node::AsMap() const {
    return as_map_;
}

int Node::AsInt() const {
    return as_int_;
}

const std::string& Node::AsString() const {
    return as_string_;
}

Document::Document(Node root) : root_(std::move(root)) {}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(std::istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    (void) &doc;
    (void) &output;

    // Реализуйте функцию самостоятельно
}

} // end namespace json