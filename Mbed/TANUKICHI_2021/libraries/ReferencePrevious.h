/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef ReferencePrevious_h
#define ReferencePrevious_h

template <typename T>
class ReferencePrevious {
private:
    int index = -1;
    int qty;
public:
    T* data;
    const T None;
    int stock = 0;
    
    ReferencePrevious(int _qty, T _None) : qty(_qty), None(_None) {
        data = new T[qty];
        initialization(None);
    }
    
    /*
    ReferencePrevious(int _qty, T first) : qty(_qty){
        data = new T[qty];
        for(int i=0; i<qty; i++)
            data[i] = first;
    }
     */
    
    void initialization(T initial){
        for(int i=0; i<qty; i++)
            data[i] = initial;
    }
    
    T& update(void){
        if(stock < qty)
            stock++;
        index++;
        index %= qty;
        data[index] = data[(index + qty - 1) % qty];
        return data[index];
    }
    
    T& update(T current){
        if(stock < qty)
            stock++;
        index++;
        index %= qty;
        data[index] = current;
        return data[index];
    }
    
    T& getPrv(int num){
        //current ------- previous
        // 0 ,1 , ------- , qty-2, qty-1
        return data[((index - num) % qty + qty) % qty];
    }
    
    T* getArray(T* array, int num){
        for(int i=0; i<num; i++)
            array[i] = getPrv(i);
        return array;
    }
    
    T* getArray(T* array, int from, int num){
        for(int i=0; i<num; i++)
            array[i] = getPrv(i + from);
        return array;
    }
};

#endif /* ReferencePrevious_h */
