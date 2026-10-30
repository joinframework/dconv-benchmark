#pragma once

#include "common/datasets.hpp"

static const Dataset canada = makeCanada (DATA_DIR "/canada.json");
static const Dataset randomBits = makeRandom ();
static const Dataset unit = makeUnit ();
static const Dataset integer = makeInteger ();
static const Dataset digits[17] = {
    makeDigits (1),  makeDigits (2),  makeDigits (3),  makeDigits (4),  makeDigits (5),  makeDigits (6),
    makeDigits (7),  makeDigits (8),  makeDigits (9),  makeDigits (10), makeDigits (11), makeDigits (12),
    makeDigits (13), makeDigits (14), makeDigits (15), makeDigits (16), makeDigits (17),
};

#define REGISTER_DATASETS(REGISTER, Adapter)       \
    REGISTER (Adapter, Canada, &canada);           \
    REGISTER (Adapter, Random, &randomBits);       \
    REGISTER (Adapter, Unit, &unit);               \
    REGISTER (Adapter, Integer, &integer);         \
    REGISTER (Adapter, Digits01, &digits[0]);      \
    REGISTER (Adapter, Digits02, &digits[1]);      \
    REGISTER (Adapter, Digits03, &digits[2]);      \
    REGISTER (Adapter, Digits04, &digits[3]);      \
    REGISTER (Adapter, Digits05, &digits[4]);      \
    REGISTER (Adapter, Digits06, &digits[5]);      \
    REGISTER (Adapter, Digits07, &digits[6]);      \
    REGISTER (Adapter, Digits08, &digits[7]);      \
    REGISTER (Adapter, Digits09, &digits[8]);      \
    REGISTER (Adapter, Digits10, &digits[9]);      \
    REGISTER (Adapter, Digits11, &digits[10]);     \
    REGISTER (Adapter, Digits12, &digits[11]);     \
    REGISTER (Adapter, Digits13, &digits[12]);     \
    REGISTER (Adapter, Digits14, &digits[13]);     \
    REGISTER (Adapter, Digits15, &digits[14]);     \
    REGISTER (Adapter, Digits16, &digits[15]);     \
    REGISTER (Adapter, Digits17, &digits[16]);
