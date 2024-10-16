/*
 * @Author: wangs 277668922@qq.com
 * @Date: 2024-10-14 16:13:33
 * @LastEditors: wangs 277668922@qq.com
 * @LastEditTime: 2024-10-16 15:49:45
 * @FilePath: \LoggerTest\Library\dhara\dhara\port.c
 * @Description:
 */
#include "nand_new.h"
#include "nand.h"

#define LOG_TAG "[DHARA]"
// #define USE_LOG_ERROR
// #define USE_LOG_INFO
// #define USE_LOG_DEBUG
#include "ulog.h"

#define LOG2_PAGE_SIZE       11
#define LOG2_OOB_SIZE        6
#define LOG2_PAGES_PER_BLOCK 6
#define LOG2_BLOCK_SIZE      (LOG2_PAGE_SIZE + LOG2_PAGES_PER_BLOCK)
#define NUM_BLOCKS           1024

#define PAGE_SIZE            (1 << LOG2_PAGE_SIZE)
#define PAGES_OOB_SIZE       (1 << LOG2_OOB_SIZE)
#define PAGES_PER_BLOCK      (1 << LOG2_PAGES_PER_BLOCK)
#define BLOCK_SIZE           (1 << LOG2_BLOCK_SIZE)
#define MEM_SIZE             (NUM_BLOCKS * BLOCK_SIZE)

const struct dhara_nand my_nand = {
    .log2_page_size = LOG2_PAGE_SIZE,
    .log2_ppb       = LOG2_PAGES_PER_BLOCK,
    .num_blocks     = NUM_BLOCKS};

// 坏块标记
#define BAD_CLOCK_CONFIRM_TWICE 1 // 坏块检查两次
#define BAD_BLOCK_OFFSET        0
#define BLOCK_GOOD              0xff
#define BLOCK_BAD               0xAA
#define BLOCK_UNKNOW            0x00

// 页使用标记
#define PAGE_USED_OFFSET 1
#define PAGE_FREE        0xff
#define PAGE_USED        0x55

uint8_t nandPageBuffer[PAGE_SIZE];
static uint8_t goodBlcokMap[NUM_BLOCKS] = {BLOCK_UNKNOW};

int dhara_nand_is_bad(const struct dhara_nand *n, dhara_block_t b)
{
    //     LOG_DEBUG("check bad block %d", b);
    //     if (goodBlcokMap[b] == BLOCK_UNKNOW) {
    //         uint8_t buffer = 0;
    //         // 读取坏块标记（块的第一、二个扇区的第一字节）
    //         NAND_ReadSpare(b * PAGES_PER_BLOCK, BAD_BLOCK_OFFSET, &goodBlcokMap[b], 1);
    // #if BAD_CLOCK_CONFIRM_TWICE == 1
    //         if (goodBlcokMap[b] == BLOCK_GOOD) {
    //             // 二次确认
    //             NAND_ReadSpare(b * PAGES_PER_BLOCK + 1, BAD_BLOCK_OFFSET, &buffer, 1);
    //             if (buffer != BLOCK_GOOD) {
    //                 // 不一致，则是坏块
    //                 goodBlcokMap[b] = BLOCK_BAD;
    //             }
    //         }
    // #endif
    //     }
    //     if (goodBlcokMap[b] != BLOCK_GOOD) {
    //         LOG_DEBUG("bad block %d", b);
    //     }
    //     return (goodBlcokMap[b] != BLOCK_GOOD);
    // 0 means good block
    // 1 means bad block
    return 0;
}

void dhara_nand_mark_bad(const struct dhara_nand *n, dhara_block_t b)
{
    // LOG_DEBUG("set %d bad block", b);

    // goodBlcokMap[b] = BLOCK_BAD;
    // uint8_t buffer  = BLOCK_BAD;
    // // 写入第一块标记
    // NAND_WriteSpare(b * PAGES_PER_BLOCK, BAD_BLOCK_OFFSET, &buffer, 1);
    // // 第二块标记
    // NAND_WriteSpare(b * PAGES_PER_BLOCK, BAD_BLOCK_OFFSET, &buffer, 1);
}

int dhara_nand_erase(const struct dhara_nand *n, dhara_block_t b,
                     dhara_error_t *err)
{
    LOG_DEBUG("erase block %d", b);
    if (NAND_EraseBlock(b) == 0) return 0;
    LOG_ERROR("erase block %d error", b);
    // TODO bad block check
    // *err = DHARA_E_BAD_BLOCK;
    return -1;
}

int dhara_nand_prog(const struct dhara_nand *n, dhara_page_t p,
                    const uint8_t *data,
                    dhara_error_t *err)
{
    LOG_DEBUG("prog page %d", p);
    if (NAND_WritePage(p, 0, data, PAGE_SIZE) == 0) {
        // // 写入页使用标记
        // uint8_t buffer = PAGE_USED;
        // NAND_WriteSpare(p, PAGE_USED_OFFSET, &buffer, 1);
        return 0;
    }
    LOG_ERROR("prog page error %d", p);
    // TODO badblock 识别
    // *err = DHARA_E_BAD_BLOCK;
    return -1;
}

int dhara_nand_is_free(const struct dhara_nand *n, dhara_page_t p)
{
    uint8_t buffer = 0;
    NAND_ReadSpare(p, PAGE_USED_OFFSET, &buffer, 1);
    LOG_DEBUG("check page free %d %d ", p, buffer);
    // free返回0
    if (buffer == PAGE_FREE)
        return 1;
    return 0;
}

int dhara_nand_read(const struct dhara_nand *n, dhara_page_t p,
                    size_t offset, size_t length,
                    uint8_t *data,
                    dhara_error_t *err)
{
    LOG_DEBUG("read page %d %d %d", p, offset, length);
    char ret = NAND_ReadPage(p, offset, data, length);
    if (ret == 0) return 0;
    // if (ret == NAND_RESPONSE_ERROR_ECC2BIT) {
    //     // ecc error
    //     *err = DHARA_E_ECC;
    // }
    LOG_ERROR("read page error %d", p);
    return -1;
}

int dhara_nand_copy(const struct dhara_nand *n,
                    dhara_page_t src, dhara_page_t dst,
                    dhara_error_t *err)
{
    LOG_DEBUG("page copy %d to %d", src, dst);
    return NAND_CopyPageWithoutWrite(src, dst);
}