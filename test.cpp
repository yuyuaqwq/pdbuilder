// pdbrain.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>

#define private public
#include <pdbuilder/pdbuilder.hpp>
#include <pdbuilder/downloader.hpp>
#undef private

#include <sezz/sezz.hpp>

namespace sezz{

void Serialize(std::ostream& os, pdbuilder::Pdber& pdber) {
    Serialize(os, pdber.module_extender_.m_Structs, pdber.module_extender_.m_Symbols);
}

template<>
pdbuilder::Pdber Deserialize<pdbuilder::Pdber>(std::istream& is) {
    pdbuilder::Pdber pdber;
    Deserialize(is, pdber.module_extender_.m_Structs, pdber.module_extender_.m_Symbols);
    return pdber;
}

template<>
void Serialize/* <symbolic_access::Member> */(std::ostream& os, symbolic_access::Member& member) {
    Serialize(os, member.Bitfield, member.Name, member.Offset);
}
template<>
symbolic_access::Member Deserialize<symbolic_access::Member>(std::istream& is) {
    symbolic_access::Member member;
    Deserialize(is, member.Bitfield, member.Name, member.Offset);
    return member;
}
}


int main()
{
   /*
    uint32_t* buf = new uint32_t[1024];
    RingQueue<uint32_t> queue(buf, 1024);
    clock_t start, end;
    start = clock();


    std::thread enqueue([&]() {
        for (int i = 0; i < 10000000; i++) {
            queue.Enqueue(100);
        }
        });

    std::thread dequeue([&]() {
        uint32_t a;
        for (int i = 0; i < 10000000; i++) {
            queue.Dequeue(&a);
        }
        });

    enqueue.join();
    dequeue.join();

    end = clock();

    std::cout << "F1运行时间" << (double)(end - start) << "ms" << std::endl;*/

    pdbuilder::Downloader downloader;
    downloader.DownloadPdb(downloader.GetPdbInfoFromImageBuf((uint8_t*)GetModuleHandleA("ntdll.dll")));

    std::fstream fs;
    fs.open("test.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

    pdbuilder::Pdber pdber{ pdbuilder::FileStream(LR"(C:\lib\SymbolicAccess-master\SymbolicAccessTest\TestData\x86_64\ntdll.pdb)") };
    sezz::Serialize(fs, pdber);

    fs.seekg(0);

    auto pdber2 = sezz::Deserialize<pdbuilder::Pdber>(fs);

    auto offset = pdber2.Struct()["_PEB"]["OSMajorVersion"].Offset();
    auto rva = pdber2.Symbol()["NtSuspendProcess"].Rva();

    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
