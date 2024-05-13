//
// Created by 24301 on 2023/1/19.
//
//
// Created by 24301 on 2023/1/10.
//
#include "Account.h"

void AccountTree::load(string filename, AVLTree<int,Block*> *blockAVLptr) {//为了更容易获取账户某笔交易发生的时间，我们利用在加载时生成的AVL<BlockID,Block *> 来加快访问
    ifstream file;
    file.open(filename);
    if (!file.is_open()) {
        cout << "Search open error\n";
        exit(0);
    }
    string tmp;
    getline(file, tmp);
    string from, to;
    double _amount;
    int Blockid;
    while (file.peek() != EOF) {
        getline(file, tmp, ',');
        getline(file, tmp, ',');
        Blockid = atoi(tmp.c_str());
        getline(file, from, ',');
        getline(file, tmp, ',');
        _amount = string2double(tmp);
        getline(file, to);

        if (!isexited(from))
            size++;
        if (!isexited(to))
            size++;

        Account tmp_acc;
        tmp_acc=IDtree[from];
        tmp_acc.tree[(blockAVLptr->Find(Blockid)->_Kv.second)->block_timestamp].push_back(-_amount);
        tmp_acc.ID=from;
        tmp_acc.amount+=(-_amount);

        tmp_acc=IDtree[to];
        tmp_acc.tree[(blockAVLptr->Find(Blockid)->_Kv.second)->block_timestamp].push_back(_amount);
        tmp_acc.ID=to;
        tmp_acc.amount+=(_amount);

    }
    file.close();
    initalvec();
}
void AccountTree::loadFromlinkedlist(LinkList& l)//从已有的区块链中加载数据
{
    double _amount;

       l.reset();//将区块链复位
       while(!l.endOfList())//遍历区块链，直到末尾
       {

           Block b;
           b=l.data();
           vector<void *> result;
           b.transTree.Inorder(result);
           for(int i=0;i<(int)result.size();i++) {

               pair<long long, Transaction> *ptr;
               ptr = (pair<long long, Transaction> *) result[i];
               _amount=ptr->second.amount;
               int Blockid=ptr->second.blockID;
               if (!isexited(ptr->second.from))//如果转出方是新的账户，则插入
               {
                   size++;
               }

               if (!isexited(ptr->second.to))//如果转入方是新的账户，则插入
               {
                   size++;
               }

               IDtree[ptr->second.from].tree[b.block_timestamp].push_back(-_amount);
               IDtree[ptr->second.from].ID=ptr->second.from;
               IDtree[ptr->second.from].amount+=(-_amount);

               IDtree[ptr->second.to].tree[b.block_timestamp].push_back(_amount);
               IDtree[ptr->second.to].ID=ptr->second.to;
               IDtree[ptr->second.to].amount+=(_amount);
           }
           l.next();//移动区块链指针
       }
       initalvec();//加载好后对树中各个账户进行func操作。
}
void AccountTree::InsertFromFile(string filename, AVLTree<int,Block*> *blockAVLptr) {//从用户指定的文件中插入新的交易
    ifstream file;
    file.open(filename);
    if (!file.is_open()) {
        cout << "InsertTransactions open error\n";
        exit(0);
    }
    string tmp;
    string from, to;
    double _amount;
    int Blockid;
    while (file.peek() != EOF) {
        getline(file, tmp, ',');
        getline(file, tmp, ',');
        Blockid = atoi(tmp.c_str());
        getline(file, from, ',');
        getline(file, tmp, ',');
        _amount = string2double(tmp);
        getline(file, to);

        if (!isexited(from))
            size++;
        if (!isexited(to))
            size++;


        IDtree[from].tree[(blockAVLptr->Find(Blockid)->_Kv.second)->block_timestamp].push_back(-_amount);
        IDtree[from].ID=from;
        IDtree[from].amount+=(-_amount);

        IDtree[to].tree[(blockAVLptr->Find(Blockid)->_Kv.second)->block_timestamp].push_back(_amount);
        IDtree[to].ID=to;
        IDtree[to].amount+=(_amount);

    }
    file.close();
    initalvec();
}
vector<pair<long long, double> > AccountTree::search1(string ID, long long left, long long right, int type,int &allsize,int k) {
    vector<pair<long long, double> > tmp;
    vector<pair<long long, double> > result;
    tmp = IDtree.Find(ID)->_Kv.second.search1(left, right, type);//根据账户ID调用对应账户的search1.
    int tmp_size = tmp.size();
    allsize=tmp_size;
    if (k <= tmp_size) {//如果外部输入的k<=tmp_size
        for (int i = 0; i < k; i++) {// 取出最大的前k个
            result.push_back(tmp[i]);
        }
    } else {//外部的k大于tmp_size
        for (int i = 0; i < tmp_size; i++) {
            result.push_back(tmp[i]);
        }
    }
    return result;
}

vector<pair<long long, double> > Account::search1(long long left, long long right, int type) {
    vector<pair<long long, double> > result;
    if(!isordered){
        int len = vec.size();
        for (int i = 0; i < len; i++) {// 因为一个账户在一个时间的所有交易存在vector中，这里我们将这些交易额相加，以便排序。
            pair<long long, vector<double>> *ptr;
            ptr = (pair<long long, vector<double>> *) vec[i];
            int transSizeofOnetime = ptr->second.size();//这一时间的交易数目
            for (int j = 0; j < transSizeofOnetime; j++) {
                pair<long long, double> buf(ptr->first, ptr->second[j]);
                orderedvec.push_back(buf);
            }
        }
        sort(orderedvec.begin(), orderedvec.end(), mycomp);//按交易额绝对值对交易进行排序
        isordered=true;
    }
    int len=orderedvec.size();
    for(int i=0;i<len;i++){
        if(orderedvec[i].first>=left&&orderedvec[i].first<=right){
            if(type==0){//全部转入和转出
                result.push_back(orderedvec[i]);
            }
            if(type<0&&orderedvec[i].second<0){//转出
                result.push_back(orderedvec[i]);
            }
            if(type>0&&orderedvec[i].second>0){//转入
                result.push_back(orderedvec[i]);
            }
        }
    }
    return result;
}

double Account::search2(long long edge) {
    double result = 0.0;
    int len = vec.size();
    for (int i = 0; i < len; i++) {
        pair<long long, vector<double>> *tmp = ((pair<long long, vector<double>> *) vec[i]);
        if (tmp->first <= edge) {//指定时间edge之前
            for (int j = 0; j < tmp->second.size(); j++) {
                result += tmp->second[j];
            }
        }
    }
    return result;
}
double AccountTree::search2(std::string ID, long long edge) {
    return IDtree[ID].search2(edge);//调用对应ID的search2
}
vector<pair<string, double>> AccountTree::search3(long long timepoint, int k) {
    vector<pair<string, double>> tmp_result, result;//分别为无序的和有序的所有账户的值
    vector<void *> inorder_vec;//中序遍历结果
    IDtree.Inorder(inorder_vec);
    for (int i = 0; i < inorder_vec.size(); i++) {
        pair<string, Account> *ptr;
        ptr = (pair<string, Account> *) inorder_vec[i];
        double tmp = ptr->second.search2(timepoint);
        pair<string, double> buf(ptr->first, tmp);
        tmp_result.push_back(buf);
    }
    int llen = tmp_result.size();//所有
    sort(tmp_result.begin(), tmp_result.end(), mycomp2);
    int len = k < llen ? k : llen;
    for (int i = 0; i < len; i++) {
        result.push_back(tmp_result[i]);
    }
    return result;
}
//
// Created by 24301 on 2023/1/19.
//
#include "Diagraph.h"
bool OLGraph::insertPoint(string ID){//插入ID点,true表示成功插入，false表示已有ID点
    VexNode tmp={ID, nullptr, nullptr,0,0};

    if(!indextree.isexited(ID)){
        xlist.push_back(tmp);
        indextree[ID]=xlist.size()-1;
        return true;
    }
    return false;
}
bool OLGraph::insertArc(string v1,string v2,double _money){//插入弧
    int i= LocateVex(v1),j= LocateVex(v2);
    if(i<0||j<0){//插入失败，将v1点插入或将v2点插入
        if(i<0){
            insertPoint(v1);
        }
        if(j<0){
            insertPoint(v2);
        }
        insertArc(v1,v2,_money);//重新插入弧
        return false;
    }
    string fromto=v1+v2;
    if(Arctree.isexited(fromto)){//已有这条弧
        Arctree[fromto]->money+=_money;
        arcnum--;//减去一条重复的弧
    }
    else{
        ArcBox *p=new ArcBox;
        Arctree[fromto]=p;//插入
        trash_can.push_back(p);
        //*p={i,j,xlist[j].firstin,xlist[i].firstout,_money};
        p->tailvex=i;p->headvex=j;p->hlink=xlist[j].firstin;p->tlink=xlist[i].firstout;p->money=_money;
        xlist[j].firstin=p;
        xlist[i].firstout=p;
        xlist[j].in_degree++;
        xlist[i].out_degree++;
        All_outdegree++;
        All_indegree++;
    }
    return true;
}
void OLGraph::Insert(vector<string> vec_id,vector<simtrans> vecArc){
    //首先插入点
    vexnum=vec_id.size();
    arcnum=vecArc.size();

    for(int i=0;i<vexnum;i++){
        insertPoint(vec_id[i]);
    }

    //之后插入弧
    for(int i=0;i<arcnum;i++){
        insertArc(vecArc[i].from,vecArc[i].to,vecArc[i].moneyy);
    }
}
vector<pair<string,int>> OLGraph::getTop(int type,int k){//type>=0出度，<0入度
    vector<VexNode> tmp(xlist);
    vector<pair<string,int>> result;
    if(type<0){//入度
        sort(tmp.begin(),tmp.end(), mycompin);
        if(k<xlist.size()) {
            for (int i = 0; i < k; i++) {
                pair<string, int> p(tmp[i].data, tmp[i].in_degree);
                result.push_back(p);
            }
        }
        else{
            for(int i=0;i<xlist.size();i++) {
                pair<string, int> p(tmp[i].data, tmp[i].in_degree);
                result.push_back(p);
            }
        }
    }
    else {//出度
        sort(tmp.begin(), tmp.end(), mycompout);
        if (k < xlist.size()) {
            for (int i = 0; i < k; i++) {
                pair<string, int> p(tmp[i].data, tmp[i].out_degree);
                result.push_back(p);
            }
        }
        else {
            for (int i = 0; i < xlist.size(); i++) {
                pair<string, int> p(tmp[i].data, tmp[i].out_degree);
                result.push_back(p);
            }
        }
    }
    return result;
}
bool OLGraph::TopologicalSort(){
    vector<VexNode> xxlist(xlist);
    ArcBox * p;
    stack<int> S;
    for(int i=0;i<vexnum;i++){
        if(xxlist[i].in_degree==0)
            S.push(i);
    }
    int count=0;
    while(!S.empty()){
        int tmp=S.top();
        S.pop();
        count++;
        for(p=xxlist[tmp].firstout;p;p=p->tlink){
            int k=p->headvex;
            if(!(--xxlist[k].in_degree))//k点入度减1后是否为0
                S.push(k);
        }//for
    }//while
    if(count<vexnum)return false;//有回路
    else
        return true;
}
double OLGraph::arcs(int a,int b)//求a,b两点之间是否弧a->b
{
    ArcBox *p;
    for(p=xlist[a].firstout;p;p=p->tlink){
        if(p->headvex==b){
            return p->money;
        }
    }
    return INFINITY;
}
void OLGraph::create(AccountTree *atree,string filename){
    vector<simtrans> vecArc;
    ifstream file;
    file.open(filename);
    if(!file.is_open()){
        cout<<"Diagraph: trans.csv open fail\n";
        exit(0);
    }
    string tmp,from,to;
    double mon;
    getline(file,tmp);//读取标题行
    while(file.peek()!=EOF){
        getline(file,tmp,',');
        getline(file,tmp,',');
        getline(file,from,',');
        getline(file,tmp,',');
        mon= string2double(tmp);
        getline(file,to);
        simtrans sim(from,to,mon);
        vecArc.push_back(sim);
    }
    file.close();
    Insert(atree->getAllAccount(),vecArc);
}
void OLGraph::create2(AccountTree *atree, LinkList &l)//利用已有的区块链进行图的初始化
{
    vector<simtrans> vecArc;
    string tmp,from,to;
    double mon;
    l.reset();//将区块链游标置于首位
    while(!l.endOfList())
    {
        Block b;
        b=l.data();
        vector<void *> result;
        b.transTree.Inorder(result);
        for(int i=0;i<result.size();i++) {
            pair<long long, Transaction> *ptr;
            ptr = (pair<long long, Transaction> *) result[i];
            from=ptr->second.from;
            to=ptr->second.to;
            mon=ptr->second.amount;
            simtrans sim(from,to,mon);
            vecArc.push_back(sim);
        }
        l.next();
    }
    Insert(atree->getAllAccount(),vecArc);
}

void OLGraph::Short1(int start,vector<double> &dis,vector<int> &pre)
{
    bool visit[vexnum];//用来标记点是否被当做基本点
    for(int i=0;i<vexnum;i++){
        dis.push_back(arcs(start,i));//求各个点到目标点距离
        visit[i]=false;
        pre.push_back(0xfffff);//初始化每个点的大小为一个大数
    }
    visit[start] = true;//将起始点本身初始化
    dis[start] = 0;//第一次直接把起点标记为基点  起点到起点的距离为0
    pre[start] = start;//起点的前一个点设置为本身
    int i, j;
    int tempv = start;//tempv来存当前这轮迭代的基点
    for (i = 1; i < vexnum; i++)//n-1次循环
    {
        for (j = 0; j < vexnum; j++)//暴力搜索图中所有的点
        {
            if (!visit[j] && dis[tempv] + arcs(tempv,j) < dis[j])
                //如果j没被选为基点过 并且 基点到起点的距离加基点到j点边的距离
                //小于j点目前到起点的距离，那么就更新
            {
                dis[j] = dis[tempv] + arcs(tempv,j);
                pre[j] = tempv;
                //更新dis数组和前驱数组
            }
        }
        int temp =INFINITY;//找下一次迭代的新基点，就是选没当过基点，并且距离
        //起点距离最小的点
        for (j = 0; j < vexnum; j++)//暴力搜索每一个点
        {
            if (!visit[j])//如果没当过基点
            {
                if (dis[j] < temp)//不断找距离起点最小点
                {
                    temp = dis[j];
                    tempv = j;//更新temp值并记录这个点的下标
                }
            }
        }
        visit[tempv] = true;//OK这个tempv点是目前离起点最近的并且没当过基点的点
        //标记成true
    }
}

void OLGraph::Short2(int start,vector<double> &dis,vector<int> &pre)
{
    struct node{
        int p,w;
        node(int a,int b):p(a),w(b){}
        bool operator< (const node& b) const
        {
            return w>b.w;
        }
    };
    priority_queue<node>*sup=new priority_queue<node>;
    bool visit[vexnum];
    for(int i=0;i<vexnum;i++){
        visit[i]=false;
        dis.push_back(99999999);
        pre.push_back(start);
    }
    dis[start]=0;
    pre[start]=start;
    sup->push(node(start,0));
    while(!sup->empty())
    {
        node front=sup->top();
        sup->pop();
        int tempv=front.p;
        if(visit[tempv]){//是否没有当过基本点
            continue;
        }
        visit[tempv]=true;
        ArcBox *pp;
        for(pp=xlist[tempv].firstout;pp;pp=pp->tlink)//利用十字链表的存储结构可以避免暴力搜索出弧的头结点
        {
            int p=pp->headvex;
            if(!visit[p]&&(dis[tempv]+pp->money<dis[p]))//更新距离
            {
                dis[p]=dis[tempv]+pp->money;
                pre[p]=tempv;
                sup->push(node(p,dis[p]));
            }
        }
    }
    delete sup;
}
void OLGraph::InsertTransFromFile(string filename)//将用户新加入的交易信息插入到图中
{
    vector<simtrans> vecArc;
    ifstream file;
    file.open(filename);
    if(!file.is_open()){
        cout<<"Diagraph: trans.csv open fail\n";
        exit(0);
    }
    string tmp,from,to;
    double mon;
    while(file.peek()!=EOF){
        getline(file,tmp,',');
        getline(file,tmp,',');
        getline(file,from,',');
        getline(file,tmp,',');
        mon= string2double(tmp);
        getline(file,to);
        simtrans sim(from,to,mon);
        vecArc.push_back(sim);
    }
    int _arcnum=vecArc.size();
    //插入弧
    for(int i=0;i<_arcnum;i++){
        insertArc(vecArc[i].from,vecArc[i].to,vecArc[i].moneyy);
    }
    arcnum+=_arcnum;
    file.close();
}
#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    emit send2_1(ui->lineEdit->text(),ui->lineEdit_2->text(),ui->lineEdit_3->text(),ui->lineEdit_4->text(),ui->lineEdit_5->text());
    this->close();

}
#include "form2_2.h"
#include "ui_form2_2.h"

Form2_2::Form2_2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form2_2)
{
    ui->setupUi(this);
}

Form2_2::~Form2_2()
{
    delete ui;
}

void Form2_2::on_pushButton_clicked()
{

    emit send2_2(ui->lineEdit->text(),ui->lineEdit_2->text());
    this->close();
}
#include "form2_3.h"
#include "ui_form2_3.h"

Form2_3::Form2_3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form2_3)
{
    ui->setupUi(this);
}

Form2_3::~Form2_3()
{
    delete ui;
}

void Form2_3::on_pushButton_clicked()
{
    emit send2_3(ui->lineEdit->text(),ui->lineEdit_2->text());
    this->close();
}
#include "form3_2.h"
#include "ui_form3_2.h"

Form3_2::Form3_2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form3_2)
{
    ui->setupUi(this);
}

Form3_2::~Form3_2()
{
    delete ui;
}

void Form3_2::on_pushButton_clicked()
{
    emit send3_2(ui->lineEdit->text(),ui->lineEdit_2->text());
    this->close();
}

#include "form3_4.h"
#include "ui_form3_4.h"

Form3_4::Form3_4(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form3_4)
{
    ui->setupUi(this);
}

Form3_4::~Form3_4()
{
    delete ui;
}

void Form3_4::on_pushButton_clicked()
{
    emit send3_4(ui->lineEdit->text(),ui->lineEdit_2->text());
    this->close();
}

//
// Created by 24301 on 2023/1/24.
//

#include "Handle.h"
#include "insert.h"
int Handle::loadblock(){
    LoadBlock lb(InitalBlockName);
    lb.InitalLinkedlist(l,linkedlist_index);
    return lb.size;
}
int Handle::loadtrans() {
    LoadTrans lt(InitalTransactionName);
    lt.InitalLinkedlist(l,linkedlist_index);
    return lt.getSize();
}
void Handle::loadAccountTree() {
    atree.loadFromlinkedlist(l);
}
void Handle::loadGraph() {
    graph.create2(&atree,l);
}
void Handle::Insert() {
    atree.InsertFromFile(InsertName,&linkedlist_index);
    graph.InsertTransFromFile(InsertName);
    //Insert::insert(atree,graph,InsertName,&linkedlist_index);
}

#include "initial.h"
#include "ui_initial.h"
#include <QFileDialog>
initial::initial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::initial)
{
    ui->setupUi(this);
}

initial::~initial()
{
    delete ui;
}

void initial::on_pushButton_clicked()
{
    //打开一个文件对话框
    QString filename;
    filename=QFileDialog::getOpenFileName(this,"打开Block文件");
    ui->lineEdit->setText(filename);
}


void initial::on_pushButton_2_clicked()
{
    //打开一个文件对话框
    QString filename;
    filename=QFileDialog::getOpenFileName(this,"打开Transaction文件");
    ui->lineEdit_2->setText(filename);
}

void initial::on_pushButton_3_clicked()
{
    emit send(ui->lineEdit->text(),ui->lineEdit_2->text());
    this->close();
}

#include "insert.h"

double string2double(string str) {
        stringstream ss;
        ss << str;
        double result;
        ss >> result;
        return result;
}
Insert::Insert(string _name)
{
    name=_name;
}
void Insert::insert(AccountTree &atree,OLGraph &graph,string filename, AVLTree<int,Block*> *blockAVLptr)
{
     ifstream file;
     file.open(filename);

     //声明局部变量
     string txID,blockID,from,money,to,tmp;
     vector<simtrans> vecArc;//记录已经生成的弧
     //读取文件直到末尾
     while(file.peek()!=EOF){
         getline(file,txID,',');//读取交易编号
         getline(file,blockID,',');//读取区块编号
         getline(file,from,',');//读取转出账户ID
         getline(file,money,',');//读取转账金额
         getline(file,to);//读取读取转入账户ID

         //插入账户树
         if (!atree.isexited(from))
            atree.size++;
         if (!atree.isexited(to))
            atree.size++;
         double amount=string2double(money);
         Account * acc=&atree.IDtree[from];
         acc->tree[(blockAVLptr->Find(atoi(blockID.c_str()))->_Kv.second)->block_timestamp].push_back(-amount);
         acc->ID=from;
         acc->amount+=(-amount);

         acc=&atree.IDtree[to];
         acc->tree[(blockAVLptr->Find(atoi(blockID.c_str()))->_Kv.second)->block_timestamp].push_back(amount);
         acc->ID=to;
         acc->amount+=(amount);
         //插入图
         simtrans sim(from,to,string2double(money));
         vecArc.push_back(sim);
     }//while
     atree.initalvec();
     int _arcnum=vecArc.size();
     for(int i=0;i<_arcnum;i++){
        graph.insertArc(vecArc[i].from,vecArc[i].to,vecArc[i].moneyy);
     }
     graph.addarcnum(_arcnum);
     file.close();
}

//
// Created by 24301 on 2023/1/19.
//
//
// Created by 24301 on 2023/1/9.
//
#include "LinkedList.h"
Node::Node(const Block &data, Node *next) : data(data), next(next) {}
Node *Node::nextNode()
{
    return next;
}
void Node::insertAfter(Node *p)
{
    p->next = next;
    next = p;
}
Node *Node::deleteAfter()
{
    Node *tmpPtr = next;
    if (next == NULL)
        return NULL;
    next = next->next;
    return tmpPtr;
}

Node *LinkList::get_front()
{
    return front;
}
LinkList::LinkList()
{
    size = 0;
    position = 0;
}
LinkList::~LinkList()
{
    clear();
}
void LinkList::deleteCurrent()
{
    if (prevPtr == NULL)
    {
        Node *tmppPtr = currPtr;
        currPtr = currPtr->nextNode();
        front = currPtr;
        delete tmppPtr;
    }
    else
    {
        Node *tmp = prevPtr->deleteAfter();
        currPtr = tmp->nextNode();
        delete tmp;
    }
    size--;
}
void LinkList::clear()
{
    for (int i = 0; i < size - 1; i++)
    {
        Node *delPtr;
        delPtr = front;
        front = front->nextNode();
        freeNode(delPtr);
    }
    rear = NULL;
    front = NULL;
    size = 0;
    size = 0;
}
bool LinkList::endOfList()
{
    return currPtr == NULL;
}
int LinkList::currentPosition()
{
    return position;
}
void LinkList::insertFront(Block &item)
{
    Node *tmp = newNode(item, front);
    front = tmp;
    reset();
    size++;
}
void LinkList::insertAt(Block &item)
{
    if (currPtr == front)
    {
        insertFront(item);
    }
    Node *tmp = newNode(item);
    prevPtr->insertAfter(tmp);
    size++;
    reset(position);
}
Block &LinkList::data()
{
    return currPtr->data;
}
Node *LinkList::newNode(Block item, Node *ptrNext)
{
    Node *newNode;
    newNode = new Node(item, ptrNext);
    if (newNode == NULL)
    {
        cout << "newNode error" << endl;
        exit(1);
    }
    return newNode;
}
void LinkList::freeNode(Node *p)
{
    delete p;
}
int LinkList::getSize()
{
    return size;
}
bool LinkList::isEmpty()
{
    return size == 0;
}
void LinkList::reset(int pos)
{
    if (0 <= pos && pos <= size)
    {
        position = 0;
        prevPtr = NULL;
        currPtr = front;
        while (pos--)
            next();
    }
    else
    {
        position = pos;
        prevPtr = NULL;
        currPtr = NULL;
    }
}
void LinkList::next()
{
    position++;
    prevPtr = currPtr;
    if (currPtr != NULL)
        currPtr = prevPtr->nextNode();
}
void LinkList::insertRear(Block &item)
{
    Node *tmp = newNode(item);
    if (isEmpty())
    {
        front = rear = tmp;
    }
    else
    {
        rear->insertAfter(tmp);
        rear = tmp;
    }
    size++;
    reset(position);
}
void LinkList::insertAfter(Block &item)
{
    if (currPtr != NULL)
    {
        Node *tmp = newNode(item, currPtr->nextNode());
        currPtr->insertAfter(tmp);
        if (rear == currPtr)
        {
            rear = tmp;
        }
        size++;
    }
}
Block LinkList::deleteFront()
{
    if (isEmpty())
    {
        cout << " delete error" << endl;
        exit(1);
    }
    Node *delNode = front;
    front = front->nextNode();
    if (--size == 0)
        rear = NULL;
    reset(--position);
    Block item = delNode->data;
    freeNode(delNode);
    return item;
}


//
// Created by 24301 on 2023/1/19.
//

#include "LoadBlock.h"
LoadBlock::LoadBlock(std::string name):filename(name),size(0)
{
    file.open(filename);
    if(!file.is_open())//打开文件失败
    {
        cout << "The filename of Block failed to open.\n";
        exit(0);
    }
}
LoadBlock::~LoadBlock() {
    file.close();
}
void LoadBlock::InitalLinkedlist(LinkList &l,AVLTree<int,Block*>& avltree)
{
    Block block;
    string tmp;
    getline(file,tmp);//读取标题行
    int index=0;
    while(file.peek() != EOF)
    {
        clock_t s,e;
        s=clock();
        getline(file,tmp,',');
        block.blockID=atoi(tmp.c_str());
        getline(file,block.hash,',');
        getline(file,tmp);
        block.block_timestamp=atoll(tmp.c_str());
        index++;
        l.insertRear(block);
        e=clock();
        Block* tmpp=l.getRearPtr();
        tmpp->time+=((double)(e-s)/CLOCKS_PER_SEC);

        pair<int,Block*> p(block.blockID,l.getRearPtr());
        avltree.Insert(p);
    }
    size=index;
}

//
// Created by 24301 on 2023/1/19.
//

#include "LoadTrans.h"

void LoadTrans::InitalLinkedlist(LinkList &l,AVLTree<int,Block*> avltree) {
    file.open(filename);
    if(!file.is_open()){
        cout<<"Trans file open file.\n";
        exit(0);
    }

    Transaction trans;
    string tmp;
    l.reset();
    getline(file,tmp);
    while(file.peek()!=EOF) {
        clock_t s,e;
        s=clock();
        getline(file, tmp, ',');
        trans.tx_id = atoll(tmp.c_str());
        getline(file, tmp, ',');
        trans.blockID = atoi(tmp.c_str());
        getline(file, trans.from, ',');
        getline(file, tmp, ',');
        trans.amount = string2double(tmp);
        getline(file, trans.to);
        //cout<<trans.blockID<<" "<<trans.from<<" "<<trans.amount<<" "<<trans.to<<endl;
        AVLTreeNode<int,Block*>* tmp_ptr =avltree.Find(trans.blockID);
        if(tmp_ptr== nullptr){
            fail++;
        }
        else
            success++;
        size++;
        pair<long long,Transaction> p(trans.tx_id,trans);
        (tmp_ptr->_Kv.second)->transTree.Insert(p);
        e=clock();
        (tmp_ptr->_Kv.second)->time+=(double)(e-s)/CLOCKS_PER_SEC;
    }
}
void LoadTrans::Inital(LinkList &l){
    file.open(filename);
    if(!file.is_open()){
        cout<<"Trans file open file.\n";
        exit(0);
    }

    Transaction trans;
    string tmp;
    l.reset();
    getline(file,tmp);
    while(file.peek()!=EOF) {
        getline(file, tmp, ',');
        trans.tx_id = atoll(tmp.c_str());
        getline(file, tmp, ',');
        trans.blockID = atoi(tmp.c_str());
        getline(file, trans.from, ',');
        getline(file, tmp, ',');
        trans.amount = string2double(tmp);
        getline(file, trans.to);
        //cout<<trans.blockID<<" "<<trans.from<<" "<<trans.amount<<" "<<trans.to<<endl;
        // AVLTreeNode<int,Block*>* tmp_ptr =avltree.Find(trans.blockID);
        l.reset();
        while(!l.endOfList()){
            if(l.data().blockID==trans.blockID){
                pair<int,Transaction> p(trans.blockID,trans);
                l.data().transTree.Insert(p);
                size++;
                success++;
                break;
            }
            else{
                l.next();
            }
        }
    }
}
void LoadTrans::Insert(LinkList &l,string name,AVLTree<int,Block*> avltree)//插入新的交易
{
    file.open(name);
    if(!file.is_open()){
        cout<<"Insert Trans file open file.\n";
        exit(0);
    }
    Transaction trans;
    string tmp;
    l.reset();
    getline(file,tmp);
    while(file.peek()!=EOF) {
        clock_t s,e;
        s=clock();
        getline(file, tmp, ',');
        trans.tx_id = atoll(tmp.c_str());
        getline(file, tmp, ',');
        trans.blockID = atoi(tmp.c_str());
        getline(file, trans.from, ',');
        getline(file, tmp, ',');
        trans.amount = string2double(tmp);
        getline(file, trans.to);
        AVLTreeNode<int,Block*>* tmp_ptr =avltree.Find(trans.blockID);
        if(tmp_ptr== nullptr){
            fail++;
        }
        else
            success++;
        size++;
        pair<long long,Transaction> p(trans.tx_id,trans);
        (tmp_ptr->_Kv.second)->transTree.Insert(p);
        e=clock();
        (tmp_ptr->_Kv.second)->time+=(double)(e-s)/CLOCKS_PER_SEC;
    }
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QLabel>
#include <fstream>
#include <QMdiSubWindow>
#include "form.h"
#include "form2_2.h"
#include "form2_3.h"
#include "form3_2.h"
#include "form3_4.h"
#include "initial.h"
#include <QFileInfo>

QString longlong2QString(long long src){
    stringstream ss;
    ss<<src;
    string str;
    ss>>str;
    return QString::fromStdString(str);
}
QString double2QString(double src){
    stringstream ss;
    ss<<src;
    string str;
    ss>>str;
    return QString::fromStdString(str);
}
QString int2QString(int src){
    stringstream ss;
    ss<<src;
    string str;
    ss>>str;
    return QString::fromStdString(str);
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QLabel *mylable=new QLabel("By: 吴国正",this);
    this->statusBar()->addPermanentWidget(mylable);
    ui->textBrowser->setReadOnly(true);
    ui->textBrowser->append("欢迎使用。");
    isinitial=false;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionBlock_triggered()//加载block的action
{
    //打开一个文件对话框
    clock_t s,e;
    s=clock();
    QString filename;
    filename=QFileDialog::getOpenFileName(this,"打开Block文件");
    handle.setname1(filename.toStdString());
    handle.loadblock();
    e=clock();
    QStatusBar *sb=this->statusBar();
    QString runtime;
    runtime="加载Block所用时间为：";
    runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s";
    sb->showMessage(runtime,100000);
}


void MainWindow::on_actionTransaction_triggered()//加载transaction的action
{
    //打开一个文件对话框
    clock_t s,e;
    s=clock();
    QString filename;
    filename=QFileDialog::getOpenFileName(this,"打开Transaction文件");
    handle.setname2(filename.toStdString());
    handle.loadtrans();
    handle.loadAccountTree();
    e=clock();
    QStatusBar *sb=this->statusBar();
    QString runtime;
    runtime="加载Transaction所用时间为：";
    runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s";
    sb->showMessage(runtime,100000);
}


void MainWindow::on_actionFile_triggered()//加载插入交易的action
{
    if(!isinitial)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("请先进行初始化！！！");
        QMessageBox::critical(this,"错误","请先进行初始化!");
        return;
    }
    //打开一个文件对话框
    QString filename;
    filename=QFileDialog::getOpenFileName(this,"打开待插入的交易文件");
    ifstream file;
    file.open(filename.toStdString());
    if(!file.is_open()){
        ui->textBrowser->clear();
        ui->textBrowser->append("文件打开失败，请检查输入后重新尝试。");
        QMessageBox::warning(this,"警告","文件打开失败，请检查输入后重新尝试。");
        return;
    }
    file.close();
    handle.setname3(filename.toStdString());

    clock_t s,e;
    s=clock();

    handle.Insert();
    ui->textBrowser->clear();
    ui->textBrowser->append("添加新交易成功。");

    e=clock();
    QStatusBar *sb=this->statusBar();
    display="添加新交易所用时间为：";
    display+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s";
    sb->showMessage(display,1000000);
}

void MainWindow::on_pushButton_clicked()//清除显示区域
{
    display.clear();
    ui->textBrowser->setText(display);
}


void MainWindow::on_action2_1_triggered()//点击查询功能2.1，弹出子窗口来接收参数
{
    if(!isinitial)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("请先进行初始化！！！");
        QMessageBox::critical(this,"错误","请先进行初始化!");
        return;
    }
       Form *f=new Form;
       f->setAttribute(Qt::WA_DeleteOnClose);
       f->show();
       connect(f,SIGNAL(send2_1(QString ,QString ,QString ,QString ,QString)),this,SLOT(set2_1(QString,QString ,QString ,QString,QString)));
}
void MainWindow::set2_1(QString ltime,QString rtime,QString ID,QString type,QString k){//利用收到的参数进行2.1查询，将结果显示在显示区域上。
    clock_t s,e;
    s=clock();
    if(ID.isEmpty()){
        ID="1BAjnzBx6jvuQJWxByQGtiYVJUCdjwiz37";
    }
    if(!handle.atree.IDtree.isexited(ID.toStdString())){
        ui->textBrowser->clear();
        ui->textBrowser->append("您所输入的账户ID不存在，请检查输入后重新尝试。");
        QMessageBox::warning(this,"警告","您所输入的账户ID不存在，请检查输入后重新尝试。");
        return;
    }
    if(k.isEmpty()){//未输入k值
        k="5";
    }
    if(k.toInt()<=0){
        ui->textBrowser->clear();
        ui->textBrowser->append("您所输入的数量非法（<=0），请检查输入后重新尝试。");
        QMessageBox::warning(this,"警告","您所输入的数量非法（<=0），请检查输入后重新尝试。");
        return;
    }
    if(ltime.isEmpty()){
        ltime="1284753029";
    }
    if(rtime.isEmpty()){
        rtime="1304892900";
    }
    if(type.isEmpty()){
        type="0";
    }
    int allsize=0;
    vector<pair<long long, double>> result;
    result=handle.atree.search1(ID.toStdString(),ltime.toLongLong(),rtime.toLongLong(),type.toInt(),allsize,k.toInt());
    e=clock();
    QStatusBar *sb=this->statusBar();
    QString runtime;
    runtime="查找指定账号在一个时间段内的所有转入或转出记录所用时间为：";
    runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s";
    sb->showMessage(runtime,1000000);
    display.clear();
    if(type=="0"){
        display+="您查询的是转入和转出记录的前"+k+"条\n";
    }
    else if(type.toInt()>0){
        display+="您查询的是转入记录的前"+k+"条\n";
    }
    else{
        display+="您查询的是转出记录的前"+k+"条\n";
    }
    display+="这一时间段总的记录数为："+double2QString(allsize)+"\n";
    display+=" 交易时间             交易金额\n";
    for(int i=0;i<(int)result.size();i++){
        display+=double2QString(i+1)+"   "+longlong2QString(result[i].first);
        display+=(":         "+double2QString(result[i].second)+"\n");
    }
    if(result.size()==0){
        display="您所查找的内容为空。\n";
        ui->textBrowser->setText(display);
    }
    else{
        ui->textBrowser->setText(display);
        ui->textBrowser->autoFormatting();
    }
}

void MainWindow::on_action2_2_triggered()//点击查询功能2.2，弹出子窗口来接收参数
{
    if(!isinitial)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("请先进行初始化！！！");
        QMessageBox::critical(this,"错误","请先进行初始化!");
        return;
    }
    Form2_2 *f=new Form2_2;
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->show();
    connect(f,SIGNAL(send2_2(QString ,QString )),this,SLOT(set2_2(QString,QString)));
}

void MainWindow::set2_2(QString ID,QString time)//利用收到的参数进行2.2查询，将结果显示在显示区域上。
{
    if(ID.isEmpty()){
        ID="1BAjnzBx6jvuQJWxByQGtiYVJUCdjwiz37";
    }
    if(time.isEmpty()){
        time="1304892900";
    }
    if(!handle.atree.IDtree.isexited(ID.toStdString())){
        ui->textBrowser->clear();
        ui->textBrowser->append("您所输入的账户ID不存在，请检查输入后重新尝试。");
        QMessageBox::warning(this,"警告","您所输入的账户ID不存在，请检查输入后重新尝试。");
        return;
    }
    else{
        display.clear();
        clock_t s,e;
        s=clock();
        double result=0.0;
        result=handle.atree.search2(ID.toStdString(),time.toLongLong());
        display+="账户"+ID+"在时刻"+time+"的余额为："+double2QString(result);
        ui->textBrowser->setText(display);
        e=clock();
        QStatusBar *sb=this->statusBar();
        QString runtime;
        runtime="查询某个账号在某个时刻的金额（允许有负数）所用时间为：";
        runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s";
        sb->showMessage(runtime,1000000);
    }
}

void MainWindow::on_action2_3_triggered()//点击查询功能2.2，弹出子窗口来接收参数
{
    if(!isinitial)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("请先进行初始化！！！");
        QMessageBox::critical(this,"错误","请先进行初始化!");
        return;
    }
    Form2_3 *f=new Form2_3;
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->show();
    connect(f,SIGNAL(send2_3(QString ,QString )),this,SLOT(set2_3(QString,QString)));
}
void MainWindow::set2_3(QString time,QString k)//利用收到的参数进行2.3查询，将结果显示在显示区域上。
{

    if(time.isEmpty()){//时间为空
        time="1304892900";
    }
    if(k.isEmpty()){//未输入k值
        k="50";
    }
    if(k.toInt()<=0){
        ui->textBrowser->clear();
        ui->textBrowser->append("您所输入的数量非法（<=0），请检查输入后重新尝试。");
        QMessageBox::warning(this,"警告","您所输入的数量非法（<=0），请检查输入后重新尝试。");
        return;
    }
    clock_t s,e;
    s=clock();
    vector< pair<string,double>> result;
    result=handle.atree.search3(time.toLongLong(),k.toInt());
    e=clock();
    QStatusBar *sb=this->statusBar();
    QString runtime;
    runtime="输出某个时刻的福布斯富豪榜所用时间为：";
    runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s";
    sb->showMessage(runtime,1000000);
    if(result.size()==0){
        display="您所查找的内容为空。\n";
        ui->textBrowser->setText(display);
        return;
    }
    display.clear();
    ui->textBrowser->clear();
    display=time+"时刻的福布斯富豪榜如下，您查询的是前"+k+"项\n";
    ui->textBrowser->append(display);
    ui->textBrowser->append("       账户                                                                                        余额");//88个空格
    for(int i=0;i<(int)result.size();i++){ 
        ui->textBrowser->append(double2QString(i+1)+"  "+QString::fromStdString(result[i].first)+("                              "+double2QString(result[i].second)+"\n"));
    }

}

void MainWindow::on_action3_1_triggered()//构造交易关系图
{
    if(!isinitial)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("请先进行初始化！！！");
        QMessageBox::critical(this,"错误","请先进行初始化!");
        return;
    }
    handle.loadGraph();
    display.clear();
    display="交易关系图构造完成\n";
    ui->textBrowser->setText(display);
}

void MainWindow::on_action3_2_triggered()//点击分析功能3.2，弹出子窗口来接收参数
{
    if(!isinitial)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("请先进行初始化！！！");
        QMessageBox::critical(this,"错误","请先进行初始化!");
        return;
    }
    Form3_2 *f=new Form3_2;
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->show();
    connect(f,SIGNAL(send3_2(QString ,QString)),this,SLOT(set3_2(QString,QString)));
}

void MainWindow::set3_2(QString k,QString type)//利用收到的参数进行3.2查询，将结果显示在显示区域上
{
    clock_t s,e;
    s=clock();
    if(k.isEmpty()){//未输入k值
        k="5";
    }
    if(k.toInt()<=0){
        ui->textBrowser->clear();
        ui->textBrowser->append("您所输入的数量非法（<=0），请检查输入后重新尝试。");
        QMessageBox::warning(this,"警告","您所输入的数量非法（<=0），请检查输入后重新尝试。");
        return;
    }
    display.clear();
    vector<pair<string,int>> result;
    result=handle.graph.getTop(type.toInt(),k.toInt());
    e=clock();
   QStatusBar *sb=this->statusBar();
   QString runtime;
   runtime="分析出度/入度所用时间为：";
   runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s";
   sb->showMessage(runtime,1000000);
    display+="平均入度：";
    display+=double2QString(handle.graph.av_in_degree());
    display+="   平均出度：";
    display+=double2QString(handle.graph.av_out_degree())+"\n";
    if(type.toInt()>=0){
        display+="出度最大的前"+k+"个如下：\n\n";
        display+="            账户                                                                    出度\n";
    }
    else{
        display+="入度最大的前"+k+"个如下：\n\n";
        display+="            账户                                                                    入度\n";
    }
    for(int i=0;i<(int)result.size();i++){
        display+=double2QString(i+1)+"   "+ QString::fromStdString(result[i].first);
        display+=("            "+double2QString(result[i].second)+"\n");
    }
    ui->textBrowser->setText(display);
}


void MainWindow::on_action3_3_triggered()//查看是否有环存在，将结果显示在屏幕上。
{
    if(!isinitial)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("请先进行初始化！！！");
        QMessageBox::critical(this,"错误","请先进行初始化!");
        return;
    }
    clock_t s,e;
    s=clock();
    display.clear();
    if(!handle.graph.TopologicalSort()){
        display+="YES，交易关系图中有环存在。\n";
    }
    else{
        display+="NO，交易关系图中无环存在。\n";
    }
    ui->textBrowser->setText(display);
    e=clock();
   QStatusBar *sb=this->statusBar();
   QString runtime;
   runtime="分析是否有环存在所用时间为：";
   runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s";
   sb->showMessage(runtime,100000);
}


void MainWindow::on_action3_4_triggered()
{
    if(!isinitial)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("请先进行初始化！！！");
        QMessageBox::critical(this,"错误","请先进行初始化!");
        return;
    }
    Form3_4 *f=new Form3_4;
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->show();
    connect(f,SIGNAL(send3_4(QString,QString)),this,SLOT(set3_4(QString,QString)));
}

void MainWindow::set3_4(QString ID,QString path){
    QString filePath;
    if(ID.isEmpty()){
        ID="1BAjnzBx6jvuQJWxByQGtiYVJUCdjwiz37";
    }
    if(!handle.atree.IDtree.isexited(ID.toStdString())){

        ui->textBrowser->clear();
        ui->textBrowser->append("您所输入的账户ID不存在，请检查输入后重新尝试。");
        QMessageBox::warning(this,"警告","您所输入的账户ID不存在，请检查输入后重新尝试。");
        return;
    }
    display.clear();
    vector<double> dis;
    vector<int> pre;
    int start=handle.graph.LocateVex(ID.toStdString());
    clock_t s,e;
    s=clock();
    handle.graph.Short2(start,dis,pre);
    e=clock();
    QStatusBar *sb=this->statusBar();
    QString runtime;
    runtime="分析最短路径所用时间为：";
    runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s";
    sb->showMessage(runtime,100000);
    if(path.isEmpty()){//未输入指定路径
        filePath="shortpath.csv";
        display="未输入指定路径，将输入到默认路径（工作目录下），文件名为 shortpath.csv\n";
        ui->textBrowser->setText(display);
        ofstream ofile;
        ofile.open("shortpath.csv");
        if(!ofile.is_open()){
            display+="打开文件失败\n";
            ui->textBrowser->setText(display);
        }
        else{
            for(int i=0;i<(int)dis.size();i++)
            {
                if(dis[i]<99999999){
                ofile<<handle.graph[pre[i]]<<"---->"<<handle.graph[i]<<":     "<<dis[i]<<endl;}
            }
            display+="打开文件成功\n";
            ui->textBrowser->setText(display);
        }
            ofile.close();
    }
    else{
        filePath=path;
        display="已输入指定路径···\n";
        ui->textBrowser->setText(display);
        ofstream ofile;
        ofile.open(path.toStdString());
        if(!ofile.is_open()){
            display+="打开文件失败\n";
            ui->textBrowser->setText(display);
        }
        else{
            for(int i=0;i<(int)dis.size();i++)
            {
                if(dis[i]<99999999)
                {
                    ofile<<handle.graph[pre[i]]<<"----->"<<handle.graph[i]<<":                              "<<dis[i]<<endl;
                }
            }
             display+="打开文件成功\n";
             ui->textBrowser->setText(display);
        }
        ofile.close();
    }
    QFileInfo info(filePath);
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->textBrowser->append("文件打开失败");
        return;
    }
    QTextStream readStream(&file);

    while(!readStream.atEnd())
    {
        ui->textBrowser->append((readStream.readLine()));
    }
    file.close();
}


void MainWindow::on_actioninitial_triggered()
{    if(isinitial)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("已经初始化！！！");
        QMessageBox::critical(this,"错误","已经初始化!");
        return;
    }
    initial *f=new initial;
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->show();
    connect(f,SIGNAL(send(QString,QString)),this,SLOT(set(QString,QString)));
}
void MainWindow::set(QString path1,QString path2){

    if(path1.isEmpty()){
        path1="E:\\Data2023\\block_part1.csv";
    }
    if(path2.isEmpty()){
        path2="E:\\Data2023\\tx_data_part1_v2.csv";
    }
    ifstream file;
    file.open(path1.toStdString());
    if(!file.is_open()){
        ui->textBrowser->clear();
        ui->textBrowser->append("区块文件打开失败，请检查输入后重新尝试。");
        QMessageBox::warning(this,"警告","区块文件打开失败，请检查输入后重新尝试。");
        return;
    }
    file.close();
    file.open(path2.toStdString());
    if(!file.is_open()){
        ui->textBrowser->clear();
        ui->textBrowser->append("交易文件打开失败，请检查输入后重新尝试。");
        QMessageBox::warning(this,"警告","交易文件打开失败，请检查输入后重新尝试。");
        return;
    }
    file.close();
    isinitial=true;
    QString runtime;
    clock_t s,e;
    double alltime=0.0;
    s=clock();
    handle.setname1(path1.toStdString());
    int blocksize=handle.loadblock();
    e=clock();
    runtime+="加载区块所用时间：";
    runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s   ";
    alltime+=(double)(e-s)/CLOCKS_PER_SEC;
    ui->textBrowser->clear();


    s=clock();
    handle.setname2(path2.toStdString());
    int transsize=handle.loadtrans();
    e=clock();
    runtime+="加载交易所用时间：";
    runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s   ";
    alltime+=(double)(e-s)/CLOCKS_PER_SEC;
    ui->textBrowser->append("区块链加载成功···");
    ui->textBrowser->append("区块链上共有"+int2QString(blocksize)+"个区块。");
    ui->textBrowser->append("区块链上共有"+int2QString(transsize)+"个交易。");
    ui->textBrowser->append("\n");

    s=clock();
    handle.loadAccountTree();
    e=clock();
    runtime+="加载账户所用时间：";
    runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s   ";
    alltime+=(double)(e-s)/CLOCKS_PER_SEC;
    ui->textBrowser->append("账户加载成功···");
    ui->textBrowser->append("共有账户"+int2QString(handle.atree.getsize())+"个。");
    ui->textBrowser->append("\n");

    s=clock();
    handle.loadGraph();
    e=clock();
    runtime+="构造图所用时间：";
    runtime+=double2QString((double)(e-s)/CLOCKS_PER_SEC)+"s   ";
    alltime+=(double)(e-s)/CLOCKS_PER_SEC;
    ui->textBrowser->append("交易关系图加载成功···");
    ui->textBrowser->append("共有顶点"+int2QString(handle.graph.getVexnum())+"个。");
    ui->textBrowser->append("共有弧"+int2QString(handle.graph.getArcnum())+"个。");

    runtime+="初始化总用时：";
    runtime+=double2QString(alltime)+"s";


    QStatusBar *sb=this->statusBar();
    sb->showMessage(runtime,1000000);
}


#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
