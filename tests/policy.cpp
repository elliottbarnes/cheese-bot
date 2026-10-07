#include "../StarCraftCannonRushBot/DecisionPolicy.h"
#include <cassert>
#include <iostream>
#include <string>
int main(int argc, char** argv) {
    using CheesePolicy::decide;
    auto opening = decide({4, 0, 0, false, true});
    assert(opening.homePylon && opening.trainWorker && !opening.cannon);
    auto overlap = decide({6, 2, 1, true, true});
    assert(overlap.proxyPylon && overlap.cannon && !overlap.trainWorker);
    auto noScout = decide({6, 2, 1, true, false});
    assert(!noScout.proxyPylon && !noScout.cannon);
    auto pressure = decide({6, 3, 1, true, true});
    assert(!pressure.proxyPylon && pressure.cannon);
    if (argc > 1 && std::string(argv[1]) == "--table") {
        for (int w=4;w<=7;++w) for (int p=0;p<=4;++p) for (int f=0;f<=1;++f)
        for (int e=0;e<=1;++e) for (int s=0;s<=1;++s) {
            auto d=decide({w,p,f,e!=0,s!=0});
            std::cout<<w<<','<<p<<','<<f<<','<<e<<','<<s<<','<<d.homePylon<<','<<d.homeForge<<','<<d.proxyPylon<<','<<d.cannon<<','<<d.trainWorker<<'\n';
        }
    } else std::cout << "C++ decision gates passed.\n";
}
