/** @file   collections.h
 *  @brief  コレクションに関する機能を提供する.
 *
 *  コレクション (リスト, スタック, キュー, ツリー) を提供する.
 *
 *  @author t-kenji <protect.2501@gmail.com>
 *  @date   2018-03-18 新規作成.
 *  @copyright  Copyright (c) 2018 t-kenji
 *
 *  This code is licensed under the MIT License.
 */
#ifndef __CTOMAT_COLLECTIONS_H__
#define __CTOMAT_COLLECTIONS_H__

#include <stdbool.h>
#include <unistd.h>

/** @defgroup cat_collections Collections
 *  汎用コレクションを提供するモジュール.
 */

/** @addtogroup cat_pool メモリプール.
 *  コレクションのメモリプール機能を提供するモジュール.
 *  @ingroup cat_collections
 *  @{
 */

/**
 *  メモリプール型.
 */
typedef struct {} *POOL;

/**
 *  メモリプールオブジェクトを初期化する.
 *
 *  @par    使用例
 *          @code
 *          POOL pool = pool_init(sizeof(int), 100);
 *          int *data = (int *)pool_alloc(pool);
 *          // do something.
 *          pool_free(pool, data);
 *          pool_release(pool);
 *          @endcode
 */
POOL pool_init(size_t data_bytes, size_t capacity);

/**
 *  メモリプールオプジェクトを解放する.
 */
void pool_release(POOL pool);

/**
 *  メモリ要素をすべて消去する.
 */
int pool_clear(POOL pool);

/**
 *  メモリ要素を取得する.
 */
void *pool_alloc(POOL pool);

/**
 *  メモリ要素を返却する.
 */
void pool_free(POOL pool, void *ptr);

/**
 *  メモリプールのデータ部サイズを取得する.
 */
ssize_t pool_data_bytes(POOL pool);

/**
 *  メモリプールの容量を取得する.
 */
ssize_t pool_capacity(POOL pool);

/**
 *  メモリプールの空き容量を取得する.
 */
ssize_t pool_freeable(POOL pool);

bool pool_contains(POOL pool, void *ptr);

/** @} */

/** @addtogroup cat_iter 反復子.
 *  コレクションの反復機能を提供するモジュール.
 *  @ingroup cat_collections
 *  @{
 */

/**
 *  汎用反復子型
 */
typedef struct iterator {
    void *object;            /**< 反復対象のオブジェクト. */
    void *(*next)(void *);   /**< 次要素取得ハンドラ. */
    void *(*data)(void *);   /**< オブジェクトのデータ取得ハンドラ. */
    void (*release)(void *); /**< 反復子の解放ハンドラ. */
} ITER;

/**
 *  反復子を解放する.
 */
void iter_release(ITER iter);

bool iter_is_end(ITER iter);

/**
 *  反復子の次を取得する.
 */
ITER iter_next(ITER iter);

/**
 *  反復子のデータ部を取得する.
 */
void *iter_data(ITER iter);

/** @} */

/** @addtogroup cat_list List 構造
 *  List 構造を提供するモジュール.
 *  @ingroup cat_collections
 *  @{
 */

/**
 *  汎用リスト型.
 */
typedef struct {} *LIST;

/**
 *  リストオブジェクトを初期化する.
 *
 *  @par    使用例
 *          @code
 *          LIST list = list_init(sizeof(int), 100);
 *          int data;
 *          data = 1;
 *          list_push(list, &data);
 *          data = 2;
 *          list_push(list, &data);
 *          for (ITER iter = list_iter(list);
 *               iter != NULL;
 *               iter = iter_next(iter)) {
 *              data = *(int *)iter_data(iter);
 *              // do something.
 *          }
 *          list_release(list);
 *          @endcode
 */
LIST list_init(size_t data_bytes, size_t capacity);

/**
 *  リストオブジェクトを解放する.
 */
void list_release(LIST list);

/**
 *  リスト要素をすべて消去する.
 */
int list_clear(LIST list);

/**
 *  要素をリストに挿入する.
 */
void *list_insert(LIST list, int index, void *data);

int list_get(LIST list, int index, void *data);

/**
 *  要素をリストの末尾に追加する.
 */
void *list_push(LIST list, void *data);

ssize_t list_pop(LIST list, void *data);

void *list_unshift(LIST list, void *data);

ssize_t list_shift(LIST list, void *data);



/**
 *  要素をリストから削除する.
 */
int list_remove(LIST list, ITER iter);

/**
 *  リストのデータ部のサイズを取得する.
 */
ssize_t list_data_bytes(LIST list);

/**
 *  リストの長さを取得する.
 */
ssize_t list_count(LIST list);

/**
 *  リストの反復子を取得する.
 */
ITER list_iter(LIST list);

/**
 *  リストを配列に変換する.
 */
int list_to_array(LIST list, void **array, size_t *count);

/** @} */

/** @addtogroup cat_stack Stack 構造
 *  Stack 構造を提供するモジュール.
 *  @ingroup cat_collections
 *  @{
 */

/**
 *  汎用スタック型.
 */
typedef struct {} *STACK;

/**
 *  スタックオブジェクトを初期化する.
 *
 *  @par    使用例
 *          @code
 *          STACK stack = stack_init(sizeof(int), 100);
 *          int data;
 *          data = 1;
 *          stack_push(stack, &data);
 *          data = 2;
 *          stack_push(stack, &data);
 *          stack_pop(stack, &data);
 *          // do something.
 *          stack_pop(stack, &data);
 *          // do something.
 *          stack_release(stack);
 *          @endcode
 */
STACK stack_init(size_t data_bytes, size_t capacity);

/**
 *  スタックオブジェクトを解放する.
 */
void stack_release(STACK stack);

/**
 *  スタック要素をすべて消去する.
 */
int stack_clear(STACK stack);

/**
 *  要素をスタックに積む.
 */
void *stack_push(STACK stack, void *data);

/**
 *  スタックから要素を取り出す.
 */
ssize_t stack_pop(STACK stack, void *data);

/**
 *  スタックから要素を削除せずに返す.
 */
int stack_peek(STACK stack, void *data);

/**
 *  スタックの深さを取得する.
 */
ssize_t stack_count(STACK stack);

/**
 *  スタックの反復子を取得する.
 *
 *  @par    使用例
 *          @code
 *          for (ITER iter = stack_iter(stack);
 *               iter != NULL;
 *               iter = iter_next(iter)) {
 *              void *data = iter_data(iter);
 *          }
 *          @endcode
 */
ITER stack_iter(STACK stack);

/** @} */

/** @addtogroup cat_queue Queue 構造
 *  Queue 構造を提供するモジュール.
 *  @ingroup cat_collections
 *  @{
 */

/**
 *  汎用キュー型.
 */
typedef struct {} *QUEUE;

/**
 *  キューオブジェクトを初期化する.
 *
 *  @par    使用例
 *          @code
 *          QUEUE que = queue_init(sizeof(int), 100);
 *          int data;
 *          data = 1;
 *          queue_enq(que, &data);
 *          data = 2;
 *          queue_enq(que, &data);
 *          queue_deq(que, &data);
 *          // do something.
 *          queue_deq(que, &data);
 *          // do something.
 *          queue_release(que);
 *          @endcode
 */
QUEUE queue_init(size_t data_bytes, size_t capacity);

/**
 *  キューオブジェクトを解放する.
 */
void queue_release(QUEUE que);

/**
 *  キュー要素をすべて消去する.
 */
int queue_clear(QUEUE que);

/**
 *  要素をキューに積める.
 */
void *queue_enq(QUEUE que, void *data);

/**
 *  キューから要素を取り出す.
 */
int queue_deq(QUEUE que, void *data);

/**
 *  キューの長さを取得する.
 */
ssize_t queue_count(QUEUE que);

/**
 *  キューの反復子を取得する.
 *
 *  @par    使用例
 *          @code
 *          for (ITER iter = queue_iter(que);
 *               iter != NULL;
 *               iter = iter_next(iter)) {
 *              void *data = iter_data(iter);
 *          }
 *          @endcode
 */
ITER queue_iter(QUEUE que);

/**
 *  キューを配列に変換する.
 */
int queue_to_array(QUEUE que, void **array, size_t *count);

/** @} */

/** @addtogroup cat_set Set 構造
 *  Set 構造を提供するモジュール.
 *  @ingroup cat_collections
 *  @{
 */

/**
 *  汎用セット型.
 */
typedef struct {} *SET;

/**
 *  セットオブジェクトを初期化する.
 *
 *  @par    使用例
 *          @code
 *          SET set = set_init(sizeof(int), 100);
 *          int data;
 *          data = 1;
 *          set_add(set, &data);
 *          data = 2;
 *          set_add(set, &data);
 *          for (ITER iter = set_iter(set);
 *               iter != NULL;
 *               iter = iter_next(iter)) {
 *              data = *(int *)iter_data(iter);
 *              // do something.
 *          }
 *          set_release(set);
 *          @endcode
 */
SET set_init(size_t data_bytes, size_t capacity);

/**
 *  セットオブジェクトを解放する.
 */
void set_release(SET set);

/**
 *  セット要素をすべて消去する.
 */
int set_clear(SET set);

/**
 *  要素をセットに追加する.
 */
void *set_add(SET set, void *data);

/**
 *  セットの長さを取得する.
 */
ssize_t set_count(SET set);

/**
 *  セットの反復子を取得する.
 */
ITER set_iter(SET set);

/** @} */

/** @addtogroup cat_ntree N-ary Tree 構造
 *  N-ary Tree 構造を提供するモジュール.
 *  @ingroup cat_collections
 *  @{
 */

/**
 *  汎用 N-ary ツリー型.
 */
typedef struct {} *NTREE;

/**
 *  汎用 N-ary ツリーノード型.
 */
typedef struct {} *NTREE_NODE;

/**
 *  N-ary ツリーオブジェクトを初期化する.
 *
 *  @par    使用例
 *          @code
 *          @endcode
 */
NTREE ntree_init(size_t data_bytes, size_t capacity);

/**
 *  N-ary ツリーオブジェクトを解放する.
 */
void ntree_release(NTREE tree);

/**
 *  N-ary ツリー要素をすべて消去する.
 */
int ntree_clear(NTREE tree);

/**
 *  N-ary 要素をツリーに挿入する.
 */
NTREE_NODE ntree_insert(NTREE tree, void *data);

NTREE_NODE ntree_insert_at(NTREE tree, NTREE_NODE parent, void *data);

int ntree_remove(NTREE tree, NTREE_NODE node);

void *ntree_data(NTREE_NODE node);

/**
 *  N-ary ツリーの要素の数を取得する.
 */
ssize_t ntree_count(NTREE tree);

/**
 *  N-ary ツリーの反復子を取得する.
 */
ITER ntree_iter(NTREE tree);

ITER ntree_children_iter(NTREE_NODE node);

/**
 *  反復子の N-ary ツリーでのネストの位置を取得する.
 */
int ntree_iter_age(ITER iter);

/** @} */

#endif /* __CTOMAT_COLLECTIONS_H__ */
