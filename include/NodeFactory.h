#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include "Node.h"
#include <memory>
#include <string>
#include <functional>
#include <map>

class NodeFactory {
public:
    using CreatorFunc = std::function<Node*()>;
    
    static NodeFactory& instance() {
        static NodeFactory instance;
        return instance;
    }
    
    void registerNodeType(const std::string& name, CreatorFunc creator) {
        m_creators[name] = creator;
    }
    
    Node* createNode(const std::string& name) {
        auto it = m_creators.find(name);
        if (it != m_creators.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    std::vector<std::string> getAvailableNodes() const {
        std::vector<std::string> names;
        for (const auto& pair : m_creators) {
            names.push_back(pair.first);
        }
        return names;
    }
    
private:
    NodeFactory() = default;
    ~NodeFactory() = default;
    NodeFactory(const NodeFactory&) = delete;
    NodeFactory& operator=(const NodeFactory&) = delete;
    
    std::map<std::string, CreatorFunc> m_creators;
};

template<typename T>
class NodeRegistrar {
public:
    NodeRegistrar(const std::string& name) {
        NodeFactory::instance().registerNodeType(name, []() { return new T(); });
    }
};

#define REGISTER_NODE(type) \
    static NodeRegistrar<type> registrar_##type(#type)

#endif // NODEFACTORY_H