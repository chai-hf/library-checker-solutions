#include "io.h"

int main() {
    rd rd;
    wt wt;
    int n = rd.ud();
    while (n--) wt.ud(rd.ud() + rd.ud());
}
