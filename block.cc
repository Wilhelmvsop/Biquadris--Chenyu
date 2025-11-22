export module block;

export class Block {
   public:
    virtual char getChar() = 0;
};

export class IBlock : public Block {
   public:
    char getChar() override;
};
export class JBlock : public Block {
   public:
    char getChar() override;
};
export class LBlock : public Block {
   public:
    char getChar() override;
};
export class OBlock : public Block {
   public:
    char getChar() override;
};
export class SBlock : public Block {
   public:
    char getChar() override;
};
export class ZBlock : public Block {
   public:
    char getChar() override;
};
export class TBlock : public Block {
   public:
    char getChar() override;
};
export class BombBlock : public Block {
   public:
    char getChar() override;
};
