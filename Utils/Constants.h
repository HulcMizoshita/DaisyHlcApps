#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

enum : u_int8_t {
  kTabA = 0,
  kTabB,
};

enum : u_int8_t {
  kRowStateEx = 0,
  kRowStateInFront,
  kRowStateInBack,
};

enum : u_int8_t {
  kAppTypeVca = 0,
  kNumAppType
};
#endif