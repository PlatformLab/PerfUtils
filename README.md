# Peformance Utilities

PerfUtils is a low-overhead library for doing performance analysis of
highly-performant software systems running on x86\_64.

## How to Use It

1. Clone the repository.

        git clone https://github.com/PlatformLab/PerfUtils.git

2. Build the library.

        pushd PerfUtils
        make
        popd

3. Instrument your application, including the header files for the utilities you would like to use, with a PerfUtils prefix.

        #include "PerfUtils/TimeTrace.h"

        using PerfUtils::TimeTrace;

        int main(){
           TimeTrace::record("Start of execution");
           uint64_t sum = 0;
           for (int i = 0; i < (1 << 20); i++) {
               sum += i;
           }
           TimeTrace::record("End of a counting loop");
           TimeTrace::record("Hello world");
           TimeTrace::print();
        }


4. Build and link against PerfUtils.

        g++ -o Main -Ipath/to/PerfUtils/include -std=c++0x Main.cc  -Lpath/to/PerfUtils/lib -lPerfUtils
