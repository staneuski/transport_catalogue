#include "json.h"

namespace json {

// ---------- Node -------------------

/*
bool operator==(const Node& lhs, const Node& rhs) {
    return lhs == rhs;
}
*/

namespace {

Node LoadNode(std::istream& input);

Node LoadString(std::istream& input) {
    input >> std::noskipws;

    bool is_in_quotes = false;
    for (char c; input >> c;) {
        is_in_quotes = (c == '\"');
        if (is_in_quotes)
            break;

        if (c == '\\') {
            input >> c;
            switch(c) {
                case 'r':
                    input.putback('\r');
                    break;
                case 'n':
                    input.putback('\n');
                    break;
                case 't':
                    input.putback('\t');
                    break;
                default:
                    input.putback(c);
                    break;
            }
        } else {
            input.putback(c);
        }
    }

    return LoadNode(input);
}

Node LoadBool(std::istream& input) {
    for (char c; input >> c && c != 'e';)
        if (c != ',')
            input.putback(c);

    return LoadNode(input);
}

Node LoadNumber(std::istream& input) {
    std::string number;

    if (input.peek() == '-')
        number += static_cast<char>(input.get());

    if (input.peek() == '0') {
        number += static_cast<char>(input.get());
    } else {
        while (std::isdigit(input.peek()))
            number += static_cast<char>(input.get());;
    }

    bool is_int = true;
    if (input.peek() == '.') {
        number += static_cast<char>(input.get());

        while (std::isdigit(input.peek()))
            number += static_cast<char>(input.get());
        is_int = false;
    }

    if (char c = input.peek(); c == 'e' || c == 'E') {
        number += static_cast<char>(input.get());

        if (c = input.peek(); c == '-' || c == '+')
            number += static_cast<char>(input.get());

        while (std::isdigit(input.peek()))
            number += static_cast<char>(input.get());;
        is_int = false;
    }

    return Node();
}

Node LoadArray(std::istream& input) {
    Array result;

    for (char c; input >> c && c != ']';) {
        if (c != ',')
            input.putback(c);
        result.push_back(LoadNode(input));
    }

    return Node(std::move(result));
}

Node LoadDict(std::istream& input) {
    Dict result;

    for (char c; input >> c && c != '}';) {
        if (c == ',')
            input >> c;

        std::string key = LoadString(input).AsString();
        input >> c;
        result.insert({std::move(key), LoadNode(input)});
    }

    return Node(std::move(result));
}

Node LoadNode(std::istream& input) {
    char c;
    input >> c;

    switch(c) {
        case ' ':
            return LoadNode(input);
        case '[':
            return LoadArray(input);
        case '{':
            return LoadDict(input);
        case 't':
        case 'f':
            input.putback(c);
            return LoadBool(input);
        case '"':
            return LoadString(input);
        default:
            input.putback(c);
            return LoadNumber(input);
    }
}

} // end namespace

// ---------- Document ----------------

Document::Document(Node root) : root_ (std::move(root)) {}

const Node& Document::GetRoot() const {
    return root_;
}


// bool operator==(const Document& lhs, const Document& rhs) {
//     return lhs.GetRoot() == rhs.GetRoot();
// }

Document Load(std::istream& input) {
    return Document{LoadNode(input)};
}

// void Print(const Document& doc, std::ostream& out) {
//     out << doc;
// }

} // end namespace json