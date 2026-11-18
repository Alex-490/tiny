#include <cstddef>
#include <cstdlib>
#include <algorithm>

//计算字符串长度
size_t strlen(char* data){
    char* p=data;
    size_t ret=0;
    while(*p!='\0'){
        ret++;
        p++;
    }

    return ret;
}

//字符串拷贝
void strcpy(char* dst,char* src,size_t size)noexcept{
    char* p=dst,*q=src;
    while(size--){
        *p=*q;
        p++;
        q++;
    }
}

//string的参数固定为char么？
//string底层能用vector么？
class string{
public:
    //string s;
    string():data_(nullptr),capacity_(0),size_(0){}
    
    ~string(){
        free(data_);
        data_=nullptr;
        //下面这两个有必要么？
        capacity_=0;
        size_=0;
    }

    //拷贝
    //string s(s1);
    //string s=s1;
    //s=s1;
    string(const string& other):data_(nullptr),size_(other.size_){
        data_=(char*)(malloc(sizeof(char)*size_));
        strcpy(data_,other.data_,other.size_);
    }

    string& operator=(const string& other){
        if(other == *this)  return *this;
        free(data_);//所有赋值运算符都要考虑赋值前资源的去处
        //no，不需要考虑，因为使用者使用s=s1;这样的代码就是默认了丢掉s原本的数据

        data_=(char*)(malloc(sizeof(char)*size_));
        strcpy(data_,other.data_,other.size_);
        return *this;
    }

    //移动
    //string s(std::move(s1));
    //s=std::move(s1);
    string(string&& other):data_((char*)malloc(sizeof(char)*other.capacity_)),size_(other.size_),capacity_(other.capacity_){
        data_=other.data_;
        other.data_=nullptr;
        other.size_=0;
        other.capacity_=0;
    }

    string& operator=(string&& other){
        if(*this==other)    return *this;
        free(data_);
        data_=other.data_;
        capacity_=other.capacity_;
        size_=other.size_;
        other.data_=nullptr;
        other.size_=0;
        other.capacity_=0;
    }

    //push_back()
    //这个参数怎么写？
    push_back(const char* c){

    }

    //s.emplace_back(std::move(c))?
    void emplace_back(char&& c){

    }

    //s+='c'; => s=s+'c';
    //s+=s1;  => s=s+s1;
    string& operator+(const string& s){
        //不断扩容直到满足需求
        while(s.size_+size_>capacity_){
            resize();
        }

        strcpy((char*)(data_+sizeof(char)*size_),s.data_,s.size_);
    }

    //返回值？
    char operator[](size_t index){

    }

    string& operator+(string&& s){
        
    }

    string& operator+(string&& s){

    }

    string& operator+(char&& s){

    }

    //这个size_是直接保存好还是用的时候用strlen()计算好？
    size_t size() const{
        return size_;
    }

    bool empty() const{
        return size_==0;
    }

private:
    char* data_;
    size_t size_;
    size_t capacity_;

    //扩容2倍
    void resize(){
        char* ptr=(char*)(malloc(sizeof(char)*capacity_*2));
        free(data_);
        data_=ptr;
        capacity_*=2;
    }
};