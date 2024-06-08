/*****************************************************************************************************************************
  This project was supported by the National Basic Research 973 Program of China under Grant No.2011CB302301
  Huazhong University of Science and Technology (HUST)   Wuhan National Laboratory for Optoelectronics

  FileName�? initialize.h
Author: Hu Yang		Version: 2.1	Date:2011/12/02
Description: 

History:
<contributor>     <time>        <version>       <desc>                   <e-mail>
Yang Hu	        2009/09/25	      1.0		    Creat SSDsim       yanghu@foxmail.com
2010/05/01        2.x           Change 
Zhiming Zhu     2011/07/01        2.0           Change               812839842@qq.com
Shuangwu Zhang  2011/11/01        2.1           Change               820876427@qq.com
Chao Ren        2011/07/01        2.0           Change               529517386@qq.com
Hao Luo         2011/01/01        2.0           Change               luohao135680@gmail.com
 *****************************************************************************************************************************/
#ifndef INITIALIZE_H
#define INITIALIZE_H 10000

#include <stdio.h>
#include "./include/bitmap.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include "avlTree.h"

#define SECTOR 512
#define BUFSIZE 200

#define DYNAMIC_ALLOCATION 0
#define STATIC_ALLOCATION 1

#define INTERLEAVE 0
#define TWO_PLANE 1

#define NORMAL    2
#define INTERLEAVE_TWO_PLANE 3
#define COPY_BACK	4

#define AD_RANDOM 1
#define AD_COPYBACK 2
#define AD_TWOPLANE 4
#define AD_INTERLEAVE 8
#define AD_TWOPLANE_READ 16

#define READ 1
#define WRITE 0

/*********************************all states of each objects************************************************
 *一下定义了channel的空闲，命令地址传输，数�?传输，传输，其他等状�?
 *还有chip的空闲，写忙，�?�忙，命令地址传输，数�?传输，擦除忙，copyback忙，其他等状�?
 *还有读写子�?�求（sub）的等待，�?�命令地址传输，�?�，读数�?传输，写命令地址传输，写数据传输，写传输，完成等状�?
 ************************************************************************************************************/

#define CHANNEL_IDLE 000
#define CHANNEL_C_A_TRANSFER 3
#define CHANNEL_GC 4           
#define CHANNEL_DATA_TRANSFER 7
#define CHANNEL_TRANSFER 8
#define CHANNEL_UNKNOWN 9

#define CHIP_IDLE 100
#define CHIP_WRITE_BUSY 101
#define CHIP_READ_BUSY 102
#define CHIP_C_A_TRANSFER 103
#define CHIP_DATA_TRANSFER 107
#define CHIP_WAIT 108
#define CHIP_ERASE_BUSY 109
#define CHIP_COPYBACK_BUSY 110
#define UNKNOWN 111

#define SR_WAIT 200                 
#define SR_R_C_A_TRANSFER 201
#define SR_R_READ 202
#define SR_R_DATA_TRANSFER 203
#define SR_W_C_A_TRANSFER 204
#define SR_W_DATA_TRANSFER 205
#define SR_W_TRANSFER 206
#define SR_COMPLETE 299

#define REQUEST_IN 300         //下一条�?�求到达的时�?
#define OUTPUT 301             //下一次数�?输出的时�?

#define GC_WAIT 400
#define GC_ERASE_C_A 401
#define GC_COPY_BACK 402
#define GC_COMPLETE 403
#define GC_INTERRUPT 0
#define GC_UNINTERRUPT 1

#define CHANNEL(lsn) (lsn&0x0000)>>16      
#define chip(lsn) (lsn&0x0000)>>16 
#define die(lsn) (lsn&0x0000)>>16 
#define PLANE(lsn) (lsn&0x0000)>>16 
#define BLOKC(lsn) (lsn&0x0000)>>16 
#define PAGE(lsn) (lsn&0x0000)>>16 
#define SUBPAGE(lsn) (lsn&0x0000)>>16  

#define PG_SUB 0xffffffff			

/*****************************************
 *函数结果状态代�?
 *Status �?函数类型，其值是函数结果状态代�?
 ******************************************/
#define TRUE		1
#define FALSE		0
#define SUCCESS		1
#define FAILURE		0
#define ERROR		-1
#define INFEASIBLE	-2
#define OVERFLOW	-3
typedef int Status;    

#define NONE -1
#define FULL 8

#define LSB_PAGE 0
#define CSB_PAGE 1
#define MSB_PAGE 2
#define TSB_PAGE 3

#define HOTREAD 5
#define HOTPROG 5

#define P_LC 0
#define P_MT 1

#define BITS_PER_CELL 4

// 保存每个请求的延迟时间，不包括dram
extern unsigned int latency[3993316];
// 保存latency偏移量的索引
extern int latency_index;
extern int trace_count;


/*****************************************
 *三�?�plane类型的bitmap
 *NONE_bitmap LC_bitmap MT_bitmap
 *对应bit�?1表示当前plane buffer的类�?
 ******************************************/
extern bitchunk_t NONE_bitmap[1];
extern bitchunk_t LC_bitmap[1];
extern bitchunk_t MT_bitmap[1];
extern char bitmap_table[16];

struct ac_time_characteristics{
    int tPROG;     //program time
    int tDBSY;     //bummy busy time for two-plane program
    int tBERS;     //block erase time
    int tCLS;      //CLE setup time
    int tCLH;      //CLE hold time
    int tCS;       //CE setup time
    int tCH;       //CE hold time
    int tWP;       //WE pulse width
    int tALS;      //ALE setup time
    int tALH;      //ALE hold time
    int tDS;       //data setup time
    int tDH;       //data hold time
    int tWC;       //write cycle time
    int tWH;       //WE high hold time
    int tADL;      //address to data loading time
    int tR;        //data transfer from cell to register
    int tAR;       //ALE to RE delay
    int tCLR;      //CLE to RE delay
    int tRR;       //ready to RE low
    int tRP;       //RE pulse width
    int tWB;       //WE high to busy
    int tRC;       //read cycle time
    int tREA;      //RE access time
    int tCEA;      //CE access time
    int tRHZ;      //RE high to output hi-z
    int tCHZ;      //CE high to output hi-z
    int tRHOH;     //RE high to output hold
    int tRLOH;     //RE low to output hold
    int tCOH;      //CE high to output hold
    int tREH;      //RE high to output time
    int tIR;       //output hi-z to RE low
    int tRHW;      //RE high to WE low
    int tWHR;      //WE high to RE low
    int tRST;      //device resetting time
}ac_timing;


struct ssd_info{ 
    double ssd_energy;                   //SSD的能耗，�?时间和芯片数的函�?,能耗因�?
    int64_t current_time;                //记录系统时间
    int64_t next_request_time;
    unsigned int real_time_subreq;       //记录实时的写请求�?数，用在全动态分配时，channel优先的情�?
    int flag;
    int active_flag;                     //记录主动写是否阻塞，如果发现柱�?�，需要将时间向前推进,0表示没有阻�?�，1表示�?阻�?�，需要向前推进时�?
    unsigned int page;
    int plane_num;                      //记录当前总共有�?�少plane数量

    unsigned int token;                  //在动态分配中，为防�?�每次分配在�?一个channel需要维持一�?令牌，每次从令牌所指的位置开始分�?
    unsigned int gc_request;             //记录在SSD�?，当前时刻有多少gc操作的�?�求

    unsigned int write_request_count;    //记录写操作的次数，基于大请求
    unsigned int read_request_count;     //记录读操作的次数,基于大�?�求
    int64_t write_avg;                   //记录用于计算写�?�求平均响应时间的时�?
    int64_t read_avg;                    //记录用于计算读�?�求平均响应时间的时�?

    unsigned int total_write;           //记录基于子�?�求的写操作�?�?
    unsigned int total_read;            //记录基于子�?�求的�?�操作个�?
    unsigned int tail_latency;          // 记录尾延�?

    int gc_rewrite;                  //记录gc产生的�?�余的写入量
    unsigned int real_written;          //记录写�?�求产生的写入量
    int free_invalid;               //空白页�??�?为无效的数量

    unsigned int min_lsn;
    unsigned long max_lsn;
    unsigned long read_count;
    unsigned long program_count;
    unsigned long erase_count;
    unsigned long direct_erase_count;
    unsigned long copy_back_count;
    unsigned long m_plane_read_count;
    unsigned long m_plane_prog_count;
    unsigned long interleave_count;
    unsigned long interleave_read_count;
    unsigned long inter_mplane_count;
    unsigned long inter_mplane_prog_count;
    unsigned long interleave_erase_count;
    unsigned long mplane_erase_conut;
    unsigned long interleave_mplane_erase_count;
    unsigned long gc_copy_back;
    unsigned long write_flash_count;     //实际产生的�?�flash的写操作
    unsigned long waste_page_count;      //记录因为高级命令的限制�?�致的页�?�?
    float ave_read_size;
    float ave_write_size;
    unsigned int request_queue_length;
    unsigned int update_read_count;      //记录因为更新操作导致的�?��?��?�出操作

    char parameterfilename[30];
    char tracefilename[30];
    char outputfilename[30];
    char statisticfilename[30];
    char statisticfilename2[30];

    FILE * outputfile;
    FILE * tracefile;
    FILE * statisticfile;
    FILE * statisticfile2;

    struct parameter_value *parameter;   //SSD参数因子
    struct dram_info *dram;
    struct request *request_queue;       //dynamic request queue
    struct request *request_tail;	     // the tail of the request queue
    struct sub_request *subs_w_head;     //当采用全动态分配时，分配是不知道应该挂载哪个channel上，所以先挂在ssd上，等进�?process函数时才挂到相应的channel的�?��?�求队列�?
    struct sub_request *subs_w_tail;
    struct event_node *event;            //事件队列，每产生一�?新的事件，按照时间顺序加到这�?队列，在simulate函数最后，根据这个队列队�?�的时间，确定时�?
    struct channel_info *channel_head;   //指向channel结构体数组的首地址
};


struct channel_info{
    int chip;                            //表示在�?�总线上有多少颗粒
    unsigned long read_count;
    unsigned long program_count;
    unsigned long erase_count;
    unsigned int token;                  //在动态分配中，为防�?�每次分配在�?一个chip需要维持一�?令牌，每次从令牌所指的位置开始分�?

    int current_state;                   //channel has serveral states, including idle, command/address transfer,data transfer,unknown
    int next_state;
    int64_t current_time;                //记录该通道的当前时�?
    int64_t next_state_predict_time;     //the predict time of next state, used to decide the sate at the moment

    struct event_node *event;
    struct sub_request *subs_r_head;     //channel上的读�?�求队列头，先服务�?�于队列头的子�?�求
    struct sub_request *subs_r_tail;     //channel上的读�?�求队列尾，新加进来的子请求加到队尾
    struct sub_request *subs_w_head;     //channel上的写�?�求队列头，先服务�?�于队列头的子�?�求
    struct sub_request *subs_w_tail;     //channel上的写�?�求队列，新加进来的子�?�求加到队尾
    struct gc_operation *gc_command;     //记录需要产生gc的位�?
    struct chip_info *chip_head;        
};


struct chip_info{
    unsigned int die_num;               //表示一�?颗粒�?有�?�少个die
    unsigned int plane_num_die;         //indicate how many planes in a die
    unsigned int block_num_plane;       //indicate how many blocks in a plane
    unsigned int page_num_block;        //indicate how many pages in a block
    unsigned int subpage_num_page;      //indicate how many subpage in a page
    unsigned int ers_limit;             //�?chip�?每块能�?��??擦除的�?�数
    unsigned int token;                 //在动态分配中，为防�?�每次分配在�?一个die需要维持一�?令牌，每次从令牌所指的位置开始分�?

    int current_state;                  //channel has serveral states, including idle, command/address transfer,data transfer,unknown
    int next_state;
    int64_t current_time;               //记录该通道的当前时�?
    int64_t next_state_predict_time;    //the predict time of next state, used to decide the sate at the moment

    unsigned long read_count;           //how many read count in the process of workload
    unsigned long program_count;
    unsigned long erase_count;

    struct ac_time_characteristics ac_timing;  
    struct die_info *die_head;
};


struct die_info{

    unsigned int token;                 //在动态分配中，为防�?�每次分配在�?一个plane需要维持一�?令牌，每次从令牌所指的位置开始分�?
    struct plane_info *plane_head;

};


struct plane_info{
    int add_reg_ppn;                    //read，write时把地址传送到该变量，该变量代表地址寄存器。die由busy变为idle时，清除地址 //有可能因为一对�?�的映射，在一�?读�?�求时，有�?�个相同的lpn，所以需要用ppn来区�?  
    unsigned int free_page;             //�?plane�?有�?�少free page
    unsigned int allocated_page;        //标识当前plane�?分配了�?�少�?
    unsigned int ers_invalid;           //记录�?plane�?擦除失效的块�?
    unsigned int active_block;          //if a die has a active block, 该项表示其物理块�?
    int can_erase_block;                //记录在一个plane�?准�?�在gc操作�?�?擦除操作的块,-1表示还没有找到合适的�?
    struct direct_erase *erase_node;    //用来记录�?以直接删除的块号,在获取新的ppn时，每当出现invalid_page_num==64时，将其添加到这�?指针上，供GC操作时直接删�?
    struct blk_info *blk_head;
    int activeblk[2];                   //分别记录LC MT对应的活跃块
};


struct blk_info{
    unsigned int erase_count;          //块的擦除次数，�?�项记录在ram�?，用于GC
    unsigned int free_page_num;        //记录该块�?的free页个数，同上
    unsigned int invalid_page_num;     //记录该块�?失效页的�?数，同上
    int last_write_page;               //记录最近一次写操作执�?�的页数,-1表示该块没有一页�??写过
    struct page_info *page_head;       //记录每一子页的状�?

    int LCMT_number[2];                 //记录当前块�?�应的bit类型到达的cell number，刚开始为NONE
};


struct page_info{                      //lpn记录该物理页存储的逻辑页，当�?�逻辑页有效时，valid_state大于0，free_state大于0�?
    int valid_state;                   //indicate the page is valid or invalid
    int free_state;                    //each bit indicates the subpage is free or occupted. 1 indicates that the bit is free and 0 indicates that the bit is used
    unsigned int lpn;                 
    unsigned int written_count;        //记录该页�?写的次数
};


struct dram_info{
    unsigned int dram_capacity;     
    int64_t current_time;

    struct dram_parameter *dram_paramters;      
    struct map_info *map;
    struct buffer_info *buffer; 

};


/*********************************************************************************************
 *buffer�?的已写回的页的�?�理方法:在buffer_info�?维持一�?队列:written。这�?队列有队首，队尾�?
 *每�??buffer management�?，�?�求命中时，这个group要移到LRU的队首，同时看这个group�?�?否有�?
 *写回的lsn，有的话，需要将这个group同时移动到written队列的队尾。这�?队列的�?�长和减少的方法
 *如下:当需要通过删除已写回的lsn为新的写请求腾出空间时，在written队�?�中找出�?以删除的lsn�?
 *当需要�?�加新的写回lsn时，找到�?以写回的页，将这个group加到指针written_insert所指队列written
 *节点前。我�?需要再维持一�?指针，在buffer的LRU队列�?指向最老的一�?写回了的页，下�?��?�再写回时，
 *�?需由这�?指针回退到前一个group写回即可�?
 **********************************************************************************************/
typedef struct buffer_group{
    TREE_NODE node;                     //树节点的结构一定�?�放在用户自定义结构的最前面，注�?!
    struct buffer_group *LRU_link_next;	// next node in LRU list
    struct buffer_group *LRU_link_pre;	// previous node in LRU list

    unsigned int group;                 //the first data logic sector number of a group stored in buffer 
    unsigned int stored;                //indicate the sector is stored in buffer or not. 1 indicates the sector is stored and 0 indicate the sector isn't stored.EX.  00110011 indicates the first, second, fifth, sixth sector is stored in buffer.
    unsigned int dirty_clean;           //it is flag of the data has been modified, one bit indicates one subpage. EX. 0001 indicates the first subpage is dirty
    int flag;			                //indicates if this node is the last 20% of the LRU list	
}buf_node;


struct dram_parameter{
    float active_current;
    float sleep_current;
    float voltage;
    int clock_time;
};


struct map_info{
    struct entry *map_entry;            //该项�?映射表结构体指针,each entry indicate a mapping information
    struct buffer_info *attach_info;	// info about attach map
};


struct controller_info{
    unsigned int frequency;             //表示该控制器的工作�?�率
    int64_t clock_time;                 //表示一�?时钟周期的时�?
    float power;                        //表示控制器单位时间的能�?
};


struct request{
    int64_t time;                      //请求到达的时间，单位为us,这里和通常的习�?不一样，通常的是ms为单位，这里需要有�?单位变换过程
    unsigned int lsn;                  //请求的起始地址，逻辑地址
    unsigned int size;                 //请求的大小，既�?�少�?扇区
    unsigned int operation;            //请求的�?�类�?1为�?�，0为写

    unsigned int* need_distr_flag;
    unsigned int complete_lsn_count;   //record the count of lsn served by buffer

    int distri_flag;		           // indicate whether this request has been distributed already

    int64_t begin_time;
    int64_t response_time;              //记录该�?�求在dram�?的响应时�?
    double energy_consumption;         //记录该�?�求的能量消耗，单位为uJ

    struct sub_request *subs;          //链接到属于�?��?�求的所有子请求
    struct request *next_node;         //指向下一�?请求结构�?
};


struct sub_request{
    unsigned int lpn;                  //这里表示该子请求的逻辑页号
    unsigned int ppn;                  //分配那个物理子页给这�?子�?�求。在multi_chip_page_mapping�?，产生子页�?�求时可能就知道psn的值，其他时候psn的值由page_map_read,page_map_write等FTL最底层函数产生�? 
    unsigned int operation;            //表示该子请求的类型，除了�?1 �?0，还有擦除，two plane等操�? 
    int size;

    unsigned int current_state;        //表示该子请求所处的状态，见宏定义sub request
    int64_t current_time;
    unsigned int next_state;
    int64_t next_state_predict_time;
    unsigned int state;              //使用state的最高位表示该子请求�?否是一对�?�映射关系中的一�?，是的话，需要�?�到buffer�?�?1表示�?一对�?�，0表示不用写到buffer
    //读�?�求不需要这�?成员，lsn加size就可以分辨出该页的状�?;但是写�?�求需要这�?成员，大部分写子请求来自于buffer写回操作，可能有类似子页不连�?的情况，所以需要单�?维持该成�?

    int bit_type;                   //记录该子请求存储应当存储的bit类型，P_LC或者P_MT

    int64_t begin_time;               //子�?�求开始时�?
    int64_t complete_time;            //记录该子请求的�?�理时间,既真正写入或者�?�出数据的时�?

    struct local *location;           //在静态分配和混合分配方式�?，已�?lpn就知道�??lpn该分配到那个channel，chip，die，plane，这�?结构体用来保存�?�算得到的地址
    struct sub_request *next_subs;    //指向属于同一个request的子请求
    struct sub_request *next_node;    //指向同一个channel�?下一�?子�?�求结构�?
    struct sub_request *update;       //因为在写操作�?存在更新操作，因为在动态分配方式中无法使用copyback操作，需要将原来的页读出后才能进行写操作，所以，将因更新产生的�?�操作挂在这�?指针�?
};


/***********************************************************************
 *事件节点控制时间的�?�长，每次时间的增加�?根据时间最近的一�?事件来确定的
 ************************************************************************/
struct event_node{
    int type;                        //记录该事件的类型�?1表示命令类型�?2表示数据传输类型
    int64_t predict_time;            //记录这个时间开始的预�?�时间，防�?�提前执行这�?时间
    struct event_node *next_node;
    struct event_node *pre_node;
};

struct parameter_value{
    unsigned int chip_num;          //记录一个SSD�?有�?�少�?颗粒
    unsigned int dram_capacity;     //记录SSD中DRAM capacity
    unsigned int cpu_sdram;         //记录片内有�?�少

    unsigned int channel_number;    //记录SSD�?有�?�少�?通道，每�?通道�?单独的bus
    unsigned int chip_channel[100]; //设置SSD中channel数和每channel上�?�粒的数�?

    unsigned int die_chip;    
    unsigned int plane_die;
    unsigned int block_plane;
    unsigned int page_block;
    unsigned int subpage_page;

    unsigned int page_capacity;
    unsigned int subpage_capacity;


    unsigned int ers_limit;         //记录每个块可擦除的�?�数
    int address_mapping;            //记录映射的类型，1：page�?2：block�?3：fast
    int wear_leveling;              // WL算法
    int gc;                         //记录gc策略
    int clean_in_background;        //清除操作�?否在前台完成
    int alloc_pool;                 //allocation pool 大小(plane，die，chip，channel),也就�?拥有active_block的单�?
    float overprovide;
    float gc_threshold;             //当达到这�?阈值时，开始GC操作，在主动写策略中，开始GC操作后可以临时中断GC操作，服务新到的请求；在�?通策略中，GC不可�?�?

    double operating_current;       //NAND FLASH的工作电流单位是uA
    double supply_voltage;	
    double dram_active_current;     //cpu sdram work current   uA
    double dram_standby_current;    //cpu sdram work current   uA
    double dram_refresh_current;    //cpu sdram work current   uA
    double dram_voltage;            //cpu sdram work voltage  V

    int buffer_management;          //indicates that there are buffer management or not
    int scheduling_algorithm;       //记录使用�?种调度算法，1:FCFS
    float quick_radio;
    int related_mapping;

    unsigned int time_step;
    unsigned int small_large_write; //the threshould of large write, large write do not occupt buffer, which is written back to flash directly

    int striping;                   //表示�?否使用了striping方式�?0表示没有�?1表示�?
    int interleaving;
    int pipelining;
    int threshold_fixed_adjust;
    int threshold_value;
    int active_write;               //表示�?否执行主动写操作1,yes;0,no
    float gc_hard_threshold;        //�?通策略中用不到�?�参数，�?有在主动写策略中，当满足这个阈值时，GC操作不可�?�?
    int allocation_scheme;          //记录分配方式的选择�?0表示动态分配，1表示静态分�?
    int static_allocation;          //记录�?那�?�静态分配方式，如ICS09那篇文章所述的所有静态分配方�?
    int dynamic_allocation;         //记录动态分配的方式
    int advanced_commands;  
    int ad_priority;                //record the priority between two plane operation and interleave operation
    int ad_priority2;               //record the priority of channel-level, 0 indicates that the priority order of channel-level is highest; 1 indicates the contrary
    int greed_CB_ad;                //0 don't use copyback advanced commands greedily; 1 use copyback advanced commands greedily
    int greed_MPW_ad;               //0 don't use multi-plane write advanced commands greedily; 1 use multi-plane write advanced commands greedily
    int aged;                       //1表示需要将这个SSD变成aged�?0表示需要将这个SSD保持non-aged
    float aged_ratio; 
    int queue_length;               //请求队列的长度限�?

    struct ac_time_characteristics time_characteristics;
};

/********************************************************
 *mapping information,state的最高位表示�?否有附加映射关系
 *********************************************************/
struct entry{                       
    unsigned int pn;                //物理号，既可以表示物理页号，也可以表示物理子页号，也�?以表示物理块�?
    int state;                      //十六进制表示的话�?0000-FFFF，每位表示相应的子页�?否有效（页映射）。比如在这个页中�?0�?1号子页有效，2�?3无效，这�?应�?�是0x0003.
    int read_count;
    int write_count;
};


struct local{          
    unsigned int channel;
    unsigned int chip;
    unsigned int die;
    unsigned int plane;
    unsigned int block;
    unsigned int page;
    unsigned int sub_page;
};


struct gc_info{
    int64_t begin_time;            //记录一个plane什么时候开始gc操作�?
    int copy_back_count;    
    int erase_count;
    int64_t process_time;          //�?plane花了多少时间在gc操作�?
    double energy_consumption;     //�?plane花了多少能量在gc操作�?
};


struct direct_erase{
    unsigned int block;
    struct direct_erase *next_node;
};


/**************************************************************************************
 *当产生一个GC操作时，将这�?结构挂在相应的channel上，等待channel空闲时，发出GC操作命令
 ***************************************************************************************/
struct gc_operation{          
    unsigned int chip;
    unsigned int die;
    unsigned int plane;
    unsigned int block;           //该参数只在可�?�?的gc函数�?使用（gc_interrupt），用来记录已近找出来的�?标块�?
    unsigned int page;            //该参数只在可�?�?的gc函数�?使用（gc_interrupt），用来记录已经完成的数�?迁移的页�?
    unsigned int state;           //记录当前gc请求的状�?
    unsigned int priority;        //记录�?gc操作的优先级�?1表示不可�?�?�?0表示�?�?�?（软阈值产生的gc请求�?
    struct gc_operation *next_node;
};

/*
 *add by ninja
 *used for map_pre function
 */
typedef struct Dram_write_map
{
    unsigned int state; 
}Dram_write_map;


struct ssd_info *initiation(struct ssd_info *);
struct parameter_value *load_parameters(char parameter_file[30]);
struct page_info * initialize_page(struct page_info * p_page);
struct blk_info * initialize_block(struct blk_info * p_block,struct parameter_value *parameter);
struct plane_info * initialize_plane(struct plane_info * p_plane,struct parameter_value *parameter );
struct die_info * initialize_die(struct die_info * p_die,struct parameter_value *parameter,long long current_time );
struct chip_info * initialize_chip(struct chip_info * p_chip,struct parameter_value *parameter,long long current_time );
struct ssd_info * initialize_channels(struct ssd_info * ssd );
struct dram_info * initialize_dram(struct ssd_info * ssd);

#endif

