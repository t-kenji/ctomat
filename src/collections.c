/** @file   collections.c
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "debug.h"
#include "collections.h"

/**
 *  メモリプールノード構造体.
 */
struct pool_node {
    struct pool_node *next; /**< 次要素へのポインタ. */
};

/**
 *  メモリプール構造体の初期化子.
 */
#define POOL_NODE_INITIALIZER \
    (struct pool_node){       \
        .next = NULL,         \
    }

/**
 *  メモリプール構造体.
 */
struct pool {
    void *mem;              /**< メモリプールで使用するメモリ領域. */
    size_t data_bytes;      /**< データ部のサイズ. */
    size_t capacity;        /**< メモリプールの容量. (要素数) */
    size_t freeable;        /**< 取得可能なメモリ要素数. */
    struct pool_node *root; /**< 取得可能なノードのリスト. */
};

/**
 *  メモリプール構造体の初期化子.
 */
#define POOL_INITIALIZER(m, b, c) \
    (struct pool){                \
        .mem = (m),               \
        .data_bytes = (b),        \
        .capacity = (c),          \
        .freeable = 0,            \
        .root = NULL,             \
    }

#define max(a, b) (((a) > (b)) ? (a) : (b))

static inline void internal_pool_push(struct pool *self,
                                      struct pool_node *node)
{
    node->next = self->root;
    self->root = node;
    ++self->freeable;
}

static inline struct pool_node *internal_pool_pop(struct pool *self)
{
    struct pool_node *node = self->root;

    if (node == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    self->root = node->next;
    --self->freeable;

    return node;
}

static inline void internal_pool_setup(struct pool *self,
                                       void *mem,
                                       size_t data_bytes,
                                       size_t capacity)
{
    size_t node_bytes = max(data_bytes, sizeof(struct pool_node));
    uintptr_t head = (uintptr_t)mem;

    *self = POOL_INITIALIZER(mem, data_bytes, capacity);
    for (size_t i = 0; i < self->capacity; ++i) {
        struct pool_node *node = (struct pool_node *)(head + (node_bytes * i));
        internal_pool_push(self, node);
    }
}

/**
 *  @details    指定の容量を備えた, POOL:: オブジェクトを確保および
 *              初期化する.
 *
 *  @param      [in]    data_bytes  データ部のサイズ.
 *  @param      [in]    capacity    プールの容量. (要素数)
 *  @return     成功時はメモリプールオブジェクトが返る.
 *              失敗時は NULL が返り, errno が適切に設定される.
 */
POOL pool_init(size_t data_bytes, size_t capacity)
{
    struct pool *self;
    size_t node_bytes;
    void *mem;

    if ((data_bytes == 0) || (capacity == 0)) {
        errno = EINVAL;
        return NULL;
    }

    self = malloc(sizeof(*self));
    node_bytes = max(data_bytes, sizeof(struct pool_node));
    mem = calloc(capacity, node_bytes);
    if ((self == NULL) || (mem == NULL)) {
        free(mem);
        free(self);
        return NULL;
    }

    internal_pool_setup(self, mem, data_bytes, capacity);

    return (POOL)self;
}

/**
 *  @details    @c pool を解放する.
 *
 *  @pre        @c pool は pool_init() の戻り値である必要がある.
 *  @param      [in,out]    pool    プールオブジェクト.
 *  @warning    本関数はスレッドセーフではない.
 */
void pool_release(POOL pool)
{
    struct pool *self = (struct pool *)pool;

    if (self != NULL) {
        free(self->mem);
        free(self);
    }
}

/**
 *  @details    @c pool の使用中メモリ要素をクリアする.
 *
 *  @pre        @c pool は pool_init() の戻り値である必要がある.
 *  @attention  本関数を呼び出したあとに, pool_alloc() で取得したメモリ要素を
 *              使用してはならない.
 *  @param      [in,out]    pool    プールオブジェクト.
 *  @return     成功時は 0 が返る.
 *              失敗時は -1 が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 */
int pool_clear(POOL pool)
{
    struct pool *self = (struct pool *)pool;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    internal_pool_setup(self, self->mem, self->data_bytes, self->capacity);

    return 0;
}

/**
 *  @details    @c pool からメモリ要素を取得する.
 *
 *  @pre        @c pool は pool_init() の戻り値である必要がある.
 *  @param      [in,out]    pool    プールオブジェクト.
 *  @return     成功時はメモリ要素のポインタが返る.
 *              失敗時は NULL が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 */
void *pool_alloc(POOL pool)
{
    struct pool *self = (struct pool *)pool;

    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return internal_pool_pop(self);
}

/**
 *  @details    @c pool にメモリ要素 @c ptr を返却する.
 *
 *  @pre        @c pool は pool_init() の戻り値である必要がある.
 *  @param      [in,out]    pool    プールオブジェクト.
 *  @param      [in]        ptr     返却するメモリ要素.
 *  @warning    本関数はスレッドセーフではない.
 */
void pool_free(POOL pool, void *ptr)
{
    struct pool *self = (struct pool *)pool;

    if ((self != NULL) && (ptr != NULL)) {
        internal_pool_push(self, ptr);
    }
}

/**
 *  @details    @c pool のデータ部のサイズを取得する.
 *
 *  @pre        @c pool は pool_init() の戻り値である必要がある.
 *  @param      [in]    pool    プールオブジェクト.
 *  @return     成功時はデータ部のサイズが返る.
 *              失敗時は -1 が返る.
 */
ssize_t pool_data_bytes(POOL pool)
{
    struct pool *self = (struct pool *)pool;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    return self->data_bytes;
}

/**
 *  @details    @c pool の容量を取得する.
 *
 *  @pre        @c pool は pool_init() の戻り値である必要がある.
 *  @param      [in]    pool    プールオブジェクト.
 *  @return     成功時はプールの容量が返る.
 *              失敗時は -1 が返る.
 */
ssize_t pool_capacity(POOL pool)
{
    struct pool *self = (struct pool *)pool;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    return self->capacity;
}

/**
 *  @details    @c pool の空き容量を取得する.
 *
 *  @pre        @c pool は pool_init() の戻り値である必要がある.
 *  @param      [in]    pool    プールオブジェクト.
 *  @return     成功時はプールの空き容量が返る.
 *              失敗時は -1 が返る.
 */
ssize_t pool_freeable(POOL pool)
{
    struct pool *self = (struct pool *)pool;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    return self->freeable;
}

bool pool_contains(POOL pool, void *ptr)
{
    struct pool *self = (struct pool *)pool;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    void *mem_end = self->mem + (self->data_bytes * self->capacity);
    return (self->mem <= ptr) && (ptr < mem_end);
}

#define NULL_ITER        \
    (ITER){              \
        .object = NULL,  \
        .next = NULL,    \
        .data = NULL,    \
        .release = NULL, \
    }

/**
 *  @details    @c iter を解放する.
 *
 *  @param      [in]    iter    反復子.
 *  @warning    本関数はスレッドセーフではない.
 */
void iter_release(ITER iter)
{
    if (iter.release != NULL) {
        iter.release(iter.object);
    }
}

bool iter_is_end(ITER iter)
{
    return iter.object == NULL;
}

/**
 *  @details    @c iter の次の反復子を取得する.
 *
 *  @param      [in]    iter    反復子.
 *  @return     成功時は次の反復子が返る. 次の要素がない場合は NULL が返る.
 *              失敗時は NULL が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 *  @sa         list_iter_next, ntree_iter_next
 */
ITER iter_next(ITER iter)
{
    if (iter.object == NULL) {
        errno = EINVAL;
        return NULL_ITER;
    }

    void *next = iter.next(iter.object);
    if (next == NULL) {
        iter_release(iter);
        return NULL_ITER;
    }

    iter.object = next;

    return iter;
}

/**
 *  @details    @c iter のデータ部を取得する.
 *
 *  @param      [in]    iter    リストの反復子.
 *  @return     成功時は, データ部のポインタが返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 *  @sa         list_iter_data, ntree_iter_data
 */
void *iter_data(ITER iter)
{
    if (iter.object == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return iter.data(iter.object);
}

/**
 *  リストノード構造体.
 */
struct list_node {
    struct list_node *prev; /**< 前要素へのポインタ. */
    struct list_node *next; /**< 次要素へのポインタ. */
    char data[];            /**< データ部. */
};

/**
 *  リストノード構造体の初期化子.
 */
#define LIST_NODE_INITIALIZER \
    (struct list_node){       \
        .prev = NULL,         \
        .next = NULL,         \
    }

/**
 *  リスト管理構造体.
 */
struct list {
    POOL pool;              /**< リストで使用するメモリプール. */
    struct list_node *root; /**< 使用中の先頭ノード. */
    struct list_node *last; /**< 使用中の末尾ノード. */
    size_t data_bytes;      /**< データ部のサイズ. */
};

/**
 *  リスト管理構造体の初期化子.
 */
#define LIST_INITIALIZER(p, b) \
    (struct list){             \
        .pool = (p),           \
        .root = NULL,          \
        .last = NULL,          \
        .data_bytes = (b),     \
    }

/**
 *  リスト反復子の次要素を取得する.
 *
 *  @param  [in]    object  反復子.
 *  @return 成功時は @c object の次の反復子が返る.
 *          失敗時は NULL が返り, errno が適切に設定される.
 *  @sa     list_iter, iter_next
 */
static inline void *list_iter_next(void *object)
{
    struct list_node *self = (struct list_node *)object;

    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return self->next;
}

/**
 *  リスト反復子のデータ部を取得する.
 *
 *  @param  [in]    object  反復子.
 *  @return 成功時はデータ部のポインタが返る.
 *          失敗時は NULL が返り, errno が適切に設定される.
 *  @sa     list_iter, iter_data
 */
static inline void *list_iter_data(void *object)
{
    struct list_node *self = (struct list_node *)object;

    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return self->data;
}

/**
 *  @details    空で, 指定の容量を備えた, LIST:: オブジェクトを確保
 *              および初期化する.
 *
 *  @param      [in]    data_bytes  データ部のサイズ.
 *  @param      [in]    capacity    リストの容量.
 *  @return     成功時は確保および初期化したオブジェクトのポインタが返る.
 *              失敗時は NULL が返り, errno が適切に設定される.
 */
LIST list_init(size_t data_bytes, size_t capacity)
{
    struct list *self;
    size_t node_bytes;
    POOL pool;

    if ((data_bytes == 0) || (capacity == 0)) {
        errno = EINVAL;
        return NULL;
    }

    self = malloc(sizeof(*self));
    node_bytes = sizeof(struct list_node) + data_bytes;
    pool = pool_init(node_bytes, capacity);
    if ((self == NULL) || (pool == NULL)) {
        pool_release(pool);
        free(self);
        errno = ENOMEM;
        return NULL;
    }

    *self = LIST_INITIALIZER(pool, data_bytes);

    return (LIST)self;
}

/**
 *  @details    @c list を解放する.
 *
 *  @pre        @c list は list_init() の戻り値である必要がある.
 *  @param      [in,out]    list    リストオブジェクト.
 *  @warning    本関数はスレッドセーフではない.
 */
void list_release(LIST list)
{
    struct list *self = (struct list *)list;

    if (self != NULL) {
        pool_release(self->pool);
        free(self);
    }
}

/**
 *  @details    @c list を空の状態にする.
 *
 *  @pre        @c list は list_init() の戻り値である必要がある.
 *  @param      [in,out]    list    リストオブジェクト.
 *  @return     成功時は 0 が返る.
 *              失敗時は -1 が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 */
int list_clear(LIST list)
{
    struct list *self = (struct list *)list;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    pool_clear(self->pool);
    self->root = NULL;
    self->last = NULL;

    return 0;
}

/**
 *  リストの先頭にノードを追加する.
 *
 *  @param  [in,out]    self    リストオブジェクト.
 *  @param  [in,out]    node    追加するノード.
 */
static void list_insert_head(struct list *self, struct list_node *node)
{
    if (self->root == NULL) {
        self->root = self->last = node;
    } else {
        node->next = self->root;
        self->root->prev = node;
        self->root = node;
    }
}

/**
 *  リストの末尾にノードを追加する.
 *
 *  @param  [in,out]    self    リストオブジェクト.
 *  @param  [in,out]    node    追加するノード.
 */
static void list_insert_tail(struct list *self, struct list_node *node)
{
    if (self->last == NULL) {
        self->root = self->last = node;
    } else {
        node->prev = self->last;
        self->last->next = node;
        self->last = node;
    }
}

/**
 *  @details    @c list の指定位置に要素を追加する.
 *
 *  @pre        @c list は list_init() の戻り値である必要がある.
 *  @param      [in,out]    list    リストオブジェクト.
 *  @param      [in]        index   追加する位置.
 *  @param      [in]        data    リストに追加するデータ.
 *  @return     成功時は追加したリスト上のデータ部のポインタが返る.
 *              失敗時は NULL が返り, errno が適切に設定される.
 *  @remarks    @c index が負数の場合はリストの最後に追加される.
 *  @warning    本関数はスレッドセーフではない.
 */
void *list_insert(LIST list, int index, void *data)
{
    struct list *self = (struct list *)list;
    struct list_node *node;
    struct list_node *iter;
    int i;

    if ((self == NULL) || (data == NULL)) {
        errno = EINVAL;
        return NULL;
    }

    node = pool_alloc(self->pool);
    if (node == NULL) {
        return NULL;
    }
    *node = LIST_NODE_INITIALIZER;
    memcpy(node->data, data, self->data_bytes);

    if (index == 0) {
        list_insert_head(self, node);
    } else if (index > 0) {
        iter = self->root;
        for (i = 0; i < index; ++i) {
            if (iter == NULL) {
                errno = ERANGE;
                return NULL;
            }
            iter = iter->next;
        }
        if (iter == NULL) {
            node->prev = self->last;
            self->last->next = node;
            self->last = node;
        } else {
            node->next = iter;
            node->prev = iter->prev;
            node->prev->next = node;
            iter->prev = node;
        }
    } else {
        list_insert_tail(self, node);
    }

    return node->data;
}

int list_get(LIST list, int index, void *data)
{
    struct list *self = (struct list *)list;
    struct list_node *node;

    if ((self == NULL) || (data == NULL)) {
        errno = EINVAL;
        return -1;
    }

    if (index >= 0) {
        node = self->root;
        for (int i = 0; i < index; ++i) {
            if (node == NULL) {
                break;
            }
            node = node->next;
        }
    } else {
        node = self->last;
    }
    if (node == NULL) {
        errno = ERANGE;
        return -1;
    }

    memcpy(data, node->data, self->data_bytes);

    return 0;

}

/**
 *  @details    @c list の末尾に要素を追加する.
 *
 *  @pre        @c list は list_init() の戻り値である必要がある.
 *  @param      [in,out]    list    リストオブジェクト.
 *  @param      [in]        data    リストに追加するデータ部.
 *  @return     成功時は追加したリスト上のデータ部のポインタが返る.
 *              失敗時は NULL が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 */
void *list_push(LIST list, void *data)
{
    return list_insert(list, -1, data);
}

ssize_t list_pop(LIST list, void *data)
{
    struct list *self = (struct list *)list;
    struct list_node *node;

    if ((self == NULL) || (data == NULL)) {
        errno = EINVAL;
        return -1;
    }

    node = self->last;
    if (node == NULL) {
        return -1;
    }
    if (self->root == node) {
        self->root = NULL;
    }
    self->last = node->prev;
    if (node->prev != NULL) {
        node->prev->next = NULL;
    }

    memcpy(data, node->data, self->data_bytes);
    pool_free(self->pool, node);

    return list_count(list);
}

void *list_unshift(LIST list, void *data)
{
    return list_insert(list, 0, data);;
}

ssize_t list_shift(LIST list, void *data)
{
    struct list *self = (struct list *)list;
    struct list_node *node;

    if ((self == NULL) || (data == NULL)) {
        errno = EINVAL;
        return -1;
    }

    node = self->root;
    if (node == NULL) {
        return -1;
    }
    self->root = node->next;
    if (self->last == node) {
        self->last = NULL;
    }
    if (node->next != NULL) {
        node->next->prev = NULL;
    }

    memcpy(data, node->data, self->data_bytes);
    pool_free(self->pool, node);

    return list_count(list);
}

/**
 *  @details    @c list から指定のデータを削除する.
 *
 *  @pre        @c list は list_init() の戻り値である必要がある.
 *  @param      [in,out]    list    リストオブジェクト.
 *  @param      [in]        iter    削除する要素の反復子.
 *  @return     成功時は 0 が返る.
 *              失敗時は -1 が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 */
int list_remove(LIST list, ITER iter)
{
    struct list *self = (struct list *)list;
    struct list_node *node;

    if ((self == NULL) || iter_is_end(iter)) {
        errno = EINVAL;
        return -1;
    }

    node = (struct list_node *)iter.object;
    if (self->root == node) {
        self->root = node->next;
    }
    if (self->last == node) {
        self->last = node->prev;
    }
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }

    pool_free(self->pool, node);

    return 0;
}

/**
 *  @details    @c list のデータ部のサイズを取得する.
 *
 *  @pre        @c list は list_init() の戻り値である必要がある.
 *  @param      [in]    list    リストオブジェクト.
 *  @return     成功時は @c list のデータ部のサイズが返る.
 *              失敗時は -1 が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 */
ssize_t list_data_bytes(LIST list)
{
    struct list *self = (struct list *)list;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    return self->data_bytes;
}

/**
 *  @details    @c list に追加されている要素の数を返す.
 *
 *  @pre        @c list は list_init() の戻り値である必要がある.
 *  @param      [in]    list    リストオブジェクト.
 *  @return     成功時は @c list に追加されている要素の数を返す.
 *              失敗時は -1 が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 */
ssize_t list_count(LIST list)
{
    struct list *self = (struct list *)list;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    return pool_capacity(self->pool) - pool_freeable(self->pool);
}

/**
 *  @details    @c list の反復子を取得する.
 *
 *  @pre        @c list は list_init() の戻り値である必要がある.
 *  @param      [in]    list    リストオブジェクト.
 *  @return     成功時は, @c list の反復子が返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 */
ITER list_iter(LIST list)
{
    struct list *self = (struct list *)list;

    if (self == NULL) {
        errno = EINVAL;
        return NULL_ITER;
    }
    if (self->root == NULL) {
        errno = ENOENT;
        return NULL_ITER;
    }

    return (ITER){
        .object = self->root,
        .next = list_iter_next,
        .data = list_iter_data,
        .release = NULL,
    };
}

/**
 *  @details    @c list に積まれた要素を配列にコピーする.
 *
 *  @pre        @c list は list_init() の戻り値である必要がある.
 *  @param      [in]    list    リストオブジェクト.
 *  @param      [out]   array   確保した配列のポインタ.
 *  @param      [out]   count   要素の数.
 *  @return     成功時は 0 が返り, @c array に確保された配列のポインタを
 *              設定する.
 *              失敗時は -1 が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 */
int list_to_array(LIST list, void **array, size_t *count)
{
    struct list *self = (struct list *)list;
    void *buf;

    if ((self == NULL) || (array == NULL) || (count == NULL)) {
        errno = EINVAL;
        return -1;
    }

    *count = list_count(list);
    *array = buf = malloc(self->data_bytes * *count);
    if (buf == NULL) {
        return -1;
    }
    while (list_shift(list, buf) > 0) {
        buf = (void *)((uintptr_t)buf + self->data_bytes);
    }

    return 0;
}

/**
 *  @details    空で, 指定の容量を備えた, STACK:: オブジェクトを確保
 *              および初期化する.
 *
 *  @param      [in]    data_bytes   データ部のサイズ.
 *  @param      [in]    capacity        スタックの容量.
 *  @return     成功時は, 確保および初期化したオブジェクトのポインタが返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 */
STACK stack_init(size_t data_bytes, size_t capacity)
{
    return (STACK)list_init(data_bytes, capacity);
}

/**
 *  @details    @c stack を解放する.
 *              @c stack は stack_init() の戻り値である必要がある.
 *
 *  @param      [in,out]    stack   スタックオブジェクト.
 *  @warning    スレッドセーフではない.
 */
void stack_release(STACK stack)
{
    list_release((LIST)stack);
}

/**
 *  @details    @c stack を空の状態にする.
 *
 *  @param      [in,out]    stack   スタックオブジェクト.
 *  @return     成功時は, 0 が返る.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
int stack_clear(STACK stack)
{
    return list_clear((LIST)stack);
}

/**
 *  @details    @c stack に要素を積む.
 *
 *  @param      [in,out]    stack   スタックオブジェクト.
 *  @param      [in]        data スタックに積むデータ.
 *  @return     成功時は, 積んだスタック上のデータ部のポインタが返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
void *stack_push(STACK stack, void *data)
{
    return list_push((LIST)stack, data);
}

/**
 *  @details    @c stack から, 最初の要素を取り除く.
 *
 *  @param      [in,out]    stack   スタックオブジェクト.
 *  @param      [out]       data データ部をコピーするバッファ.
 *  @return     成功時は, @c stack に残っている要素の数が返る.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 *  @todo       動的なメモリ確保 (反復子) を必要としないように作り変える.
 */
ssize_t stack_pop(STACK stack, void *data)
{
    return list_pop((LIST)stack, data);
}

int stack_peek(STACK stack, void *data)
{
    return list_get((LIST)stack, -1, data);
}

/**
 *  @details    @c stack に積まれている要素の数を返す.
 *
 *  @param      [in]    stack   スタックオブジェクト.
 *  @return     成功時は, @c stack に積まれている要素の数を返す.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
ssize_t stack_count(STACK stack)
{
    return list_count((LIST)stack);
}

/**
 *  @details    @c stack の反復子を取得する.
 *
 *  @param      [in]    stack   スタックオブジェクト.
 *  @return     成功時は, @c stack の反復子が返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
ITER stack_iter(STACK stack)
{
    return list_iter((LIST)stack);
}

/**
 *  @details    空で, 指定の容量を備えた, QUEUE:: オブジェクトを
 *              確保および初期化する.
 *
 *  @param      [in]    data_bytes   データ部のサイズ.
 *  @param      [in]    capacity        キューの容量.
 *  @return     成功時は, 確保および初期化したオブジェクトのポインタが返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 */
QUEUE queue_init(size_t data_bytes, size_t capacity)
{
    return (QUEUE)list_init(data_bytes, capacity);
}

/**
 *  @details    @c que を解放する.
 *              @c que は queue_init() の戻り値である必要がある.
 *
 *  @param      [in,out]    que キューオブジェクト.
 *  @warning    スレッドセーフではない.
 */
void queue_release(QUEUE que)
{
    list_release((LIST)que);
}

/**
 *  @details    @c que を空の状態にする.
 *
 *  @param      [in,out]    que キューオブジェクト.
 *  @return     成功時は, 0 が返る.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
int queue_clear(QUEUE que)
{
    return list_clear((LIST)que);
}

/**
 *  @details    @c que の最後に要素を追加する.
 *
 *  @param      [in,out]    que     キューオブジェクト.
 *  @param      [in]        data    キューに追加するデータ.
 *  @return     成功時は, 追加したキュー上のデータ部のポインタが返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
void *queue_enq(QUEUE que, void *data)
{
    return list_push((LIST)que, data);
}

/**
 *  @details    @c que の最初の要素をコピーし, 削除する.
 *
 *  @param      [in,out]    que     キューオブジェクト.
 *  @param      [out]       data    データ部をコピーするバッファ.
 *  @return     成功時は, @c que に残っている要素の数が返る.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 *  @todo       動的なメモリ確保 (反復子) を必要としないように作り変える.
 */
int queue_deq(QUEUE que, void *data)
{
    return list_shift((LIST)que, data);
}

/**
 *  @details    @c que に積まれた要素の数を返す.
 *
 *  @param      [in]    que キューオブジェクト.
 *  @return     成功時は, @c que に積まれた要素の数を返す.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
ssize_t queue_count(QUEUE que)
{
    return list_count((LIST)que);
}

/**
 *  @details    @c que の反復子を取得する.
 *
 *  @param      [in]    que キューオブジェクト.
 *  @return     成功時は, @c que の反復子が返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
ITER queue_iter(QUEUE que)
{
    return list_iter((LIST)que);
}

/**
 *  @details    @c que に積まれた要素を配列にコピーする.
 *
 *  @param      [in]    que     キューオブジェクト.
 *  @param      [out]   array   確保した配列のポインタ.
 *  @param      [out]   count   要素の数.
 *  @return     成功時は, 0 が返り, @c array に確保された配列のポインタを
 *              設定する.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */

int queue_to_array(QUEUE que, void **array, size_t *count)
{
    return list_to_array((LIST)que, array, count);
}

/**
 *  @details    空で, 指定の容量を備えた, SET:: オブジェクトを確保
 *              および初期化する.
 *
 *  @param      [in]    data_bytes  データ部のサイズ.
 *  @param      [in]    capacity    セットの容量.
 *  @return     成功時は, 確保および初期化したオブジェクトのポインタが返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 */
SET set_init(size_t data_bytes, size_t capacity)
{
    return (SET)list_init(data_bytes, capacity);
}

/**
 *  @details    @c set を解放する.
 *              @c set は set_init() の戻り値である必要がある.
 *
 *  @param      [in,out]    set セットオブジェクト.
 *  @warning    スレッドセーフではない.
 */
void set_release(SET set)
{
    list_release((LIST)set);
}

/**
 *  @details    @c set を空の状態にする.
 *
 *  @param      [in,out]    set セットオブジェクト.
 *  @return     成功時は, 0 が返る.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
int set_clear(SET set)
{
    return list_clear((LIST)set);
}

/**
 *  @details    @c set に要素を追加する.
 *              指定データがすでに追加されている場合は何もしない.
 *
 *  @param      [in,out]    set     セットオブジェクト.
 *  @param      [in]        data    セットに追加するデータ.
 *  @return     成功時は追加したセット上のデータ部のポインタが返る.
 *              失敗時は NULL が返り, errno が適切に設定される.
 *  @warning    本関数はスレッドセーフではない.
 *  @todo       動的なメモリ確保 (反復子) を必要としないように作り変える.
 */
void *set_add(SET set, void *data)
{
    LIST list = (LIST)set;
    ssize_t data_bytes;

    if ((list == NULL) || (data == NULL)) {
        errno = EINVAL;
        return NULL;
    }

    data_bytes = list_data_bytes(list);
    for (ITER iter = list_iter(list); !iter_is_end(iter); iter = iter_next(iter)) {
        void *p = iter_data(iter);
        if (memcmp(data, p, data_bytes) == 0) {
            iter_release(iter);
            return p;
        }
    }

    return list_insert(list, -1, data);
}

/**
 *  @details    @c set に追加されている要素の数を返す.
 *
 *  @param      [in]    set セットオブジェクト.
 *  @return     成功時は, @c set に追加されている要素の数を返す.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
ssize_t set_count(SET set)
{
    return list_count((LIST)set);
}

/**
 *  @details    @c set の反復子を取得する.
 *
 *  @param      [in]    set セットオブジェクト.
 *  @return     成功時は, @c set の反復子が返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
ITER set_iter(SET set)
{
    return list_iter((LIST)set);
}

enum ntree_trav_action {
    ACT_INACTIVE,
    ACT_GOING_LEFT,
    ACT_GOING_RIGHT,
};

/**
 *  N-ary ツリーノード構造体.
 */
struct ntree_node {
    struct ntree_node *first_child;  /**< 最初の子要素へのポインタ. */
    struct ntree_node *next_sibling; /**< 次の兄弟要素へのポインタ. */
    struct ntree_node *parent;       /**< 親要素へのポインタ. */
    enum ntree_trav_action action;
    int age;                         /**< 世代. (ツリー上での深さ) */
    char data[];                     /**< データ部. */
};

/**
 *  N-ary ツリーノード構造体の初期化子.
 */
#define NTREE_NODE_INITIALIZER  \
    (struct ntree_node){        \
        .first_child = NULL,    \
        .next_sibling = NULL,   \
        .parent = NULL,         \
        .action = ACT_INACTIVE, \
        .age = 0,               \
    }

/**
 *  N-ary ツリー管理構造体.
 */
struct ntree {
    POOL pool;               /**< ツリーで使用するメモリプール. */
    struct ntree_node *root; /**< ツリーの根. */
    size_t data_bytes;       /**< データ部のサイズ. */
};

/**
 *  N-ary ツリー管理構造体の初期化子.
 */
#define NTREE_INITIALIZER(p, b) \
    (struct ntree){             \
        .pool = (p),            \
        .root = NULL,           \
        .data_bytes = (b),      \
    }

/**
 *  N-ary ツリー反復子構造体.
 */
struct ntree_iter {
    STACK fringe;            /**< 探索経路を保持するスタック. */
    int age;                 /**< 現在地の世代. */
    struct ntree_node *node; /**< 現在のノード. */
};

/**
 *  N-ary ツリー反復子構造体の初期化子.
 */
#define NTREE_ITER_INITIALIZER \
    (struct ntree_iter){       \
        .fringe = NULL,        \
        .age = 0,              \
        .node = NULL,          \
    }

/**
 *  ツリー反復子を解放する.
 *
 *  @param      [in,out]    object  反復子オブジェクト.
 */
static void ntree_iter_release(void *object)
{
    struct ntree_iter *self = (struct ntree_iter *)object;

    if (self != NULL) {
        stack_release(self->fringe);
        free(self);
    }
}

/**
 *  次の反復子を取得する.
 *
 *  ツリー全体の次の反復子を取得する.
 *
 *  @param      [in]    object  N-ary ツリーの反復子.
 *  @return     成功時は, 次の反復子が返る. 次の要素がない場合は NULL が返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @remarks    走査は深さ (子要素) 優先で行われる.
 */
static void *ntree_iter_next(void *object)
{
    struct ntree_iter *self = (struct ntree_iter *)object;
    struct ntree_node *node;
    int ret;

    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (stack_count(self->fringe) == 0) {
        errno = ENOENT;
        return NULL;
    }

    ret = stack_pop(self->fringe, &node);
    if (ret == -1) {
        return NULL;
    }
    self->age = node->age;
    self->node = node;
    if (node->next_sibling != NULL) {
        stack_push(self->fringe, &node->next_sibling);
    }
    if (node->first_child != NULL) {
        stack_push(self->fringe, &node->first_child);
    }

    return self;
}

/**
 *  @details    @c object のデータ部を取得する.
 *
 *  @param      [in]    object  N-ary ツリーの反復子.
 *  @return     成功時はデータ部のポインタが返る.
 *              失敗時は NULL が返り, errno が適切に設定される.
 */
static void *ntree_iter_data(void *object)
{
    struct ntree_iter *self = (struct ntree_iter *)object;

    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return self->node->data;
}

/**
 *  ツリー反復子を取得する.
 *
 *  @param      [in]    self    ツリーオブジェクト.
 *  @param      [in]    node    反復子の起点となる要素.
 *  @return     成功時は, @c parent より後の要素の反復子が返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @remarks    走査は深さ (子要素) 優先で行われる.
 */
static struct ntree_iter *internal_ntree_iter(struct ntree *self, struct ntree_node *node)
{
    struct ntree_iter *iter;

    iter = malloc(sizeof(*iter));
    if (iter == NULL) {
        return NULL;
    }
    *iter = NTREE_ITER_INITIALIZER;
    iter->fringe = stack_init(sizeof(struct ntree_node *), pool_capacity(self->pool));
    if (iter->fringe == NULL) {
        free(iter);
        return NULL;
    }

    if (node != NULL) {
        stack_push(iter->fringe, &node);
    }

    return iter;
}

/**
 *  @details    @c t に追加された @c parent の子に要素を追加するための再帰処理.
 *              @c parent が複数存在する場合は, 最初に見つかった要素の子として
 *              追加する.
 *
 *  @param      [in,out]    self    ツリーオブジェクト.
 *  @param      [in,out]    node    探索中の要素.
 *  @param      [in]        parent  探索対象のデータ部.
 *  @param      [in]        data     ツリーに追加するデータ.
 *  @return     成功時は, 追加したツリー上のデータ部のポインタが返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @remarks    探索は幅 (同一の親に追加された要素) 優先で行われる.
 *  @pre        @c self の非 NULL は呼び出し側で保証すること.
 *  @pre        @c node の非 NULL は呼び出し側で保証すること.
 *  @pre        @c parent の非 NULL は呼び出し側で保証すること.
 *  @pre        @c data の非 NULL は呼び出し側で保証すること.
 */
static struct ntree_node *internal_ntree_insert(struct ntree *self,
                                                struct ntree_node *node,
                                                void *parent,
                                                void *data)
{
    if ((node->parent != NULL)
        && (memcmp(parent, node->parent->data, self->data_bytes) == 0)) {

        struct ntree_node *sibling = node;
        while (sibling->next_sibling != NULL) {
            sibling = sibling->next_sibling;
        }
        sibling = sibling->next_sibling = pool_alloc(self->pool);
        if (sibling == NULL) {
            return NULL;
        }
        *sibling = NTREE_NODE_INITIALIZER;
        sibling->parent = node->parent;
        sibling->age = node->parent->age + 1;
        memcpy(sibling->data, data, self->data_bytes);
        return sibling;
    } else {
        struct ntree_node *added = NULL;
        if (node->next_sibling != NULL) {
            added = internal_ntree_insert(self,
                                          node->next_sibling,
                                          parent,
                                          data);
        }
        if ((added == NULL) && (node->first_child != NULL)) {
            added = internal_ntree_insert(self,
                                          node->first_child,
                                          parent,
                                          data);
        }
        return added;
    }
}

/**
 *  @details    空で, 指定の容量を備えた, NTREE:: オブジェクトを確保
 *              および初期化する.
 *              N-ary ツリーのため, 同一の親に複数の子を追加することができる.
 *
 *  @param      [in]    data_bytes  データ部のサイズ.
 *  @param      [in]    capacity    ツリーの容量.
 *  @return     成功時は確保および初期化したオブジェクトのポインタが返る.
 *              失敗時は NULL が返り, errno が適切に設定される.
 */
NTREE ntree_init(size_t data_bytes, size_t capacity)
{
    struct ntree *self;
    size_t node_bytes;
    POOL pool;

    if ((data_bytes == 0) || (capacity == 0)) {
        errno = EINVAL;
        return NULL;
    }

    self = malloc(sizeof(*self));
    node_bytes = sizeof(struct ntree_node) + data_bytes;
    pool = pool_init(node_bytes, capacity);
    if ((self == NULL) || (pool == NULL)) {
        pool_release(pool);
        free(self);
        errno = ENOMEM;
        return NULL;
    }

    *self = NTREE_INITIALIZER(pool, data_bytes);

    return (NTREE)self;
}

/**
 *  @details    @c tree を解放する.
 *              @c tree は ntree_init() の戻り値である必要がある.
 *
 *  @param      [in,out]    tree    ツリーオブジェクト.
 *  @warning    スレッドセーフではない.
 */
void ntree_release(NTREE tree)
{
    struct ntree *self = (struct ntree *)tree;

    if (self != NULL) {
        pool_release(self->pool);
        free(self);
    }
}

/**
 *  @details    @c tree を空の状態にする.
 *
 *  @param      [in,out]    tree    ツリーオブジェクト.
 *  @return     成功時は, 0 が返る.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
int ntree_clear(NTREE tree)
{
    struct ntree *self = (struct ntree *)tree;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    pool_clear(self->pool);

    return 0;
}

NTREE_NODE ntree_insert(NTREE tree, void *data)
{
    return ntree_insert_at(tree, NULL, data);
}

/**
 *  @details    @c tree の指定位置に要素を追加する.
 *
 *  @param      [in,out]    tree    ツリーオブジェクト.
 *  @param      [in]        parent  子として追加する親要素.
 *  @param      [in]        data    ツリーに追加するデータ.
 *  @return     成功時は, 追加したツリー上のデータ部のポインタが返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
NTREE_NODE ntree_insert_at(NTREE tree, NTREE_NODE parent, void *data)
{
    struct ntree *self = (struct ntree *)tree;
    struct ntree_node *node = (struct ntree_node *)parent;
    struct ntree_node *added = NULL;
    int age;

    if ((self == NULL) || (data == NULL)) {
        errno = EINVAL;
        return NULL;
    }

    if (self->root == NULL) {
        added = self->root = pool_alloc(self->pool);
        age = 1;
    } else if (node == NULL) {
        struct ntree_node *sibling = self->root;
        while (sibling->next_sibling != NULL) {
            sibling = sibling->next_sibling;
        }
        added = sibling->next_sibling = pool_alloc(self->pool);
        age = 1;
    } else {
        if (node->first_child == NULL) {
            added = node->first_child = pool_alloc(self->pool);
        } else {
            struct ntree_node *child = node->first_child;
            while (child->next_sibling != NULL) {
                child = child->next_sibling;
            }
            added = child->next_sibling = pool_alloc(self->pool);
        }
        age = node->age + 1;
    }
    if (added == NULL) {
        return NULL;
    }

    *added = NTREE_NODE_INITIALIZER;
    added->parent = node;
    added->age = age;
    memcpy(added->data, data, self->data_bytes);

    return (NTREE_NODE)added;
}

NTREE_NODE ntree_insert_at_by_data(NTREE tree, void *parent, void *data)
{
    struct ntree *self = (struct ntree *)tree;

    if ((self == NULL) || (data == NULL)) {
        errno = EINVAL;
        return NULL;
    }

    if (self->root == NULL) {
        self->root = pool_alloc(self->pool);
        if (self->root == NULL) {
            return NULL;
        }
        *self->root = NTREE_NODE_INITIALIZER;
        self->root->age = 1;
        memcpy(self->root->data, data, self->data_bytes);
        return (NTREE_NODE)self->root;
    } else {
        return (NTREE_NODE)internal_ntree_insert(self,
                                                 self->root,
                                                 parent,
                                                 data);
    }
}

static struct ntree_node *save_returning_node(struct ntree_node *current, struct ntree_node *older)
{
    struct ntree_node *node;

    if (current->action == ACT_GOING_LEFT) {
        node = current->first_child;
        current->first_child = older;
    } else {
        node = current->next_sibling;
        current->next_sibling = older;
    }

    return node;
}

static struct ntree_node *restore_returning_node(struct ntree_node *current, struct ntree_node *just_done)
{
    struct ntree_node *older;

    if (current->action == ACT_GOING_LEFT) {
        older = current->first_child;
        current->first_child = just_done;
    } else {
        older = current->next_sibling;
        current->next_sibling = just_done;
    }

    return older;
}

int ntree_remove(NTREE tree, NTREE_NODE node)
{
    struct ntree *self = (struct ntree *)tree;

    if ((self == NULL) || (node == NULL)) {
        errno = EINVAL;
        return -1;
    }
    if (self->root == NULL) {
        errno = ENOENT;
        return -1;
    }

    struct ntree_node *current = (struct ntree_node *)node;
    if (current == self->root) {
        self->root = current->next_sibling;
    } else {
        struct ntree_node *parent = current->parent;
        if ((parent != NULL) && (parent->first_child == current)) {
            parent->first_child = current->next_sibling;
        } else {
            struct ntree_node *old_sib = (parent != NULL) ? parent->first_child : self->root;
            while (old_sib->next_sibling != current) {
                old_sib = old_sib->next_sibling;
            }
            old_sib->next_sibling = current->next_sibling;
        }
    }
    current->parent = current->next_sibling = NULL;

    struct ntree_node *older = NULL;
    while (current != NULL) {
        if (current->action != ACT_INACTIVE) {
            older = restore_returning_node(current, NULL);
        }
        if ((current->action == ACT_INACTIVE) && (current->first_child != NULL)) {
            current->action = ACT_GOING_LEFT;
            struct ntree_node *tmp = current;
            current = save_returning_node(current, older);
            older = tmp;
        } else if ((current->action != ACT_GOING_RIGHT) && (current->next_sibling != NULL)) {
            current->action = ACT_GOING_RIGHT;
            struct ntree_node *tmp = current;
            current = save_returning_node(current, older);
            older = tmp;
        } else {
            current->action = ACT_INACTIVE;
            pool_free(self->pool, current);
            current = older;
        }
    }

    return 0;
}

void *ntree_data(NTREE_NODE node)
{
    struct ntree_node *self = (struct ntree_node *)node;

    return (self != NULL) ? self->data : NULL;
}

/**
 *  @details    @c tree に追加されている要素の数を返す.
 *
 *  @param      [in]    tree    ツリーオブジェクト.
 *  @return     成功時は, @c tree に追加されている要素の数を返す.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
ssize_t ntree_count(NTREE tree)
{
    struct ntree *self = (struct ntree *)tree;

    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    return pool_capacity(self->pool) - pool_freeable(self->pool);
}

/**
 *  @details    @c tree の反復子を取得する.
 *
 *  @param      [in]    tree    ツリーオブジェクト.
 *  @return     成功時は, @c tree の反復子が返る.
 *              失敗時は, NULL が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
ITER ntree_iter(NTREE tree)
{
    struct ntree *self = (struct ntree *)tree;

    if (self == NULL) {
        errno = EINVAL;
        return NULL_ITER;
    }

    struct ntree_iter *iter = internal_ntree_iter(self, self->root);
    if (iter == NULL) {
        return NULL_ITER;
    }

    if (ntree_iter_next(iter) == NULL) {
        ntree_iter_release(iter);
        return NULL_ITER;
    }

    return (ITER){
        .object = iter,
        .next = ntree_iter_next,
        .data = ntree_iter_data,
        .release = ntree_iter_release,
    };
}

static inline void *ntree_children_iter_next(void *object)
{
    struct ntree_node *self = (struct ntree_node *)object;

    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return self->next_sibling;
}

static inline void *ntree_children_iter_data(void *object)
{
    struct ntree_node *self = (struct ntree_node *)object;

    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return self->data;
}

ITER ntree_children_iter(NTREE_NODE node)
{
    struct ntree_node *self = (struct ntree_node *)node;

    if (self == NULL) {
        errno = EINVAL;
        return NULL_ITER;
    }
    if (self->first_child == NULL) {
        errno = ENOENT;
        return NULL_ITER;
    }

    return (ITER){
        .object = self->first_child,
        .next = ntree_children_iter_next,
        .data = ntree_children_iter_data,
        .release = NULL,
    };
}

/**
 *  @details    @c iter のツリー上での世代 (深さ) を取得する.
 *
 *  @param      [in]    iter    N-ary ツリーの反復子.
 *  @return     成功時は, 世代が返る.
 *              失敗時は, -1 が返り, errno が適切に設定される.
 *  @warning    スレッドセーフではない.
 */
int ntree_iter_age(ITER iter)
{
    if (iter.object == NULL) {
        errno = EINVAL;
        return -1;
    }

    return ((struct ntree_iter *)(iter.object))->age;
}
