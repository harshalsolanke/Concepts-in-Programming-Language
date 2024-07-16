//BASIC GARBAGE COLLECTION IMPLEMENTATION
//SHOWCASES BOTH MARK SWEEP AND REFERANCE COUNTING MECHANISM

#include<stdio.h>
#include<stdbool.h>

#define HEAP_SIZE 1000
typedef enum{FREE,ALLOCATED} Status;
char Heap[HEAP_SIZE];

//meta data struct
typedef struct Meta_Data_Tag
{
    int size;
    Status status;
    struct Meta_Data_Tag* next;
}Meta_Data;

//data struct with 3 pointers
typedef struct Data_Node_Tag
{
    int data;
    int referance_count;
    bool active;
    struct Data_Node_Tag* ptr[3];
}Data;

//Free List pointer
Meta_Data* free_ptr = (void*)Heap;
Data * array[8];

//Initializing Heap
void Initialize()
{
    free_ptr->status = FREE;
    free_ptr->size = HEAP_SIZE - sizeof(Meta_Data);
    free_ptr->next = NULL;

    printf("\nSize available: %d\n",free_ptr->size);
}

//Print Free List
void Print_Free_List()
{
    Meta_Data* ptr = free_ptr;

    while(ptr!=NULL)
    {
        printf("Free Block is present at %d of size %d.\n\n",(void*)ptr+sizeof(Meta_Data),ptr->size);
        ptr = ptr->next;
    }
    return;
}

//Split Block
Meta_Data* Split_Block(Meta_Data*ptr,int size)
{
    Meta_Data* new;

    new = (void*)ptr + size +1;
    new->next = ptr->next;
    new->status = FREE;
    new->size = ptr->size-size;

    printf("Block allocated from %d to %d by SPLITTING!!\n",(void*)ptr+sizeof(Meta_Data),(void*)ptr+size);

    return (void*)new;
}

//Allocate Memory Function
void* Allocate(int size)
{
    Meta_Data*prev,*ptr;
    
    prev=NULL;
    ptr=free_ptr;

    if(ptr!=NULL)
    {
        int flag=0;
        while(ptr!=NULL && flag==0)
        {
            if(ptr->size >= size)
            {
                flag=1;
            }
            else
            {
                prev=ptr;
                ptr=ptr->next;
                printf("One Block Checked..\n");
            }
        }
        if(flag==1)
        {
            if(ptr->size-size>=sizeof(Meta_Data)+1)
            {   
                Meta_Data*new = Split_Block(ptr,size+sizeof(Meta_Data));
                ptr->size=size;

                if(prev!=NULL)
                {
                    prev->next=new;
                }
                else
                {
                    free_ptr = new;
                }
            }
            else
            {
                printf("Block Allocated WITHOUT SPLITTING from %d to %d!!\n",(void*)ptr+sizeof(Meta_Data),(void*)ptr+sizeof(Meta_Data)+ptr->size);
            
                if(prev!=NULL)
                {
                    prev->next=ptr->next;
                }
                else
                {
                    free_ptr=free_ptr->next;
                }
            }
            ptr->status=ALLOCATED;
            ptr->next=NULL;

            ptr = (void*)ptr+sizeof(Meta_Data)+1;
            Print_Free_List();
            return (void*)ptr;
        }
        else
        {
            printf("\nEnough Memory Not Present!!\n");
        }
    }
    else
    {
        printf("\nNO FREE BLOCKS AVAILABLE!!\n");
    }

    return NULL;
}

//Free Memory Function
void Free(void* ptr)
{
    if(ptr!=NULL)
    {
    Meta_Data* del;
    del = (void*)ptr-sizeof(Meta_Data)-1;
    del->status=FREE;

    printf("\nptr=%d, del=%d, del size:%d\n",ptr,del,del->size);

    if((free_ptr!=NULL && del<free_ptr) || (free_ptr==NULL))
    {
        if((void*)del+del->size+sizeof(Meta_Data)+1==free_ptr)
        {
            //Merge
            del->size+=free_ptr->size;
            del->next=free_ptr->next;
            printf("Blocks Merged at start of freelist!!\n");
            printf("Merged from %d to %d.\n\n",del,free_ptr);
        }
        else
        {
            del->next=free_ptr;
            printf("New Block added at start of freelist!!\n");
        }
        free_ptr=del;
        printf("FreeList pointer changed!!\n");
    }
    else if(free_ptr!=NULL)
    {
        Meta_Data* front=free_ptr;
        Meta_Data* back=NULL;

        while(front<del)
        {
            back=front;
            front=front->next;
        }
        if((void*)del+del->size+sizeof(Meta_Data)+1==front)
        {
            //Merge Front
            printf("Merging %d with next block at %d....\n",del,front);
            del->size+=front->size+sizeof(Meta_Data);
            del->next=front->next;
            front->next=NULL;

            if(back!=NULL)
            {
                back->next=del;
            }
            printf("Merged with next free block!!\n");
            printf("Merged from %d to %d.\n",del,(void*)del+del->size);
        }
        else
        {
            if(back!=NULL)
            {
                back->next=del;
            }
            del->next=front;
            printf("New Block Added to Free List!!\n");
        }
        if(back != NULL && (void*)back+back->size+sizeof(Meta_Data)+1 == del)
        {
            //Merge Back
            printf("Merging %d with previous bock at %d....\n",del,back);
            back->size += del->size+sizeof(Meta_Data);
            back->next = del->next;
            printf("Blocks Merged from %d to %d\n",back,(void*)back+back->size);
        }
        else
        {
            if(back!=NULL)
            {
                back->next=del;
            }
            del->next=front;
            printf("New Block added to Free List!!\n");
        }
    }
    }
    else
    {
        printf("\nCannot free-invalid pointer!!\n\n");
    }
    return;
}

//Properly setting Data node in Heap
void Set_Node(Data* node,int data)
{
    if(node!=NULL)
    {
        node->referance_count = 0;
        node->data = data;
        node->active = false;
        for(int i=0;i<3;i++)
        {
            node->ptr[i] = NULL;
        }
    }
}

//Print Freed Node Details
void Print_node(int i)
{
    if(array[i]!=NULL)
	{
        printf("Data = %d\t Reference Count = %d\t Size = %d\n",array[i]->data,array[i]->referance_count,sizeof(Data));
    }
}

//Print All Node
void Print_All_Active_Node(Data* root)
{
    if(root!=NULL)
    {
        printf("Data Value: %d\t Refrance Count: %d\t Active Status: %d\n",root->data,root->referance_count,root->active);
        Print_All_Active_Node(root->ptr[0]);
        Print_All_Active_Node(root->ptr[1]);
        Print_All_Active_Node(root->ptr[2]);
    }
}

//Print Adjacency List
void Adj_List()
{
    printf("-------------------------------------------------------------------------------------\n");
    printf("\nADJACENCY LIST:-\n\n");

    for(int i=0;i<8;i++)
    {
        if(array[i]!=NULL)
        {
            printf("Data(%d): ",array[i]->data);
            
            for(int j=0;j<3;j++)
            {
                if(array[i]->ptr[j] != NULL)
                {
                    printf("%d -> ",array[i]->ptr[j]->data);
                }
            }
            printf("NULL\n");
        }
    }
}

//Print Adjacency Matrix
void Adj_Matrix()
{
    printf("-------------------------------------------------------------------------------------\n");
    printf("\nADJACENCY MATRIX:-\n\n");
    int matrix[8][8];

    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            matrix[i][j] = 0;
        }
    }

    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            if(array[i]!=NULL && array[j]!=NULL)
            {
                for(int k=0;k<3 && i!=j;k++)
                {
                    if((array[i]->ptr[k]!=NULL) && (array[i]->ptr[k]->data==array[j]->data))
                    {
                        matrix[i][j] = 1;
                    }
                }
            }
        }
    }

    //printing
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }
    printf("-------------------------------------------------------------------------------------\n");
}

//Check for temp is active or not wrt to root
bool Check_Node_Active(Data* root,Data* temp)
{
    bool ret = false;

    if(root!=NULL && temp!=NULL)
    {
        if(root->data == temp->data)
        {
            ret = true;
        }
        for(int i = 0 ; i < 3 && ret == false ; i++)
        {
            if((root->ptr[i] != NULL) && (root->ptr[i]->data == temp->data))
            {
                ret = true;
            }
            else
            {
                ret = Check_Node_Active(root->ptr[i],temp);
            }
        }
    }
    //printf("Data Node:%d is Active:%d\n",temp->data,ret);
    return ret;
}

//Referance Counting Mechanism
void Referance_Counting(Data * root1,Data* root2)
{
    printf("-------------------------------------------------------------------------------------\n\n");
    printf("Applying REFERANCE COUNTING Mechanism For Garbage Collection....\n");
    int i = 0;
    while(i < 8)
    {
        if((Check_Node_Active(root1,array[i]) == false) && (Check_Node_Active(root2,array[i]) == false))
        {
            for(int j=0;j<3;j++)
            {
                if(array[i] !=NULL && array[i]->ptr[j] != NULL)
                {
                    array[i]->ptr[j]->referance_count--;
                    array[i]->ptr[j] = NULL;
                }
            }
            printf("Garbage Details:- ");
            Print_node(i);
            Free(array[i]);
            array[i] = NULL;
        }
        i++;
    }
}

//Mark Method
void Mark(Data* root)
{
    if(root!=NULL)
    {
        //Marking all the nodes connected from root as active
        root->active = true;
        Data* curr = root;
        Data* prev = NULL;

        for(int i=0;i<3;i++)
        {
            if(curr != NULL && curr->ptr[i] != NULL)
            {
                curr->ptr[i]->active = true;
            }
            if(i==2 && curr != NULL)
            {
                prev = curr;
                i = 0;
                curr = curr->ptr[i];
            }
        } 
    }
}

//Sweep Method
void Sweep()
{
    for(int i=0;i<8;i++)
    {
        if(array[i]->active == false)
        {
            printf("Garbage Nodes: ");
            Print_node(i);
            Free(array[i]);
            array[i] = NULL;
        }
    }
}

//Main
int main()
{
    Initialize();

    Data *root1 = NULL,*root2 = NULL;

    printf("Starting Address: %d\n",free_ptr);

    array[3] = root1 = Allocate(sizeof(Data));
    Set_Node(root1,5);
    root1->referance_count++;

    array[0] = root2 = Allocate(sizeof(Data));
    Set_Node(root2,1);
    root2->referance_count++;

    //Make pointers for all the nodes in heap
    //for 1 and 5 we already have root2 and root1 respectively
    //we need for 2,3,7,8,9,10
    Data* ptr2 = array[1] = Allocate(sizeof(Data));Set_Node(ptr2,2);
    Data* ptr3 = array[2] = Allocate(sizeof(Data));Set_Node(ptr3,3);
    Data* ptr7 = array[4] = Allocate(sizeof(Data));Set_Node(ptr7,7);
    Data* ptr8 = array[5] = Allocate(sizeof(Data));Set_Node(ptr8,8);
    Data* ptr9 = array[6] = Allocate(sizeof(Data));Set_Node(ptr9,9);
    Data* ptr10 = array[7] = Allocate(sizeof(Data));Set_Node(ptr10,10);

    //Making the network as given in the question
    root1->ptr[0] = root2;  root2->referance_count++;
    root2->ptr[0] = ptr2;   ptr2->referance_count++;
    root2->ptr[1] = ptr9;   ptr9->referance_count++;
    root2->ptr[2] = ptr10;  ptr10->referance_count++;
    ptr7->ptr[0] = root2;   root2->referance_count++;
    ptr7->ptr[1] = ptr8;    ptr8->referance_count++;
    ptr3->ptr[0] = ptr8;    ptr8->referance_count++;
    ptr3->ptr[1] = ptr10;   ptr10->referance_count++;
    ptr8->ptr[0] = ptr9;    ptr9->referance_count++;

    printf("-------------------------------------------------------------------------------------\n");
    printf("Node through root1:-\n");
    Print_All_Active_Node(root1);
    printf("\nNode through root2:-\n");
    Print_All_Active_Node(root2);

    Adj_List();
    Adj_Matrix();

    Referance_Counting(root1,root2);
    Adj_List();
    Adj_Matrix();

    printf("\nMarking the active nodes in Heap....\n\n");
    Mark(root1);
    Mark(root2);
    Print_All_Active_Node(root1);
    Print_All_Active_Node(root2);
    printf("-------------------------------------------------------------------------------------\n");

    printf("\nSweeping Garbage....\n\n");
    Sweep();

    Adj_List();
    Adj_Matrix();
    return 0;
}
