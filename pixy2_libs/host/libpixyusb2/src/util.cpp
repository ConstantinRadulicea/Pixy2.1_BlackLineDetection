#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include "util.h"
#include <chrono>


uint32_t millis()
{
  static auto start = std::chrono::high_resolution_clock::now();
  //uint64_t c = clock();
  //c *= 1000;
  //c /= CLOCKS_PER_SEC;
  //return c;
  std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
  return (uint32_t)(duration.count() * 1000.0);
}

void delayMicroseconds(uint32_t us)
{
  // Called from TPixy2 class --  not needed because we are using USB not serial
}

Console Serial;


void Console::print(const char *msg)
{
  printf("%s", msg);
}

void Console::println(const char *msg)
{
  printf("%s\n", msg);
}


