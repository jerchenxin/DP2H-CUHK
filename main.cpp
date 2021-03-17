#include "test.h"

int main() {
//    TestLabelGraph t = TestLabelGraph("/Users/chenxin/Desktop/Biogrid");
//    TestLabelGraph t = TestLabelGraph("/Users/chenxin/Desktop/d_add", true);
//    t.TestAddEdgeManual(3, 4, 1 << 1);
//    TestLabelGraph t = TestLabelGraph("/Users/chenxin/Desktop/g1");
//    t.TestConstruction();
//    t.TestTwoHopCover()z;
//    boost::dynamic_bitset<> label(4, 0);
//    label[0] = 1;
//    label[1] = 1;
//    label[3] = 1;
//    t.TestOneQuery(3, 5, label);
//    t.TestOneQuery(23354, 63141, label);
//    t.TestDeleteEdge(1000);
//    t.TestAddEdge(1000);
//    t.TestQueryBFS();

    LabelGraph* g1 = new LabelGraph("/Users/chenxin/Desktop/g_3", true);
    LabelGraph* g2 = new LabelGraph("/Users/chenxin/Desktop/g_4", true);
    g1->ConstructIndex();
    g1->PrintLabel();

    g2->ConstructIndex();
    g2->PrintLabel();

    g1->DynamicDeleteEdge(2, 5, 1 << 2);
    g1->PrintLabel();

    std::cout << g1->Query(5, 3, 6) << std::endl;
    std::cout << g1->QueryBFS(5, 3, 6) << std::endl;
}


