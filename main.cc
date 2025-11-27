import Game;
import InputHandler;

import <iostream>;
import <string>;
import <vector>;
import <memory>;

bool parseFlag(GameSettings& settings, const std::vector<std::string>& args);

int main(int argc, char* argv[]) {
    GameSettings settings{
        false,           67, "sequence1.txt",
        "sequence2.txt", 0,  std::make_shared<InputHandler>()};

    std::vector<std::string> args{};
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (!parseFlag(settings, args)) {
        return 1;
    }
    Game game{settings};
    game.play();
    return 0;
}

bool parseFlag(GameSettings& settings, const std::vector<std::string>& args) {
    for (unsigned int i = 0; i < args.size(); ++i) {
        const std::string& flag = args[i];
        if (flag == "-text") {
            settings.tuiOnly = true;
        } else if (flag == "-seed") {
            if (i + 1 >= args.size()) {
                std::cerr << "seed: please provide seed" << std::endl;
                return false;
            }
            settings.seed = std::stoul(args[++i]);
        } else if (flag == "-scriptfile1") {
            if (i + 1 >= args.size()) {
                std::cerr << "scriptfile: please provide scriptfile"
                          << std::endl;
                return false;
            }
            settings.script1 = args[++i];
        } else if (flag == "-scriptfile2") {
            if (i + 1 >= args.size()) {
                std::cerr << "scriptfile: please provide scriptfile"
                          << std::endl;
                return false;
            }
            settings.script2 = args[++i];
        } else if (flag == "-startlevel") {
            if (i + 1 >= args.size()) {
                std::cerr << "startlevel: please provide the start level"
                          << std::endl;
                return false;
            }
            settings.startLevel = std::stoi(args[++i]);
        } else {
            std::cerr << "invalid flag " << flag << std::endl;
            return false;
        }
    }
    return true;
};
