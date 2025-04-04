#include <cstdio>
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

int p[32];
int neib_cnt[32];

int main(int argc, char **argv){
    int partition_num = stoi(argv[1]);
    string partition_path = argv[2];
    string graph_meta_partition_path = argv[3];

    ifstream infile(partition_path);
    if(!infile){
        cout << "Can not open infile: " << partition_path << endl;
        return -1;
    }
    ofstream outfile(graph_meta_partition_path);
    if(!outfile){
        cout << "Can not open outfile: " << graph_meta_partition_path << endl;
        return -1;
    }

    for(int i = 0; i < partition_num + 1; ++i){
        infile >> p[i];
    }

    for(int i = 0; i < partition_num; ++i){
        outfile << p[i+1] - p[i];
        if(i+1 < partition_num){
            outfile << " ";
        }
    }

    return 0;
}