/** @file   utils.cpp
 *  @brief  Test for Utilities.
 *
 *  @author t-kenji <protect.2501@gmail.com>
 *  @date   2018-11-24 create new.
 */
#include "catch2/catch.hpp"

extern "C" {
#include "debug.h"
#include "utils.h"
}

SCENARIO("パターンによる文字列の一致が検出出来ること", "[utils][string][match]") {

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "aaa";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "aaa";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "aba";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "aa";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "aaaa";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "aa?";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "a*";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "a*a";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "a*b";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "*a";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "*b";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }
    }

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "ABCDEfg";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "abcdefg";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "abcDefg";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "ABCDEfg";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }
    }

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "\"value\"";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"*\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }
    }

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "\"va?lue\"";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"va\\*lue\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }
    }

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "\"va*lue\"";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"va\\*lue\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }
    }

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "\"va\\\"lue\"";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"*\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }
    }

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "'value'";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "'*'";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }
    }

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "\"\"\"value\"\"\"";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"*\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"\"*\"\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"\"\"*\"\"\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"+\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"\"+\"\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"\"\"+\"\"\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }
    }

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "'''value'''";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "'*'";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "''*''";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "'''*'''";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "'+'";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "''+''";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致しないこと") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == false);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "'''+'''";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }
    }

    GIVEN("入力文字列を以下とする") {
        std::string input_str = "\"a\\\"b\\tc\"";
        INFO("入力文字列: '" + input_str + "'");

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"*\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }

        WHEN("パターンを以下とする") {
            std::string pattern_str = "\"+\"";
            INFO("パターン: '" + pattern_str + "'");

            THEN("一致すること") {
                REQUIRE(string_match(pattern_str.c_str(), input_str.c_str()) == true);
            }
        }
    }
}
