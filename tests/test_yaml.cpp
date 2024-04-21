#include "../Glacier/config.h"
#include "../Glacier/log.h"
#include "yaml-cpp/yaml.h"

Glacier::ConfigVar<int>::ptr g_int_value_config = Glacier::Config::Lookup("system.port", (int)8080, "system port");

Glacier::ConfigVar<float>::ptr g_float_value_config = Glacier::Config::Lookup("system.port", (float)10.2f, "system port");

void print_yaml(const YAML::Node& node, int level) {
    if (node.IsScalar()) {
        GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Type() << " - " << level;
    } else if (node.IsNull()) {
        GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << std::string(level * 4, ' ') << "NULL - " << node.Type() << " - " << level;
    } else if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << std::string(level * 4, ' ') << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << std::string(level * 4, ' ') << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml() {
    YAML::Node root = YAML::LoadFile("/home/penguin/code/Glacier/bin/conf/log.yml");

    print_yaml(root, 0);

    GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << root;
}

void test_config() {
    GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << "before: " << g_float_value_config->toString();

    YAML::Node root = YAML::LoadFile("/home/penguin/code/Glacier/bin/conf/log.yml");
    Glacier::Config::LoadFromYaml(root);

    GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << "after: " << g_float_value_config->toString();
}

int main(int argc, char** argv) {
    // GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << g_int_value_config->getValue();
    // GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << g_float_value_config->toString();

    // test_yaml();
    test_config();

    return 0;
}
