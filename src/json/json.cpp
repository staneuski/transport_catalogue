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
                    s.push_back('\r');
                    break;
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                default:
                    s.push_back(c);
                    break;
            }
        } else {
            s.push_back(c);
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

    const auto& append_number = [&number, &input]() {
        number.push_back(static_cast<char>(input.get()));
    };

    const auto& append_digits = [&input, &append_number]() {
        while (std::isdigit(input.peek()))
            append_number();
    };

    if (input.peek() == '-')
        append_number();

    if (input.peek() == '0')
        append_number();
    else 
        append_digits();

    bool is_int = true;
    if (input.peek() == '.') {
        append_number();
        append_digits();

        is_int = false;
    }

    if (char c = input.peek(); c == 'e' || c == 'E') {
        append_number();

        if (c = input.peek(); c == '-' || c == '+')
            append_number();
        append_digits();
    
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
    out_ << "\"";
    for (char c : s)
        switch(c) {
            case '\"':
                out_ << "\\\"";
                break;
            case '\\':
                out_ << "\\\\";
                break;
            case '\t':
                out_ << "\\t";
                break;
            case '\r':
                out_ << "\\r";
                break;
            case '\n':
                out_ << "\\n";
                break;
            default:
                out_ << c; break;
        }
    out_ << "\"";
}

void NodePrinter::operator()(const Array& array) const {
    out_ << '[';

    bool is_first = true;
    for (const Node& node : array) {
        if (is_first)
            is_first = false;
        else
            out_ << ", ";

        out_ << node;
    }

    out_ << ']';
}

void NodePrinter::operator()(const Dict& map) const {
    out_ << '{';

    bool is_first = true;
    for (const auto& [key, node] : map) {
        if (is_first)
            is_first = false;
        else
            out_ << ", ";

        this->operator()(key);
        out_ << ':' << node;
    }

    out_ << '}';
}

} // end namespace json