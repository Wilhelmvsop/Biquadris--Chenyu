import <iostream>;
import <string>;
import InputHandler;
using namespace std;
int main() {
    InputHandler ih;

    // registerAlias(alias, command)
    std::string alias_left   = "left";
    std::string cmd_left     = "MOVE_LEFT";
    std::string alias_right  = "right";
    std::string cmd_right    = "MOVE_RIGHT";
    std::string alias_rotate     = "rotate";
    std::string cmd_rotate       = "ROTATE";
    std::string alias_levelup = "levelup";
    std::string cmd_levelup       = "levelup";

    ih.registerAlias(alias_left,  cmd_left);
    ih.registerAlias(alias_right, cmd_right);
    // register rotate early so 'r' is ambiguous between "right" and "rotate"
    ih.registerAlias(alias_rotate, cmd_rotate);
    ih.registerAlias(alias_levelup, cmd_levelup);

    std::string q1 = "left";
    std::string q2 = "right";
    cout << ih.resolve(q1) << "\n"; // Expected: MOVE_LEFT
    cout << ih.resolve(q2) << "\n"; // Expected: MOVE_RIGHT

    std::string q3 = "l";   
    std::string q4 = "r";
    cout << ih.resolve(q3) << "\n"; // Expected: Invalid_CMD
    cout << ih.resolve(q4) << "\n"; // Expected: Invalid_CMD

    std::string alias_anything = "anything";
    std::string cmd_anything   = "ANYTHING";
    ih.registerAlias(alias_anything, cmd_anything);

    // register rotate alias as well
    ih.registerAlias(alias_rotate, cmd_rotate);

    std::string q5 = "le";
    std::string q6 = "lef";
    cout << ih.resolve(q5) << "\n"; // Expected: Invalid_CMD
    cout << ih.resolve(q6) << "\n"; // Expected: MOVE_LEFT

    std::string q7 = "anything";
    cout << ih.resolve(q7) << "\n"; // Expected: ANYTHING

    std::string oldName = "right";
    std::string newName = "rr";
    bool renamed = ih.rename(oldName, newName);

    
    std::string q9  = "rr";
    cout << ih.resolve(q9) << "\n"; // Expected: MOVE_RIGHT

}
