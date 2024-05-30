#ifndef _TRACKING_DATA_H
#define _TRACKING_DATA_H

#include <stdint.h>
#include "UTCClock.h"

class TrackingData {
public:
  TrackingData() = default;
  ~TrackingData() = default;

  void setValue1(int32_t value) { value1 = value; }
  void setValue2(int32_t value) { value2 = value; }
  void setValue3(double value) { value3 = value; }
  void setTimestamp(UTCClock::time_point value) { timestamp = value; }


  int32_t getValue1() const { return value1; }
  int32_t getValue2() const { return value2; }
  double getValue3() const { return value3; }
  UTCClock::time_point getTimestamp() const { return timestamp; }

private:
  int32_t value1;
  int32_t value2;
  double value3;
  UTCClock::time_point timestamp;
};

#endif // _TRACKING_DATA_H
