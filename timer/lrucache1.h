#include <cstddef>
#include <unordered_map>
#include <mutex>

#define PAGESIZE 4096//定义页的大小 
#define CACHESIZE 10//定义cache的大小

template <typename K,typename V>
class node{
public:
    //构造函数就传进来了还是后面直接把资源赋值给已经建好的长度固定的链表
    node(K key,V value):key_(key),value_(value),next_(nullptr),prev_(nullptr){}
    ~node(){
        
    }
private:
    K key_;
    V value_;
    node* next_;
    node* prev_;
};


//淘汰最近最久未使用的页来装新页
template <typename K,typename V>
class lrucache{
public:
    //head_这样初始化xing么？head_指向的是一个对象
    lrucache():head_(nullptr),,tail_(nullptr),size_(0){
        head_->prev_=tail_;
        head_->next_=tail_;
        tail_->next_=head_;
        tail_->prev_=head_;
    }

    ~lrucache(){
        //head_和tail_之间的node对象都是匿名状态，需要在这里把它们全部释放了？
        //还是那个问题，这里面保管的资源怎么处理？key和value可能是指针类型
        node* p=head_->next_;
        node* tmp=nullptr;
        while(p!=tail_){
            tmp=p;
            p=p->next_;
            tmp::~node();//这样写?
        }

        //map_呢？
    }
    
    //访问
    V get(const K& key){
        std::lock_guard<std::mutex> lock(mutex_);
        
        if(map_.count(key)){
            //这个过程中map_[key]发生了变化怎么办
            node* node=map_[key];
            //先取下来
            takeDown(node);
            //头插法放到最前面去
            putFront(node);
            //数据不需要动，动一下记录的时间
            node->clock_=click++;
            return node->value_;
        }
    }

    //放新的
    void put(const K& key,const V& value){
        std::lock_guard<std::mutex> lock(mutex_);

        //修改已有的value
        if(map_.count(key)){
            node* node=map[key];
            node=takeDown(node);
            putFront(node);
            node->value_=value;
            return;
        }

        //evict个腾出个新的
        if(size_==CACHESIZE){
            node* node=evictOne();
            map_.erase(node->key_);
            map_[key]=node;
            node->key_=key;
            node->value_=value;
            putFront(node);
            return;
        }else{
            //造个新的
            node* node=new node{key,value};
            map_[key]=node;
            putFront(node);
            return;
        }
    }

    //取下节点，但是成员可能还需要修改
    node* takeDown(node* node){
        node->prev_->next_=node->next_;
        node->next_->prev_=node->prev_;
        node->prev_=nullptr;
        node->next_=nullptr;
        size_--;//size_的变化放这里好还是放put里好
        
        return node;
    }

    //头插法挂上节点，成员可能还需要修改
    void putFront(node* node){
        node->next_=head_->next_;
        node->next_->prev_=node;
        node->prev_=head_;
        node->prev_->next_=node;
        size_++;
    }

    //链表最后一个节点就是最久未被使用的，直接用这个就行
    //问题是怎么拿下来？环形链表？
    node* evictOne(){
        node* node=tail_->prev_;
        node=takeDown(node);//为什么这里node的资源不需要处理？也许key和valuezai外面是指针呢

        return node;
    }

private:
    std::mutex mutex_;
    size_t size_;
    node* head_;
    node* tail_;
    //map的作用是为了方便直接找到这个node，不需要从前到后遍历
    //找到后摘下来放到最前面去因此需要双向链表，不然node前的和node后的连不起来
    //但是这样放到链表头好像也没体现出lrucache的优越
    std::unordered_map<K,node*> map_;//为什么用k找v不干脆用map?因为要使用局部性原理的性质
};

//这个双向链表是一开始就在，还是说一步步扩容的？