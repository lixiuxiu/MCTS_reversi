

#include <ctime>
#include <exception>
#include <iostream>
#include <cmath>
#include <cassert>
#include <limits>
#include <stack>
#include <cstring>
#include "jsoncpp/json.h"
using namespace std;
const double EPSILON = 0.0000001;
int currBotColor; // 电脑执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[8][8] = { 0 }; // 先x后y，记录棋盘状态
int blackPieceCount = 2, whitePieceCount = 2;
const int VALUE[8][8] = {
        20, -3, 11,  8,  8, 11, -3, 20,
        -3, -7, -4,  1,  1, -4, -7, -3,
        11, -4,  2,  2,  2,  2, -4, 11,
        8,  1,  2, -3, -3,  2,  1,  8,
        8,  1,  2, -3, -3,  2,  1,  8,
        11, -4,  2,  2,  2,  2, -4, 11,
        -3, -7, -4,  1,  1, -4, -7, -3,
        20, -3, 11,  8,  8, 11, -3, 20,
};

// 向Direction方向改动坐标，并返回是否越界 这个是八个方向上的
inline bool MoveStep(int &x, int &y, int Direction){ //注意：这里调用一次move,x,y也相应移动了一步
    if (Direction == 0 || Direction == 6 || Direction == 7)
        x++;
    if (Direction == 0 || Direction == 1 || Direction == 2)
        y++;
    if (Direction == 2 || Direction == 3 || Direction == 4)
        x--;
    if (Direction == 4 || Direction == 5 || Direction == 6)
        y--;
    if (x < 0 || x > 7 || y < 0 || y > 7)
        return false;
    return true;
}

void draw(int b[8][8]){
    cout << "  0 1 2 3 4 5 6 7"<<endl;//输出列号
    for (int i = 0; i<8; i++)
    {
        cout<<i;
        for (int j = 0; j<8; j++)
        {
            cout << " ";
            switch (b[j][i])
            {
                case -1:cout << "○"; break;
                case 1:cout << "●"; break;
                case 0:cout << " "; break;
                default:break;
            }
        }
        cout<<endl;
    }

}

bool ProcStep(int xPos, int yPos, int color, bool checkOnly = false) {
    int effectivePoints[8][2];
    int dir, x, y, currCount;
    bool isValidMove = false;   //是否是合法的移动
    if (gridInfo[xPos][yPos] != 0)   //如果棋盘上这个位置有子，下面都是这个地方没字的情况
        return false;                 //返回不合法
    for (dir = 0; dir < 8; dir++)   // 这里遍历八个方向，每次搞一个方向 搞一圈下来把所有方向上面的夹住的对方的棋子保存在effectivePoints上
    {
        x = xPos;
        y = yPos;
        currCount = 0;
        while (1)
        {
            if (!MoveStep(x, y, dir))//朝着一个方向上移动判断是否超界，边界就break, 每while一次都会往dir方向前进一步
            {
                currCount = 0;
                break;
            }
            if (gridInfo[x][y] == -color)  //移动一下还是对方棋子那就对方棋子个数加1，把对方的个数和位置记下来保存到effectivePoints数组上
            {
                currCount++; //数字就加一
                effectivePoints[currCount][0] = x;
                effectivePoints[currCount][1] = y;
            }
            else if (gridInfo[x][y] == 0) //没有棋子
            {
                currCount = 0;
                break;
            }
            else     //这种情况指的是这个棋子是我方，那么计数器就不置0了，直接跳出，这时已经是我方棋子夹住了currCount多的敌方棋子
            {
                break;
            }
        }
        if (currCount != 0)
        {
            isValidMove = true;  //如果八个方向只要有一个方向使对方子可以反转的，这步棋就是可以的
            if (checkOnly)    //如果只是检查这步是否合法就返回合法，下面的不进行反转操作
                return true;
            if (color == 1) //如果是黑棋
            {
                blackPieceCount += currCount;
                whitePieceCount -= currCount;   //改变黑白棋子个数
            }
            else  //白棋
            {
                whitePieceCount += currCount;
                blackPieceCount -= currCount;
            }
            while (currCount > 0)  //这里把加在中间的对方棋子翻转过去
            {
                x = effectivePoints[currCount][0];
                y = effectivePoints[currCount][1];
                gridInfo[x][y] *= -1;
                currCount--;
            }
        }
    }
    if (isValidMove) //如果这个地方是合法的坐标，那么下面就对这盘棋的黑白进行改变+1
    {
        gridInfo[xPos][yPos] = color;
        if (color == 1)
            blackPieceCount++;
        else
            whitePieceCount++;
        return true;
    }
    else
        return false;
}

class UCTreeNode {
public:
    int board[8][8];  //  当前棋盘布局
    UCTreeNode *vpChildren_i[64];   //孩子结点布局
    bool isLeaf_i=true;
    double nVisits_i=0;
    double totValue_i=0;
    int childNum=0;
    int xPos;
    int yPos;
    int position[64][2];    //可下子位置
    int blackNum=2;
    int whiteNum=2;
    int colorToPlay; // 1是黑。-1是白  就是本局面最后一个棋子落子的颜色

    UCTreeNode() {
        for(int i=0; i<8 ;i++)
            for (int j = 0; j <8; j++) {
                memset(board[i],0,sizeof(board[i]));
            }

    }  // default constructor

    void init(){
        for (int i = 0; i <8; i++) {
            memcpy(board[i],gridInfo[i], sizeof(board[i]));
        }
        blackNum=blackPieceCount;
        whiteNum=whitePieceCount;
        colorToPlay=-currBotColor; //把玩家下完的棋局状态记录下来
    }

    void inherit(UCTreeNode preNode){    //把棋盘复制到当前结点来
        for (int i = 0; i <8; i++) {
            memcpy(board[i],preNode.board[i], sizeof(board[i]));
        }
        blackNum=preNode.blackNum;
        whiteNum=preNode.whiteNum;


    }

    void copy(UCTreeNode &temp){
        for (int i = 0; i <8; i++) {
            memcpy(temp.board[i],board[i], sizeof(board[i]));
        }
        temp.blackNum=blackNum;
        temp.whiteNum=whiteNum;
        temp.colorToPlay=colorToPlay;
        temp.xPos=xPos;
        temp.yPos=yPos;
    }

    bool isLeaf() const {
        return isLeaf_i;
    }

    bool ProcStep(int xP, int yP,int color, bool checkOnly = false) {
        //draw(board);
        int effectivePoints[8][2];
        int dir, x, y, currCount;
        bool isValidMove = false;   //是否是合法的移动
        if (board[xP][yP] != 0)   //如y果棋盘上这个位置有子，下面都是这个地方没字的情况
            return false;//返回不合法
        for (dir = 0; dir < 8; dir++)   // 这里遍历八个方向，每次搞一个方向 搞一圈下来把所有方向上面的夹住的对方的棋子保存在effectivePoints上
        {
            x = xP;
            y = yP;
            currCount = 0;
            while (1)
            {
                if (!MoveStep(x, y, dir))//朝着一个方向上移动判断是否超界，边界就break, 每while一次都会往dir方向前进一步
                {
                    currCount = 0;
                    break;
                }
                if (board[x][y] == -color)   //注意这里在两个函数中运行颜色有区别
                    //移动一下还是对方棋子那就对方棋子个数加1，把对方的个数和位置记下来保存到effectivePoints数组上
                {
                    currCount++; //数字就加一
                    effectivePoints[currCount][0] = x;
                    effectivePoints[currCount][1] = y;
                }
                else if (board[x][y] == 0) //没有棋子
                {
                    currCount = 0;
                    break;
                }
                else     //这种情况指的是这个棋子是我方，那么计数器就不置0了，直接跳出，这时已经是我方棋子夹住了currCount多的敌方棋子
                {
                    break;
                }
            }
            if (currCount != 0)
            {
                isValidMove = true;  //如果八个方向只要有一个方向使对方子可以反转的，这步棋就是可以的
                if (checkOnly)    //如果只是检查这步是否合法就返回合法，下面的不进行反转操作
                    return true;
                if (color == 1) //如果是黑棋,也就是下一步棋是白棋，那么把黑子翻成白棋
                {
                    whiteNum -= currCount;
                    blackNum += currCount;   //改变黑白棋子个数
                }
                else  //白棋
                {
                    whiteNum += currCount;
                    blackNum -= currCount;
                }
                while (currCount > 0)  //这里把加在中间的对方棋子翻转过去
                {
                    x = effectivePoints[currCount][0];
                    y = effectivePoints[currCount][1];
                    board[x][y] *= -1;
                    currCount--;
                }
            }
        }
        if (isValidMove) //如果这个地方是合法的坐标，那么下面就对这盘棋的黑白进行改变+1
        {
            board[xP][yP] = color;
            if (color == 1)
                blackNum++;
            else
                whiteNum++;
            return true;
        }
        else
            return false;
    }

    int PossiblePos(){
        int posCount=0;
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++)  //遍历棋盘找出可以落子的点，
                if (ProcStep(x, y,-colorToPlay, true))
                {
                    position[posCount][0] = x;
                    position[posCount++][1] = y;
                }
        childNum=posCount;
        return posCount;
    }

    int selectAction() {     //选择一个子节点，下了子去更改他的局面

        //assert(!isLeaf_i); //不是叶子结点
        int selected = 0;
        childNum=PossiblePos();
        //cout<<"childNum：："<<childNum<<endl;
        if(childNum==0)
            return -1;
        double bestValue = -numeric_limits<double>::max();
        //cout<<"每个孩子计算的值是："<<endl;
        for (int k = 0; k < childNum; k++)  //遍历n个孩子结点
        {
            UCTreeNode *pCur = vpChildren_i[k]; // ptr to current child node
            double uctValue = pCur->totValue_i / (pCur->nVisits_i + EPSILON) +
                              sqrt(log(nVisits_i + 1) / (pCur->nVisits_i + EPSILON));
            //pCur->Value();
            //cout<<"UCT:"<<uctValue<<endl;
            if (uctValue >= bestValue) {
                selected = k;
                bestValue = uctValue;
            }

        } // for loop

        //cout<<"孩子有："<<childNum<<endl;
        //cout<<"UCT选择的结点是："<<selected<<".该节点的值是：";
        //vpChildren_i[selected]->Value();
        //cout<<"选择的点是:"<<vpChildren_i[selected]->xPos<<vpChildren_i[selected]->yPos<<endl;
        vpChildren_i[selected]->inherit(*this);//把父节点的局面给这个子节点
        //这里是变动后的下一步棋局面

        vpChildren_i[selected]->ProcStep(position[selected][0],position[selected][1],vpChildren_i[selected]->colorToPlay);

        return selected; //找出uct最大的结点返回

    } // selectAction

    void expand() {    // 扩展可以下子的结点局面，不用先布好局，后面选择哪个布哪个
        if (!isLeaf_i)
            return;

        childNum=PossiblePos();
        for (int k = 0; k < childNum; k++){     //把每个子节点局面搞好棋盘布局
            vpChildren_i[k] = new UCTreeNode();
            //子局面下子后棋局会发生变化的，不能在这里先变
            vpChildren_i[k]->xPos=position[k][0];
            vpChildren_i[k]->yPos=position[k][1];     //本次即将落子点x和y记录下来
            vpChildren_i[k]->colorToPlay=-colorToPlay;
            //cout<<"第"<<k<<"个孩子的下步是："<<vpChildren_i[k]->xPos<<vpChildren_i[k]->yPos<<endl;
        }


        if(childNum!=0)
            isLeaf_i=false;
    }

    int simulation() {  //返回最后的结果值
        int zoneNum=0;
        UCTreeNode *temp=new UCTreeNode();
        copy(*temp);
        while (temp->blackNum+temp->whiteNum<64) {
            int posnum=temp->PossiblePos();

            if(posnum!=0) {
                int max=-10;
                int tempi;

                //int r = rand() % posnum;
                for(int i=0;i<posnum;i++){
                    int x1 = temp->position[i][0];
                    int y1 = temp->position[i][1];
                    if(VALUE[x1][y1]>max){
                        max=VALUE[x1][y1];
                        tempi=i;
                    }
                }

                int x = temp->position[tempi][0];
                int y = temp->position[tempi][1];

                temp->ProcStep(x, y, -temp->colorToPlay);  //原来是-的，搞了好久==
                //draw(temp->board);
            } else
                zoneNum++;
            if(zoneNum>1) break;   //避免两边都没子可下，一直循环的情况
            temp->colorToPlay=-temp->colorToPlay;
        }
        //cout<<"模拟落子：黑子"<<temp->blackNum<<",白子："<<temp->whiteNum<<endl;
        if((currBotColor==-1 && temp->whiteNum > temp->blackNum) || (currBotColor==1 && temp->whiteNum < temp->blackNum) )
            return 1;  //电脑是什么颜色，最后这个颜色多就是胜利。
        else
            return 0; //好吧，输和平局都返回0；

    }

    void updateStats(double value) {

        nVisits_i++;         // increment the number of visits
        totValue_i += value; // update the total value for all visits
    }

    void iterate() {
        stack<UCTreeNode *> visited;
        UCTreeNode *pCur = this;
        visited.push(this);
        int action = 0;
        //cout<<"选择的点是否是叶子结点："<<pCur->isLeaf_i<<endl;
        while (!pCur->isLeaf()) {
            action = pCur->selectAction();
            pCur = pCur->vpChildren_i[action];
            visited.push(pCur);
        }
        //cout<<"以上从跟往下走到叶子结点"<<endl;

        //但是这里根节点在主程序中扩展了先
        if(pCur->nVisits_i>75){//如果该结点被模拟50次，就去扩展他的叶子结点访问
            pCur->expand();
            action = pCur->selectAction();
            if(action==-1)
                return;
            pCur = pCur->vpChildren_i[action];
            visited.push(pCur);
        }

        double value = pCur->simulation();
        //pCur->Value();
        //cout<<"模拟下棋得到的值是："<<value<<endl;
        while (!visited.empty()) {
            pCur = visited.top();
            //如果即将下子的颜色是电脑的颜色，电脑胜，这个子加1，否则加0
            if(pCur->colorToPlay==-currBotColor)
                pCur->updateStats(1.0-value);
            else
                pCur->updateStats(value);
            visited.pop();
        }

    }

    int bestAction() { //返回utc最大的那个值
        int selected = 0;
        double bestValue = 0;
        for (int k = 0; k < childNum; ++k) {
            UCTreeNode *pCur = vpChildren_i[k]; // ptr to current child node
            assert(0 != pCur);
            double expValue = pCur->totValue_i / pCur->nVisits_i;

            if (expValue >= bestValue) {
                selected = k;
                bestValue = expValue;
            }
        } // for loop
        return selected;
    } // bestAction

    void Value() const{
        cout << totValue_i << "/" << nVisits_i << endl;
    }

};

int main()
{
    int x, y;
    srand((unsigned)time(0));
    // 初始化棋盘
    gridInfo[3][4] = gridInfo[4][3] = 1;  //|白|黑|
    gridInfo[3][3] = gridInfo[4][4] = -1; //|黑|白|

    // 读入JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input; //三维，保存n个回合双方的棋子坐标
    reader.parse(str, input);

    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();//大概就是第几回合的意思

    currBotColor = input["requests"][(Json::Value::UInt) 0]["x"].asInt() < 0 ? 1 : -1; // 第一回合收到坐标是-1, -1，说明我是黑方
    //cout<<"电脑颜色是："<<currBotColor<<endl;
    for (int i = 0; i < turnID; i++)  //以往回合的落子信息已经保存到全局变量gridInfo里面了
    {
        // 根据这些输入输出逐渐恢复状态到当前回合
        x = input["requests"][i]["x"].asInt();
        y = input["requests"][i]["y"].asInt();
        if (x >= 0)
            ProcStep(x, y, -currBotColor); // 模拟对方落子
        x = input["responses"][i]["x"].asInt();
        y = input["responses"][i]["y"].asInt();
        if (x >= 0)
            ProcStep(x, y, currBotColor); // 模拟己方落子
    }

    // 看看玩家本回合输入
    x = input["requests"][turnID]["x"].asInt();
    y = input["requests"][turnID]["y"].asInt();
    //cout<<x<<y<<endl;
    if (x >= 0)
        ProcStep(x, y, -currBotColor); // 模拟对方落子

    // 做出决策（你只需修改以下部分）
    int resultX=-1, resultY=-1;
    UCTreeNode tree;
    tree.init(); //当前棋局当下来
    //draw(tree.board);
    tree.expand();
    if(tree.childNum!=0){   //如果等于0就会跳过该局
        int i=0;
        while(1) {
            int ms=clock()%CLOCKS_PER_SEC*1000/CLOCKS_PER_SEC;
            if(ms>900)
                break;
            //cout<<"i:"<<i++<<endl;
            tree.iterate();
            //cout<<endl;

        }

        //cout<<endl;
        int bestAction = tree.bestAction();
        resultX=tree.vpChildren_i[bestAction]->xPos;
        resultY=tree.vpChildren_i[bestAction]->yPos;
    }

    // 决策结束，输出结果（你只需修改以上部分）

    Json::Value ret;
    ret["response"]["x"] = resultX;
    ret["response"]["y"] = resultY;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl; //这是电脑上的决定

    return 0;
}

