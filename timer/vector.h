#include <cstddef>
#include <expect>

//二维及多维的要怎么实现呢？
template <typename T>
class vector{
public:
    //构造函数不止一种
    //vector<int>(2);
    vector(size_t size):size_(size),capacity_(size),data_(malloc(sizeof(T)*size)){
        //这样对么？
    }

    //拷贝
    vector(const vector& other):size_(other.size_),
                capacity_(other.capacity_),
                data_(malloc(sizeof(T)*capacity_)){
        for(int i=0;i<size_;i++){
            *(T*)(data_+sizeof(T)*i)=*(T*)(other.data_+sizeof(T)*i);
        }
    }

    vector& operator=(const vector& other){
        if(other==*this)    return *this;
        
        cleanData();
        capacity_=other.capacity_;
        resize(capacity_);
        size_=other.size_;

        for(size_t i=0;i<size_;i++){
            *(T*)(data_+sizeof(T)*i)=*(T*)(other.data_+sizeof(T)*i);//"="在这里是一种拷贝
        }
    }

    //移动构造
    //参数一定要引用么？可以用指针么？
    vector(vector& other){
        //直接把other.data_ move()进来，不需要进行深拷贝可行么？
        //不需要move，直接接管other.data_;
        capacity_=other.capacity_;
        size_=other.size_;
        data_=other.data_;
        other.data_=nullptr;
        other.size_=0;
        other.capacity_=0;
    }

    //移动赋值
    //可以直接接管other.data_,不是么？
    //移动赋值比移动构造多个销毁原data_
    vector& operator=(vector&& other){
        if(other==*this)    return *this;
        
        cleanData();//处理原data_
        capacity_=other.capacity_;
        size_=other.size_;
        data_=other.data_;
        other.data_=nullptr;

        // for(size_t i=0;i<size_;i++){
        //     *(T*)(data_+sizeof(T)*i)=std::move(*(T*)(other.data_+sizeof(T)*i));//直接作为右值移动
        // }
    }

    //析构
    ~vector(){
        for(int i=0;i<size_;i++){
            T* tmp=data_+i*sizeof(T);
            free(*tmp);
        }
    }

    //[]运算符
    //这里返回值是？
    //vet[1]=2;这里可以被赋值
    //int a=vet[2];这里是T
    T operator[](size_t i){
        if(i<size_){
            std::runtime_error("")
        }
        return *(T*)(data_+sizeof(T)*i);
    }

    //vet.push_back(a);
    void push_back(T t){
        //空间不够就扩容
        if(size_==capacity_){
            resize();
        }
        *(T*)(data_+sizeof(T)*size_)=t;
        ++size_;
    }

    size_t size()const{
        return size_;
    }

    //emplace_back 和 push_back的区别是？
    //难道是move？
    void emplace_back(T&& t){

    }

    //清理资源
    void cleanData(){
        for(size_t i=0;i<size_;i++){
            T* tmp=*(T*)(data_+sizeof(T)*i);
            //T需要分为内置类型和类类型?
            T::~T();//类类型这样析构？

        }
    }

private:
    T* data_;
    size_t size_;
    size_t capacity_;

    //这里面需要增加mutex么？也许会有多线程操作？

    //扩容
    void resize(){
        capacity_*=2;
        void* ptr=malloc(sizeof(T)*capacity_);
        for(int i=0;i<size_;i++){
            *(T*)(ptr+sizeof(T)*i)=*(T*)(data_+sizeof(T)*i)
            free(T);
        }
        
        data_=ptr;
    }

    void resize(size_t capacity){
        capacity_=capacity_;
        void* ptr=malloc(sizeof(T)*capacity_);
        for(int i=0;i<size_;i++){
            *(T*)(ptr+sizeof(T)*i)=*(T*)(data_+sizeof(T)*i)
            free(T);
        }
        
        data_=ptr;
    }
};