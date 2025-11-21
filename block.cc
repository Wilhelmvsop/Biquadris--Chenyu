export module Block;
import <vector>;
import <utility>;

export class Block {
     protected:
     std::vector<std::pair<int, int>> coords;
     char ch;
     int motherLevel;

     public:
     Block(std::vector<std::pair<int, int>> coords, char ch, int level);
     bool isCleared();
     void deleteCoords(std::pair<int, int>);
     
     // getters and setters
     std::vector<std::pair<int, int>> getCoords();
     char getChar();
     int getMotherLevel();
     void setCoords(std::vector<std::pair<int, int>> coords);

     virtual std::vector<std::pair<int, int>> getRotatedCoords() = 0;
     virtual ~Block() = default;
};

