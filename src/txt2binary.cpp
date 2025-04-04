#include <iostream>
#include <fstream>
#include <string>

int txt2binary(std::string infile, std::string outfile){
    // 打开输入文件
    std::ifstream inFile(infile, std::ios::in);
    if (!inFile) {
        std::cerr << "无法打开输入文件" << std::endl;
        return 1;
    }

    // 打开输出文件(二进制模式)
    std::ofstream outFile(outfile, std::ios::out | std::ios::binary);
    if (!outFile) {
        std::cerr << "无法创建输出文件" << std::endl;
        return 1;
    }
    
    int num;

    while (inFile >> num) {
        outFile.write(reinterpret_cast<char*>(&num), sizeof(num));
    }

    // 关闭文件
    inFile.close();
    outFile.close();

    std::cout << "文件处理完成" << std::endl;
    return 0;
}

int main(int argc, char **argv) {
    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <infile_path>" << std::endl;
        return 1;
    }
    std::string infile_base = argv[1];
    std::string infile_txt = infile_base + ".txt";
    std::string infile_bin = infile_base + ".bin";

    txt2binary(infile_txt, infile_bin);
    
    return 0;
}