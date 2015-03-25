#ifndef BPAGE_H
#define BPAGE_H
#define MAXKEYS 5
#define MINKEYS MAXKEYS/2
#define NULO (-1)
#include "Indice.h"
#include<vector>
using std::vector;
class BPAGE{
        public:
                int keycount;
                vector<Indice>indices;
                vector<int>children;
              

}; 
#endif
