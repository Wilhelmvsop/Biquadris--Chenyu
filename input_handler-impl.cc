module InputHandler;

import <string>;
import <vector>;
import <memory>;

void InputHandler::useDefaultAliases() {
    std::vector<std::pair<std::string, std::string>> aliases = {
        {"left", "left"},
        {"right", "right"},
        {"down", "down"},
        {"drop", "drop"},
        {"clockwise", "clockwise"},
        {"cw", "clockwise"},
        {"clock", "clockwise"},
        {"counterclockwise", "counterclockwise"},
        {"ccw", "counterclockwise"},
        {"levelup", "levelup"},
        {"leveldown", "leveldown"},
        {"norandom", "norandom"},
        {"random", "random"},
        {"sequence", "sequence"},
        {"restart", "restart"},
        {"quit", "quit"},  // for gracefully exit
        {"I", "I"},
        {"J", "J"},
        {"L", "L"},
        {"O", "O"},
        {"S", "S"},
        {"Z", "Z"},
        {"T", "T"}};
    for (auto& p : aliases) registerAlias(p.first, p.second);
}

InputHandler::InputHandler() : root(std::make_shared<Node>()) {
    useDefaultAliases();
}

InputHandler::~InputHandler() {}

// Helper: update subtreeWords and representative along path s.
// delta == 1 for insertion, -1 for removal. rep is the cmd to set on insertion.
void InputHandler::changeCountsOnPath(const std::string& s, int delta,
                                      const std::string& rep) {
    std::shared_ptr<Node> node = root;
    for (char c : s) {
        if (!node->children[c]) node->children[c] = std::make_shared<Node>();
        node = node->children[c];
        node->subtreeWords += delta;
        if (delta > 0) {
            if (node->subtreeWords == 1)
                node->representative = rep;  // newly unique
            else
                node->representative.clear();  // ambiguous
        } else {
            if (node->subtreeWords <= 0) {
                node->representative.clear();
            } else if (node->subtreeWords == 1) {
                if (node->isWord) {
                    node->representative = node->cmd;
                } else {
                    std::string rep;
                    for (size_t i = 0; i < node->children.size(); ++i) {
                        std::shared_ptr<Node> ch = node->children[i];
                        if (ch && ch->subtreeWords > 0) {
                            rep = ch->representative;
                            break;
                        }
                    }
                    node->representative = rep;
                }
            } else {
                node->representative.clear();
            }
        }
    }
}

void InputHandler::updateRepresentativeOnPath(const std::string& s,
                                              const std::string& rep) {
    std::shared_ptr<Node> node = root;
    for (char c : s) {
        if (!node->children[c]) node->children[c] = std::make_shared<Node>();
        node = node->children[c];
        if (node->subtreeWords == 1) node->representative = rep;
    }
}

bool InputHandler::registerAlias(const std::string& alias,
                                 const std::string& cmd) {
    if (alias.empty()) return false;
    std::shared_ptr<Node> node = root;
    for (char c : alias) {
        if (!node->children[c]) node->children[c] = std::make_shared<Node>();
        node = node->children[c];
    }
    if (node->isWord) {
        // alias already existed: update cmd and adjust representatives if
        // needed
        node->cmd = cmd;
        updateRepresentativeOnPath(alias, cmd);
        return true;
    }
    // new alias: increment counts and set representative where appropriate
    changeCountsOnPath(alias, 1, cmd);
    node->isWord = true;
    node->cmd = cmd;
    return true;
}

bool InputHandler::rename(const std::string& oldName,
                          const std::string& newName) {
    if (oldName.empty() || newName.empty()) return false;
    std::shared_ptr<Node> oldNode = findNode(oldName);
    if (!oldNode || !oldNode->isWord) return false;
    std::string cmd = oldNode->cmd;
    // remove old mapping
    oldNode->isWord = false;
    oldNode->cmd.clear();
    changeCountsOnPath(oldName, -1, std::string());
    // register new mapping
    registerAlias(newName, cmd);
    return true;
}

std::string InputHandler::resolve(const std::string& input) {
    if (input.empty()) return std::string("INVALID_CMD");

    // Parse optional leading multiplier digits
    size_t i = 0;
    while (i < input.size() && std::isdigit(input[i])) ++i;
    std::string multiplierStr = input.substr(0, i);
    std::string rest = input.substr(i);

    if (rest.empty()) return std::string("INVALID_CMD");

    // Resolve the rest using existing trie logic
    std::shared_ptr<Node> n = findNode(rest);
    std::string resolved;
    if (n && n->isWord)
        resolved = n->cmd;
    else {
        std::shared_ptr<Node> node = n;  // already found
        if (!node) return std::string("INVALID_CMD");
        if (node->subtreeWords == 1)
            resolved = node->representative;
        else
            return std::string("INVALID_CMD");
    }

    // If there was a multiplier prefix, check whether this command permits
    // multipliers.
    if (!multiplierStr.empty()) {
        // Commands that do NOT accept a multiplier
        std::vector<std::string> disallowed = {"restart", "hint", "norandom",
                                               "random"};
        for (auto& d : disallowed) {
            if (resolved == d) return std::string("INVALID_CMD");
        }
    }

    return resolved;
}

std::shared_ptr<Node> InputHandler::findNode(const std::string& s) {
    std::shared_ptr<Node> node = root;
    for (char c : s) {
        if (!node->children[c]) return nullptr;
        node = node->children[c];
    }
    return node;
}
