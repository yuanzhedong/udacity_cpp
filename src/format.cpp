#include "format.h"

#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

string Format::ConvertToString(int input) {
  string res;
  if (input < 10) {
    res = "0" + std::to_string(input);
  } else {
    res = std::to_string(input);
  }
  return res;
}

string Format::ElapsedTime(long int seconds) {
  int h = seconds / 3600;
  int m = seconds % 3600 / 60;
  int s = seconds % 3600 % 60;
  string res;
  res += Format::ConvertToString(h);
  res += ":";
  res += Format::ConvertToString(m);
  res += ":";
  res += Format::ConvertToString(s);
  return res;
}