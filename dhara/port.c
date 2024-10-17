/*
 * @Author: wangs 277668922@qq.com
 * @Date: 2024-10-14 16:13:33
 * @LastEditors: wangs 277668922@qq.com
 * @LastEditTime: 2024-10-17 09:50:34
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




static uint8_t goodBlcokMap[NUM_BLOCKS] = {BLOCK_UNKNOW};

/**
 * @description: 判断是否为坏块
 * @param {dhara_nand} *n
 * @param {dhara_block_t} b 块号
 * @return {*} 0表示好块，1表示坏块
 */
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
    return 0;
}

/**
 * @description: 标记一个坏块
 * @param {dhara_nand} *n
 * @param {dhara_block_t} b 块号
 * @return {*} 
 */
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


/**
 * @description:  块擦除
 * @param {dhara_nand} *n 
 * @param {dhara_block_t} b 块号
 * @param {dhara_error_t} *err 错误代码
 * @return {*} 0 擦除成功，-1 擦除失败
 */
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

/**
 * @description: 页写入
 * @param {dhara_nand} *n
 * @param {dhara_page_t} p 页号
 * @param {uint8_t} *data 写入的数据
 * @param {dhara_error_t} *err 错误代码
 * @return {*} 0 写入成功，-1 写入失败
 */
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

/**
 * @description: 页是否空闲
 * @param {dhara_nand} *n 
 * @param {dhara_page_t} p 页号
 * @return {*} 1 空闲，0 不空闲
 */
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

/**
 * @description: 页读取
 * @param {dhara_nand} *n
 * @param {dhara_page_t} p 页号
 * @param {size_t} offset 偏移量
 * @param {size_t} length 长度
 * @param {uint8_t} *data 数据缓存
 * @param {dhara_error_t} *err 错误代码
 * @return {*} 0 读取成功，-1 读取失败
 */
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


/**
 * @description: 页拷贝
 * @param {dhara_nand} *n
 * @param {dhara_page_t} src 源页号
 * @param {dhara_page_t} dst 目标页号
 * @param {dhara_error_t} *err 错误代码
 * @return {*} 0 拷贝成功，否则拷贝失败
 */
int dhara_nand_copy(const struct dhara_nand *n,
                    dhara_page_t src, dhara_page_t dst,
                    dhara_error_t *err)
{
    LOG_DEBUG("page copy %d to %d", src, dst);
    return NAND_CopyPageWithoutWrite(src, dst);
}