#include <cstddef>
#include <chrono>
#include <unordered_map>
#include <mutex>

#define PAGESIZE 4096//定义页的大小 
#define CACHESIZE 10//定义cache的大小

size_t click=0;//全局时间

class node{
public:
    //构造函数就传进来了还是后面直接把资源赋值给已经建好的长度固定的链表
    node():data_(nullptr),clock_(0),next_(nullptr),prev_(nullptr){}
    ~node(){
        
    }
private:
    void* data_;
    size_t clock_;//记录最近一次被使用的时间
    node* next_;
    node* prev_;
};


//淘汰最近最久未使用的页来装新页
class lrucache{
public:
    lrucache():head_(nullptr),size_(0){

    }

    //命中？怎么命中？传入的什么参数？
    void get(){
        node* node;

        //先取下来
        takeDown(node);
        //头插法放到最前面去
        putFront(node);
        //数据不需要动，动一下记录的时间
        node->clock_=click++;
    }

    //放新页
    void put(void* data){
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(size_==CACHESIZE){
                auto node=evictOne();
                node->data_=data;
                node->clock_=click++;

                //头插法放入表头
                node->next_=head_;
                head_->prev=node;
                head_=node;
                size_++;
            }
        }
    }

    //取下节点，但是成员可能还需要修改
    node* takeDown(node* target){
        target->prev_->next_=target->next_;
        target->next_->prev_=target->prev_;
        target->prev_=nullptr;
        target->next_=nullptr;
        return target;
    }

    //头插法挂上节点，成员可能还需要修改，但是clock_=click++是肯定的
    void putFront(node* node){
        node->next_=head_;
        head_->prev_=node;
        head_=node;
        node->clock_=click++;
    }

    node* evictOne(){
        //bianli找最近最久未使用节点
        node* cur=head_;
        node* target=cur;
        while(cur){
            if(cur->clock_<target){
                target=cur->clock_;
                target=cur;
            }
            cur=cur->next_;
        }

        //找到后把这个节点摘下来把里面的资源处理了，然后返回被使用 
        target=takeDown(target);
        writeData(target->data_);

        return target;
    }

    //被选择淘汰的结点里的数据处理
    void writeData(void* ptr){

    }

private:
    std::mutex mutex_;
    size_t size_;
    node* head_;
    std::unordered_map<node*,void*> map_;//为什么不干脆用map?因为要使用局部性原理的性质
};

//这个双向链表是一开始就在，还是说一步步扩容的？