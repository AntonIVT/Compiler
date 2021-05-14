#pragma once

#include <cstdlib>
#include <cstdio>
#include <cassert>

typedef long long List_Iterator;

template <typename List_El>
struct List
{
private:
    
    struct Node
    {
        List_El value;
        long long prev;
        long long next;
    };
    
    Node *data;
    long long size;
    long long capacity;
    long long head;
    long long free;              
    bool boost_mode;       
    
    enum List_error 
    {
        LIST_OK = 0,
        LIST_ERROR = 1,
        LIST_OVERFLOW = 2,
        LIST_WRONG_INDEX = 3,
        LIST_WRONG_REALLOC = 4
    };
    
    List_error ListVerify()
    {
        if (this->data == NULL)
        {
            printf("Error in list because pointer to data == NULL\n");
            return LIST_ERROR;
        }
        if (this->size > this->capacity)
        {
            printf("Error in list because of overflow\n");
            return LIST_OVERFLOW;
        }
        
        return LIST_OK;
    }
    
    bool isIteratorValid(List_Iterator iter)
    {
        if (ListVerify() != LIST_OK)
            return false;
        
        if (iter < 0 || iter >= this->capacity || this->data[iter].prev == -1)
        {
            printf("%lli\n%lli\n", iter, this->capacity);
            printf("Wrong iterator\n");
            printf("%lli\n", this->data[iter].prev);
            return false;
        }
        else
            return true;
    }
    
    List_error ListResize(long long new_cap)
    {        
        if (ListVerify() != LIST_OK)
            return LIST_ERROR;
            
        if (new_cap < this->size || 
           (this->boost_mode == false && new_cap <= this->capacity) || 
           (this->boost_mode == true && new_cap < this->size))
            return LIST_ERROR; 
        
        struct Node *new_data = (struct Node *)realloc(this->data, new_cap * sizeof(struct Node));
        
        if (new_data == NULL)
            return LIST_WRONG_REALLOC;
        
        this->data = new_data;
        
        if (new_cap == this->size)
        {
            this->capacity = new_cap;
            return LIST_OK;
        }
        
        int last_free = this->free;
        if (last_free == -1)
            this->free = this->capacity;
        else    
        {
            while(this->data[last_free].next != -1)
                last_free = this->data[last_free].next;
            
            this->data[last_free].next = this->capacity;
        }
        for (int i = this->capacity; i < new_cap - 1; i++)
        {
            //Node free_node = {0, -1, i + 1};
            Node free_node = {};
            free_node.prev = -1;
            free_node.next = i + 1;
            
            this->data[i] = free_node;
        }
        //Node last_node = {0, -1, -1};
        Node last_node = {};
        last_node.prev = -1;
        last_node.next = -1;
        
        this->data[new_cap - 1] = last_node;
            
        this->capacity = new_cap;
        return LIST_OK;
    }
    
    List_error insert_internal(List_El x, long long array_number)
    {   
        if (ListVerify() != LIST_OK)
            return LIST_ERROR;
        
        if (array_number < 0 || array_number >= this->capacity)       
            return LIST_WRONG_INDEX;
        
        if (this->data[array_number].prev == -1 && !(this->size == 0 && array_number == 0))
            return LIST_WRONG_INDEX;
        
        if (this->capacity == this->size)
        {
            if (ListResize(this->capacity * 2) != LIST_OK)
                return LIST_WRONG_REALLOC;
        }
        
        int free_tmp = this->free;
        this->free = this->data[free_tmp].next;                    
        
        if (this->size == 0)
        {   
            this->head = free_tmp;
            struct Node new_node = {x, this->head, this->head};
            this->data[this->head] = new_node;
            
            this->size++;
            return LIST_OK;
        }
        
        struct Node new_node = {x, this->data[array_number].prev, array_number};
        
        this->data[free_tmp] = new_node;
        this->data[new_node.prev].next = free_tmp;
        this->data[new_node.next].prev = free_tmp;
            
        this->size++;
        
        return LIST_OK;
    }
    
    List_error erase_internal(long long array_number)
    {   
        if (ListVerify() != LIST_OK)
            return LIST_ERROR;
        
        if (array_number < 0 || array_number >= this->capacity) 
            return LIST_ERROR;
        
        if (this->data[array_number].prev == -1)
            return LIST_ERROR;
        
        if (this->size == 1)
        {
            this->head = -1;
            
            struct Node dead_node = {};
            dead_node.prev = -1;
            dead_node.next = this->free;
            
            this->data[array_number] = dead_node;
            this->free = array_number;
            
            this->size--;
            this->boost_mode = 0;
            
            return LIST_OK;
        }
        
        if (this->head == array_number)
            this->head = this->data[array_number].next;
        
        this->data[this->data[array_number].prev].next = this->data[array_number].next;
        this->data[this->data[array_number].next].prev = this->data[array_number].prev;
        
        struct Node dead_node = {};
        dead_node.prev = -1;
        dead_node.next = this->free;
        
        this->data[array_number] = dead_node;
        this->free = array_number;
        
        this->size--;
        
        return LIST_OK;
    }
    
public:
    
    List_error construct(long long capacity)
    {   
        if (capacity <= 0)
            capacity = 1;
        
        this->data = (struct Node *)calloc(capacity, sizeof(struct Node));
        
        assert(this->data != NULL);
        
        this->free = 0; 
        for (int i = 0; i < capacity - 1; i++)
        {
            //struct Node free_node = {0, -1, i + 1};
            Node free_node = {};
            free_node.prev = -1;
            free_node.next = -i + 1;
            
            this->data[i] = free_node;
        }
        //struct Node last_node = {0, -1 , -1};
        Node last_node = {};
        last_node.prev = -1;
        last_node.next = -1;
        
        this->data[capacity - 1] = last_node;
        
        this->head = -1;
        this->capacity = capacity;
        this->size = 0;
        this->boost_mode = false;
        
        return ListVerify();
    }
    
    List_error destruct()
    {        
        if (ListVerify() != LIST_OK)
            return LIST_ERROR;
    
        std::free(data);
        this->capacity = 0;
        this->size =  0;
        this->head = -1;
        this->free = -1;
        this->boost_mode =  false;
        
        return ListVerify();
    }
    
    List_error boost()
    {        
        if (ListVerify() != LIST_OK)
            return LIST_ERROR;
        
        if (this->boost_mode == true)
            return LIST_OK;
        
        if (this->size == 0)
        {
            this->boost_mode = true;
            return LIST_OK;
        }
        
        struct Node *new_data = (struct Node *)calloc(this->size, sizeof(struct Node));
        
        if (new_data == NULL)
            return LIST_WRONG_REALLOC;
        
        int curr_node = this->head;
        for (int i = 0; i < this->size; i++)
        {
            new_data[i].value = this->data[curr_node].value;
            new_data[i].next = (i + 1) % this->size;
            new_data[i].prev = (i - 1 + this->size) % this->size;
            curr_node = this->data[curr_node].next;
        }
        
        std::free(this->data);
        this->data = new_data;
        this->capacity = this->size;
        this->free = -1;
        this->head = 0;
        this->boost_mode = true;
            
        return LIST_OK;
    }
    
    List_Iterator push_back(List_El x)
    {            
        if (ListVerify() != LIST_OK)
            return -1;
                
        if (this->size > 0)
        {
            if (insert_internal(x, this->head) == LIST_OK)
                return this->data[this->head].prev;
            else
                return -1;
        }
        else
        {
            if (insert_internal(x, 0) == LIST_OK)
                return this->head;
            else
                return -1;
        }
    }
    
    List_Iterator push_front(List_El x)
    {
        if (ListVerify() != LIST_OK)
            return -1;
        
        List_error check = LIST_OK;
    
        if (this->boost_mode == true) this->boost_mode = false;
        
        if (this->size > 0)
            check = insert_internal(x, this->head);
        else
            check = insert_internal(x, 0);
        
        if (check == LIST_OK)
        {
            this->head = this->data[this->head].prev;
            return this->head;
        }
        else 
            return -1;
    }
    
    List_error pop_back()
    {        
        if (ListVerify() != LIST_OK)
            return LIST_ERROR;
        
        if (this->size == 0)
            return LIST_ERROR;
        
        return erase_internal(this->data[this->head].prev);    
    }
    
    List_error pop_front()
    {        
        if (ListVerify() != LIST_OK)
            return LIST_ERROR;
        
        if (this->size == 0)
            return LIST_ERROR;
        
        if (this->boost_mode == true) this->boost_mode = false;
        
        return erase_internal(this->head);  
    }
    
    long long get_size()
    {        
        if (ListVerify() != LIST_OK)
            return -1;
        
        return this->size;
    }
    
    bool is_boosted()
    {        
        if (ListVerify() != LIST_OK)
            return false;
        
        return this->boost_mode;
    }
    
    List_Iterator get_iter(long long logic_number)
    {        
        if (ListVerify() != LIST_OK)
            return -1;
        
        if (logic_number >= this->size || logic_number < 0)
            return -1;
        
        if (this->boost_mode == true)
            return logic_number;
        
        List_Iterator iterator = this->head; 
        
        for (int i = 0; i < logic_number; i++)
            iterator = this->data[iterator].next;
        
        return iterator;
    }
    
    List_El get(List_Iterator iter)
    {        
        if (ListVerify() != LIST_OK)
        {
            abort();
            return this->data[0].value;
        }   
        if (isIteratorValid(iter))
            return this->data[iter].value;
        else
        {
         abort();
         return this->data[0].value;
        }   
    }
    
    List_Iterator begin()
    {        
        if (ListVerify() != LIST_OK)
            return -1;
        
        return this->head;
    }
    
    List_Iterator end()
    {        
        if (ListVerify() != LIST_OK)
            return -1;
        
        return this->data[this->head].prev;
    }
    
    List_Iterator iter_inc(List_Iterator iter)
    {        
        if (ListVerify() != LIST_OK)
            return -1;
        
        if (isIteratorValid(iter))
            return this->data[iter].next;
        else
            return -1;
    }
    
    List_Iterator iter_dec(List_Iterator iter)
    {        
        if (ListVerify() != LIST_OK)
            return -1;
            
        if (isIteratorValid(iter))
            return this->data[iter].prev;
        else
            return -1;
    }
    
    List_Iterator insert_bef(List_El x, List_Iterator iter)
    {        
        if (ListVerify() != LIST_OK)
            return -1;
        
        if (!isIteratorValid(iter))
            return -1;
        
        this->boost_mode = false;
        
        if (iter == this->head)
            return push_front(x);
        
        insert_internal(x, iter);
        return this->data[iter].prev;
    } 
    
    List_Iterator insert_aft(List_El x, List_Iterator iter)
    {        
        if (ListVerify() != LIST_OK)
            return -1;
        
        if (!isIteratorValid(iter))
            return -1;
            
        if (iter == this->data[this->head].prev)
            return push_back(x);
        
        insert_internal(x, this->data[iter].next);
        return this->data[iter].next;
    }
    
    List_error erase(List_Iterator iter)
    {
        if (ListVerify() != LIST_OK)
            return LIST_ERROR;
        
        if (!isIteratorValid( iter))
            return LIST_ERROR;
    
        if (iter == this->data[this->head].prev)
            return pop_back();
            
        this->boost_mode = false;
        
        if (iter == this->head)
            return pop_front();
        
        return erase_internal(iter);
    }
};


