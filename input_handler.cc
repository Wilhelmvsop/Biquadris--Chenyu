export module InputHandler;

import <string>;
import <vector>;
import <memory>;

// Prefix-tree (trie) based InputHandler.
// - registerAlias(alias, command): map an alias string to a logical command
// name.
// - rename(oldName, newName): move existing alias to a new alias.
export struct Node {
    // Use a fixed-size vector indexed by char value (0..255).
    std::vector<std::shared_ptr<Node>> children =
        std::vector<std::shared_ptr<Node>>(256, nullptr);
    bool isWord = false;
    std::string cmd;
    int subtreeWords = 0;  // number of registered commands in this subtree
    std::string representative;  // valid when subtreeWords == 1
};

export class InputHandler {
    std::shared_ptr<Node> root;

    std::shared_ptr<Node> findNode(const std::string& s);
    // Update subtreeWords and representative along a path; delta is +1 or -1.
    void changeCountsOnPath(const std::string& s, int delta,
                            const std::string& rep);
    // When an existing alias's command string is updated, update
    // representatives on path where subtreeWords==1
    void updateRepresentativeOnPath(const std::string& s,
                                    const std::string& rep);

    // Register all default aliases for command (called in ctor)
    void useDefaultAliases();

   public:
    InputHandler();
    ~InputHandler();

    bool registerAlias(const std::string& alias, const std::string& command);
    bool rename(const std::string& oldName, const std::string& newName);
    // resolve: exact match preferred; otherwise unique-prefix resolution.
    std::string resolve(const std::string& alias);
};
