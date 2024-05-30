#ifndef _UTCCLOCK_H
#define _UTCCLOCK_H

#include <chrono>
#include <iostream>

// Define a custom clock type
class UTCClock {
public:
  typedef std::chrono::duration<int64_t> duration;
  typedef std::chrono::time_point<UTCClock> time_point;
  static const bool is_steady = true;

  static time_point now() noexcept {
    return time_point(std::chrono::duration_cast<duration>(
      std::chrono::system_clock::now().time_since_epoch()
    ));
  }
};

#endif