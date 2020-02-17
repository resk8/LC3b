#include <map>
#include <vector>

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
/* control signals from the control store */
enum CS_BITS {
  SR1_NEEDED,
  SR2_NEEDED,
  DRMUX,
  
  ADDR1MUX,
  ADDR2MUX1, ADDR2MUX0, 
  LSHF1,
  ADDRESSMUX,
  SR2MUX, 
  ALUK1, ALUK0,
  ALU_RESULTMUX,

  BR_OP,
  UNCOND_OP,
  TRAP_OP,
  BR_STALL,
  
  DCACHE_EN,
  DCACHE_RW,
  DATA_SIZE,

  DR_VALUEMUX1, DR_VALUEMUX0,
  LD_REG,
  LD_CC,
  NUM_CONTROL_STORE_BITS
};


enum AGEX_CS_BITS {
  AGEX_ADDR1MUX,
  AGEX_ADDR2MUX1, AGEX_ADDR2MUX0, 
  AGEX_LSHF1,
  AGEX_ADDRESSMUX,
  AGEX_SR2MUX,
  AGEX_ALUK1, AGEX_ALUK0, 
  AGEX_ALU_RESULTMUX,

  AGEX_BR_OP,
  AGEX_UNCOND_OP,
  AGEX_TRAP_OP,
  AGEX_BR_STALL,
  AGEX_DCACHE_EN,
  AGEX_DCACHE_RW,
  AGEX_DATA_SIZE,
  
  AGEX_DR_VALUEMUX1, AGEX_DR_VALUEMUX0,
  AGEX_LD_REG,
  AGEX_LD_CC,
  NUM_AGEX_CS_BITS
};

enum MEM_CS_BITS {
  MEM_BR_OP,
  MEM_UNCOND_OP,
  MEM_TRAP_OP,
  MEM_BR_STALL,
  MEM_DCACHE_EN,
  MEM_DCACHE_RW,
  MEM_DATA_SIZE,

  MEM_DR_VALUEMUX1, MEM_DR_VALUEMUX0,
  MEM_LD_REG,
  MEM_LD_CC,
  NUM_MEM_CS_BITS
};

enum SR_CS_BITS {
  SR_DR_VALUEMUX1, SR_DR_VALUEMUX0,
  SR_LD_REG,
  SR_LD_CC,
  NUM_SR_CS_BITS
};

class Simulator;
class MicroSequencer
{
  public:
  MicroSequencer(Simulator & intance) : _simulator(intance) {}
  ~MicroSequencer(){}

  Simulator & simulator() { return _simulator; }

  void Initialize();

  void init_control_store(char *ucode_filename);
  int GetMicroCodeBitsAt(int index, int bits) const;
  void SetMicroCodeBitsAt(int index, int bits, int val);

  static int GetNumOfControlStoreBits() { return NUM_CONTROL_STORE_BITS; }
  static int GetNumOfAGEXControlBits()  { return NUM_AGEX_CS_BITS; }
  static int GetNumOfMEMControlBits()   { return NUM_MEM_CS_BITS; }
  static int GetNumOfSRControlBits()    { return NUM_SR_CS_BITS; }

  /***************************************************************/
  /* Functions to get at the control bits.                       */
  /***************************************************************/
  int Get_SR1_NEEDED(std::vector<int> & x) const    { return (x[SR1_NEEDED]); }
  int Get_SR2_NEEDED(std::vector<int> & x) const    { return (x[SR2_NEEDED]); }
  int Get_DRMUX(std::vector<int> & x) const         { return (x[DRMUX]);}
  int Get_DE_BR_OP(std::vector<int> & x) const      { return (x[BR_OP]); } 
  int Get_ADDR1MUX(std::vector<int> & x) const      { return (x[AGEX_ADDR1MUX]); }
  int Get_ADDR2MUX(std::vector<int> & x) const      { return ((x[AGEX_ADDR2MUX1] << 1) + x[AGEX_ADDR2MUX0]); }
  int Get_LSHF1(std::vector<int> & x) const         { return (x[AGEX_LSHF1]); }
  int Get_ADDRESSMUX(std::vector<int> & x) const    { return (x[AGEX_ADDRESSMUX]); }
  int Get_SR2MUX(std::vector<int> & x) const        { return (x[AGEX_SR2MUX]); }
  int Get_ALUK(std::vector<int> & x) const          { return ((x[AGEX_ALUK1] << 1) + x[AGEX_ALUK0]); }
  int Get_ALU_RESULTMUX(std::vector<int> & x) const { return (x[AGEX_ALU_RESULTMUX]); }
  int Get_BR_OP(std::vector<int> & x) const         { return (x[MEM_BR_OP]); }
  int Get_UNCOND_OP(std::vector<int> & x) const     { return (x[MEM_UNCOND_OP]); }
  int Get_TRAP_OP(std::vector<int> & x) const       { return (x[MEM_TRAP_OP]); }
  int Get_DCACHE_EN(std::vector<int> & x) const     { return (x[MEM_DCACHE_EN]); }
  int Get_DCACHE_RW(std::vector<int> & x) const     { return (x[MEM_DCACHE_RW]); }
  int Get_DATA_SIZE(std::vector<int> & x) const     { return (x[MEM_DATA_SIZE]); } 
  int Get_DR_VALUEMUX1(std::vector<int> & x) const  { return ((x[SR_DR_VALUEMUX1] << 1 ) + x[SR_DR_VALUEMUX0]); }
  int Get_AGEX_LD_REG(std::vector<int> & x) const   { return (x[AGEX_LD_REG]); }
  int Get_AGEX_LD_CC(std::vector<int> & x) const    { return (x[AGEX_LD_CC]); }
  int Get_MEM_LD_REG(std::vector<int> & x) const    { return (x[MEM_LD_REG]); }
  int Get_MEM_LD_CC(std::vector<int> & x) const     { return (x[MEM_LD_CC]); }
  int Get_SR_LD_REG(std::vector<int> & x) const     { return (x[SR_LD_REG]); }
  int Get_SR_LD_CC(std::vector<int> & x) const      { return (x[SR_LD_CC]); }
  int Get_DE_BR_STALL(std::vector<int> & x) const   { return (x[BR_STALL]); }
  int Get_AGEX_BR_STALL(std::vector<int> & x) const { return (x[AGEX_BR_STALL]); }
  int Get_MEM_BR_STALL(std::vector<int> & x) const  { return (x[MEM_BR_STALL]); }
  
  void print_CS(int *CS, int num) const;
  
  private:
  Simulator & _simulator;
  
  /***************************************************************/
  /* The control store rom.                                      */
  /***************************************************************/
  std::map<int,std::map<int,int>> CONTROL_STORE;
};