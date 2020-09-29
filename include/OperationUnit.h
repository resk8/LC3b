#include<memory>
#ifdef __linux__ 
    #include "../include/LC3b.h"

#else
    #include "LC3b.h"
#endif

class Latch;
class OperationUnit
{
    public:
    virtual ~OperationUnit() = default;
    static std::unique_ptr<OperationUnit> MakeUnit(Latch & latch);
};

class Shifter : public OperationUnit
{
    public:
    Shifter(bits16 source, bitfield<6> control);
    ~Shifter(){}
};

class Alu : public OperationUnit
{
    public:
    Alu(bits16 source1, bits16 source2, bits2 control);
    ~Alu(){}
};
