#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <cfenv> // Include to handle floating-point exceptions
#include "TrackingData.h"

bool setup() {
  // Clear all floating point exceptions
  std::feclearexcept(FE_ALL_EXCEPT);

  // Enable divide-by-zero exceptions
#ifdef __APPLE__
// does not have feenableexcept 
#else
  std::feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif

  return true;
}

std::vector<char> readFile(const std::string& fileName) {
  std::ifstream file(fileName, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file");
  }
  return std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

template<typename T>
T deserialize(const std::vector<char>& data, size_t& offset) {
  T value;
  memcpy(&value, data.data() + offset, sizeof(T));
  offset += sizeof(T);
  return value;
}

template<typename T>
void serialize(const T& value, std::ofstream& out) {
  out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

TrackingData initializeFromData(const std::vector<char>& data) {
  size_t offset = 0;
  TrackingData obj;
  obj.setValue1(deserialize<int32_t>(data, offset));
  obj.setValue2(deserialize<int32_t>(data, offset));
  obj.setValue3(deserialize<double>(data, offset));
  obj.setTimestamp(UTCClock::time_point(UTCClock::duration(deserialize<int64_t>(data, offset))));
  return obj;
}

int print(const TrackingData& obj) {
  std::cout << "value1: " << obj.getValue1() << " "
    << "value2: " << obj.getValue2() << " "
    << "value3: " << obj.getValue3() << " "
    << "timestamp: " << obj.getTimestamp().time_since_epoch().count() << " custom duration units" << std::endl;
  return 0;
}

int process(const TrackingData& obj) {
  // Do something with the object
  return 0;
}

int stringToBinary(const std::string& asciiFileName, const std::string& binaryFileName) {
  int32_t a, b;
  double c;
  int64_t timestamp;

  std::ifstream asciiFile(asciiFileName);
  std::ofstream binaryFile(binaryFileName, std::ios::binary);

  if (!asciiFile.is_open() || !binaryFile.is_open()) {
    throw std::runtime_error("Could not open file");
  }

  // Read values in order and write to binary file
  asciiFile >> a >> b >> c >> timestamp;
  serialize(a, binaryFile);
  serialize(b, binaryFile);
  serialize(c, binaryFile);
  serialize(timestamp, binaryFile);

  asciiFile.close();
  binaryFile.close();
  return 0;
}

#ifndef LIBFUZZER_FUZZ
int main(int argc, char* argv[]) {
  setup();  // Enable floating point exceptions

  if (argc == 4 && strcmp(argv[1], "--convert") == 0) {
    return stringToBinary(argv[2], argv[3]);
  }

  if (argc == 3 && strcmp(argv[1], "--print") == 0) {
    std::vector<char> data = readFile(argv[2]);
    TrackingData myObject = initializeFromData(data);
    return print(myObject);
  }

  if (argc == 2) {
    std::vector<char> data = readFile(argv[1]);
    TrackingData myObject = initializeFromData(data);
    return process(myObject);
  }

  std::cout << "Usage: " << argv[0] << " [--convert <ascii_file> <binary_file>] | [--print <binary_file>] | [<binary_file>]" << std::endl;

  return 0;
}
#endif

#ifdef LIBFUZZER_FUZZ
extern "C" int LLVMFuzzerTestOneInput(const uint8_t * data, size_t size) {
  setup();  // Enable floating point exceptions

  std::vector<char> inputData(data, data + size);
  TrackingData myObject = initializeFromData(inputData);
  return process(myObject);
}
#endif
