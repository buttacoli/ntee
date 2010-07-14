#ifndef INCLUDE_TESTABLECODE_HPP
#define INCLUDE_TESTABLECODE_HPP

struct TestableCode {
   // Code to be run in a test main
   virtual int test_code(int argc, char** argv) = 0;
};

#endif
