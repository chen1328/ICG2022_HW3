#ifndef HASHFUNCTION_H
#define HASHFUNCTION_H

#include "headers.h"
using namespace std;


// HashFunction Declarations.
class HashFunction
{
public:
    // HashFunction Public Methods.
    size_t operator()(const vector<int>& ptnIndex) const
    {
        return ((ptnIndex[0] * 2 + 7) + (ptnIndex[1] * 3 + 5) + (ptnIndex[2] * 5 + 3)) % MOD;
    }
private:
    // HashFunction Private data.
    const int MOD = 1000000007;
};

#endif