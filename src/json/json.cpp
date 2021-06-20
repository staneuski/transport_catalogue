#include "json.h"

namespace json {

// ---------- Node -------------------

namespace {

Node LoadArray(std::istream& input) {
    Array result;

    char c;
    for (; input >> c && c != ']';) {
        if (c != ',')
            input.putback(c);
        result.push_back(LoadNode(input));
    }

    if (c != ']')
        throw ParsingError("unable to convert input to array");
    return Node(std::move(result));
}

Node LoadString(std::istream& input) {
    std::string s;

    input >> std::noskipws;
    char c;
    for (; input >> c && c != '\"';) {
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
    input >> std::skipws;

    if (c != '\"')
        throw ParsingError("unable to convert '" + s + "' to string");
    return Node(move(s));
}

Node LoadDict(std::istream& input) {
    Dict result;

    char c;
    for (; input >> c && c != '}';) {
        if (c != ',')
            input.putback(c);

        std::string key = LoadNode(input).AsString();
        input >> c;
        result.insert({std::move(key), LoadNode(input)});
    }

    if (c != '}')
        throw ParsingError("unable to convert input to map");
    return Node(std::move(result));
}

Node LoadNull(std::istream& input) {
    std::string s{};

    for (char c; input >> c && s.size() < 4;)
        s += c;

    if (s != "null")
        throw ParsingError("unable to convert '" + s + "' to nullptr");
    else
        return Node();
}

Node LoadBool(std::istream& input) {
    std::string s{};

    for (char c; input >> c && s.size() <= 5;) {
        s += c;
        if (c == 'e')
            break;
    }

    if (s == "true" || s == "false")
        return Node(s == "true");
    else
        throw ParsingError("unable to convert '" + s + "' to boolean");
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
            number += static_cast<char>(input.get());
        is_int = false;
    }

    try {
        if (is_int)
            return Node(std::stoi(std::move(number)));
        else
            return Node(std::stod(std::move(number)));
    } catch (const std::exception&) {
        throw ParsingError("unable to convert '" + number + "' to number");
    }
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
    out << '[';

    bool is_first = true;
    for (const Node& node : array) {
        if (is_first)
            is_first = false;
        else
            out << ", ";

        out << node;
    }

    out << ']';
}

void NodePrinter::operator()(const Dict& map) const {
    out << '{';

    bool is_first = true;
    for (const auto& [key, node] : map) {
        if (is_first)
            is_first = false;
        else
            out << ", ";

        this->operator()(key);
        out << ':' << node;
    }

    out << '}';
}

} // end namespace json