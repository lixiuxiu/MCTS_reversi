//
// Created by ������ on 2017/11/7.
//

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
const double EPSILON = 1e-6;
int currBotColor; // ���ִ����ɫ��1Ϊ�ڣ�-1Ϊ�ף�����״̬��ͬ��
int gridInfo[8][8] = { 0 }; // ��x��y����¼����״̬
int blackPieceCount = 2, whitePieceCount = 2;

// ��Direction����Ķ����꣬�������Ƿ�Խ�� ����ǰ˸������ϵ�
inline bool MoveStep(int &x, int &y, int Direction){ //ע�⣺�������һ��move,x,yҲ��Ӧ�ƶ���һ��
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
    cout << "  0 1 2 3 4 5 6 7"<<endl;//����к�
    for (int i = 0; i<8; i++)
    {
        cout<<i;
        for (int j = 0; j<8; j++)
        {
            cout << " ";
            switch (b[j][i])
            {
                case -1:cout << "��"; break;
                case 1:cout << "��"; break;
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
    bool isValidMove = false;   //�Ƿ��ǺϷ����ƶ�
    if (gridInfo[xPos][yPos] != 0)   //������������λ�����ӣ����涼������ط�û�ֵ����
        return false;                 //���ز��Ϸ�
    for (dir = 0; dir < 8; dir++)   // ��������˸�����ÿ�θ�һ������ ��һȦ���������з�������ļ�ס�ĶԷ������ӱ�����effectivePoints��
    {
        x = xPos;
        y = yPos;
        currCount = 0;
        while (1)
        {
            if (!MoveStep(x, y, dir))//����һ���������ƶ��ж��Ƿ񳬽磬�߽��break, ÿwhileһ�ζ�����dir����ǰ��һ��
            {
                currCount = 0;
                break;
            }
            if (gridInfo[x][y] == -color)  //�ƶ�һ�»��ǶԷ������ǾͶԷ����Ӹ�����1���ѶԷ��ĸ�����λ�ü��������浽effectivePoints������
            {
                currCount++; //���־ͼ�һ
                effectivePoints[currCount][0] = x;
                effectivePoints[currCount][1] = y;
            }
            else if (gridInfo[x][y] == 0) //û������
            {
                currCount = 0;
                break;
            }
            else     //�������ָ��������������ҷ�����ô�������Ͳ���0�ˣ�ֱ����������ʱ�Ѿ����ҷ����Ӽ�ס��currCount��ĵз�����
            {
                break;
            }
        }
        if (currCount != 0)
        {
            isValidMove = true;  //����˸�����ֻҪ��һ������ʹ�Է��ӿ��Է�ת�ģ��ⲽ����ǿ��Ե�
            if (checkOnly)    //���ֻ�Ǽ���ⲽ�Ƿ�Ϸ��ͷ��غϷ�������Ĳ����з�ת����
                return true;
            if (color == 1) //����Ǻ���
            {
                blackPieceCount += currCount;
                whitePieceCount -= currCount;   //�ı�ڰ����Ӹ���
            }
            else  //����
            {
                whitePieceCount += currCount;
                blackPieceCount -= currCount;
            }
            while (currCount > 0)  //����Ѽ����м�ĶԷ����ӷ�ת��ȥ
            {
                x = effectivePoints[currCount][0];
                y = effectivePoints[currCount][1];
                gridInfo[x][y] *= -1;
                currCount--;
            }
        }
    }
    if (isValidMove) //�������ط��ǺϷ������꣬��ô����Ͷ�������ĺڰ׽��иı�+1
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
    int board[8][8];  //  ��ǰ���̲���
    UCTreeNode *vpChildren_i[64];   //���ӽ�㲼��
    bool isLeaf_i=true;
    double nVisits_i=0;
    double totValue_i=0;
    int childNum=0;
    int xPos;
    int yPos;
    int position[64][2];    //������λ��
    int blackNum=2;
    int whiteNum=2;
    int colorToPlay; // 1�Ǻڡ�-1�ǰ�  ���Ǳ��������һ���������ӵ���ɫ
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
        colorToPlay=-currBotColor; //�������������״̬��¼����
    }
    void inherit(UCTreeNode preNode){    //�����̸��Ƶ���ǰ�����
        for (int i = 0; i <8; i++) {
            memcpy(board[i],preNode.board[i], sizeof(board[i]));
        }
        blackNum=preNode.blackNum;
        whiteNum=preNode.whiteNum;
        colorToPlay=-preNode.colorToPlay;

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
        bool isValidMove = false;   //�Ƿ��ǺϷ����ƶ�
        if (board[xP][yP] != 0)   //��y�����������λ�����ӣ����涼������ط�û�ֵ����
            return false;//���ز��Ϸ�
        for (dir = 0; dir < 8; dir++)   // ��������˸�����ÿ�θ�һ������ ��һȦ���������з�������ļ�ס�ĶԷ������ӱ�����effectivePoints��
        {
            x = xP;
            y = yP;
            currCount = 0;
            while (1)
            {
                if (!MoveStep(x, y, dir))//����һ���������ƶ��ж��Ƿ񳬽磬�߽��break, ÿwhileһ�ζ�����dir����ǰ��һ��
                {
                    currCount = 0;
                    break;
                }
                if (board[x][y] == -color)   //ע������������������������ɫ������
                    //�ƶ�һ�»��ǶԷ������ǾͶԷ����Ӹ�����1���ѶԷ��ĸ�����λ�ü��������浽effectivePoints������
                {
                    currCount++; //���־ͼ�һ
                    effectivePoints[currCount][0] = x;
                    effectivePoints[currCount][1] = y;
                }
                else if (board[x][y] == 0) //û������
                {
                    currCount = 0;
                    break;
                }
                else     //�������ָ��������������ҷ�����ô�������Ͳ���0�ˣ�ֱ����������ʱ�Ѿ����ҷ����Ӽ�ס��currCount��ĵз�����
                {
                    break;
                }
            }
            if (currCount != 0)
            {
                isValidMove = true;  //����˸�����ֻҪ��һ������ʹ�Է��ӿ��Է�ת�ģ��ⲽ����ǿ��Ե�
                if (checkOnly)    //���ֻ�Ǽ���ⲽ�Ƿ�Ϸ��ͷ��غϷ�������Ĳ����з�ת����
                    return true;
                if (color == 1) //����Ǻ���,Ҳ������һ�����ǰ��壬��ô�Ѻ��ӷ��ɰ���
                {
                    whiteNum -= currCount;
                    blackNum += currCount;   //�ı�ڰ����Ӹ���
                }
                else  //����
                {
                    whiteNum += currCount;
                    blackNum -= currCount;
                }
                while (currCount > 0)  //����Ѽ����м�ĶԷ����ӷ�ת��ȥ
                {
                    x = effectivePoints[currCount][0];
                    y = effectivePoints[currCount][1];
                    board[x][y] *= -1;
                    currCount--;
                }
            }
        }
        if (isValidMove) //�������ط��ǺϷ������꣬��ô����Ͷ�������ĺڰ׽��иı�+1
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
            for (int x = 0; x < 8; x++)  //���������ҳ��������ӵĵ㣬
                if (ProcStep(x, y,-colorToPlay, true))
                {
                    position[posCount][0] = x;
                    position[posCount++][1] = y;
                }
        childNum=posCount;
        return posCount;
    }
    int selectAction()  {     //ѡ��һ���ӽڵ㣬������ȥ�������ľ���
        //cout<<"����selectAction"<<endl;
        //assert(!isLeaf_i); //����Ҷ�ӽ��
        cout<<"seclect���Ƿ���Ҷ�ӽ�㣺"<<isLeaf_i<<endl;
        int selected = 0;
        childNum=PossiblePos();
        //cout<<"childNum"<<childNum<<endl;
        if(childNum==0)
            return -1;
        double bestValue = -numeric_limits<double>::max();

        for (int k = 0; k < childNum; k++)  //����n�����ӽ��
        {

            UCTreeNode *pCur = vpChildren_i[k]; // ptr to current child node
            pCur->totValue_i=pCur->colorToPlay==currBotColor?pCur->totValue_i:pCur->nVisits_i-pCur->totValue_i;
            double uctValue = pCur->totValue_i / (pCur->nVisits_i + EPSILON) +
                              sqrt(log(nVisits_i + 1) / (pCur->nVisits_i + EPSILON));
            //cout<<"UCT"<<uctValue<<endl;
            if (uctValue >= bestValue) {
                selected = k;
                bestValue = uctValue;
            }
        } // for loop

        //cout<<"�����У�"<<childNum<<endl;
        //cout<<"UCTѡ��Ľ���ǣ�"<<selected<<".�ýڵ��ֵ�ǣ�";
        //vpChildren_i[selected]->Value();
        //cout<<"ѡ��ĵ���:"<<vpChildren_i[selected]->xPos<<vpChildren_i[selected]->yPos<<endl;
        vpChildren_i[selected]->inherit(*this);//�Ѹ��ڵ�ľ��������ӽڵ�
        //�����Ǳ䶯�����һ�������
        //cout<<"���ӽ��ѡ��"<<selected<<";"<<position[selected][0]<<position[selected][1]<<endl;
        vpChildren_i[selected]->ProcStep(position[selected][0],position[selected][1],vpChildren_i[selected]->colorToPlay);

        return selected; //�ҳ�uct���Ľ�㷵��

    } // selectAction
    void expand() {    // ��չ�������ӵĽ����棬�����Ȳ��þ֣�����ѡ���ĸ����ĸ�
         if (!isLeaf_i)
             return;

        childNum=PossiblePos();
        for (int k = 0; k < childNum; k++){     //��ÿ���ӽڵ���������̲���
            vpChildren_i[k] = new UCTreeNode();
              //�Ӿ������Ӻ���ֻᷢ���仯�ģ������������ȱ�
            vpChildren_i[k]->xPos=position[k][0];
            vpChildren_i[k]->yPos=position[k][1];     //���μ������ӵ�x��y��¼����
            vpChildren_i[k]->colorToPlay=-colorToPlay;
            //cout<<"��"<<k<<"�����ӵ��²��ǣ�"<<vpChildren_i[k]->xPos<<vpChildren_i[k]->yPos<<endl;
        }
        cout<<endl;
        cout<<"=expand="<<endl;
        cout<<"���ӽ��λ�ã�";
        for (int i = 0; i < childNum; i++) {
            cout<<position[i][0]<<position[i][1]<<",";
            cout<<vpChildren_i[i]->xPos<<vpChildren_i[i]->yPos<<"  ";
        }
        cout<<"=expand end="<<endl;
        cout<<endl;

        if(childNum!=0)
             isLeaf_i=false;
    } // expand
    int simulation() {  //�������Ľ��ֵ
        UCTreeNode *temp=new UCTreeNode();
        copy(*temp);
        while (temp->PossiblePos()!=0) {
            int r=rand()%temp->PossiblePos();
            int x=temp->position[r][0];
            int y=temp->position[r][1];
            temp->ProcStep(x,y,-temp->colorToPlay);  //ԭ����-�ģ����˺þ�==
            temp->colorToPlay=-temp->colorToPlay;
            //draw(temp->board);
        }

        if((colorToPlay==-1 && temp->whiteNum > temp->blackNum) || (colorToPlay==1 && temp->whiteNum < temp->blackNum) )
            return 1;  //ģ��ʱ��ǰ���ǰ��ӻ��ߺ��ӣ���ǰʤ
        else
            return 0;

    } // rollout
    void updateStats(int value) {
        nVisits_i++;         // increment the number of visits
        totValue_i += value; // update the total value for all visits
    }
    void iterate() {
        stack<UCTreeNode *> visited;
        UCTreeNode *pCur = this;
        visited.push(this);
        int action = 0; // next selected action
        //cout<<"ѡ��ĵ��Ƿ���Ҷ�ӽ�㣺"<<pCur->isLeaf_i<<endl;
        while (!pCur->isLeaf()) {
            action = pCur->selectAction();
            cout<<"ѡ���"<<action<<"���ӽ��������"<<pCur->vpChildren_i[action]->xPos<<pCur->vpChildren_i[action]->yPos<<endl;
            pCur->vpChildren_i[action]->Value();
            pCur = pCur->vpChildren_i[action];

            visited.push(pCur);
        }
        //cout<<"���ϴӸ������ߵ�Ҷ�ӽ��"<<endl;

        //����������ڵ�������������չ����
        if(pCur->nVisits_i>50){//����ý�㱻���ʹ��ˣ���ȥ��չ����Ҷ�ӽ�����
           pCur->expand();
            cout<<"Ҷ�����չ���ӽ��λ�ã�";
            for (int i = 0; i < pCur->childNum; i++) {
                cout<<pCur->position[i][0]<<pCur->position[i][1]<<",";
                cout<<pCur->vpChildren_i[i]->xPos<<pCur->vpChildren_i[i]->yPos<<"  ";
            }
           action = pCur->selectAction();
           if(action==-1){
               cout<<"action=-1,��ֲ�����������";
               return;
           }
           pCur = pCur->vpChildren_i[action];
        }
        visited.push(pCur);
        double value = pCur->simulation();
        //pCur->Value();
        //cout<<"ģ������õ���ֵ�ǣ�"<<value<<endl;
        while (!visited.empty()) {
            pCur = visited.top();
            value=pCur->colorToPlay==currBotColor?value:1-value;
            pCur->updateStats(value);
            visited.pop();
        }

    }
    int bestAction() { //����utc�����Ǹ�ֵ
        int selected = 0;
        double bestValue = 0;
        for (int k = 0; k < childNum; ++k) {
            cout<<"children :"<<childNum;

            UCTreeNode *pCur = vpChildren_i[k]; // ptr to current child node
            pCur->Value();
            assert(0 != pCur);
            double expValue =pCur->totValue_i/pCur->nVisits_i;
            cout<<"value:"<<expValue<<endl;
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


/*int main(){
    srand((unsigned)time(0));
    gridInfo[3][4] = gridInfo[4][3] = 1;  //|��|��|
    gridInfo[3][3] = gridInfo[4][4] = -1; //|��|��|
    //draw(gridInfo);
    currBotColor = 1;  //���Ϊ����1�������ǰ���-1
    int px=4,py=5;       //����ģ������������ڣ�3��2����,��ʵ���������ŵ�3�������ŵ�2��
    ProcStep(px,py,currBotColor);     //���Ӻ�ʵ����ָı�
    //draw(gridInfo);

    //�����ǵ������ӵĹ���
    UCTreeNode tree;
    tree.init(); //��ǰ��ֵ�����

    if(tree.PossiblePos()==0) cout<<"��ֽ���"<<endl;

    for (int i=0;i<20;i++) {
        //cout<<"i:"<<i<<endl;
        tree.iterate();
        //cout<<endl;
    }
    int bestAction = tree.bestAction();
    int x=tree.vpChildren_i[bestAction]->xPos;
    int y=tree.vpChildren_i[bestAction]->yPos;
    cout<<"���ս��"<<x<<y<<endl;
    return 0;
}*/



int main()
{
    int x, y;
    srand((unsigned)time(0));
    // ��ʼ������
    gridInfo[3][4] = gridInfo[4][3] = 1;  //|��|��|
    gridInfo[3][3] = gridInfo[4][4] = -1; //|��|��|

    // ����JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input; //��ά������n���غ�˫������������
    reader.parse(str, input);

    // �����Լ��յ���������Լ���������������ָ�״̬
    int turnID = input["responses"].size();//��ž��ǵڼ��غϵ���˼

    currBotColor = input["requests"][(Json::Value::UInt) 0]["x"].asInt() < 0 ? 1 : -1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�
    //cout<<"������ɫ�ǣ�"<<currBotColor<<endl;
    for (int i = 0; i < turnID; i++)  //�����غϵ�������Ϣ�Ѿ����浽ȫ�ֱ���gridInfo������
    {
        // ������Щ��������𽥻ָ�״̬����ǰ�غ�
        x = input["requests"][i]["x"].asInt();
        y = input["requests"][i]["y"].asInt();
        if (x >= 0)
            ProcStep(x, y, -currBotColor); // ģ��Է�����
        x = input["responses"][i]["x"].asInt();
        y = input["responses"][i]["y"].asInt();
        if (x >= 0)
            ProcStep(x, y, currBotColor); // ģ�⼺������
    }

    // ������ұ��غ�����
    x = input["requests"][turnID]["x"].asInt();
    y = input["requests"][turnID]["y"].asInt();
    //cout<<x<<y<<endl;
    if (x >= 0)
        ProcStep(x, y, -currBotColor); // ģ��Է�����

    // �������ߣ���ֻ���޸����²��֣�
    int resultX=-1, resultY=-1;
    UCTreeNode tree;
    tree.init(); //��ǰ���down����
    tree.expand();
    draw(tree.board);
    cout<<tree.childNum;
    if(tree.childNum!=0){
    while(1) {
        int ms=clock()%CLOCKS_PER_SEC*1000/CLOCKS_PER_SEC;
        if(ms>970)
            break;
        tree.iterate();
    }
    int bestAction = tree.bestAction();
    resultX=tree.vpChildren_i[bestAction]->xPos;
    resultY=tree.vpChildren_i[bestAction]->yPos;
    }

    // ���߽���������������ֻ���޸����ϲ��֣�

    Json::Value ret;
    ret["response"]["x"] = resultX;
    ret["response"]["y"] = resultY;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl; //���ǵ����ϵľ���

    return 0;
}