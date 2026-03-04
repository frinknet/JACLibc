#ifndef CRYPTO_AES_H
#define CRYPTO_AES_H
#pragma once

#include <crypto/base.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Paste your static const __jacl_aes_sbox[256], __jacl_aes_rsbox[256],
 * __jacl_aes_rcon[11], etc., keeping the __jacl_ prefixes. */

typedef struct {
	uint32_t rk[60];
	int      nr;
} __jacl_aes_ctx;

/* Paste:
 *   __jacl_aes_sub_word
 *   __jacl_aes_rot_word
 *   __jacl_aes_key_expansion
 *   __jacl_aes_add_round_key
 *   __jacl_aes_sub_bytes / __jacl_aes_inv_sub_bytes
 *   __jacl_aes_shift_rows / __jacl_aes_inv_shift_rows
 *   __jacl_aes_mix_columns / __jacl_aes_inv_mix_columns
 *   __jacl_aes_setkey_enc / __jacl_aes_setkey_dec
 *   __jacl_aes_encrypt / __jacl_aes_decrypt
 *   __jacl_aes_ctr
 * exactly as they are in crypto.h.
 */

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_AES_H */

