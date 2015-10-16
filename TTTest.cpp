#include "TimeTrace.h"

using PerfUtils::TimeTrace;

int main(){
   TimeTrace tt;
   tt.record("Start of execution");
   uint64_t sum = 0;
   for (int i = 0; i < (1 << 20); i++) {
       sum += i;
   }
   tt.record("End of a counting loop");
   tt.print();

   TimeTrace* globalTT = TimeTrace::getGlobalInstance();
   globalTT->record("Hello world");
   globalTT->print();
}
