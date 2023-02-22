#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <math.h>

using namespace std;

struct arg_mapper{
    int id;
    int nr_map;
    int nr_reduce;
    std::vector<std::string> *file_list;
    std::vector<std::map<int, std::vector<int>>> *list;
};

struct arg_reducer{
    int id;
    int nr_map;
    std::vector<std::map<int, std::vector<int>>> *list;
};


set<int> convertToSet(vector<int> v)
{
    set<int> s;
    for (int x : v) {
        s.insert(x);
    }
    return s;
}


vector<int> perfect_power(int nr, int r_max)
{

    int left, rigth, mid;
    vector<int> powers;

    for(int i = 2; i < r_max + 2; i++){

        left = 1;
        rigth = sqrt(nr);
        while(left <= rigth){

            mid = left + (rigth - left) / 2;
            if(pow(mid, i) == nr){
                powers.push_back(i);
                break;
            }
            if(pow(mid, i) < nr){
                left = mid + 1;
            }
            if(pow(mid, i) > nr){
                rigth = mid - 1;
            }
        }
    }

    return powers;
}