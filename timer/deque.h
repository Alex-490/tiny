#include <cstddef>
#include <cstdlib>

//这些初始大小到底是字节还是元素个数（那样的话实际字节数就是元素大小*个数）？
#define MAPSIZE 10 //中控器的初始大小，可以指向10个buffer
#define BUFFERSIZE 10

//buffer单元,真正保存元素的地方
//一个buffer满了该怎么扩容？
//buffer怎么确定自己这块是从屁股开始填元素还是从头开始填元素？
//单个buffer的元素放和取的方向是固定的，因此不存在front、back这样的字眼
template <class T>
class buffer{
public:
    buffer(bool flag):flag_(flag){}

    //放
    //参数这样对么？
    void push(const T&& t){
        //也许这个逻辑要让上层的控制器判断
        // if(size_==capacity_){

        // }
        
        *(T*)(start_+sizeof(T)*index_)=t;
        if(flag_){
            ++index_;
        }else{
            --index_;
        }
        ++size_;
    }

    //读
    T get() const{
        return *(T*)(start_+sizeof(T)*index_);
    }

    //取
    //需要防御性编程么？如果没有元素可以pop了呢？
    T pop(){
        T ret=get();
        if(flag_){
            --index_;
        }else{
            ++index_;
        }
        return ret;
    }

    //控制器判断这个buffer满了没，满了就新建一个buffer
    bool hasFool() const{
        return (size_==capacity_);
    }
private:
    T* start_;
    bool flag_;//ture就是从左到右，false就是从右到左
    size_t index_;
    size_t size_;
    size_t capacity_;
};

//中控器，每个元素是一个指向某个buffer的地址
//中控器应该可以扩容
template <class T>
class map{
public:
    //无参构造
    map():capacity_(MAPSIZE){}
    ~map();

    //如果map就能把一切干完，那还需要dequeue类做什么呢？
    push_front(const T& t){
        //检查最上面的buffer满了没，满了就扩容
        if(a_->hasFool()){
            //如果最上面的buffer到头了就扩容
            if(a_==start_){
                resize
            }
        }
    }

    push_back(){

    }

    pop_front(){

    }
    pop_back();
    front();
    
    back(){

    }

    size_t size()const{
        return size_;
    }
    
    operator[]();

    bool empty() const{
        return size_==0;
    }

private:
    buffer<T>** start_;
    //每个元素的地址是void*还是什么类型？
    size_t capacity_;//容量
    size_t size_;//已经用了多少buffer
    buffer<T>* a_;//向上增加buffer
    buffer<T>* b_;//向下增加buffer

    //扩容
    void resize(){
        capacity_*=2;
        buffer<T>* ptr=(buffer<T>*)(malloc(sizeof(buffer<T>*)*capacity_));
        
        //buffer<T>* mid=(buffer<T>*)(ptr+sizeof(buffer<T>*)*(capacity_/2));

        //找到新内存开始把旧内存有效数据腾进去的位置
        buffer<T>* start=ptr+sizeof(buffer<T>*)*(capacity_/2);
        for(size_ i=0;i<size_;i++){
            *(buffer<T>*)(start+i*sizeof(buffer<T>*))=*(buffer<T>*)(a_+i*sizeof(buffer<T>*));
        }

        start_=ptr;
    }
};


//为什么不用一个大buffer，而是要分段呢？这样好扩容的方式能用在vector上么？

class deque{
public:
    deque();
    ~deque();


private:

};