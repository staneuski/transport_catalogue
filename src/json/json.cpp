#include "json.h"

namespace json {

// ---------- Node -------------------

namespace {

Node LoadArray(std::istream& input) {
    Array result;

    for (char c; input >> c && c != ']';) {
        if (c != ',')
            input.putback(c);
        result.push_back(LoadNode(input));
    }

    return Node(std::move(result));
}

Node LoadString(std::istream& input) {
    input >> std::noskipws;

    std::string s;
    for (char c; input >> c && c != '\"';) {
        if (c == '\\') {
            input >> c;
            switch(c) {
                case 'r':
                    s += '\r';
                    break;
                case 'n':
                    s += '\n';
                    break;
                case 't':
                    s += '\t';
                    break;
                default:
                    s += c;
                    break;
            }
        } else {
            s += c;
        }
    }

    return Node(move(s));
}

Node LoadDict(std::istream& input) {
    Dict result;

    for (char c; input >> c && c != '}';) {
        if (c == ' ' || c == ',')
            continue;

        std::string key = LoadString(input).AsString();
        input >> c;
        result.insert({std::move(key), LoadNode(input)});
    }

    return Node(std::move(result));
}

Node LoadNull(std::istream& input) {
    std::string s{};

    for (char c; input >> c && s.size() < 4;)
        s += c;

    // std::cout << '[' << s << ']' << std::endl;

    return Node();
}

Node LoadBool(std::istream& input) {
    std::string s{};

    for (char c; input >> c && s.size() <= 5;) {
        s += c;
        if (c == 'e')
            break;
    }

    // if (s == "true" || s == "false")
    return Node(s == "true");
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

    if (is_int)
        return Node(std::stoi(std::move(number)));
    else
        return Node(std::stod(std::move(number)));
}

} // end namespace

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
        case 'n':
            input.putback(c);
            return LoadNull(input);
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

// ---------- NodePrinter -------------

void NodePrinter::operator()(const std::string& s) const {
    out << "\"";
    for (char c : s)
        switch(c) {
            case '\"':
                out << "\\\"";
                break;
            case '\\':
                out << "\\\\";
                break;
            case '\t':
                out << "\\t";
                break;
            case '\r':
                out << "\\r";
                break;
            case '\n':
                out << "\\n";
                break;
            default:
                out << c; break;
        }
    out << "\"";
}

void NodePrinter::operator()(const Array& array) const {
    bool is_first = true;
    for (const Node& node : array) {
        if (is_first) {
            out << '[';
            is_first = false;
        } else {
            out << ", ";
        }

        out << node;
    }
    out << ']';
}

void NodePrinter::operator()(const Dict& map) const {
    bool is_first = true;
    for (const auto& [key, node] : map) {
        if (is_first) {
            out << '{';
            is_first = false;
        } else {
            out << ", ";
        }

        this->operator()(key);
        out << ": " << node;
    }
    out << '}';
}

} // end namespace json