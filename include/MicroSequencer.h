#include <vector>
#ifdef __linux__ 
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64

class Simulator;
class MicroSequencer
{
  public:
  MicroSequencer(Simulator & intance);
  ~MicroSequencer(){}

  Simulator & simulator() { return _simulator; }

  void Initialize();
  void init_control_store(char *ucode_filename);
  void SetMicroCodeBitsAt(uint8_t index, uint8_t bits, bool val);
  bool GetMicroCodeBitsAt(uint8_t index, uint8_t bits) const;
  cs_bits & GetMicroCodeAt(uint8_t row);

  /***************************************************************/
  /* Functions to get at the control bits.                       */
  /***************************************************************/
  bool Get_SR1_NEEDED(const cs_bits & x) const         { return (x[SR1_NEEDED]); }
  bool Get_SR2_NEEDED(const cs_bits & x) const         { return (x[SR2_NEEDED]); }
  bool Get_DRMUX(const cs_bits & x) const              { return (x[DRMUX]);}
  bool Get_DE_BR_OP(const cs_bits & x) const           { return (x[BR_OP]); } 
  bool Get_ADDR1MUX(const agex_cs_bits & x) const      { return (x[AGEX_ADDR1MUX]); }
  bits2 Get_ADDR2MUX(const agex_cs_bits & x) const     { return ((x[AGEX_ADDR2MUX1] << 1) + x[AGEX_ADDR2MUX0]); }
  bool Get_LSHF1(const agex_cs_bits & x) const         { return (x[AGEX_LSHF1]); }
  bool Get_ADDRESSMUX(const agex_cs_bits & x) const    { return (x[AGEX_ADDRESSMUX]); }
  bool Get_SR2MUX(const agex_cs_bits & x) const        { return (x[AGEX_SR2MUX]); }
  bits2 Get_ALUK(const agex_cs_bits & x) const         { return ((x[AGEX_ALUK1] << 1) + x[AGEX_ALUK0]); }
  bool Get_ALU_RESULTMUX(const agex_cs_bits & x) const { return (x[AGEX_ALU_RESULTMUX]); }
  bool Get_BR_OP(const mem_cs_bits & x) const          { return (x[MEM_BR_OP]); }
  bool Get_UNCOND_OP(const mem_cs_bits & x) const      { return (x[MEM_UNCOND_OP]); }
  bool Get_TRAP_OP(const mem_cs_bits & x) const        { return (x[MEM_TRAP_OP]); }
  bool Get_DCACHE_EN(const mem_cs_bits & x) const      { return (x[MEM_DCACHE_EN]); }
  bool Get_DCACHE_RW(const mem_cs_bits & x) const      { return (x[MEM_DCACHE_RW]); }
  bool Get_DATA_SIZE(const mem_cs_bits & x) const      { return (x[MEM_DATA_SIZE]); } 
  bits2 Get_DR_VALUEMUX1(const sr_cs_bits & x) const   { return ((x[SR_DR_VALUEMUX1] << 1 ) + x[SR_DR_VALUEMUX0]); }
  bool Get_AGEX_LD_REG(const agex_cs_bits & x) const   { return (x[AGEX_LD_REG]); }
  bool Get_AGEX_LD_CC(const agex_cs_bits & x) const    { return (x[AGEX_LD_CC]); }
  bool Get_MEM_LD_REG(const mem_cs_bits & x) const     { return (x[MEM_LD_REG]); }
  bool Get_MEM_LD_CC(const mem_cs_bits & x) const      { return (x[MEM_LD_CC]); }
  bool Get_SR_LD_REG(const sr_cs_bits & x) const       { return (x[SR_LD_REG]); }
  bool Get_SR_LD_CC(const sr_cs_bits & x) const        { return (x[SR_LD_CC]); }
  bool Get_DE_BR_STALL(const cs_bits & x) const        { return (x[BR_STALL]); }
  bool Get_AGEX_BR_STALL(const agex_cs_bits & x) const { return (x[AGEX_BR_STALL]); }
  bool Get_MEM_BR_STALL(const mem_cs_bits & x) const   { return (x[MEM_BR_STALL]); }
  
  void print_CS(const cs_bits & CS, int num) const;
  
  private:
  Simulator & _simulator;
  
  /***************************************************************/
  /* The control store rom.                                      */
  /***************************************************************/
  std::vector<cs_bits> CONTROL_STORE;
};
