#pragma once
#include "json.h"

#include <memory>

namespace json {

// ---------- Builder -----------------

class KeyItemContext;
class DictItemContext;
class ArrayItemContext;

class Builder {
    struct ContainerLevel { int array = 0; int dict = 0; };

public:
    KeyItemContext Key(std::string key);

    Builder& Value(Node::Value value);

    ArrayItemContext StartArray();

    DictItemContext StartDict();

    inline Builder& EndArray() {
        if (!nodes_stack_.back()->IsArray() || nodes_stack_.empty())
            throw std::logic_error("not an array");

        --level_.array;
        return Value(ReleaseBack().AsArray());
    }

    inline Builder& EndDict() {
        if (!nodes_stack_.back()->IsDict() || nodes_stack_.empty())
            throw std::logic_error("not a dict");

        --level_.dict;
        return Value(ReleaseBack().AsDict());
    }

    Node Build();

private:
    Node root_;
    std::vector<std::unique_ptr<Node>> nodes_stack_;
    ContainerLevel level_;

    inline bool IsRoot() {
        return !nodes_stack_.empty()
            && !(nodes_stack_.back()->IsString() || nodes_stack_.back()->IsArray());
    }

    [[nodiscard]] inline Node ReleaseBack() {
        Node node = *nodes_stack_.back();
        nodes_stack_.pop_back();
        return node;
    }
};

// ---------- ItemContext -------------

class ItemContext {
public:
    ItemContext(Builder& builder) : builder_(builder) { }

protected:
    inline Builder& Get() {
        return builder_;
    }

private:
    Builder& builder_;
};

class DictItemContext final : public ItemContext {
public:
    DictItemContext(Builder& builder) : ItemContext(builder) {}

    KeyItemContext Key(std::string value);

    inline Builder& EndDict() {
        return Get().EndDict();
    }
};

class ArrayItemContext final : public ItemContext {
public:
    ArrayItemContext(Builder& builder) : ItemContext(builder) {}

    inline ArrayItemContext Value(Node::Value value) {
        Get().Value(std::move(value));
        return ArrayItemContext(Get());
    }

    inline DictItemContext StartDict() {
        return Get().StartDict();
    }

    inline ArrayItemContext StartArray() {
        return Get().StartArray();
    }

    inline Builder& EndArray() {
        return Get().EndArray();
    }
};

class KeyItemContext final : public ItemContext {
public:
    KeyItemContext(Builder& builder) : ItemContext(builder) {}

    inline DictItemContext Value(Node::Value value) {
        Get().Value(std::move(value));
        return DictItemContext(Get());
    }

    inline DictItemContext StartDict() {
        return Get().StartDict();
    }

    inline ArrayItemContext StartArray() {
        return Get().StartArray();
    }
};

} // namespace json