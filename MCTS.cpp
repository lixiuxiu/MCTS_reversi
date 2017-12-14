//
// Created by 梨休休 on 2017/11/19.
//
/*
#include <iostream>
#include <assert.h>
#include <stack>
#include <limits>
#include <cmath>
using namespace std;
const double EPSILON = 1e-6;
class UCTreeNode {
private:
    UCTreeNode *vpChildren_i[5];
    bool isLeaf_i=true;
    double nVisits_i=0;
    double totValue_i=0;
    int childNum=5;

    int selectAction() {
        assert(!isLeaf_i); //不是叶子结点
        int selected = 0;
        double bestValue = -numeric_limits<double>::max();
        for (int k = 0; k < childNum; ++k)  //遍历n个孩子结点
        {
            UCTreeNode *pCur = vpChildren_i[k]; // ptr to current child node
            //assert(0 != pCur); //孩子结点不是空
            double uctValue = pCur->totValue_i / (pCur->nVisits_i + EPSILON) +
                              sqrt(log(nVisits_i + 1) / (pCur->nVisits_i + EPSILON));
            if (uctValue >= bestValue) {
                selected = k;
                bestValue = uctValue;
            }

        } // for loop
        return selected; //找出uct最大的结点返回

    } // selectAction

    void expand() {
        if (!isLeaf_i)
            return;
        //isLeaf_i = false;
        for (int k = 0; k < childNum; ++k)
            vpChildren_i[k] = new UCTreeNode();
        isLeaf_i=false;


    } // expand

    int rollOut()  //返回最后的结果值
    {
        return rand() % 2;


    } // rollout

    void updateStats(int value) {
        nVisits_i++;         // increment the number of visits
        totValue_i += value; // update the total value for all visits
    }

public:
    UCTreeNode() {
        for (int k = 0; k < childNum; ++k) {
            vpChildren_i[k] = 0;
        }

    }  // default constructor
    UCTreeNode(const UCTreeNode &tree) {
        if (isLeaf_i) {
            return;
        }
        for (int k = 0; k < childNum; ++k) {
            assert(0 != tree.vpChildren_i[k]);
            vpChildren_i[k] = new UCTreeNode(*tree.vpChildren_i[k]);
        }

    } // copy constructor

    bool isLeaf() const {
        return isLeaf_i;
    }

    void iterate() {
        stack<UCTreeNode *> visited;
        UCTreeNode *pCur = this;
        visited.push(this);
        int action = 0; // next selected action

        while (!pCur->isLeaf()) {
            action = pCur->selectAction();
            pCur = pCur->vpChildren_i[action];
            visited.push(pCur);
        }
        pCur->expand();
        action = pCur->selectAction();
        pCur = pCur->vpChildren_i[action];
        visited.push(pCur);

        double value = rollOut();
        while (!visited.empty()) {
            pCur = visited.top();
            // get the current node in the path
            pCur->updateStats(value);   // update statistics
            visited.pop();
            pCur->Value();
            // remove the current node from the stack
        }

    } // iterate

    int bestAction() { //返回utc最大的那个值
        int selected = 0;
        double bestValue = -numeric_limits<double>::max();
        for (int k = 0; k < childNum; ++k) {
            UCTreeNode *pCur = vpChildren_i[k]; // ptr to current child node
            assert(0 != pCur);
            double expValue = pCur->totValue_i / (pCur->nVisits_i + EPSILON);
            expValue += static_cast<double>(rand()) * EPSILON / RAND_MAX;
            if (expValue >= bestValue) {
                selected = k;
                bestValue = expValue;
            }
        } // for loop
        return selected;
    } // bestAction

    void Value() const {
        cout << totValue_i << "/" << nVisits_i << endl;
    }
};

int main(){
    UCTreeNode tree;

    for(int k=0; k<1000; ++k)
    {
        tree.iterate();
        cout << endl;
    }
    cout << endl;
    int bestAction = tree.bestAction();
    cout << "Best Action: " << bestAction << std::endl;
    return 0;
}
 */