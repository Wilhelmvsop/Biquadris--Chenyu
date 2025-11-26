module Game;

import Player;
import InputHandler;
import Levels;
import Renderers;
import <iostream>;
import <sstream>;
import <vector>;
import <algorithm>;
import <fstream>;
import <memory>;

Game::Game(std::istream *in, InputHandler *ih, const std::vector<Renderer *> &renders)
    : p1{nullptr}, p2{nullptr}, turn{1}, input{in}, inputHandler{ih}, ownerOfInputHandler{ih == nullptr}, renderers{renders}
{
    // create default levels and players
    LevelFactory lf;
    auto l1 = lf.createLevel(0);
    auto l2 = lf.createLevel(0);
    // Create a non-owning shared_ptr to the external input stream via aliasing
    auto inputOwner = std::make_shared<int>(0);
    auto sharedInput = std::shared_ptr<std::istream>(inputOwner, input);
    p1 = new Player(l1, sharedInput);
    p2 = new Player(l2, sharedInput);
    if (ownerOfInputHandler)
        inputHandler = new InputHandler();
}

Game::~Game()
{
    delete p1;
    delete p2;
    if (ownerOfInputHandler)
        delete inputHandler;
    // shared_ptr fields in Debuff clean up automatically
}

void Game::cleanupDebuff(Debuff &d)
{
    d.force.reset();
    d.insert.first.reset();
    d.insert.second = 0;
    d.blind = false;
    d.heaviness = 0;
}

void Game::applyDebuffToOpponent(const Debuff &d, Player *opponent)
{
    // apply by updating opponent's pending debuff (class members)
    Debuff &pending = (opponent == p1) ? DebuffP1 : DebuffP2;
    pending.blind = pending.blind || d.blind;
    pending.heaviness += d.heaviness;
    if (d.force)
    {
        pending.force = d.force;
    }
    if (d.insert.first)
    {
        pending.insert = d.insert;
    }
}

void Game::parseFirstWord(const std::string &line, std::string &first, std::string &extra)
{
    std::istringstream iss(line);
    if (!(iss >> first))
    { // if no first token -> empty line
        first.clear();
        extra.clear();
        return;
    }
    // read the remaining part (single extra token if present)
    iss >> extra;
}

void Game::processCmd(const std::string &rawFirst, const std::string &extra, int multiplier, Player *cur, Player *other, Debuff &pendingCur, Debuff &pendingOther, std::istream *&curInput, std::vector<std::ifstream *> &fileStack, bool &shouldQuit)
{
    // parse multiplier from rawFirst
    size_t i = 0;
    while (i < rawFirst.size() && rawFirst[i] >= '0' && rawFirst[i] <= '9')
        ++i;
    int cnt = 1;
    if (i > 0)
        cnt = std::stoi(rawFirst.substr(0, i));
    std::string cmd = (i > 0) ? rawFirst.substr(i) : rawFirst;
    // if multiplier passed as second param, prefer it (used for repeat)
    if (multiplier > 0)
        cnt = multiplier;
    // resolve command alias; ignore invalid commands
    std::string resolved = cmd;
    if (inputHandler)
        resolved = inputHandler->resolve(cmd);
    if (resolved == "INVALID_CMD" || resolved.empty()) {
        return; // skip invalid or empty commands safely
    }

    // handle sequence command specially
    if (resolved == "sequence")
    {
        if (extra.empty())
            return; // nothing to do
        std::ifstream *fs = new std::ifstream(extra);
        if (!fs->is_open())
        {
            delete fs;
            return;
        } // file not found
        fileStack.emplace_back(fs);
        curInput = fileStack.back();
        // make sure players read prompts from new input (aliasing non-owning)
        auto curOwner = std::make_shared<int>(0);
        p1->setInput(std::shared_ptr<std::istream>(curOwner, curInput));
        p2->setInput(std::shared_ptr<std::istream>(curOwner, curInput));
        return;
    }

    // restart: game-level reset
    if (resolved == "restart")
    {
        // call each player's restart logic
        cur->play("restart", Debuff{});
        other->play("restart", Debuff{});
        cleanupDebuff(DebuffP1);
        cleanupDebuff(DebuffP2);
        turn = 1; // reset current turn
        turnCount = 0;
        return;
    }

    // For other commands, we will repeat cnt times (where cnt>=1)
    for (int t = 0; t < cnt; ++t)
    {
        // pass the pending debuff to player
        Debuff deb = (cur == p1) ? DebuffP1 : DebuffP2;
        PlayResult res = cur->play(resolved, deb, extra);
        // If the player triggered a special action on drop, apply to opponent
        if (res.status == PlayStatus::Endturn || res.status == PlayStatus::Lost)
        {
            // apply returned debuff to opponent
            if (!(res.debuff == Debuff{}))
            {
                applyDebuffToOpponent(res.debuff, other);
            }
        }
        // Clear pending for this player if this turn was Endturn (i.e., drop occurred)
        if (res.status == PlayStatus::Endturn)
        {
            if (cur == p1)
                cleanupDebuff(DebuffP1);
            else
                cleanupDebuff(DebuffP2);
            // switch turn
            turn = 3 - turn;
            ++turnCount;
            break; // stop repeating after Endturn
        }
        if (res.status == PlayStatus::Lost)
        {
            shouldQuit = true;
            return;
        }
    }
}

static RenderPackage buildPackage(Player *p)
{
    RenderPackage pkg;
    pkg.score = p->getScore();
    pkg.highscore = p->getHighscore();
    pkg.level = p->getLevelNum();
    // getPixels now returns vector<vector<char>>, convert to char(*)[11]
    auto pixelsVec = p->getPixels();
    char (*pixelsArr)[11] = new char[18][11];
    for (int r = 0; r < 18; ++r)
    {
        for (int c = 0; c < 11; ++c)
        {
            pixelsArr[r][c] = pixelsVec[r][c];
        }
    }
    pkg.pixels = pixelsArr;
    pkg.nextBlock = p->getNextBlock();
    pkg.lost = false;
    return pkg;
}

void Game::play()
{
    if (!p1 || !p2)
        return;
    // input stack for sequence file support
    std::vector<std::ifstream *> fileStack;
    std::istream *currentInput = input;
    // pending debuffs for players (use class members)
    DebuffP1 = Debuff{};
    DebuffP2 = Debuff{};
    turnCount = 0;

    // main loop
    while (true)
    {
        // render both players
        RenderPackage pkg1 = buildPackage(p1);
        RenderPackage pkg2 = buildPackage(p2);
        for (auto r : renderers)
        {
            if (r)
                r->render(pkg1, pkg2);
        }
        // clean up pixels allocated by getPixels
        delete[] pkg1.pixels;
        delete[] pkg2.pixels;

        // Read command from input
        std::string line;
        if (!std::getline(*currentInput, line))
        {
            // if EOF on a file stream, pop and restore previous input
            if (!fileStack.empty())
            {
                // delete the top file stream and pop
                delete fileStack.back();
                fileStack.pop_back();
                if (!fileStack.empty())
                {
                    currentInput = fileStack.back();
                }
                else
                {
                    currentInput = input;
                }
                // reset players' input to currentInput (aliasing non-owning)
                auto curOwner2 = std::make_shared<int>(0);
                p1->setInput(std::shared_ptr<std::istream>(curOwner2, currentInput));
                p2->setInput(std::shared_ptr<std::istream>(curOwner2, currentInput));
                continue;
            }
            else
            {
                break; // actual EOF
            }
        }

        // token example: "3right" or "sequence commands.txt" etc.
        std::string first, extra;
        parseFirstWord(line, first, extra);
        int multiplier = 0; // prefer digits inside first
        Player *cur = (turn == 1) ? p1 : p2;
        Player *other = (turn == 1) ? p2 : p1;
        bool shouldQuit = false;
        try
        {
            processCmd(first, extra, multiplier, cur, other, (turn == 1 ? DebuffP1 : DebuffP2), (turn == 1 ? DebuffP2 : DebuffP1), currentInput, fileStack, shouldQuit);
            if (shouldQuit)
            {
                // render final status
                for (auto r : renderers)
                {
                    if (r)
                    {
                        RenderPackage f1 = buildPackage(p1);
                        RenderPackage f2 = buildPackage(p2);
                        r->render(f1, f2);
                        delete[] f1.pixels;
                        delete[] f2.pixels;
                    }
                }
                break;
            }
        }
        catch (...)
        {
            // ignore invalid command; continue
        }
    }
    // cleanup any remaining opened sequence file streams
    for (auto fs : fileStack)
    {
        delete fs;
    }
    fileStack.clear();
}
// helper access for test
Player *Game::getPlayer(int idx) { return idx == 1 ? p1 : p2; }
