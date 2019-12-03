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

VOID arch_x86_set_data_instr(data_instr *arch_x86_data, char const *rtn_name, char const *hmc_instr_name, char const *x86_instr_name, UINT32 instr_len){
    arch_x86_data->rtn_name = rtn_name;
    arch_x86_data->arch_instr_name = hmc_instr_name;
    arch_x86_data->x86_instr_name = x86_instr_name;
    arch_x86_data->instr_len = instr_len;
};

// =====================================================================

VOID initialize_intrinsics_hmc(data_instr hmc_x86_data[20]) {
    arch_x86_set_data_instr(&hmc_x86_data[0], "_hmc128_saddimm_s", "HMC_ADD_SINGLE_128OPER", "x86_ADD_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[1], "_hmc64_incr_s", "HMC_INCR_SINGLE_64OPER", "x86_INCR_SINGLE_64OPER", 8);
    arch_x86_set_data_instr(&hmc_x86_data[2], "_hmc64_bwrite_s", "HMC_BITWRITE_SINGLE_64OPER", "x86_BITWRITE_SINGLE_64OPER", 8);
    arch_x86_set_data_instr(&hmc_x86_data[3], "_hmc128_bswap_s", "HMC_BITSWAP_SINGLE_128OPER", "x86_BITSWAP_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[4], "_hmc128_and_s", "HMC_AND_SINGLE_128OPER", "x86_AND_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[5], "_hmc128_nand_s", "HMC_NAND_SINGLE_128OPER", "x86_NAND_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[6], "_hmc128_nor_s", "HMC_NOR_SINGLE_128OPER", "x86_NOR_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[7], "_hmc128_or_s", "HMC_OR_SINGLE_128OPER", "x86_OR_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[8], "_hmc128_xor_s", "HMC_XOR_SINGLE_128OPER", "x86_XOR_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[9], "_hmc64_cmpswapgt_s", "HMC_CMPSWAPGT_SINGLE_64OPER", "x86_CMPSWAPGT_SINGLE_64OPER", 8);
    arch_x86_set_data_instr(&hmc_x86_data[10], "_hmc64_cmpswaplt_s", "HMC_CMPSWAPLT_SINGLE_64OPER", "x86_CMPSWAPLT_SINGLE_64OPER", 8);
    arch_x86_set_data_instr(&hmc_x86_data[11], "_hmc128_cmpswapz_s", "HMC_CMPSWAPZ_SINGLE_128OPER", "x86_CMPSWAPZ_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[12], "_hmc128_cmpswapgt_s", "HMC_CMPSWAPGT_SINGLE_128OPER", "x86_CMPSWAPGT_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[13], "_hmc128_cmpswaplt_s", "HMC_CMPSWAPLT_SINGLE_128OPER", "x86_CMPSWAPLT_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[14], "_hmc64_cmpswapeq_s", "HMC_CMPSWAPEQ_SINGLE_64OPER", "x86_CMPSWAPEQ_SINGLE_64OPER", 8);
    arch_x86_set_data_instr(&hmc_x86_data[15], "_hmc64_equalto_s", "HMC_EQUALTO_SINGLE_64OPER", "x86_EQUALTO_SINGLE_64OPER", 8);
    arch_x86_set_data_instr(&hmc_x86_data[16], "_hmc128_equalto_s", "HMC_EQUALTO_SINGLE_128OPER", "x86_EQUALTO_SINGLE_128OPER", 16);
    arch_x86_set_data_instr(&hmc_x86_data[17], "_hmc64_cmpgteq_s", "HMC_CMPGTEQ_SINGLE_64OPER", "x86_CMPGTEQ_SINGLE_64OPER", 8);
    arch_x86_set_data_instr(&hmc_x86_data[18], "_hmc64_cmplteq_s", "HMC_CMPLTEQ_SINGLE_64OPER", "x86_CMPLTEQ_SINGLE_64OPER", 8);
    arch_x86_set_data_instr(&hmc_x86_data[19], "_hmc64_cmplt_s", "HMC_CMPLT_SINGLE_64OPER", "x86_CMPLT_SINGLE_64OPER", 8);
}

// =====================================================================

VOID initialize_intrinsics_vima(data_instr vim_x86_data[64]) {
    arch_x86_set_data_instr(&vim_x86_data[0], "_vim64_iadds", "VIMA_IADDS_64VECTOR_32OPER", "x86_IADDS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[1], "_vim2K_iadds", "VIMA_IADDS_2KVECTOR_32OPER", "x86_IADDS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[2], "_vim64_iaddu", "VIMA_IADDU_64VECTOR_32OPER", "x86_IADDU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[3], "_vim2K_iaddu", "VIMA_IADDU_2KVECTOR_32OPER", "x86_IADDU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[4], "_vim64_isubs", "VIMA_ISUBS_64VECTOR_32OPER", "x86_ISUBS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[5], "_vim2K_isubs", "VIMA_ISUBS_2KVECTOR_32OPER", "x86_ISUBS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[6], "_vim64_isubu", "VIMA_ISUBU_64VECTOR_32OPER", "x86_ISUBU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[7], "_vim2K_isubu", "VIMA_ISUBU_2KVECTOR_32OPER", "x86_ISUBU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[8], "_vim64_iabss", "VIMA_IABSS_64VECTOR_32OPER", "x86_IABSS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[9], "_vim2K_iabss", "VIMA_IABSS_2KVECTOR_32OPER", "x86_IABSS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[10], "_vim64_imaxs", "VIMA_IMAXS_64VECTOR_32OPER", "x86_IMAXS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[11], "_vim2K_imaxs", "VIMA_IMAXS_2KVECTOR_32OPER", "x86_IMAXS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[12], "_vim64_imins", "VIMA_IMINS_64VECTOR_32OPER", "x86_IMINS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[13], "_vim2K_imins", "VIMA_IMINS_2KVECTOR_32OPER", "x86_IMINS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[14], "_vim64_iandu", "VIMA_IANDU_64VECTOR_32OPER", "x86_IANDU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[15], "_vim2K_iandu", "VIMA_IANDU_2KVECTOR_32OPER", "x86_IANDU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[16], "_vim64_iorun", "VIMA_IORUN_64VECTOR_32OPER", "x86_IORUN_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[17], "_vim2K_iorun", "VIMA_IORUN_2KVECTOR_32OPER", "x86_IORUN_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[18], "_vim64_ixoru", "VIMA_IXORU_64VECTOR_32OPER", "x86_IXORU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[19], "_vim2K_ixoru", "VIMA_IXORU_2KVECTOR_32OPER", "x86_IXORU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[20], "_vim64_inots", "VIMA_INOTS_64VECTOR_32OPER", "x86_INOTS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[21], "_vim2K_inots", "VIMA_INOTS_2KVECTOR_32OPER", "x86_INOTS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[22], "_vim64_imsks", "VIMA_IMSKS_64VECTOR_32OPER", "x86_IMSKS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[23], "_vim2K_imsks", "VIMA_IMSKS_2KVECTOR_32OPER", "x86_IMSKS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[24], "_vim64_imsku", "VIMA_IMSKU_64VECTOR_32OPER", "x86_IMSKU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[25], "_vim2K_imsku", "VIMA_IMSKU_2KVECTOR_32OPER", "x86_IMSKU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[26], "_vim64_islts", "VIMA_ISLTS_64VECTOR_32OPER", "x86_ISLTS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[27], "_vim2K_islts", "VIMA_ISLTS_2KVECTOR_32OPER", "x86_ISLTS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[28], "_vim64_isltu", "VIMA_ISLTU_64VECTOR_32OPER", "x86_ISLTU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[29], "_vim2K_isltu", "VIMA_ISLTU_2KVECTOR_32OPER", "x86_ISLTU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[30], "_vim64_icmqs", "VIMA_ICMQS_64VECTOR_32OPER", "x86_ICMQS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[31], "_vim2K_icmqs", "VIMA_ICMQS_2KVECTOR_32OPER", "x86_ICMQS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[32], "_vim64_icmqu", "VIMA_ICMQU_64VECTOR_32OPER", "x86_ICMQU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[33], "_vim2K_icmqu", "VIMA_ICMQU_2KVECTOR_32OPER", "x86_ICMQU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[34], "_vim64_isllu", "VIMA_ISLLU_64VECTOR_32OPER", "x86_ISLLU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[35], "_vim2K_isllu", "VIMA_ISLLU_2KVECTOR_32OPER", "x86_ISLLU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[36], "_vim64_isrlu", "VIMA_ISRLU_64VECTOR_32OPER", "x86_ISRLU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[37], "_vim2K_isrlu", "VIMA_ISRLU_2KVECTOR_32OPER", "x86_ISRLU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[38], "_vim64_isras", "VIMA_ISRAS_64VECTOR_32OPER", "x86_ISRAS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[39], "_vim2K_isras", "VIMA_ISRAS_2KVECTOR_32OPER", "x86_ISRAS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[40], "_vim64_idivs", "VIMA_IDIVS_64VECTOR_32OPER", "x86_IDIVS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[41], "_vim2K_idivs", "VIMA_IDIVS_2KVECTOR_32OPER", "x86_IDIVS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[42], "_vim64_idivu", "VIMA_IDIVU_64VECTOR_32OPER", "x86_IDIVU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[43], "_vim2K_idivu", "VIMA_IDIVU_2KVECTOR_32OPER", "x86_IDIVU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[44], "_vim64_imods", "VIMA_IMODS_64VECTOR_32OPER", "x86_IMODS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[45], "_vim2K_imods", "VIMA_IMODS_2KVECTOR_32OPER", "x86_IMODS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[46], "_vim64_imodu", "VIMA_IMODU_64VECTOR_32OPER", "x86_IMODU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[47], "_vim2K_imodu", "VIMA_IMODU_2KVECTOR_32OPER", "x86_IMODU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[48], "_vim64_imuls", "VIMA_IMULTS_64VECTOR_32OPER", "x86_IMULTS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[49], "_vim2K_imuls", "VIMA_IMULTS_2KVECTOR_32OPER", "x86_IMULTS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[50], "_vim64_imulu", "VIMA_IMULTU_64VECTOR_32OPER", "x86_IMULTU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[51], "_vim2K_imulu", "VIMA_IMULTU_2KVECTOR_32OPER", "x86_IMULTU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[52], "_vim32_imuls", "VIMA_IMULTS_32VECTOR_64OPER", "x86_IMULTS_32VECTOR_64OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[53], "_vim1K_imuls", "VIMA_IMULTS_1KVECTOR_64OPER", "x86_IMULTS_1KVECTOR_64OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[54], "_vim32_imulu", "VIMA_IMULTU_32VECTOR_64OPER", "x86_IMULTU_32VECTOR_64OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[55], "_vim1K_imulu", "VIMA_IMULTU_1KVECTOR_64OPER", "x86_IMULTU_1KVECTOR_64OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[56], "_vim64_imovs", "VIMA_IMOVS_64VECTOR_32OPER", "x86_IMOVS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[57], "_vim2K_imovs", "VIMA_IMOVS_2KVECTOR_32OPER", "x86_IMOVS_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[58], "_vim64_imovu", "VIMA_IMOVU_64VECTOR_32OPER", "x86_IMOVU_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[59], "_vim2K_imovu", "VIMA_IMOVU_2KVECTOR_32OPER", "x86_IMOVU_2KVECTOR_32OPER", 8192);
    arch_x86_set_data_instr(&vim_x86_data[60], "_vim64_fmovs", "VIMA_FMOVS_64VECTOR_32OPER", "x86_FMOVS_2KVECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[61], "_vim64_fsubs", "VIMA_FSUBS_64VECTOR_32OPER", "x86_FSUBS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[62], "_vim64_fmuls", "VIMA_FMULS_64VECTOR_32OPER", "x86_FMULS_64VECTOR_32OPER", 256);
    arch_x86_set_data_instr(&vim_x86_data[63], "_vim64_fcsum", "VIMA_FCSUM_64VECTOR_32OPER", "x86_FCSUM_64VECTOR_32OPER", 256);
}

// =====================================================================

VOID initialize_intrinsics_mips(data_instr mps_x86_data[28]) {
    arch_x86_set_data_instr(&mps_x86_data[0], "_mps32_add", "MIPS_ADD32_OPER", "x86_ADD32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[1], "_mps32_addu", "MIPS_ADDU32_OPER", "x86_ADDU32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[2], "_mps32_sub", "MIPS_SUB32_OPER", "x86_SUB32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[3], "_mps32_subu", "MIPS_SUBU32_OPER", "x86_SUBU32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[4], "_mps32_addi", "MIPS_ADDI32_OPER", "x86_ADDI32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[5], "_mps32_addiu", "MIPS_ADDIU32_OPER", "x86_ADDIU32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[6], "_mps32_and", "MIPS_AND32_OPER", "x86_AND32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[7], "_mps32_nor", "MIPS_NOR32_OPER", "x86_NOR32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[8], "_mps32_or", "MIPS_OR32_OPER", "x86_OR32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[9], "_mps32_xor", "MIPS_XOR32_OPER", "x86_XOR32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[10], "_mps32_andi", "MIPS_ANDI32_OPER", "x86_ANDI32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[11], "_mps32_ori", "MIPS_ORI32_OPER", "x86_ORI32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[12], "_mps32_xori", "MIPS_XORI32_OPER", "x86_XORI32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[13], "_mps32_slt", "MIPS_SLT32_OPER", "x86_SLT32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[14], "_mps32_sltu", "MIPS_SLTU32_OPER", "x86_SLTU32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[15], "_mps32_slti", "MIPS_SLTI32_OPER", "x86_SLTI32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[16], "_mps32_sltiu", "MIPS_SLTIU32_OPER", "x86_SLTIU32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[17], "_mps32_sll", "MIPS_SLL32_OPER", "x86_SLL32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[18], "_mps32_srl", "MIPS_SRL32_OPER", "x86_SRL32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[19], "_mps32_sra", "MIPS_SRA32_OPER", "x86_SRA32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[20], "_mps32_div", "MIPS_DIV32_OPER", "x86_DIV32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[21], "_mps32_divu", "MIPS_DIVU32_OPER", "x86_DIVU32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[22], "_mps32_mod", "MIPS_MOD32_OPER", "x86_MOD32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[23], "_mps32_modu", "MIPS_MODU32_OPER", "x86_IMODU32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[24], "_mps32_mult", "MIPS_MULT32_OPER", "x86_MULT32_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[25], "_mps32_multu", "MIPS_MULTU32_OPER", "x86_IMSKU_2KVECTOR_32OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[26], "_mps64_mult", "MIPS_MULT64_OPER", "x86_MULT64_OPER", 32);
    arch_x86_set_data_instr(&mps_x86_data[27], "_mps64_multu", "MIPS_MULTU64_OPER", "x86_MULTU64_OPER", 32);
}

// =====================================================================

VOID initialize_intrinsics(data_instr hmc_x86_data[20], data_instr vim_x86_data[64], data_instr mps_x86_data[28]) {
    // HMC instructions
    initialize_intrinsics_hmc(hmc_x86_data);
    // VIMA instructions
    initialize_intrinsics_vima(vim_x86_data);
    // MIPS instructions
    initialize_intrinsics_mips(mps_x86_data);
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

INT icheck_conditions_vima(std::string rtn_name) {
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
    (rtn_name.compare(4, cmp_name79.size(), cmp_name79.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name80.size(), cmp_name80.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name81.size(), cmp_name81.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name82.size(), cmp_name82.c_str()) == 0) ||
    (rtn_name.compare(4, cmp_name83.size(), cmp_name83.c_str()) == 0)) {
        return 1;
    }
    return 0;
}

// =====================================================================

INT icheck_conditions_mips(std::string rtn_name) {
    if ((rtn_name.compare(4, cmp_name84.size(), cmp_name84.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name85.size(), cmp_name85.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name86.size(), cmp_name86.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name87.size(), cmp_name87.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name88.size(), cmp_name88.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name89.size(), cmp_name89.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name90.size(), cmp_name90.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name91.size(), cmp_name91.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name92.size(), cmp_name92.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name93.size(), cmp_name93.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name94.size(), cmp_name94.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name95.size(), cmp_name95.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name96.size(), cmp_name96.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name97.size(), cmp_name97.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name98.size(), cmp_name98.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name99.size(), cmp_name99.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name100.size(), cmp_name100.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name101.size(), cmp_name101.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name102.size(), cmp_name102.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name103.size(), cmp_name103.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name104.size(), cmp_name104.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name105.size(), cmp_name105.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name106.size(), cmp_name106.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name107.size(), cmp_name107.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name108.size(), cmp_name108.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name109.size(), cmp_name109.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name110.size(), cmp_name110.c_str()) == 0) ||
        (rtn_name.compare(4, cmp_name111.size(), cmp_name111.c_str()) == 0) ) {
        return 1;
    }
    return 0;
}
// =====================================================================

INT icheck_conditions(std::string rtn_name) {
    if ((rtn_name.size() > 4) && (icheck_conditions_hmc(rtn_name) || icheck_conditions_vima(rtn_name) || icheck_conditions_mips(rtn_name))) {
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

        // Selects between x86, HMC and VIMA simulation
        if (KnobTrace.Value().compare(0, 4, "ix86") == 0) {
            strcpy(NewInstruction.opcode_assembly, archx_x86_data.x86_instr_name);
        } else {
            strcpy(NewInstruction.opcode_assembly, archx_x86_data.arch_instr_name);
        }

        // Record opcode size
        NewInstruction.opcode_size = archx_x86_data.instr_len;

        if (icheck_conditions_vima(rtn_name)) {
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

VOID specific_trace_generation(std::string rtn_name, const char *arch_name, int n_instr, data_instr *arch_x86_data, RTN rtn) {
    if (KnobTrace.Value().compare(0, 4, arch_name) == 0 || KnobTrace.Value().compare(0, 4, "ix86") == 0) {
        for (int n = 0; n < n_instr; n++) {
            std::string arch_x86_cmp_name(arch_x86_data[n].rtn_name);
            if ((4 < rtn_name.size()) && (4 < arch_x86_cmp_name.size()) && rtn_name.compare(4, arch_x86_cmp_name.size(), arch_x86_cmp_name.c_str()) == 0) {
                arch_x86_trace_instruction(rtn, arch_x86_data[n]);
            }
        } 
    } else {
        ERROR_PRINTF("Cannot execute %s instructions on different architecture!\n", arch_name);
        exit(1);
    }
}

VOID synthetic_trace_generation(std::string rtn_name, data_instr hmc_x86_data[20], data_instr vim_x86_data[64], data_instr mps_x86_data[28], RTN rtn) {
    // If the instruction name contains "_hmc", it can be only an HMC or x86 instruction
    if (rtn_name.size() > 4) {
        if (rtn_name.compare(4, 4, "_hmc") == 0) {
            specific_trace_generation(rtn_name, "iHMC", 20, hmc_x86_data, rtn);
        } else if (rtn_name.compare(4, 4, "_vim") == 0) {
            specific_trace_generation(rtn_name, "iVIM", 64, vim_x86_data, rtn);
        } else if (rtn_name.compare(4, 4, "_mps") == 0) {
            specific_trace_generation(rtn_name, "iMPS", 64, mps_x86_data, rtn);
        }
    }
}
