/** @file   main.cpp
 *  @brief  Catch2 startup routine.
 *
 *  @author t-kenji <protect.2501@gmail.com>
 *  @date   2018-11-19 create new.
 *  @copyright  Copyright (c) 2018 t-kenji
 *
 *  This code is licensed under the MIT License.
 */
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

/**
 *  startup routine.
 *
 *  @param  [in]    argc    command-line argument count.
 *  @param  [in]    argv    command-line argument vector.
 *  @return return zero if success, return -1 if failed.
 */
int main(int argc, char **argv)
{
    return Catch::Session().run(argc, argv);
}
