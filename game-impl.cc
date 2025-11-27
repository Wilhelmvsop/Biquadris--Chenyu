module Game;

Game::Game(const GameSettings& settings)
    : p1{nullptr},
      p2{nullptr},
      turnCount{1},
      input{&std::cin, [](std::istream*) {}},  // don't delete &cin
      inputHandler{settings.ih},
      renderers{} {
    LevelFactory lf;
    auto l1 =
        lf.createLevel(settings.startLevel, settings.seed, settings.script1);
    auto l2 =
        lf.createLevel(settings.startLevel, settings.seed, settings.script2);
    p1 = make_shared<Player>(l1, input);
    p2 = make_shared<Player>(l2, input);

    renderers.emplace_back(new TuiRenderer());
    if (!settings.tuiOnly) {
        renderers.emplace_back(new GuiRenderer());
    }
}

Game::~Game() {}

void Game::cleanupDebuff(Debuff& d) {
    d.force.reset();
    d.insert.first.reset();
    d.insert.second = 0;
    d.blind = false;
    d.heaviness = 0;
}

void Game::parseFirstWord(const std::string& line, std::string& first,
                          std::string& extra) {
    std::istringstream iss(line);
    if (!(iss >> first)) {  // if no first token -> empty line
        first.clear();
        extra.clear();
        return;
    }
    // read the remaining part (single extra token if present)
    iss >> extra;
}

void Game::say(const std::string& msg) const noexcept {
#ifndef TESTING
    std::cout << msg << std::endl;
#endif
}

std::istream& Game::prompt(std::istream& in, std::string& response,
                           const std::string& msg) {
#ifndef TESTING
    std::cout << msg;
#endif
    return getline(in, response);
}

void Game::processCmd(const std::string& rawFirst, const std::string& extra,
                      Player& cur, Player& other, std::istream& curInput,
                      int& whoLost) {
    // parse multiplier from rawFirst
    size_t i = 0;
    while (i < rawFirst.size() && rawFirst[i] >= '0' && rawFirst[i] <= '9') ++i;
    int cnt = 1;
    if (i > 0) cnt = std::stoi(rawFirst.substr(0, i));
    if (cnt == 0) cnt = 1;  // 0 is invalid, do 1 instead

    // resolve command alias;
    std::string cmd = (i > 0) ? rawFirst.substr(i) : rawFirst;
    std::string resolved = inputHandler->resolve(cmd);
    if (resolved == "INVALID_CMD" || resolved.empty()) {
        throw std::runtime_error("Invalid command, try again.");
    }

    // special command for gracefully exits
    if (resolved == "quit") {
        exit(0);
    }

    // If non-multipliable commands, play once and return
    if (resolved == "sequence") {
        if (extra.empty()) {
            throw std::runtime_error("sequence: please provide the file name.");
        }

        auto fs = std::make_shared<std::ifstream>(extra);
        if (!fs->is_open()) {
            throw std::runtime_error("file " + extra + "  doesn't exists.");
        }  // file not found

        input = fs;
        // set special input source for players
        p1->setInput(input);
        p2->setInput(input);
        return;
    }

    if (resolved == "restart") {
        // call each player's restart logic
        p1->play("restart");
        p2->play("restart");
        cleanupDebuff(DebuffP1);
        cleanupDebuff(DebuffP2);
        turnCount = 1;
        return;
    }

    if (resolved == "norandom") {
        if (extra.empty()) {
            throw std::runtime_error("norandom: please provide file name.");
        }
        cur.play("norandom", extra);
        return;
    }

    if (resolved == "random") {
        cur.play("random");
        return;
    }

    // Multipliable commands
    for (int t = 0; t < cnt; ++t) {
        PlayResult res = cur.play(resolved, extra, t == cnt - 1);
        Debuff& otherDebuff = (&other == p1.get()) ? DebuffP1 : DebuffP2;

        // Cast debuff to opponent
        if (res.debuff != Debuff{}) {
            otherDebuff += res.debuff;
        }

        if (res.status == PlayStatus::Lost) {
            whoLost = (&cur == p1.get()) ? 1 : 2;
            return;
        }

        // End player's turn: when it's last rep and status endturn
        if (t == cnt - 1 && res.status == PlayStatus::Endturn) {
            Debuff& curDebuff = (&cur == p1.get()) ? DebuffP1 : DebuffP2;
            cleanupDebuff(curDebuff);
            ++turnCount;
            bool fits = other.updateDebuff(otherDebuff);
            if (!fits) {
                whoLost = (&other == p1.get()) ? 1 : 2;
            }
            break;  // stop repeating after Endturn
        }
    }
}

static RenderPackage buildPackage(std::shared_ptr<Player> p) {
    RenderPackage pkg;
    pkg.score = p->getScore();
    pkg.highscore = p->getHighscore();
    pkg.level = p->getLevelNum();
    pkg.pixels = p->getPixels();
    pkg.nextBlock = p->getNextBlock();
    pkg.lost = false;
    return pkg;
}

void Game::render(int whoLost) {
    RenderPackage pkg1 = buildPackage(p1);
    RenderPackage pkg2 = buildPackage(p2);
    if (whoLost) {
        RenderPackage& lostPkg = (whoLost == 1) ? pkg1 : pkg2;
        lostPkg.lost = true;
    }
    for (auto r : renderers) {
        r->render(pkg1, pkg2);
    }
}

void Game::play() {
    if (!p1 || !p2) return;

    std::string line;

    // render first frame
    render();

    // main loop
    while (prompt(*input, line, turnCount % 2 == 1 ? "p1: " : "p2: ")) {
        // first word: something like "3rig"
        // extra word: for some command e.g. sequence "seq.txt"
        std::string first, extra;
        parseFirstWord(line, first, extra);
        Player& cur = (turnCount % 2 == 1) ? *p1 : *p2;
        Player& other = (turnCount % 2 == 1) ? *p2 : *p1;
        int whoLost = 0;
        try {
            processCmd(first, extra, cur, other, *input, whoLost);
            if (whoLost) {
                render(whoLost);
                return;
            }
        } catch (const std::runtime_error& err) {
            // not gonna render if there is error
            // (so that user can see message instead of big screen)
            say(err.what());
            continue;
        }

        render();
    }
}
// helper access for test
std::shared_ptr<Player>& Game::getPlayer(int idx) { return idx == 1 ? p1 : p2; }
