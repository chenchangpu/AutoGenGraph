#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

std::string remove_txt_suffix(const std::string& filename) {
    std::string suffix = ".txt";
    if (filename.size() >= suffix.size() &&
        filename.compare(filename.size() - suffix.size(), suffix.size(), suffix) == 0) {
        return filename.substr(0, filename.size() - suffix.size());
    }
    return filename;  // 原样返回
}

int main(int argc, char **argv) {
    if(argc != 4){
        cout << "Usage:" << argv[0] << "base_path" << " <in_neib_table>" << "<in_rmat_part>" << endl;
    }
    // 文件路径
    string base_path = argv[1];                   // dahu: 须修改
    if(base_path.back() != '/'){                        // 加'/'
        base_path += "/";
    }
    const string neib_file = base_path + argv[2];      // dahu: 须修改
    const string split_file = base_path + argv[3];      // dahu: 须修改

    // 读取分割点
    vector<int> split_points;
    ifstream split_in(split_file);
    if (!split_in) {
        cerr << "无法打开分割点文件: " << split_file << endl;
        return 1;
    }

    int point;
    while (split_in >> point) {
        split_points.push_back(point);
    }
    split_in.close();

    // 打开输入文件
    ifstream input(neib_file);
    if (!input) {
        cerr << "无法打开输入文件: " << neib_file << endl;
        return 1;
    }

    // 创建输出文件流数组
    int num_splits = split_points.size() - 1;
    vector<ofstream> outputs(num_splits);
    string neib_file_base = remove_txt_suffix(neib_file);
    
    for (int i = 0; i < num_splits; i++) {
        string output_file = neib_file_base + "_" + to_string(i + 1) + ".txt";  // dahu: 须修改
        outputs[i].open(output_file);
        
        if (!outputs[i]) {
            cerr << "无法创建输出文件: " << output_file << endl;
            return 1;
        }
        
        cout << "已创建输出文件: " << output_file << endl;
    }

    // 读取输入文件并分配到各个输出文件
    string line;
    int current_line = 0;
    int current_split = 0;
    
    while (getline(input, line)) {
        // 确定当前行应该属于哪个分割
        while (current_split < num_splits - 1 && current_line >= split_points[current_split + 1]) {
            current_split++;
        }
        
        // 将行写入对应的输出文件
        if (current_line >= split_points[current_split] && 
            current_line < split_points[current_split + 1]) {
            outputs[current_split] << line << "\n";
        }
        
        current_line++;
    }

    // 关闭所有文件
    input.close();
    for (int i = 0; i < num_splits; i++) {
        int lines_written = (i < num_splits - 1) ? 
                           (split_points[i + 1] - split_points[i]) : 
                           (current_line - split_points[i]);
        outputs[i].close();

        // dahu: 须修改
        cout << "已生成文件 " << neib_file_base + "_" + to_string(i + 1) + ".txt"  
             << ", 包含行数: " << lines_written << endl;
    }

    cout << "所有文件分割完成!" << endl;

    return 0;
}