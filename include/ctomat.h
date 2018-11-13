/** @file       ctomat.h
 *  @brief      C library for encoding, decoding TOMl dATa.
 *
 *  @author     t-kenji <protect.2501@gmail.com>
 *  @date       2018-11-14 create new.
 *  @copyright  Copyright © 2018 t-kenji
 *
 *  This code is licensed under the MIT License.
 */
#ifndef __CTOMAT_H__
#define __CTOMAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct toml *toml_t;

toml_t toml_create(void);
int toml_delete(toml_t object, bool forced);

toml_t toml_object_get(toml_t object, const char *key);
const char *toml_string_value(toml_t string);
int toml_string_set(toml_t string, const char *value);

toml_t toml_load(const char *pathname);
toml_t toml_load_from_memory(const char *buf, size_t length);
int toml_save(toml_t object, const char *pathname);
int toml_save_to_memory(toml_t object, char *buf, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* __CTOMAT_H__ */
