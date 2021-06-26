#include "json_builder.h"

namespace json {

// ---------- Builder -----------------

KeyItemContext Builder::Key(std::string key) {
    if (!nodes_stack_.back()->IsDict() || nodes_stack_.empty())
        throw std::logic_error("not a dict");

    nodes_stack_.emplace_back(std::make_unique<Node>(key));
    return KeyItemContext(*this);
}

Builder& Builder::Value(Node::Value value) {
    Node node = std::visit(NodeGetter{}, value);

    if (nodes_stack_.empty()) {
        nodes_stack_.emplace_back(std::make_unique<Node>(node));
    } else if (nodes_stack_.back()->IsArray()) {
        Array array = ReleaseBack().AsArray();
        array.push_back(std::move(node));

        nodes_stack_.emplace_back(std::make_unique<Node>(array));
    } else if (level_.dict && nodes_stack_.back()->IsString()) {
        const std::string key = ReleaseBack().AsString();
        Dict dict = ReleaseBack().AsDict();
        dict.insert({key, std::move(node)});

        nodes_stack_.emplace_back(std::make_unique<Node>(dict));
    } else {
        throw std::logic_error("not a dict");
    }

    return *this;
}

ArrayItemContext Builder::StartArray() {
    if (IsRoot())
        throw std::logic_error("try to add new root");

    nodes_stack_.emplace_back(std::make_unique<Node>(json::Array{}));
    ++level_.array;
    return ArrayItemContext(*this);
}

DictItemContext Builder::StartDict() {
    if (IsRoot())
        throw std::logic_error("try to add new root");

    nodes_stack_.emplace_back(std::make_unique<Node>(json::Dict{}));
    ++level_.dict;
    return DictItemContext(*this);
}

Node Builder::Build() {
    if (nodes_stack_.size() != 1)
        throw std::logic_error(
            "root has " + std::to_string(nodes_stack_.size()) + " nodes"
        );
    else if (level_.array || level_.dict)
        throw std::logic_error(
            std::to_string(level_.array + level_.dict) + " containers are not closed"
        );

    return *nodes_stack_.back();
}

// ---------- ItemContext -------------

KeyItemContext DictItemContext::Key(std::string value) {
    return Get().Key(std::move(value));
}

} // end namespace json