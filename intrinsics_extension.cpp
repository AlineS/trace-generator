#include "intrinsics_extension.hpp"

//==============================================================================
// Intrinsics Functions
//==============================================================================

VOID write_dynamic_char(char *dyn_str, THREADID threadid) {
    TRACE_GENERATOR_DEBUG_PRINTF("write_dynamic_char()\n");
    /// If the pin-points disabled this region
    if (!is_instrumented) {
        return;
    }
    else {
        // This lock is necessary because when using a parallel program
        // the thread master may write on multiple threads
        // ex: omp_parallel_start / omp_parallel_end
        PIN_GetLock(&thread_data[threadid].dyn_lock, threadid);
            // ~ gzwrite(thread_data[threadid].gzDynamicTraceFile, dyn_str, strlen(dyn_str));
            gzwrite(thread_data[threadid].gzDynamicTraceFile, dyn_str, strlen(dyn_str));
        PIN_ReleaseLock(&thread_data[threadid].dyn_lock);
    }
};

// =====================================================================

VOID write_static_char(char *stat_str) {
    TRACE_GENERATOR_DEBUG_PRINTF("write_static_char()\n");
    // puts("write_static_char");
    // printf("static: %s\n", stat_str);
    PIN_GetLock(&lock, 1);
        gzwrite(gzStaticTraceFile, stat_str, strlen(stat_str));
    PIN_ReleaseLock(&lock);
};

// =====================================================================

VOID hmc_write_memory(ADDRINT *read, UINT32 size, UINT32 bbl, THREADID threadid) {
    TRACE_GENERATOR_DEBUG_PRINTF("hmc_write_memory()\n");

    if (thread_data[threadid].is_instrumented_bbl == false) return;     // If the pin-points disabled this region

    char mem_str[TRACE_LINE_SIZE];

    sprintf(mem_str, "%c %d %" PRIu64 " %d\n", 'R', size, (uint64_t)&read, bbl);
    gzwrite(thread_data[threadid].gzMemoryTraceFile, mem_str, strlen(mem_str));
};

// =====================================================================
//
// VOID hmc_write_memory(ADDRINT *read1, ADDRINT *read2, ADDRINT *write, UINT32 size, UINT32 bbl, THREADID threadid) {
//     TRACE_GENERATOR_DEBUG_PRINTF("hmc_write_memory()\n");
//
//     if (thread_data[threadid].is_instrumented_bbl == false) return;     // If the pin-points disabled this region
//
//     char mem_str[TRACE_LINE_SIZE];
//
//     sprintf(mem_str, "%c %d %" PRIu64 " %d\n", 'R', size, (uint64_t)&read1, bbl);
//     gzwrite(thread_data[threadid].gzMemoryTraceFile, mem_str, strlen(mem_str));
//
//     sprintf(mem_str, "%c %d %" PRIu64 " %d\n", 'R', size, (uint64_t)&read2, bbl);
//     gzwrite(thread_data[threadid].gzMemoryTraceFile, mem_str, strlen(mem_str));
//
//     sprintf(mem_str, "%c %d %" PRIu64 " %d\n", 'W', size, (uint64_t)&write, bbl);
//     gzwrite(thread_data[threadid].gzMemoryTraceFile, mem_str, strlen(mem_str));
// };
//
// =====================================================================

VOID hmc_x86_set_data_instr(data_instr *hmc_x86_data, char const *rtn_name, char const *hmc_instr_name, char const *x86_instr_name, UINT32 instr_len){
    hmc_x86_data->rtn_name = rtn_name;
    hmc_x86_data->arch_instr_name = hmc_instr_name;
    hmc_x86_data->x86_instr_name = x86_instr_name;
    hmc_x86_data->instr_len = instr_len;
};

// =====================================================================

VOID chive_x86_set_data_instr(data_instr *chive_x86_data, char const *rtn_name, char const *chive_instr_name, char const *x86_instr_name, UINT32 instr_len){
    chive_x86_data->rtn_name = rtn_name;
    chive_x86_data->arch_instr_name = chive_instr_name;
    chive_x86_data->x86_instr_name = x86_instr_name;
    chive_x86_data->instr_len = instr_len;
};

// =====================================================================

VOID initialize_intrinsics_hmc(data_instr hmc_x86_data[20]) {
    hmc_x86_set_data_instr(&hmc_x86_data[0], "_hmc128_saddimm_s", "HMC_ADD_SINGLE_128OPER", "x86_ADD_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[1], "_hmc64_incr_s", "HMC_INCR_SINGLE_64OPER", "x86_INCR_SINGLE_64OPER", 8);
    hmc_x86_set_data_instr(&hmc_x86_data[2], "_hmc64_bwrite_s", "HMC_BITWRITE_SINGLE_64OPER", "x86_BITWRITE_SINGLE_64OPER", 8);
    hmc_x86_set_data_instr(&hmc_x86_data[3], "_hmc128_bswap_s", "HMC_BITSWAP_SINGLE_128OPER", "x86_BITSWAP_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[4], "_hmc128_and_s", "HMC_AND_SINGLE_128OPER", "x86_AND_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[5], "_hmc128_nand_s", "HMC_NAND_SINGLE_128OPER", "x86_NAND_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[6], "_hmc128_nor_s", "HMC_NOR_SINGLE_128OPER", "x86_NOR_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[7], "_hmc128_or_s", "HMC_OR_SINGLE_128OPER", "x86_OR_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[8], "_hmc128_xor_s", "HMC_XOR_SINGLE_128OPER", "x86_XOR_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[9], "_hmc64_cmpswapgt_s", "HMC_CMPSWAPGT_SINGLE_64OPER", "x86_CMPSWAPGT_SINGLE_64OPER", 8);
    hmc_x86_set_data_instr(&hmc_x86_data[10], "_hmc64_cmpswaplt_s", "HMC_CMPSWAPLT_SINGLE_64OPER", "x86_CMPSWAPLT_SINGLE_64OPER", 8);
    hmc_x86_set_data_instr(&hmc_x86_data[11], "_hmc128_cmpswapz_s", "HMC_CMPSWAPZ_SINGLE_128OPER", "x86_CMPSWAPZ_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[12], "_hmc128_cmpswapgt_s", "HMC_CMPSWAPGT_SINGLE_128OPER", "x86_CMPSWAPGT_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[13], "_hmc128_cmpswaplt_s", "HMC_CMPSWAPLT_SINGLE_128OPER", "x86_CMPSWAPLT_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[14], "_hmc64_cmpswapeq_s", "HMC_CMPSWAPEQ_SINGLE_64OPER", "x86_CMPSWAPEQ_SINGLE_64OPER", 8);
    hmc_x86_set_data_instr(&hmc_x86_data[15], "_hmc64_equalto_s", "HMC_EQUALTO_SINGLE_64OPER", "x86_EQUALTO_SINGLE_64OPER", 8);
    hmc_x86_set_data_instr(&hmc_x86_data[16], "_hmc128_equalto_s", "HMC_EQUALTO_SINGLE_128OPER", "x86_EQUALTO_SINGLE_128OPER", 16);
    hmc_x86_set_data_instr(&hmc_x86_data[17], "_hmc64_cmpgteq_s", "HMC_CMPGTEQ_SINGLE_64OPER", "x86_CMPGTEQ_SINGLE_64OPER", 8);
    hmc_x86_set_data_instr(&hmc_x86_data[18], "_hmc64_cmplteq_s", "HMC_CMPLTEQ_SINGLE_64OPER", "x86_CMPLTEQ_SINGLE_64OPER", 8);
    hmc_x86_set_data_instr(&hmc_x86_data[19], "_hmc64_cmplt_s", "HMC_CMPLT_SINGLE_64OPER", "x86_CMPLT_SINGLE_64OPER", 8);
}

// =====================================================================

VOID initialize_intrinsics_chive(data_instr chive_x86_data[60]) {
    chive_x86_set_data_instr(&chive_x86_data[0], "_chive64_adds", "CHIVE_ADD_64VECTOR_32OPER", "x86_ADD_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[1], "_chive2K_adds", "CHIVE_ADD_2KVECTOR_32OPER", "x86_ADD_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[2], "_chive64_addu", "CHIVE_ADDU_64VECTOR_32OPER", "x86_ADDU_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[3], "_chive2K_addu", "CHIVE_ADDU_2KVECTOR_32OPER", "x86_ADDU_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[4], "_chive64_subs", "CHIVE_SUB_64VECTOR_32OPER", "x86_SUB_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[5], "_chive2K_subs", "CHIVE_SUB_2KVECTOR_32OPER", "x86_SUB_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[6], "_chive64_subu", "CHIVE_SUBU_64VECTOR_32OPER", "x86_SUBU_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[7], "_chive2K_subu", "CHIVE_SUBU_2KVECTOR_32OPER", "x86_SUBU_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[8], "_chive64_abs", "CHIVE_ABS_64VECTOR_32OPER", "x86_ABS_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[9], "_chive2K_abs", "CHIVE_ABS_2KVECTOR_32OPER", "x86_ABS_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[10], "_chive64_max", "CHIVE_MAX_64VECTOR_32OPER", "x86_MAX_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[11], "_chive2K_max", "CHIVE_MAX_2KVECTOR_32OPER", "x86_MAX_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[12], "_chive64_min", "CHIVE_MIN_64VECTOR_32OPER", "x86_MIN_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[13], "_chive2K_min", "CHIVE_MIN_2KVECTOR_32OPER", "x86_MIN_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[14], "_chive64_and", "CHIVE_AND_64VECTOR_32OPER", "x86_AND_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[15], "_chive2K_and", "CHIVE_AND_2KVECTOR_32OPER", "x86_AND_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[16], "_chive64_or", "CHIVE_OR_64VECTOR_32OPER", "x86_OR_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[17], "_chive2K_or", "CHIVE_OR_2KVECTOR_32OPER", "x86_OR_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[18], "_chive64_xor", "CHIVE_XOR_64VECTOR_32OPER", "x86_XOR_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[19], "_chive2K_xor", "CHIVE_XOR_2KVECTOR_32OPER", "x86_XOR_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[20], "_chive64_not", "CHIVE_NOT_64VECTOR_32OPER", "x86_NOT_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[21], "_chive2K_not", "CHIVE_NOT_2KVECTOR_32OPER", "x86_NOT_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[22], "_chive64_masks", "CHIVE_MASK_64VECTOR_32OPER", "x86_MASK_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[23], "_chive2K_masks", "CHIVE_MASK_2KVECTOR_32OPER", "x86_MASK_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[24], "_chive64_masku", "CHIVE_MASKU_64VECTOR_32OPER", "x86_MASKU_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[25], "_chive2K_masku", "CHIVE_MASKU_2KVECTOR_32OPER", "x86_MASKU_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[26], "_chive64_slts", "CHIVE_SLT_64VECTOR_32OPER", "x86_SLT_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[27], "_chive2K_slts", "CHIVE_SLT_2KVECTOR_32OPER", "x86_SLT_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[28], "_chive64_sltu", "CHIVE_SLTU_64VECTOR_32OPER", "x86_SLTU_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[29], "_chive2K_sltu", "CHIVE_SLTU_2KVECTOR_32OPER", "x86_SLTU_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[30], "_chive64_cmpeqs", "CHIVE_CEQ_64VECTOR_32OPER", "x86_CEQ_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[31], "_chive2K_cmpeqs", "CHIVE_CEQ_2KVECTOR_32OPER", "x86_CEQ_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[32], "_chive64_cmpequ", "CHIVE_CEQU_64VECTOR_32OPER", "x86_CEQU_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[33], "_chive2K_cmpequ", "CHIVE_CEQU_2KVECTOR_32OPER", "x86_CEQU_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[34], "_chive64_sll", "CHIVE_SLL_64VECTOR_32OPER", "x86_SLL_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[35], "_chive2K_sll", "CHIVE_SLL_2KVECTOR_32OPER", "x86_SLL_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[36], "_chive64_srl", "CHIVE_SRL_64VECTOR_32OPER", "x86_SRL_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[37], "_chive2K_srl", "CHIVE_SRL_2KVECTOR_32OPER", "x86_SRL_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[38], "_chive64_sra", "CHIVE_SRA_64VECTOR_32OPER", "x86_SRA_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[39], "_chive2K_sra", "CHIVE_SRA_2KVECTOR_32OPER", "x86_SRA_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[40], "_chive64_divs", "CHIVE_DIV_64VECTOR_32OPER", "x86_DIV_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[41], "_chive2K_divs", "CHIVE_DIV_2KVECTOR_32OPER", "x86_DIV_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[42], "_chive64_divu", "CHIVE_DIVU_64VECTOR_32OPER", "x86_DIVU_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[43], "_chive2K_divu", "CHIVE_DIVU_2KVECTOR_32OPER", "x86_DIVU_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[44], "_chive64_mods", "CHIVE_MOD_64VECTOR_32OPER", "x86_MOD_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[45], "_chive2K_mods", "CHIVE_MOD_2KVECTOR_32OPER", "x86_MOD_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[46], "_chive64_modu", "CHIVE_MODU_64VECTOR_32OPER", "x86_MODU_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[47], "_chive2K_modu", "CHIVE_MODU_2KVECTOR_32OPER", "x86_MODU_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[48], "_chive64_mults", "CHIVE_MULT_64VECTOR_32OPER", "x86_MULT_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[49], "_chive2K_mults", "CHIVE_MULT_2KVECTOR_32OPER", "x86_MULT_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[50], "_chive64_multu", "CHIVE_MULTU_64VECTOR_32OPER", "x86_MULTU_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[51], "_chive2K_multu", "CHIVE_MULTU_2KVECTOR_32OPER", "x86_MULTU_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[52], "_chive32_mults", "CHIVE_MULT_32VECTOR_64OPER", "x86_MULT_32VECTOR_64OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[53], "_chive1K_mults", "CHIVE_MULT_1KVECTOR_64OPER", "x86_MULT_1KVECTOR_64OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[54], "_chive32_multu", "CHIVE_MULTU_32VECTOR_64OPER", "x86_MULTU_32VECTOR_64OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[55], "_chive1K_multu", "CHIVE_MULTU_1KVECTOR_64OPER", "x86_MULTU_1KVECTOR_64OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[56], "_chive64_movs", "CHIVE_MOVS_64VECTOR_32OPER", "x86_MOVS_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[57], "_chive2K_movs", "CHIVE_MOVS_2KVECTOR_32OPER", "x86_MOVS_2KVECTOR_32OPER", 8192);
    chive_x86_set_data_instr(&chive_x86_data[58], "_chive64_movu", "CHIVE_MOVU_64VECTOR_32OPER", "x86_MOVU_64VECTOR_32OPER", 256);
    chive_x86_set_data_instr(&chive_x86_data[59], "_chive2K_movu", "CHIVE_MOVU_2KVECTOR_32OPER", "x86_MOVU_2KVECTOR_32OPER", 8192);
}

// =====================================================================

VOID initialize_intrinsics(data_instr hmc_x86_data[20], data_instr chive_x86_data[60]) {
    // HMC instructions
    initialize_intrinsics_hmc(hmc_x86_data);
    // CHIVE instructions
    initialize_intrinsics_chive(chive_x86_data);
}

// =====================================================================

INT icheck_conditions_hmc(std::string rtn_name) {
    if ((rtn_name.compare(4, cmp_name0.size(), cmp_name0.c_str()) == 0)  ||
    (rtn_name.compare(4, cmp_name1.size(), cmp_name1.c_str()) == 0)   ||
    (rtn_name.compare(4, cmp_name2.size(), cmp_name2.c_str()) == 0)   ||
    (rtn_name.compare(4, cmp_name3.size(), cmp_name3.c_str()) == 0)   ||
    (rtn_name.compare(4, cmp_name4.size(), cmp_name4.c_str()) == 0)   ||
    (rtn_name.compare(4, cmp_name5.size(), cmp_name5.c_str()) == 0)   ||
    (rtn_name.compare(4, cmp_name6.size(), cmp_name6.c_str()) == 0)   ||
    (rtn_name.compare(4, cmp_name7.size(), cmp_name7.c_str()) == 0)   ||
    (rtn_name.compare(4, cmp_name8.size(), cmp_name8.c_str()) == 0)   ||
    (rtn_name.compare(4, cmp_name9.size(), cmp_name9.c_str()) == 0)   ||
    (rtn_name.compare(4, cmp_name10.size(), cmp_name10.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name11.size(), cmp_name11.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name12.size(), cmp_name12.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name13.size(), cmp_name13.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name14.size(), cmp_name14.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name15.size(), cmp_name15.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name16.size(), cmp_name16.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name17.size(), cmp_name17.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name18.size(), cmp_name18.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name19.size(), cmp_name19.c_str()) == 0)) {
        return 1;
    }
    return 0;
}

// =====================================================================

INT icheck_conditions_chive(std::string rtn_name) {
    if ((rtn_name.compare(4, cmp_name20.size(), cmp_name20.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name21.size(), cmp_name21.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name22.size(), cmp_name22.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name23.size(), cmp_name23.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name24.size(), cmp_name24.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name25.size(), cmp_name25.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name26.size(), cmp_name26.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name27.size(), cmp_name27.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name28.size(), cmp_name28.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name29.size(), cmp_name29.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name30.size(), cmp_name30.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name31.size(), cmp_name31.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name32.size(), cmp_name32.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name33.size(), cmp_name33.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name34.size(), cmp_name34.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name35.size(), cmp_name35.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name36.size(), cmp_name36.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name37.size(), cmp_name37.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name38.size(), cmp_name38.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name39.size(), cmp_name39.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name40.size(), cmp_name40.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name41.size(), cmp_name41.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name42.size(), cmp_name42.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name43.size(), cmp_name43.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name44.size(), cmp_name44.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name45.size(), cmp_name45.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name46.size(), cmp_name46.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name47.size(), cmp_name47.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name48.size(), cmp_name48.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name49.size(), cmp_name49.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name50.size(), cmp_name50.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name51.size(), cmp_name51.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name52.size(), cmp_name52.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name53.size(), cmp_name53.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name54.size(), cmp_name54.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name55.size(), cmp_name55.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name56.size(), cmp_name56.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name57.size(), cmp_name57.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name58.size(), cmp_name58.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name59.size(), cmp_name59.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name60.size(), cmp_name60.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name61.size(), cmp_name61.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name62.size(), cmp_name62.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name63.size(), cmp_name63.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name64.size(), cmp_name64.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name65.size(), cmp_name65.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name66.size(), cmp_name66.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name67.size(), cmp_name67.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name68.size(), cmp_name68.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name69.size(), cmp_name69.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name70.size(), cmp_name70.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name71.size(), cmp_name71.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name72.size(), cmp_name72.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name73.size(), cmp_name73.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name74.size(), cmp_name74.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name75.size(), cmp_name75.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name76.size(), cmp_name76.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name77.size(), cmp_name77.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name78.size(), cmp_name78.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name79.size(), cmp_name79.c_str()) == 0)) {
        return 1;
    }
    return 0;
}

// =====================================================================

INT icheck_conditions(std::string rtn_name) {
    if ((rtn_name.size() > 4) && (icheck_conditions_hmc(rtn_name) || icheck_conditions_chive(rtn_name))) {
        return 1;
    }
    return 0;
};

// =====================================================================

VOID arch_x86_trace_instruction(RTN arch_rtn, data_instr archx_x86_data) {
    opcode_package_t NewInstruction;
    char bbl_count_str[TRACE_LINE_SIZE];
    char opcode_str[TRACE_LINE_SIZE];
    uint32_t i, j, bicount;
    int ireg, breg, rreg, wreg;
    std::string rtn_name;
    bool read_regs[MAX_REGISTER_NUMBER], write_regs[MAX_REGISTER_NUMBER];
    // Open HMC routine
    if (RTN_Valid(arch_rtn)) {
        RTN_Open(arch_rtn);
        rtn_name = RTN_Name(arch_rtn);
        sprintf(bbl_count_str, "#%s\n", archx_x86_data.rtn_name);
        write_static_char(bbl_count_str);
        count_trace++;
        sprintf(bbl_count_str, "@%u\n", count_trace);
        write_static_char(bbl_count_str);
        bicount = 0;
        char *hmc_bbl_str = new char[32];
        sprintf(hmc_bbl_str, "%u\n", count_trace);
        RTN_InsertCall(arch_rtn, IPOINT_BEFORE, (AFUNPTR)hmc_write_memory, IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_UINT32, archx_x86_data.instr_len, IARG_UINT32, count_trace, IARG_THREAD_ID, IARG_END);
        RTN_InsertCall(arch_rtn, IPOINT_BEFORE, (AFUNPTR)write_dynamic_char, IARG_PTR, hmc_bbl_str, IARG_THREAD_ID, IARG_END);

        for (i = 0; i < MAX_REGISTER_NUMBER; i++) {
            read_regs[i] = false;
            write_regs[i] = false;
        }

        // Identify read/write registers
        for(INS ins = RTN_InsHead(arch_rtn); INS_Valid(ins); ins = INS_Next(ins)) {

            // record all read registers
            for (i = 0; i < INS_MaxNumRRegs(ins); i++) {
                rreg = INS_RegR(ins, i);
                if (rreg > 0 && read_regs[rreg] == false && write_regs[rreg] == false) {
                    read_regs[rreg] = true;
                }
            }
            // record all write registers
            for (j = 0; j < INS_MaxNumWRegs(ins); j++) {
                wreg = INS_RegW(ins, j);
                if (wreg > 0 && write_regs[wreg] == false) {
                    write_regs[wreg] = true;
                }
            }

            breg = INS_MemoryBaseReg(ins);
            ireg = INS_MemoryIndexReg(ins);

            // Record base register if it is not into read and write register's lists
            if (breg > 0 && read_regs[breg] == false && write_regs[breg] == false) {
                read_regs[breg] = true;
                bicount++;
            }
            // Record index register if it is not into read and write register's lists
            if (ireg > 0 && read_regs[ireg] == false && write_regs[ireg] == false) {
                read_regs[ireg] = true;
                bicount++;
            }
        }

        // Record all read and write registers into read and write register's list
        int r_index = 0, w_index = 0;
        for (i = 0; i < MAX_REGISTER_NUMBER; i++) {
            if (read_regs[i] == true) {
                NewInstruction.read_regs[r_index++] = i;
            }
            if (write_regs[i] == true) {
                NewInstruction.write_regs[w_index++] = i;
            }
        }

        // Record opcode address
        NewInstruction.opcode_address = RTN_Address(arch_rtn);

        // Selects between x86, HMC and CHIVE simulation
        if (KnobTrace.Value().compare(0, 4, "ix86") == 0) {
            strcpy(NewInstruction.opcode_assembly, archx_x86_data.x86_instr_name);
        } else {
            strcpy(NewInstruction.opcode_assembly, archx_x86_data.arch_instr_name);
        }

        // Record opcode size
        NewInstruction.opcode_size = archx_x86_data.instr_len;

        if (icheck_conditions_chive(rtn_name)) {
            printf("%s\n", rtn_name.c_str());
            NewInstruction.opcode_operation = INSTRUCTION_OPERATION_HIVE_INT_ALU;
            // tratar os demais casos: INSTRUCTION_OPERATION_HIVE_INT_MUL, INSTRUCTION_OPERATION_HIVE_INT_DIV
        } else {
            if ((rtn_name.compare(4, cmp_name0.size(), cmp_name0.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name1.size(), cmp_name1.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name2.size(), cmp_name2.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name9.size(), cmp_name9.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name10.size(), cmp_name10.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name17.size(), cmp_name17.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name9.size(), cmp_name9.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name10.size(), cmp_name10.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name17.size(), cmp_name17.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name18.size(), cmp_name18.c_str()) == 0) ||
            (rtn_name.compare(4, cmp_name19.size(), cmp_name19.c_str()) == 0)) {
                NewInstruction.opcode_operation = INSTRUCTION_OPERATION_HMC_ROA;
            } else {
                NewInstruction.opcode_operation = INSTRUCTION_OPERATION_HMC_ROWA;
            }
        }

        NewInstruction.is_read = 1;
        NewInstruction.is_read2 = 0;
        NewInstruction.is_write = 0;
        NewInstruction.is_indirect = 0;
        NewInstruction.is_predicated = 0;
        NewInstruction.is_prefetch = 0;
        NewInstruction.opcode_to_trace_string(opcode_str);
        write_static_char(opcode_str);
        RTN_Close(arch_rtn);
    }
};

// =====================================================================

VOID synthetic_trace_generation(std::string rtn_name, data_instr hmc_x86_data[20], data_instr chive_x86_data[60], RTN rtn) {
    // If the instruction name contains "_hmc", it can be only an HMC or x86 instruction
    if (rtn_name.size() > 4 && rtn_name.compare(4, 4, "_hmc") == 0) {
        if (KnobTrace.Value().compare(0, 6, "iCHIVE") == 0) {
            ERROR_PRINTF("Cannot execute HMC instructions directly on CHIVE!\n");
            exit(1);
        } else {
            for (int n = 0; n < 20; n++) {
                std::string hmc_x86_cmp_name (hmc_x86_data[n].rtn_name);
                if ((4 < rtn_name.size()) && (4 < hmc_x86_cmp_name.size()) && rtn_name.compare(4, hmc_x86_cmp_name.size(), hmc_x86_cmp_name.c_str()) == 0){
                    arch_x86_trace_instruction(rtn, hmc_x86_data[n]);
                }
            }
        }
    }
    // If the instruction name contains "_ch", it can be only a CHIVE or x86 instruction
    if (rtn_name.size() > 4 && rtn_name.compare(4, 6, "_chive") == 0) {
        if (KnobTrace.Value().compare(0, 4, "iHMC") == 0) {
            ERROR_PRINTF("Cannot execute CHIVE instructions directly on HMC!\n");
            exit(1);
        } else {
            for (int n = 0; n < 60; n++) {
                std::string chive_x86_cmp_name (chive_x86_data[n].rtn_name);
                if ((4 < rtn_name.size()) && (4 < chive_x86_cmp_name.size()) && rtn_name.compare(4, chive_x86_cmp_name.size(), chive_x86_cmp_name.c_str()) == 0) {
                    arch_x86_trace_instruction(rtn, chive_x86_data[n]);
                }
            }
        }
    }
}
