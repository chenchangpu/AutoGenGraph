#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <set>

using namespace std;

void unique_edges(string infile_path, string outfile_path){
    ifstream infile(infile_path, ios::in);
    if(!infile){
        cout << "无法打开输入文件：" << infile_path << endl;
        exit(1);  // 添加退出
    }

    // 先读取所有数据
    int num_v, num_e;
    int self_loop = 0;
    infile >> num_v >> num_e;
    set<pair<int, int>> ues;
    for(int i = 0; i < num_e; ++i){
        int x, y;
        infile >> x >> y;
        if(x > y){
            swap(x, y);  // x <= y
        }
        if(x == y){
            ++self_loop;
            continue;
        }
        ues.insert(pair<int, int>(x, y));
    }
    infile.close();  // 关闭输入文件

    // 然后再写入
    ofstream outfile(outfile_path, ios::out);
    if(!outfile){
        cout << "无法打开输出文件：" << outfile_path << endl;
        exit(1);  // 添加退出
    }

    int cnt = ues.size();
    printf("自环数量：%d\n", self_loop);
    printf("重边数量：%d\n", num_e - cnt - self_loop);

    outfile << num_v << " " << cnt << endl;
    for(auto e: ues){
        outfile << e.first << " " << e.second << endl;
    }
    outfile.close();
}

int main(int argc, char **argv){
    if(argc != 3){
        cout << "Usage: " << argv[0] << " input_file.txt output_file.txt" << endl;
        return 1;  // 添加返回值表示错误
    }

    string input_file = argv[1];
    string output_file = argv[2];

    unique_edges(input_file, output_file);
    return 0;
}