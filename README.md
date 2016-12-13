# Peformance Utilities

PerfUtils is a low-overhead library for doing performance analysis of
highly-performant software systems running on x86\_64.

## How to Use It

1. Clone the repository into a subdirectory of your application.
    
        git clone https://github.com/PlatformLab/PerfUtils.git

    Afterwards, your directory structure should look like this.

       application_directory/
           PerfUtils/

2. Build the library.

        pushd PerfUtils
        make
        popd

3. Instrument your application, including the header files for the utilities you would like to use.

        #include "TimeTrace.h"

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
    
        g++ -o Main -IPerfUtils -std=c++0x Main.cc  -LPerfUtils -lPerfUtils
