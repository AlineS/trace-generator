// INTRINSICS EXTENSION HEADER

#include "../../../../sinuca.hpp"

#ifdef TRACE_GENERATOR_DEBUG
    #define TRACE_GENERATOR_DEBUG_PRINTF(...) DEBUG_PRINTF(__VA_ARGS__);
#else
    #define TRACE_GENERATOR_DEBUG_PRINTF(...)
#endif

//==============================================================================
// Commandline Switches
//==============================================================================
KNOB<string> KnobTrace(KNOB_MODE_WRITEONCE, "pintool", "trace", "null",
    "simulates intrinsics-x86 or instrisics-hmc or CHIVE");

//==============================================================================
// Global Variables
//==============================================================================

// Force each thread's data to be in its own data cache line so that
// multiple threads do not contend for the same data cache line.
// This avoids the false sharing problem.
#define PADSIZE 64

class thread_data_t {
    public:
        PIN_LOCK dyn_lock;
        bool is_instrumented_bbl;

        gzFile gzDynamicTraceFile;
        gzFile gzMemoryTraceFile;

        UINT8 pad[PADSIZE];
};

PIN_LOCK lock;                  /// Lock for methods shared among multiple threads
thread_data_t* thread_data;     /// Lock for methods shared among multiple threads

uint32_t count_trace = 0;       /// Current BBL trace number
bool is_instrumented = false;   /// Will be enabled by PinPoints

std::ofstream StaticTraceFile;
gzFile gzStaticTraceFile;

// Other architectures structure (HMC/CHIVE)
struct data_instr {
    char const *rtn_name;
    char const *arch_instr_name;
    char const *x86_instr_name;
    UINT32 instr_len;
};

// HMC Instruction names
std::string cmp_name0 ("_hmc128_saddimm_s");    std::string cmp_name1 ("_hmc64_incr_s");
std::string cmp_name2 ("_hmc64_bwrite_s");      std::string cmp_name3 ("_hmc128_bswap_s");
std::string cmp_name4 ("_hmc128_or_s");         std::string cmp_name5 ("_hmc128_and_s");
std::string cmp_name6 ("_hmc128_nor_s");        std::string cmp_name7 ("_hmc128_xor_s");
std::string cmp_name8 ("_hmc128_nand_s");       std::string cmp_name9 ("_hmc64_equalto_s");
std::string cmp_name10 ("_hmc128_equalto_s");   std::string cmp_name11 ("_hmc64_cmpswapgt_s");
std::string cmp_name12 ("_hmc64_cmpswaplt_s");  std::string cmp_name13 ("_hmc128_cmpswapz_s");
std::string cmp_name14 ("_hmc64_cmpswapeq_s");  std::string cmp_name15 ("_hmc128_cmpswapgt_s");
std::string cmp_name16 ("_hmc128_cmpswaplt_s"); std::string cmp_name17 ("_hmc64_cmpgteq_s");
std::string cmp_name18 ("_hmc64_cmplteq_s");    std::string cmp_name19 ("_hmc64_cmplt_s");

// CHIVE Instruction names
std::string cmp_name20 ("_chive64_adds");        std::string cmp_name21 ("_chive2K_adds");
std::string cmp_name22 ("_chive64_addu");        std::string cmp_name23 ("_chive2K_addu");
std::string cmp_name24 ("_chive64_subs");        std::string cmp_name25 ("_chive2K_subs");
std::string cmp_name26 ("_chive64_subu");        std::string cmp_name27 ("_chive2K_subu");
std::string cmp_name28 ("_chive64_abs");         std::string cmp_name29 ("_chive2K_abs");
std::string cmp_name30 ("_chive64_max");         std::string cmp_name31 ("_chive2K_max");
std::string cmp_name32 ("_chive64_min");         std::string cmp_name33 ("_chive2K_min");
std::string cmp_name34 ("_chive64_and");         std::string cmp_name35 ("_chive2K_and");
std::string cmp_name36 ("_chive64_or");          std::string cmp_name37 ("_chive2K_or");
std::string cmp_name38 ("_chive64_xor");         std::string cmp_name39 ("_chive2K_xor");
std::string cmp_name40 ("_chive64_not");         std::string cmp_name41 ("_chive2K_not");
std::string cmp_name42 ("_chive64_masks");       std::string cmp_name43 ("_chive2K_masks");
std::string cmp_name44 ("_chive64_masku");       std::string cmp_name45 ("_chive2K_masku");
std::string cmp_name46 ("_chive64_slts");        std::string cmp_name47 ("_chive2K_slts");
std::string cmp_name48 ("_chive64_sltu");        std::string cmp_name49 ("_chive2K_sltu");
std::string cmp_name50 ("_chive64_cmpeqs");      std::string cmp_name51 ("_chive2K_cmpeqs");
std::string cmp_name52 ("_chive64_cmpequ");      std::string cmp_name53 ("_chive2K_cmpequ");
std::string cmp_name54 ("_chive64_sll");         std::string cmp_name55 ("_chive2K_sll");
std::string cmp_name56 ("_chive64_srl");         std::string cmp_name57 ("_chive2K_srl");
std::string cmp_name58 ("_chive64_sra");         std::string cmp_name59 ("_chive2K_sra");
std::string cmp_name60 ("_chive64_divs");        std::string cmp_name61 ("_chive2K_divs");
std::string cmp_name62 ("_chive64_divu");        std::string cmp_name63 ("_chive2K_divu");
std::string cmp_name64 ("_chive64_mods");        std::string cmp_name65 ("_chive2K_mods");
std::string cmp_name66 ("_chive64_modu");        std::string cmp_name67 ("_chive2K_modu");
std::string cmp_name68 ("_chive64_mults");       std::string cmp_name69 ("_chive2K_mults");
std::string cmp_name70 ("_chive64_multu");       std::string cmp_name71 ("_chive2K_multu");
std::string cmp_name72 ("_chive32_mults");       std::string cmp_name73 ("_chive1K_mults");
std::string cmp_name74 ("_chive32_multu");       std::string cmp_name75 ("_chive1K_multu");
std::string cmp_name76 ("_chive64_movs");        std::string cmp_name77 ("_chive2K_movs");
std::string cmp_name78 ("_chive64_movu");        std::string cmp_name79 ("_chive2K_movu");

//==============================================================================
// Header Functions
//==============================================================================
VOID write_dynamic_char(char *dyn_str, THREADID threadid);
VOID write_static_char(char *stat_str);
VOID hmc_write_memory(ADDRINT *read, UINT32 size, UINT32 bbl, THREADID threadid);
VOID initialize_intrinsics(data_instr hmc_x86_data[20]);
VOID initialize_intrinsics_hmc(data_instr hmc_x86_data[20]);
VOID initialize_intrinsics_chive(data_instr hive_x86_data[56]);
VOID hmc_x86_set_data_instr(data_instr *hmc_x86_data, char const *rtn_name, char const *hmc_instr_name, char const *x86_instr_name, UINT32 instr_len);
VOID hive_x86_set_data_instr(data_instr *hive_x86_data, char const *rtn_name, char const *chive_instr_name, char const *x86_instr_name, UINT32 instr_len);
VOID synthetic_trace_generation(std::string rtn_name, data_instr hmc_x86_data[20]);
VOID arch_x86_trace_instruction(RTN hmc_x86_rtn, data_instr *hmc_x86_data);
INT icheck_conditions(std::string rtn_name);
INT icheck_conditions_hmc(std::string rtn_name);
INT icheck_conditions_chive(std::string rtn_name);
