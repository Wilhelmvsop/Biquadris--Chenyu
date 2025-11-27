import Game;
import InputHandler;

import <iostream>;
import <string>;
import <vector>;

int main(int argc, char* argv[]) {
    bool tuiOnly = false;
    unsigned int seed = 67;
    std::string scriptfile1 = "sequence1.txt";
    std::string scriptfile2 = "sequence2.txt";
    int startLevel = 0;

    std::vector<std::string> args{};
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    for (int i = 0; i < args.size(); ++i) {
        const std::string& flag = args[i];
        if (flag == "-text") {
            tuiOnly = true;
        } else if (flag == "-seed") {
            if (i + 1 >= args.size()) {
                std::cerr << "Go goon" << std::endl;
                return;
            }
            seed = std::stoul(args[++i]);
        } else if (flag == "-scriptfile1") {
            if (i + 1 >= args.size()) {
                std::cerr << "Go goon" << std::endl;
                return;
            }
            scriptfile1 = args[++i];
        } else if (flag == "-scriptfile2") {
            if (i + 1 >= args.size()) {
                std::cerr << "Go goon" << std::endl;
                return;
            }
            scriptfile2 = args[++i];
        } else if (flag == "-startlevel") {
            if (i + 1 >= args.size()) {
                std::cerr << "Go goon" << std::endl;
                return;
            }
            startLevel = std::stoi(args[++i]);
        } else {
            std::cerr << "Go GOON" << std::endl;
        }
    }

    Game game{{tuiOnly, seed, scriptfile1, scriptfile2, startLevel,
               make_shared<Inputhandler>()}};
    game.play();
}
