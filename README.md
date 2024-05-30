# Analyzing C++ Code With Mayhem

This project demonstrates how to analyze a C++ program that involves a class
file using Mayhem. The example provided includes a driver program that
initializes an object of a class with data read from a binary file. The project
includes functions for serializing and deserializing the class object, as well
as converting human-readable ASCII input files to binary format. 

## Project Structure

- `UTCClock.h`: Defines a custom clock type used in `TrackingData`.
- `TrackingData.h`: Defines the `TrackingData` class with various member variables.
- `driver.cpp`: The main driver program that initializes `TrackingData`
  objects, processes them, and includes functions for serialization and
  deserialization.
- `Makefile`: The build script to compile the project to a binary.
- `sample_input.txt`: An example ASCII input file.
- `inputs/`: Directory containing the initial binary input files for AFL.

## Getting Started

### Prerequisites

- g++ compiler
- make utility
- docker utility


### Usage

1. **Modify `TrackingData.h` to match your class:**

  This is an example class file with typical setters and getters. You will be
  using your own classes.  

  - We assume each data type has a setter and a getter.
  - Note that the class does not have pointers.  The way we
    serialize/deserialize does not handle pointer values.

  If you have pointers, then you will need to define a utility class like
  `TrackingData.h` without pointers, and then create an instance of your
  desired class from it. 


2. **Update `initializeFromData` in `driver.cpp`:**

    In `driver.cpp`, update the `initializeFromData` function to correctly deserialize each member of your class from the binary data.

3. **Compile the project:**

    Use the provided `Makefile` to compile the project:

    ```sh
    make
    ```

4. **Convert ASCII input to binary format:**

    Use the `--convert` option of the driver program to create an initial binary input file:

    ```sh
    ./driver --convert sample_input.txt sample_input.bin
    mkdir inputs
    cp sample_input.bin inputs/
    ```

5. **Make a docker image:**

  To be written. 

## Adding New Class Files

To analyze a new class, follow these steps:

1. **Create a Header File:**

    Define your class in a header file similar to `TrackingData.h`. Ensure to include getter and setter methods for each member variable.

    ```cpp
    #ifndef _NEW_CLASS_H
    #define _NEW_CLASS_H

    class NewClass {
    public:
      NewClass() = default;
      ~NewClass() = default;

      // Add your setters and getters
      void setValue(int value) { this->value = value; }
      int getValue() const { return value; }

    private:
      int value;
    };

    #endif // _NEW_CLASS_H
    ```

2. **Update `initializeFromData`:**

    In `driver.cpp`, modify the `initializeFromData` function to initialize an object of your new class.

    ```cpp
    NewClass initializeFromData(const std::vector<char>& data) {
      size_t offset = 0;
      NewClass obj;
      obj.setValue(deserialize<int>(data, offset));
      // Deserialize and set other members...
      return obj;
    }
    ```

3. **Update Serialization/Deserialization:**

    Ensure that your serialization and deserialization functions in `driver.cpp` handle all the member variables of your new class.

4. **Update `Makefile`:**

    Ensure that your `Makefile` includes your new header file.

    ```makefile
    driver.o: driver.cpp NewClass.h UTCClock.h
    ```

5. **Compile and Run:**

    Follow the steps in the Usage section to compile the project and run with
    the new Class.

## Other ways to do this

### Alternate 1: Subclass

Here we are reading in values and creating an instance of the class. If you're
class isn't amenable to this, consider defining a subclass:

```c++
#include <fstream>
#include <iostream>
#include <string>

class TrackingDataReader : public TrackingData {
public:
    TrackingDataReader() : TrackingData() { }

    // Method to read values from a file and set them
    bool readFromFile(const std::string& filename) {
        int32_t value1, value2;
        double value3;

        std::ifstream infile(filename);
        if (!infile.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        infile >> value1 >> value2 >> value3;
        if (infile.fail()) {
            std::cerr << "Error reading values from file" << std::endl;
            return false;
        }
        infile.close();

        // Use the base class's setters to set the values
        setValue1(value1);
        setValue2(value2);
        setValue3(value3);

        return true;
    }
};
```
and then use the subclass in your code:
```
int main() {
    TrackingDataReader dataReader;

    std::string filename = "data.txt";

    // Read values from the file and set them in the TrackingData object
    if (dataReader.readFromFile(filename)) {
        std::cout << "Values read and set successfully." << std::endl;
    } else {
        std::cerr << "Failed to read and set values." << std::endl;
    }

    return 0;
}
```

### Alternate 2: Use protected inheritence

```cpp
#include <fstream>
#include <iostream>
#include <string>

class TrackingData {
public:
    TrackingData() {}
    ~TrackingData() {}

    void setValue1(int32_t value) { this->value1 = value; }
    void setValue2(int32_t value) { this->value2 = value; }
    void setValue3(double value) { this->value3 = value; }

private:
    int32_t value1;
    int32_t value2;
    double value3;
};

class TrackingDataReader : protected TrackingData {
public:
    TrackingDataReader() : TrackingData() {}

    // Method to read values from a file and set them
    bool readFromFile(const std::string& filename) {
        int32_t value1, value2;
        double value3;

        std::ifstream infile(filename);
        if (!infile.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        infile >> value1 >> value2 >> value3;
        if (infile.fail()) {
            std::cerr << "Error reading values from file" << std::endl;
            return false;
        }
        infile.close();

        // Directly access private members through protected inheritance
        this->value1 = value1;
        this->value2 = value2;
        this->value3 = value3;

        return true;
    }

    // Method to output the values (for demonstration purposes)
    void printValues() const {
        std::cout << "Value1: " << value1 << "\n"
                  << "Value2: " << value2 << "\n"
                  << "Value3: " << value3 << std::endl;
    }
};
```
And then use it in your code as above.

### Option 3: Use boost

You need to make some modifications to your class to enable serialization.
Specifically, you need to provide access to the Boost.Serialization library to
the private members of your class. This is typically done using the friend
keyword and a private serialization function.

```cpp
#include <iostream>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

class TrackingData {
public:
    TrackingData() : value1(0), value2(0), value3(0.0) {}
    TrackingData(int32_t v1, int32_t v2, double v3) : value1(v1), value2(v2), value3(v3) {}

private:
    int32_t value1;
    int32_t value2;
    double value3;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(value1);
        ar & BOOST_SERIALIZATION_NVP(value2);
        ar & BOOST_SERIALIZATION_NVP(value3);
    }
};
```

You can then serialize the data:
```cpp
void saveTrackingData(const TrackingData& data, const std::string& filename) {
    std::ofstream ofs(filename);
    boost::archive::text_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(data);
}
```

and also deserialize:
```cpp
void loadTrackingData(TrackingData& data, const std::string& filename) {
    std::ifstream ifs(filename);
    boost::archive::text_iarchive ia(ifs);
    ia >> BOOST_SERIALIZATION_NVP(data);
}
```

## Contributing

Contributions are welcome! Please submit a pull request or open an issue to
discuss any changes.

## License

This project is licensed under the MIT License. See the LICENSE file for details.
