/** @file   collections.cpp
 *  @brief  コレクションのテスト.
 *
 *  @author t-kenji <protect.2501@gmail.com>
 *  @date   2018-03-18 新規作成.
 */
#include "catch2/catch.hpp"

extern "C" {
#include "debug.h"
#include "collections.h"
}

SCENARIO("メモリプールが初期化できること", "[pool][init]") {
    GIVEN("特になし") {
        WHEN("プールを初期化する") {
            size_t capacity = 0;
            INFO("プール容量: " + std::to_string(capacity));

            POOL pool = pool_init(sizeof(int), capacity);

            THEN("初期化できないこと") {
                INFO("インスタンスが NULL であること");
                REQUIRE(pool == NULL);
            }

            pool_release(pool);
        }
        WHEN("プールを初期化する") {
            size_t capacity = 1;
            INFO("プール容量: " + std::to_string(capacity));

            POOL pool = pool_init(sizeof(int), capacity);

            THEN("初期化できること") {
                INFO("インスタンスが NULL ではないこと");
                REQUIRE(pool != NULL);
            }
            THEN("初期化できること") {
                INFO("データ部のサイズが " + std::to_string(sizeof(int)) + " であること");
                REQUIRE(pool_data_bytes(pool) == sizeof(int));
            }
            THEN("初期化できること") {
                INFO("全体の容量が " + std::to_string(capacity) + " であること");
                REQUIRE(pool_capacity(pool) == capacity);
            }
            THEN("初期化できること") {
                INFO("空き容量が " + std::to_string(capacity) + " であること");
                REQUIRE(pool_freeable(pool) == capacity);
            }

            pool_release(pool);
        }
        WHEN("プールを初期化する") {
            size_t capacity = 5;
            INFO("プール容量: " + std::to_string(capacity));

            POOL pool = pool_init(sizeof(int), capacity);

            THEN("初期化できること") {
                INFO("インスタンスが NULL ではないこと");
                REQUIRE(pool != NULL);
            }
            THEN("初期化できること") {
                INFO("データ部のサイズが " + std::to_string(sizeof(int)) + " であること");
                REQUIRE(pool_data_bytes(pool) == sizeof(int));
            }
            THEN("初期化できること") {
                INFO("全体の容量が " + std::to_string(capacity) + " であること");
                REQUIRE(pool_capacity(pool) == capacity);
            }
            THEN("初期化できること") {
                INFO("空き容量が " + std::to_string(capacity) + " であること");
                REQUIRE(pool_freeable(pool) == capacity);
            }

            pool_release(pool);
        }
    }
}

SCENARIO("メモリプールからメモリを取得する", "[pool][alloc]") {
    GIVEN("プールを初期化する") {
        size_t capacity = 1;
        INFO("プール容量: " + std::to_string(capacity));

        POOL pool = pool_init(sizeof(int), capacity);
        REQUIRE(pool != NULL);

        WHEN("メモリを取得する") {
            int count = 1;
            void *ptrs[count];
            INFO(std::to_string(count) + " 個取得する");
            for (int i = 0; i < count; ++i) {
                ptrs[i] = pool_alloc(pool);
            }

            THEN("メモリが取得できること") {
                INFO("戻り値が NULL ではないこと");
                REQUIRE(ptrs[0] != NULL);
            }
            THEN("メモリが取得できること") {
                size_t freeable = capacity - count;
                INFO("空き容量が " + std::to_string(freeable) + " であること");
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < count; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }
        WHEN("メモリを取得する") {
            int count = 2;
            void *ptrs[count];
            INFO(std::to_string(count) + " 個取得する");
            for (int i = 0; i < count; ++i) {
                ptrs[i] = pool_alloc(pool);
            }

            THEN("メモリが取得できること") {
                INFO(std::to_string(capacity) + "回は戻り値が NULL ではないこと");
                REQUIRE(ptrs[0] != NULL);
            }
            THEN("メモリが取得できないこと") {
                INFO(std::to_string(capacity) + "回を超えたら戻り値が NULL になること");
                REQUIRE(ptrs[1] == NULL);
            }
            THEN("メモリが取得できること") {
                size_t freeable = 0;
                INFO("空き容量が " + std::to_string(freeable) + " であること");
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < count; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }

        pool_release(pool);
    }

    GIVEN("プールを初期化する") {
        size_t capacity = 5;
        INFO("プール容量: " + std::to_string(capacity));

        POOL pool = pool_init(sizeof(int), capacity);
        REQUIRE(pool != NULL);

        WHEN("メモリを取得する") {
            int count = 1;
            void *ptrs[count];
            INFO(std::to_string(count) + " 個取得する");
            for (int i = 0; i < count; ++i) {
                ptrs[i] = pool_alloc(pool);
            }

            THEN("メモリが取得できること") {
                INFO(std::to_string(count) + "回は戻り値が NULL ではないこと");
                REQUIRE(ptrs[0] != NULL);
            }
            THEN("メモリが取得できること") {
                size_t freeable = capacity - count;
                INFO("空き容量が " + std::to_string(freeable) + " であること");
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < count; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }
        WHEN("メモリを取得する") {
            int count = 4;
            void *ptrs[count];
            INFO(std::to_string(count) + " 個取得する");
            for (int i = 0; i < count; ++i) {
                ptrs[i] = pool_alloc(pool);
            }

            THEN("メモリが取得できること") {
                INFO(std::to_string(count) + "回は戻り値が NULL ではないこと");
                REQUIRE(ptrs[0] != NULL);
                REQUIRE(ptrs[1] != NULL);
                REQUIRE(ptrs[2] != NULL);
                REQUIRE(ptrs[3] != NULL);
            }
            THEN("メモリが取得できること") {
                size_t freeable = capacity - count;
                INFO("空き容量が " + std::to_string(freeable) + " であること");
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < count; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }
        WHEN("メモリを取得する") {
            int count = 5;
            void *ptrs[count];
            INFO(std::to_string(count) + " 個取得する");
            for (int i = 0; i < count; ++i) {
                ptrs[i] = pool_alloc(pool);
            }

            THEN("メモリが取得できること") {
                INFO(std::to_string(count) + "回は戻り値が NULL ではないこと");
                REQUIRE(ptrs[0] != NULL);
                REQUIRE(ptrs[1] != NULL);
                REQUIRE(ptrs[2] != NULL);
                REQUIRE(ptrs[3] != NULL);
                REQUIRE(ptrs[4] != NULL);
            }
            THEN("メモリが取得できること") {
                size_t freeable = capacity - count;
                INFO("空き容量が " + std::to_string(freeable) + " であること");
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < count; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }
        WHEN("メモリを取得する") {
            int count = 6;
            void *ptrs[count];
            INFO(std::to_string(count) + " 個取得する");
            for (int i = 0; i < count; ++i) {
                ptrs[i] = pool_alloc(pool);
            }

            THEN("メモリが取得できること") {
                INFO(std::to_string(capacity) + "回は戻り値が NULL ではないこと");
                REQUIRE(ptrs[0] != NULL);
                REQUIRE(ptrs[1] != NULL);
                REQUIRE(ptrs[2] != NULL);
                REQUIRE(ptrs[3] != NULL);
                REQUIRE(ptrs[4] != NULL);
            }
            THEN("メモリが取得できないこと") {
                INFO(std::to_string(capacity) + "回を超えたら戻り値が NULL になること");
                REQUIRE(ptrs[5] == NULL);
            }
            THEN("メモリが取得できること") {
                size_t freeable = 0;
                INFO("空き容量が " + std::to_string(freeable) + " であること");
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < count; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }

        pool_release(pool);
    }
}

SCENARIO("メモリプールにメモリを解放する", "[pool][free]") {
    GIVEN("プールを使用する") {
        size_t capacity = 5;
        INFO("プール容量: " + std::to_string(capacity));
        int allocated = 1;
        void *ptrs[allocated];
        INFO("取得: " + std::to_string(allocated));

        POOL pool = pool_init(sizeof(int), capacity);
        REQUIRE(pool != NULL);
        for (int i = 0; i < allocated; ++i) {
            ptrs[i] = pool_alloc(pool);
        }
        REQUIRE(pool_freeable(pool) == capacity - allocated);

        WHEN("メモリを解放する") {
            int freed = 1;
            INFO(std::to_string(freed) + "個解放する");
            for (int i = 0; i < freed; ++i) {
                pool_free(pool, ptrs[i]);
            }

            THEN("メモリが解放できること") {
                size_t freeable = capacity - allocated + freed;
                INFO("空き容量が " + std::to_string(freeable));
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < allocated - freed; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }

        pool_release(pool);
    }

    GIVEN("プールを使用する") {
        size_t capacity = 5;
        INFO("プール容量: " + std::to_string(capacity));
        int allocated = 4;
        void *ptrs[allocated];
        INFO("取得: " + std::to_string(allocated));

        POOL pool = pool_init(sizeof(int), capacity);
        REQUIRE(pool != NULL);
        for (int i = 0; i < allocated; ++i) {
            ptrs[i] = pool_alloc(pool);
        }
        REQUIRE(pool_freeable(pool) == capacity - allocated);

        WHEN("メモリを解放する") {
            int freed = 1;
            INFO(std::to_string(freed) + "個解放する");
            for (int i = 0; i < freed; ++i) {
                pool_free(pool, ptrs[i]);
            }

            THEN("メモリが解放できること") {
                size_t freeable = capacity - allocated + freed;
                INFO("空き容量が " + std::to_string(freeable));
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < allocated - freed; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }
        WHEN("メモリを解放する") {
            int freed = 3;
            INFO(std::to_string(freed) + "個解放する");
            for (int i = 0; i < freed; ++i) {
                pool_free(pool, ptrs[i]);
            }

            THEN("メモリが解放できること") {
                size_t freeable = capacity - allocated + freed;
                INFO("空き容量が " + std::to_string(freeable));
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < allocated - freed; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }

        pool_release(pool);
    }

    GIVEN("プールを使用する") {
        size_t capacity = 5;
        INFO("プール容量: " + std::to_string(capacity));
        int allocated = 5;
        void *ptrs[allocated];
        INFO("取得: " + std::to_string(allocated));

        POOL pool = pool_init(sizeof(int), capacity);
        REQUIRE(pool != NULL);
        for (int i = 0; i < allocated; ++i) {
            ptrs[i] = pool_alloc(pool);
        }
        REQUIRE(pool_freeable(pool) == capacity - allocated);

        WHEN("メモリを解放する") {
            int freed = 2;
            INFO(std::to_string(freed) + "個解放する");
            for (int i = 0; i < freed; ++i) {
                pool_free(pool, ptrs[i]);
            }

            THEN("メモリが解放できること") {
                size_t freeable = capacity - allocated + freed;
                INFO("空き容量が " + std::to_string(freeable));
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < allocated - freed; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }
        WHEN("メモリを解放する") {
            int freed = 5;
            INFO(std::to_string(freed) + "個解放する");
            for (int i = 0; i < freed; ++i) {
                pool_free(pool, ptrs[i]);
            }

            THEN("メモリが解放できること") {
                size_t freeable = capacity - allocated + freed;
                INFO("空き容量が " + std::to_string(freeable));
                REQUIRE(pool_freeable(pool) == freeable);
            }

            for (int i = 0; i < allocated - freed; ++i) {
                pool_free(pool, ptrs[i]);
            }
        }

        pool_release(pool);
    }
}

SCENARIO("メモリプールをクリアする", "[pool][clear]") {
    GIVEN("プールを使用する") {
        size_t capacity = 5;
        INFO("プール容量: " + std::to_string(capacity));
        int allocated = 1;
        INFO("取得: " + std::to_string(allocated));

        POOL pool = pool_init(sizeof(int), capacity);
        REQUIRE(pool != NULL);
        for (int i = 0; i < allocated; ++i) {
            (void)pool_alloc(pool);
        }
        REQUIRE(pool_freeable(pool) == capacity - allocated);

        WHEN("プールをクリアする") {
            pool_clear(pool);

            THEN("メモリが解放できること") {
                INFO("空き容量が " + std::to_string(capacity));
                REQUIRE(pool_freeable(pool) == capacity);
            }
        }

        pool_release(pool);
    }

    GIVEN("プールを使用する") {
        size_t capacity = 5;
        INFO("プール容量: " + std::to_string(capacity));
        int allocated = 5;
        INFO("取得: " + std::to_string(allocated));

        POOL pool = pool_init(sizeof(int), capacity);
        REQUIRE(pool != NULL);
        for (int i = 0; i < allocated; ++i) {
            (void)pool_alloc(pool);
        }
        REQUIRE(pool_freeable(pool) == capacity - allocated);

        WHEN("プールをクリアする") {
            pool_clear(pool);

            THEN("メモリが解放できること") {
                INFO("空き容量が " + std::to_string(capacity));
                REQUIRE(pool_freeable(pool) == capacity);
            }
        }

        pool_release(pool);
    }
}

SCENARIO("リストが初期化できること", "[list][init]") {
    GIVEN("特になし") {
        WHEN("リストを初期化する") {
            LIST list = list_init(sizeof(int), 5);

            THEN("インスタンスが NULL ではないこと") {
                REQUIRE(list != NULL);
            }

            list_release(list);
        }
    }
}

SCENARIO("キューが初期化できること", "[queue][init]") {
    GIVEN("特になし") {
        WHEN("キューを容量 0 で初期化する") {
            QUEUE que = queue_init(sizeof(int), 0);

            THEN("インスタンスが NULL となる") {
                REQUIRE(que == NULL);
            }

            queue_release(que);
        }

        WHEN("キューを容量 5 で初期化する") {
            QUEUE que = queue_init(sizeof(int), 5);

            THEN("インスタンスが NULL ではないこと") {
                REQUIRE(que != NULL);
            }

            queue_release(que);
        }

        WHEN("キューを容量 500 で初期化する") {
            QUEUE que = queue_init(sizeof(int), 500);

            THEN("インスタンスが NULL ではないこと") {
                REQUIRE(que != NULL);
            }

            queue_release(que);
        }
    }
}

SCENARIO("キューに要素が追加できること", "[queue][enq]") {
    GIVEN("キューを容量 5 で初期化しておく") {
        QUEUE que = queue_init(sizeof(int), 5);

        WHEN("要素を追加しない") {
            THEN("キューの長さが 0 であること") {
                REQUIRE(queue_count(que) == 0);
            }
        }

        WHEN("要素を 1 つ追加する") {
            int a = 0x55;
            REQUIRE(queue_enq(que, &a) != NULL);

            THEN("キューの長さが 1 であること") {
                REQUIRE(queue_count(que) == 1);
            }
        }

        WHEN("要素を 5 つ追加する") {
            for (int i = 0; i < 5; ++i) {
                REQUIRE(queue_enq(que, &i) != NULL);
            }

            THEN("キューの長さが 5 であること") {
                REQUIRE(queue_count(que) == 5);
            }
        }

        WHEN("要素を 6 つ追加する") {
            for (int i = 0; i < 5; ++i) {
                REQUIRE(queue_enq(que, &i) != NULL);
            }

            THEN("6 つ目の追加に失敗すること") {
                int a = 0x55;
                REQUIRE(queue_enq(que, &a) == NULL);
            }

            THEN("キューの長さが 5 であること") {
                int a = 0x55;
                queue_enq(que, &a);
                REQUIRE(queue_count(que) == 5);
            }
        }

        queue_release(que);
    }
}

SCENARIO("キューから要素を取り出せること", "[queue][deq]") {
    GIVEN("キューを容量 5 で初期化しておく") {
        QUEUE que = queue_init(sizeof(int), 5);

        WHEN("要素を追加しない") {
            THEN("要素が取り出せないこと") {
                int a = -1;
                REQUIRE(queue_deq(que, &a) == -1);
            }
        }

        WHEN("要素を 1 つ追加する") {
            int a = 0x55;
            REQUIRE(queue_enq(que, &a) != NULL);

            THEN("追加順に要素が取り出せること") {
                int b = 0;
                REQUIRE(queue_deq(que, &b) == 0);
                REQUIRE(b == 0x55);
            }
        }

        WHEN("要素を 5 つ追加する") {
            for (int i = 0; i < 5; ++i) {
                REQUIRE(queue_enq(que, &i) != NULL);
            }

            THEN("追加順に要素が取り出せること") {
                for (int i = 0; i < 5; ++i) {
                    int b = 0;
                    REQUIRE(queue_deq(que, &b) == (4 - i));
                    REQUIRE(b == i);
                }
            }
        }

        WHEN("要素の追加と取り出しを繰り返す") {
            int a = 0;
            REQUIRE(queue_enq(que, &a) != NULL);
            a++;

            THEN("追加順に要素が取り出せること") {
                int b = 0;
                REQUIRE(queue_deq(que, &b) == 0);
                REQUIRE(b == 0);

                REQUIRE(queue_enq(que, &a) != NULL);
                a++;
                REQUIRE(queue_enq(que, &a) != NULL);
                a++;
                REQUIRE(queue_deq(que, &b) == 1);
                REQUIRE(b == 1);
                REQUIRE(queue_deq(que, &b) == 0);
                REQUIRE(b == 2);

                REQUIRE(queue_enq(que, &a) != NULL);
                a++;
                REQUIRE(queue_enq(que, &a) != NULL);
                a++;
                REQUIRE(queue_enq(que, &a) != NULL);
                a++;
                REQUIRE(queue_deq(que, &b) == 2);
                REQUIRE(b == 3);
                REQUIRE(queue_deq(que, &b) == 1);
                REQUIRE(b == 4);
                REQUIRE(queue_deq(que, &b) == 0);
                REQUIRE(b == 5);

                REQUIRE(queue_enq(que, &a) != NULL);
                a++;
                REQUIRE(queue_enq(que, &a) != NULL);
                a++;
                REQUIRE(queue_enq(que, &a) != NULL);
                a++;
                REQUIRE(queue_enq(que, &a) != NULL);
                a++;
                REQUIRE(queue_deq(que, &b) == 3);
                REQUIRE(b == 6);
                REQUIRE(queue_deq(que, &b) == 2);
                REQUIRE(b == 7);
                REQUIRE(queue_deq(que, &b) == 1);
                REQUIRE(b == 8);
                REQUIRE(queue_deq(que, &b) == 0);
                REQUIRE(b == 9);
            }
        }

        queue_release(que);
    }
}

SCENARIO("ツリーが初期化できること", "[ntree][init]") {
    GIVEN("特になし") {
        WHEN("ツリーを初期化する") {
            size_t capacity = 1;
            INFO("容量: " + std::to_string(capacity));

            NTREE tree = ntree_init(sizeof(int), capacity);

            THEN("インスタンスが NULL ではないこと") {
                REQUIRE(tree != NULL);
            }

            ntree_release(tree);
        }

        WHEN("ツリーで初期化する") {
            size_t capacity = 5;
            INFO("容量: " + std::to_string(capacity));

            NTREE tree = ntree_init(sizeof(int), capacity);

            THEN("インスタンスが NULL ではないこと") {
                REQUIRE(tree != NULL);
            }

            ntree_release(tree);
        }
    }
}

SCENARIO("ツリーに要素が追加できること", "[ntree][insert]") {
    GIVEN("ツリーを初期化しておく") {
        size_t capacity = 1;
        INFO("容量: " + std::to_string(capacity));

        NTREE tree = ntree_init(sizeof(int), capacity);

        WHEN("要素を追加しない") {
            THEN("ツリーの要素数が 0 であること") {
                REQUIRE(ntree_count(tree) == 0);
            }
        }

        WHEN("要素を root に追加する") {
            /*  root
             *  /
             * 10
             */
            int count = 1;
            int data[count] = {10};
            INFO("追加する要素: [10]");

            NTREE_NODE nodes[count];
            for (int i = 0; i < count; ++i) {
                nodes[i] = ntree_insert(tree, &data[0]);
            }

            THEN("要素が追加できていること") {
                REQUIRE(ntree_data(nodes[0]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[0]) == data[0]);
            }
            THEN("ツリーの要素数が 1 であること") {
                REQUIRE(ntree_count(tree) == 1);
            }
        }

        WHEN("要素を root に追加する") {
            /*  root
             *  /
             * 11-101?
             */
            int count = 2;
            int data[count] = {11,101};
            INFO("追加する要素: [11, 101?]");

            NTREE_NODE nodes[count];
            for (int i = 0; i < count; ++i) {
                nodes[i] = ntree_insert(tree, &data[0]);
            }

            THEN("要素が追加できていること") {
                REQUIRE(ntree_data(nodes[0]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[0]) == data[0]);
                REQUIRE(nodes[1] == NULL);
            }
            THEN("ツリーの要素数が 1 であること") {
                REQUIRE(ntree_count(tree) == 1);
            }
        }

        ntree_release(tree);
    }

    GIVEN("ツリーを初期化しておく") {
        size_t capacity = 5;
        INFO("容量: " + std::to_string(capacity));

        NTREE tree = ntree_init(sizeof(int), capacity);

        WHEN("要素を root に追加する") {
            /*  root
             *  /
             * 10
             */
            int count = 1;
            int data[count] = {10};
            INFO("追加する要素: [10]");

            NTREE_NODE nodes[count];
            for (int i = 0; i < count; ++i) {
                nodes[i] = ntree_insert(tree, &data[0]);
            }

            THEN("要素が追加できていること") {
                REQUIRE(ntree_data(nodes[0]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[0]) == data[0]);
            }
            THEN("ツリーの要素数が 1 であること") {
                REQUIRE(ntree_count(tree) == 1);
            }
        }

        WHEN("要素を root に追加する") {
            /*  root
             *  /
             * 0-1-2-3-4
             */
            int count = 5;
            int data[count] = {0,1,2,3,4};
            INFO("追加する要素: [0, 1, 2, 3, 4]");

            NTREE_NODE nodes[count];
            for (int i = 0; i < count; ++i) {
                nodes[i] = ntree_insert(tree, &data[i]);
            }

            THEN("要素が追加できていること") {
                REQUIRE(ntree_data(nodes[0]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[0]) == data[0]);
                REQUIRE(ntree_data(nodes[1]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[1]) == data[1]);
                REQUIRE(ntree_data(nodes[2]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[2]) == data[2]);
                REQUIRE(ntree_data(nodes[3]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[3]) == data[3]);
                REQUIRE(ntree_data(nodes[4]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[4]) == data[4]);
            }

            THEN("ツリーの要素数が 5 であること") {
                REQUIRE(ntree_count(tree) == 5);
            }
        }

        WHEN("要素を階層的に追加する") {
            /*       root
             *        /
             *       0
             *      /
             *     1-2
             *    /
             *   3
             *  /
             * 4
             */
            int count = 5;
            int data[count] = {0,1,2,3,4};
            INFO("追加する要素: [0: [1: [3: [4]], 2]]");

            NTREE_NODE nodes[count];
            nodes[0] = ntree_insert_at(tree, NULL, &data[0]);
            nodes[1] = ntree_insert_at(tree, nodes[0], &data[1]);
            nodes[2] = ntree_insert_at(tree, nodes[0], &data[2]);
            nodes[3] = ntree_insert_at(tree, nodes[1], &data[3]);
            nodes[4] = ntree_insert_at(tree, nodes[3], &data[4]);

            THEN("要素が追加できていること") {
                REQUIRE(ntree_data(nodes[0]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[0]) == data[0]);
                REQUIRE(ntree_data(nodes[1]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[1]) == data[1]);
                REQUIRE(ntree_data(nodes[2]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[2]) == data[2]);
                REQUIRE(ntree_data(nodes[3]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[3]) == data[3]);
                REQUIRE(ntree_data(nodes[4]) != NULL);
                REQUIRE(*(int *)ntree_data(nodes[4]) == data[4]);
            }
            THEN("ツリーの要素数が 5 であること") {
                REQUIRE(ntree_count(tree) == 5);
            }
        }

        ntree_release(tree);
    }
}

SCENARIO("ツリーを反復子で処理できること", "[ntree][iterator]") {
    GIVEN("ツリーを初期化しておく") {
        size_t capacity = 5;
        INFO("容量: " + std::to_string(capacity));

        NTREE tree = ntree_init(sizeof(int), capacity);

        WHEN("要素を root に追加する") {
            /*  root
             *  /
             * 0-1-2-3-4
             */
            int count = 5;
            int data[count] = {0,1,2,3,4};
            INFO("追加する要素: [0, 1, 2, 3, 4]");

            for (int i = 0; i < count; ++i) {
                ntree_insert(tree, &data[i]);
            }

            THEN("反復子でツリーに沿った順に値が取得できること") {
                ITER iter = ntree_iter(tree);
                REQUIRE(iter_is_end(iter) != true);

                REQUIRE(*(int *)iter_data(iter) == data[0]);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[1]);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[2]);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[3]);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[4]);

                iter_release(iter);
            }
        }

        WHEN("要素を階層的に追加する") {
            /*       root
             *        /
             *       0
             *      /
             *     1-2
             *    /
             *   3
             *  /
             * 4
             */
            int count = 5;
            int data[count] = {0,1,2,3,4};
            INFO("追加する要素: [0: [1: [3: 4], 2]]");

            NTREE_NODE node0, node1, node3;
            node0 = ntree_insert(tree, &data[0]);
            node1 = ntree_insert_at(tree, node0, &data[1]);
            ntree_insert_at(tree, node0, &data[2]);
            node3 = ntree_insert_at(tree, node1, &data[3]);
            ntree_insert_at(tree, node3, &data[4]);

            THEN("反復子でツリーに沿った順に値が取得できること") {
                ITER iter = ntree_iter(tree);
                REQUIRE(iter_is_end(iter) != true);

                REQUIRE(*(int *)iter_data(iter) == data[0]);
                REQUIRE(ntree_iter_age(iter) == 1);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[1]);
                REQUIRE(ntree_iter_age(iter) == 2);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[3]);
                REQUIRE(ntree_iter_age(iter) == 3);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[4]);
                REQUIRE(ntree_iter_age(iter) == 4);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[2]);
                REQUIRE(ntree_iter_age(iter) == 2);

                iter_release(iter);
            }

            THEN("0 の子を反復子で順に値が取得できること") {
                ITER iter = ntree_children_iter(node0);
                REQUIRE(iter_is_end(iter) != true);

                REQUIRE(*(int *)iter_data(iter) == data[1]);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[2]);
                REQUIRE(iter_is_end(iter_next(iter)) == true);
            }
        }

        WHEN("要素を階層的に追加する") {
            /*     root
             *      /
             *     0
             *    /
             *   1-2
             *  / /
             * 4 3
             */
            int count = 5;
            int data[count] = {0,1,2,3,4};
            INFO("追加する要素: [0: [1: 4], [2: 3]]");

            NTREE_NODE node0, node1, node2;
            node0 = ntree_insert(tree, &data[0]);
            node1 = ntree_insert_at(tree, node0, &data[1]);
            node2 = ntree_insert_at(tree, node0, &data[2]);
            ntree_insert_at(tree, node2, &data[3]);
            ntree_insert_at(tree, node1, &data[4]);

            THEN("反復子でツリーに沿った順に値が取得できること") {
                ITER iter = ntree_iter(tree);
                REQUIRE(iter_is_end(iter)!= true);

                REQUIRE(*(int *)iter_data(iter) == data[0]);
                REQUIRE(ntree_iter_age(iter) == 1);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[1]);
                REQUIRE(ntree_iter_age(iter) == 2);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[4]);
                REQUIRE(ntree_iter_age(iter) == 3);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[2]);
                REQUIRE(ntree_iter_age(iter) == 2);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[3]);
                REQUIRE(ntree_iter_age(iter) == 3);

                REQUIRE(iter_is_end(iter_next(iter)) == true);
            }

            THEN("0 の子を反復子で順に値が取得できること") {
                ITER iter = ntree_children_iter(node0);
                REQUIRE(iter_is_end(iter) != true);

                REQUIRE(*(int *)iter_data(iter) == data[1]);
                iter = iter_next(iter);
                REQUIRE(*(int *)iter_data(iter) == data[2]);
                REQUIRE(iter_is_end(iter_next(iter)) == true);
            }
        }

        ntree_release(tree);
    }
}

SCENARIO("ツリーから要素が削除できること", "[ntree][remove]") {
    GIVEN("要素を root に追加したツリーを用意する") {
        /* root
         *  /
         * 10
         */
        size_t capacity = 5;
        int data = 10;
        INFO("追加する要素: [10]");

        NTREE tree = ntree_init(sizeof(int), capacity);
        NTREE_NODE node = ntree_insert(tree, &data);

        WHEN("要素を削除する") {
            INFO("削除する要素: 10");

            REQUIRE(ntree_remove(tree, node) == 0);

            THEN("削除できること") {
                REQUIRE(ntree_count(tree) == 0);
            }
        }

        ntree_release(tree);
    }

    GIVEN("要素を root に追加したツリーを用意する") {
        /*  root
         *  /
         * 0-1-2-3-4
         */
        size_t capacity = 5;
        int data[capacity] = {0,1,2,3,4};
        INFO("追加する要素: [0, 1, 2, 3, 4]");

        NTREE tree = ntree_init(sizeof(int), capacity);
        NTREE_NODE nodes[capacity];
        for (int i = 0; i < 5; ++i) {
            nodes[i] = ntree_insert(tree, &data[i]);
        }

        WHEN("要素を削除する") {
            INFO("削除する要素: 1");

            REQUIRE(ntree_remove(tree, nodes[1]) == 0);

            THEN("ツリーが [0, 2, 3, 4] となること") {
                REQUIRE(ntree_count(tree) == 4);

                ITER iter = ntree_iter(tree);
                REQUIRE(*(int *)iter_data(iter) == 0);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 2);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 3);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 4);
                iter_release(iter);
            }
        }

        WHEN("要素を削除する") {
            INFO("削除する要素: 3");

            REQUIRE(ntree_remove(tree, nodes[3]) == 0);

            THEN("ツリーが [0, 1, 2, 4] となること") {
                REQUIRE(ntree_count(tree) == 4);

                ITER iter = ntree_iter(tree);
                REQUIRE(*(int *)iter_data(iter) == 0);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 1);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 2);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 4);
                iter_release(iter);
            }
        }

        ntree_release(tree);
    }

    GIVEN("要素を root に追加したツリーを用意する") {
        /*  root
         *  /
         * 0-1-2-3-4
         *      /
         *     5-6
         */
        size_t capacity = 10;
        int data[capacity] = {0,1,2,3,4,5,6};
        INFO("追加する要素: [0, 1, 2, 3: [5, 6], 4]");

        NTREE tree = ntree_init(sizeof(int), capacity);
        NTREE_NODE nodes[capacity];
        int i = 0;
        while (i < 5) {
            nodes[i] = ntree_insert(tree, &data[i]);
            ++i;
        }
        while (i < 7) {
            nodes[i] = ntree_insert_at(tree, nodes[3], &data[i]);
            ++i;
        }

        WHEN("要素を削除する") {
            INFO("削除する要素: 2");

            REQUIRE(ntree_remove(tree, nodes[2]) == 0);

            THEN("ツリーが [0, 1, 3: [5, 6], 4] となること") {
                REQUIRE(ntree_count(tree) == 6);

                ITER iter = ntree_iter(tree);
                REQUIRE(*(int *)iter_data(iter) == 0);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 1);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 3);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 5);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 6);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 4);
                iter_release(iter);
            }
        }

        WHEN("要素を削除する") {
            INFO("削除する要素: 3");

            REQUIRE(ntree_remove(tree, nodes[3]) == 0);

            THEN("ツリーが [0, 1, 2, 4] となること") {
                REQUIRE(ntree_count(tree) == 4);

                ITER iter = ntree_iter(tree);
                REQUIRE(*(int *)iter_data(iter) == 0);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 1);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 2);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 4);
                iter_release(iter);
            }
        }

        WHEN("要素を削除する") {
            INFO("削除する要素: 5");

            REQUIRE(ntree_remove(tree, nodes[5]) == 0);

            THEN("ツリーが [0, 1, 2, 3: [6], 4] となること") {
                REQUIRE(ntree_count(tree) == 6);

                ITER iter = ntree_iter(tree);
                REQUIRE(*(int *)iter_data(iter) == 0);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 1);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 2);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 3);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 6);
                REQUIRE(*(int *)iter_data(iter_next(iter)) == 4);
                iter_release(iter);
            }
        }

        ntree_release(tree);
    }
}
