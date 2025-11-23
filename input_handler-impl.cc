module InputHandler;

import <string>;
import <vector>;

InputHandler::InputHandler() : root(new Node()) {}

InputHandler::~InputHandler() { deleteSubtree(root); }

// Helper: update subtreeWords and representative along path s.
// delta == 1 for insertion, -1 for removal. rep is the cmd to set on insertion.
void InputHandler::changeCountsOnPath(const std::string &s, int delta, const std::string &rep)
{
    Node *node = root;
    for (char c : s)
    {
        if (!node->children[c])
            node->children[c] = new Node();
        node = node->children[c];
        node->subtreeWords += delta;
        if (delta > 0)
        {
            if (node->subtreeWords == 1)
                node->representative = rep; // newly unique
            else
                node->representative.clear(); // ambiguous
        }
        else
        {
            if (node->subtreeWords <= 0)
            {
                node->representative.clear();
            }
            else if (node->subtreeWords == 1)
            {
                if (node->isWord)
                {
                    node->representative = node->cmd;
                }
                else
                {
                    std::string rep;
                    for (size_t i = 0; i < node->children.size(); ++i)
                    {
                        Node *ch = node->children[i];
                        if (ch && ch->subtreeWords > 0)
                        {
                            rep = ch->representative;
                            break;
                        }
                    }
                    node->representative = rep;
                }
            }
            else
            {
                node->representative.clear();
            }
        }
    }
}

void InputHandler::updateRepresentativeOnPath(const std::string &s, const std::string &rep)
{
    Node *node = root;
    for (char c : s)
    {
        if (!node->children[c])
            node->children[c] = new Node();
        node = node->children[c];
        if (node->subtreeWords == 1)
            node->representative = rep;
    }
}

bool InputHandler::registerAlias(const std::string &alias, const std::string &cmd)
{
    if (alias.empty())
        return false;
    Node *node = root;
    for (char c : alias)
    {
        if (!node->children[c])
            node->children[c] = new Node();
        node = node->children[c];
    }
    if (node->isWord)
    {
        // alias already existed: update cmd and adjust representatives if needed
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

bool InputHandler::rename(const std::string &oldName, const std::string &newName)
{
    if (oldName.empty() || newName.empty())
        return false;
    Node *oldNode = findNode(oldName);
    if (!oldNode || !oldNode->isWord)
        return false;
    std::string cmd = oldNode->cmd;
    // remove old mapping
    oldNode->isWord = false;
    oldNode->cmd.clear();
    changeCountsOnPath(oldName, -1, std::string());
    // register new mapping
    registerAlias(newName, cmd);
    return true;
}

std::string InputHandler::resolve(const std::string &input)
{
    if (input.empty())
        return std::string("INVALID_CMD");
    Node *n = findNode(input);
    if (n && n->isWord)
        return n->cmd;
    // else try unique-prefix resolution using subtreeWords/representative
    Node *node = findNode(input);
    if (!node)
        return std::string("INVALID_CMD");
    if (node->subtreeWords == 1)
        return node->representative;
    return std::string("INVALID_CMD");
}

Node *InputHandler::findNode(const std::string &s)
{
    Node *node = root;
    for (char c : s)
    {
        if (!node->children[c])
            return nullptr;
        node = node->children[c];
    }
    return node;
}

void InputHandler::deleteSubtree(Node *n)
{
    if (!n)
        return;
    for (size_t i = 0; i < n->children.size(); ++i)
    {
        if (n->children[i])
            deleteSubtree(n->children[i]);
    }
    delete n;
}
