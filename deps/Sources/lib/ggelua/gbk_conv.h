/* gbk_conv.h — 自包含 UTF-8 ↔ GBK 编码转换 (自动生成, 勿手动编辑)
 * 
 * 替代系统 iconv，为 iOS/Android 提供 GBK 编码支持。
 * 码表从 Windows CP936 提取，覆盖完整 GBK 字符集。
 */
#ifndef GBK_CONV_H
#define GBK_CONV_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* UTF-8 → GBK  转换。返回写入 out 的字节数，失败返回 0。
 * out_size 必须足够大（最坏情况与 in_len 相同）。 */
size_t utf8_to_gbk(const char *in, size_t in_len, char *out, size_t out_size);

/* GBK → UTF-8  转换。返回写入 out 的字节数，失败返回 0。
 * out_size 最坏情况为 in_len * 3/2 + 1。 */
size_t gbk_to_utf8(const char *in, size_t in_len, char *out, size_t out_size);

#ifdef __cplusplus
}
#endif

#endif /* GBK_CONV_H */
