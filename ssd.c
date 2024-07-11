/*****************************************************************************************************************************
  This project was supported by the National Basic Research 973 Program of China under Grant No.2011CB302301
  Huazhong University of Science and Technology (HUST)   Wuhan National Laboratory for Optoelectronics

FileName: ssd.c
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



#include "ssd.h"
#include "initialize.h"
#include "pagemap.h"
#include "string.h"

unsigned int request_id = 0;
/********************************************************************************************************************************
  1，main函数中initiatio()函数用来初�?�化ssd,�?2，make_aged()函数使SSD成为aged，aged的ssd相当于使用过一段时间的ssd，里面有失效页，
  non_aged的ssd�?新的ssd，无失效页，失效页的比例�?以在初�?�化参数�?设置�?3，pre_process_page()函数提前�?一遍�?��?�求，把读�?�求
  的lpn<--->ppn映射关系事先建立好，写�?�求的lpn<--->ppn映射关系在写的时候再建立，�?��?�理trace防�?��?��?�求�?读不到数�?�?4，simulate()�?
  核心处理函数，trace文件从�?�进来到处理完成都由这个函数来完成；5，statistic_output()函数将ssd结构�?的信�?输出到输出文件，输出的是
  统�?�数�?和平均数�?，输出文件较小，trace_output文件则很大很详细�?6，free_all_node()函数释放整个main函数�?申�?�的节点
 *********************************************************************************************************************************/
int  main()
{
    unsigned  int i,j,k,t;
    

#ifdef DEBUG
    printf("enter main\n"); 
#endif

    
    for(int index_i = 0;index_i < 1;index_i ++){
        struct ssd_info *ssd;
        ssd=(struct ssd_info*)malloc(sizeof(struct ssd_info));
        alloc_assert(ssd,"ssd");
        memset(ssd,0, sizeof(struct ssd_info));
        switch (index_i)
        {
        case 0:
            strncpy(ssd->tracefilename,"./workloads/unzip/hm_0.csv",45);
            strncpy(ssd->statisticfilename,"hm_0.dat",16);
            break;
        case 1:
            strncpy(ssd->tracefilename,"./workloads/unzip/hm_1.csv",45);
            strncpy(ssd->statisticfilename,"hm_1.dat",16);
            break;
        case 2:
             strncpy(ssd->tracefilename,"./workloads/unzip/src2_0.csv",45);
            strncpy(ssd->statisticfilename,"src2_0.dat",16);
            break;
        case 3:
             strncpy(ssd->tracefilename,"./workloads/unzip/stg_0.csv",45);
            strncpy(ssd->statisticfilename,"stg_0.dat",16);
            break;
        case 4:
             strncpy(ssd->tracefilename,"./workloads/unzip/usr_0.csv",45);
            strncpy(ssd->statisticfilename,"usr_0.dat",16);
            break;
        case 5:
            strncpy(ssd->tracefilename,"./workloads/unzip/wdev_0.csv",45);
            strncpy(ssd->statisticfilename,"wdev_0.dat",16);
            break;
        case 6:
            strncpy(ssd->tracefilename,"./workloads/unzip/prxy_0.csv",45);
            strncpy(ssd->statisticfilename,"prxy_0.dat",16);
            break;
        case 7:
            strncpy(ssd->tracefilename,"./workloads/unzip/proj_3.csv",45);
            strncpy(ssd->statisticfilename,"proj_3.dat",16);
            break;
        case 8:
            strncpy(ssd->tracefilename,"./workloads/rsrch/rsrch_0.csv",45);
            strncpy(ssd->statisticfilename,"rsrch_0.dat",16);
            break;
        case 9:
            strncpy(ssd->tracefilename,"./workloads/rsrch/rsrch_1.csv",45);
            strncpy(ssd->statisticfilename,"rsrch_1.dat",16);
        case 10:
            strncpy(ssd->tracefilename,"./workloads/unzip/proj_1.csv",45);
            strncpy(ssd->statisticfilename,"proj_1.dat",16);
        default:
            break;
        }
    ssd=initiation(ssd);
    printf("***************************BDP_without_invalid*******************\n");
    printf("***************************%s*******************\n",ssd->statisticfilename);
    for (i=0;i<ssd->parameter->channel_number;i++)
    {
        for(t=0;t < ssd->parameter->chip_channel[0];t++){
            for (j=0;j<ssd->parameter->die_chip;j++)
            {
                for (k=0;k<ssd->parameter->plane_die;k++)
                {
                    printf("%d,%d,%d,%d:  %5d\n",i,t,j,k,ssd->channel_head[i].chip_head[t].die_head[j].plane_head[k].free_page);
                }
            }
        }
        
    }

    // 刚开始，所有的plane buffer类型都为NONE
    ssd->plane_num = ssd->parameter->channel_number * ssd->parameter->chip_channel[0] * ssd->parameter->die_chip * ssd->parameter->plane_die;
    for(int i = 0;i < ssd->plane_num;i++){
        // 刚开始的bitmap_table都初始化为MT_FULL，表示为当前没有空闲的MT位置
        bitmap_table[i] = MT_FULL;
        current_buffer[i] = NONE;
    }
    make_aged(ssd);
    pre_process_page(ssd);
    // for(int i = 0;i < ssd->plane_num;i++){
    //     if(bitmap_table[i] != NONE && bitmap_table[i] != FULL){
    //         bitmap_table[i] /= 2;
    //     }
    // }

    for (i=0;i<ssd->parameter->channel_number;i++)
    {
        for(t=0;t < ssd->parameter->chip_channel[0];t++){
            for (j=0;j<ssd->parameter->die_chip;j++)
            {
                for (k=0;k<ssd->parameter->plane_die;k++)
                {
                    printf("%d,%d,%d,%d:  %5d\n",i,t,j,k,ssd->channel_head[i].chip_head[t].die_head[j].plane_head[k].free_page);
                }
            }
        }
        
    }
    fprintf(ssd->outputfile,"\t\t\t\t\t\t\t\t\tOUTPUT\n");
    fprintf(ssd->outputfile,"****************** TRACE INFO ******************\n");
    ssd=simulate(ssd);
    printf("dram node %u\n",avlTreeCount(ssd->dram->buffer));
    printf("update write %llu + free invalid %llu = %llu, real written %llu\n",ssd->update_write,ssd->free_invalid,ssd->update_write + ssd->free_invalid,ssd->real_written);
    statistic_output(ssd);  
    /*	free_all_node(ssd);*/

    printf("\n");
    printf("the %s simulation is completed!\n",ssd->statisticfilename);

    printf("\n");
    printf("the simulation is completed!\n");

    for (i=0;i<ssd->parameter->channel_number;i++)
    {
        for(t=0;t < ssd->parameter->chip_channel[0];t++){
            for (j=0;j<ssd->parameter->die_chip;j++)
            {
                for (k=0;k<ssd->parameter->plane_die;k++)
                {
                    printf("%d,%d,%d,%d:  %5d\n",i,t,j,k,ssd->channel_head[i].chip_head[t].die_head[j].plane_head[k].free_page);
                }
            }
        }
        
    }
    free(ssd);
    ssd = NULL;
    }

    
    return 1;
    /* 	_CrtDumpMemoryLeaks(); */
}


/******************simulate() *********************************************************************
 *simulate()�?核心处理函数，主要实现的功能包括
 *1,从trace文件�?获取一条�?�求，挂到ssd->request
 *2，根据ssd�?否有dram分别处理读出来的请求，把这些请求处理成为读写子�?�求，挂到ssd->channel或者ssd�?
 *3，按照事件的先后来�?�理这些读写子�?�求�?
 *4，输出每条�?�求的子请求都�?�理完后的相关信�?到outputfile文件�?
 **************************************************************************************************/
struct ssd_info *simulate(struct ssd_info *ssd)
{
    int flag=1,flag1=0;
    double output_step=0;
    unsigned int a=0,b=0;
    //errno_t err;

    printf("\n");
    printf("begin simulating.......................\n");
    printf("\n");
    printf("\n");
    printf("   ^o^    OK, please wait a moment, and enjoy music and coffee   ^o^    \n");

    ssd->tracefile = fopen(ssd->tracefilename,"r");
    if(ssd->tracefile == NULL)
    {  
        printf("the trace file can't open\n");
        return NULL;
    }

    fprintf(ssd->outputfile,"      arrive           lsn     size ope     begin time    response time    process time\n");	
    fflush(ssd->outputfile);
    int t = 0;
    int sq = 0;
    while(flag!=100)      
    {
        flag=get_requests(ssd);
        // t++;
        if(flag == 1)
        {   
            if(sq == 0){
                ssd->first_time = ssd->request_queue->time;
                ssd->sub_total_time = 0;
            }
            if (ssd->parameter->dram_capacity!=0)
            {
                buffer_management(ssd);  
                distribute(ssd); 
            } 
            else
            {
                no_buffer_distribute(ssd);
            }
            printf("%d\n",t++);		
        }
        sq++;
        process(ssd);    
        trace_output(ssd);
        // printf("%d\n",sq);
        if(flag == 0 && ssd->request_queue == NULL)
            flag = 100;
        if(ssd->current_time > ssd->biggest_time){
            ssd->biggest_time = ssd->current_time;
        }
    }
    printf("error times is %d\n",ssd->error_times);
    fclose(ssd->tracefile);
    for(int i = 0;i < ssd->parameter->channel_number;i ++){
        for(int j = 0;j < ssd->parameter->chip_channel[0];j++){
            printf("channel %d chip %d busy time is %llu\n",i,j,ssd->channel_head[i].chip_head[j].busy_time);
        }
        printf("channel %d busy time is %llu\n",i,ssd->channel_head[i].busy_time);
    }
    printf("request gap time is %llu\n",ssd->last_time - ssd->first_time);
    printf("simulate time is %llu\n",ssd->current_time - ssd->first_time);
    printf("biggest time to first request is %llu\n",ssd->biggest_time - ssd->first_time);
    for(int i = 0;i < ssd->parameter->channel_number;i++){
        printf("channel %d prog_nums %llu\n",i,ssd->channel_head[i].prog_sub_nums);
        printf("channel %d read_nums %llu\n",i,ssd->channel_head[i].read_sub_nums);
        for(int j = 0;j < ssd->parameter->chip_channel[0];j++){
            printf("channel %d chip %d prog_nums %llu\n",i,j,ssd->channel_head[i].chip_head[j].prog_sub_nums);
            printf("channel %d chip %d read_nums %llu\n",i,j,ssd->channel_head[i].chip_head[j].read_sub_nums);
        }
    }
    return ssd;
}



/********    get_request    ******************************************************
 *	1.get requests that arrived already
 *	2.add those request node to ssd->reuqest_queue
 *	return	0: reach the end of the trace
 *			-1: no request has been added
 *			1: add one request to list
 *SSD模拟器有三�?�驱动方�?:时钟驱动(精确，太�?) 事件驱动(�?程序采用) trace驱动()�?
 *两�?�方式推进事件：channel/chip状态改变、trace文件请求达到�?
 *channel/chip状态改变和trace文件请求到达�?散布在时间轴上的点，每�?�从当前状态到�?
 *下一�?状态都要到达最近的一�?状态，每到达一�?点执行一�?process
 ********************************************************************************/
int get_requests(struct ssd_info *ssd)  
{  
    char buffer[200];
    unsigned int lsn=0;
    int device,  size, ope,large_lsn, i = 0,j=0;
    struct request *request1;
    int flag = 1;
    long filepoint; 
    int64_t time_t = 0;
    int64_t nearest_event_time;    

#ifdef DEBUG
    printf("enter get_requests,  current time:%lld\n",ssd->current_time);
#endif

    // �?改了这里，�?�ssd->current_time进�?�更新，否则当运行完工作负载时，ssd->current_time一直不�?，�?�致后面请求一直卡�?
    if(feof(ssd->tracefile)){
        ssd->current_time = find_nearest_event(ssd);
        return 0;
    }
         

    filepoint = ftell(ssd->tracefile);	
    fgets(buffer, 200, ssd->tracefile); 
    // sscanf(buffer,"%lld %d %d %d %d",&time_t,&device,&lsn,&size,&ope);
    int response;
    char workload[4];
    char type[5];
    // sscanf(buffer,"%lld %d %d %lld %d %d",&time_t,&device,&ope,&lsn,&size,&response);
    sscanf(buffer,"%lld,%[^,],%d,%[^,],%lld,%d,%d",&time_t,workload,&device,type,&lsn,&size,&response);        
    size /= SECTOR;
    if(type!= NULL && strstr(type,"Read")){
        ope = 1;
    }else{
        ope = 0;
    }
    if(ope != READ && ope != WRITE){
        printf("ope is error\n");
    }
    

    if ((device<0)&&(lsn<0)&&(size<0)&&(ope<0))
    {
        return 100;
    }
    if (lsn<ssd->min_lsn) 
        ssd->min_lsn=lsn;
    if (lsn>ssd->max_lsn)
        ssd->max_lsn=lsn;
    
    /******************************************************************************************************
     *上层文件系统发送给SSD的任何�?�写命令包括两个部分（LSN，size�? LSN�?逻辑扇区号，对于文件系统而言，它所看到的存
     *储空间是一�?线性的连续空间。例如，读�?�求�?260�?6）表示的�?需要�?�取从扇区号�?260的逻辑扇区开始，总共6�?扇区�?
     *large_lsn: channel下面有�?�少个subpage，即多少个sector。overprovide系数：SSD�?并不�?所有的空间都可以给用户使用�?
     *比�??32G的SSD�?能有10%的空间保留下来留作他�?，所以乘�?1-provide
     ***********************************************************************************************************/
    large_lsn=(unsigned int )((ssd->parameter->chip_num*ssd->parameter->die_chip*ssd->parameter->plane_die*ssd->parameter->block_plane*ssd->parameter->page_block*ssd->parameter->subpage_page)*(1-ssd->parameter->overprovide));
    lsn = lsn%large_lsn;
    if(ssd->request_queue != NULL){
        ssd->last_time = ssd->request_queue->time;
    }
    // nearest_event_time不为max表示有请求被阻塞，它的通道空闲时间比当前时间晚
    nearest_event_time=find_nearest_event(ssd);
   
    unsigned long long old_current_time;
    if(ssd->current_time > ssd->biggest_time){
        ssd->biggest_time = ssd->current_time;
    }

    // modify
    // old_current_time = ssd->current_time;
    // unsigned long long gap;
    // if(nearest_event_time != MAX_INT64){
    //     gap = nearest_event_time - ssd->current_time;
    //     ssd->current_time = nearest_event_time; 
    //     if(nearest_event_time < time_t || ssd->request_queue_length >= ssd->parameter->queue_length){    
    //         fseek(ssd->tracefile,filepoint,0);
    //         gap = ssd->current_time - old_current_time;
    //         return -1;
    //     }
    // }else{
    //     if(ssd->current_time < time_t){
    //         ssd->current_time = time_t;
    //     }
    // }
    // gap = ssd->current_time - old_current_time;

     if (nearest_event_time==MAX_INT64)
    {
        ssd->current_time=time_t;           

        //if (ssd->request_queue_length>ssd->parameter->queue_length)    //如果请求队列的长度超过了配置文件�?所设置的长�?                     
        //{
        //printf("error in get request , the queue length is too long\n");
        //}
    }
    else
    {   
        if(nearest_event_time<time_t)
        {
            /*******************************************************************************
             *回滚，即如果没有把time_t赋给ssd->current_time，则trace文件已�?�的一条�?�录回滚
             *filepoint记录了执行fgets之前的文件指针位�?，回滚到文件�?+filepoint�?
             *int fseek(FILE *stream, long offset, int fromwhere);函数设置文件指针stream的位�?�?
             *如果执�?�成功，stream将指向以fromwhere（偏移起始位�?：文件头0，当前位�?1，文件尾2）为基准�?
             *偏移offset（指针偏移量）个字节的位�?。�?�果执�?�失�?(比�?�offset超过文件�?�?大小)，则不改变stream指向的位�?�?
             *文本文件�?能采用文件头0的定位方式，�?程序�?打开文件方式�?"r":以只读方式打开文本文件	
             **********************************************************************************/
            fseek(ssd->tracefile,filepoint,0); 
            if(ssd->current_time<=nearest_event_time)
                ssd->current_time=nearest_event_time;
            return -1;
        }
        else
        {
            if (ssd->request_queue_length>=ssd->parameter->queue_length)
            {
                fseek(ssd->tracefile,filepoint,0);
                ssd->current_time=nearest_event_time;
                return -1;
            } 
            else
            {
                ssd->current_time=time_t;
            }
        }
    }
    
    if(ssd->current_time > ssd->biggest_time){
        ssd->biggest_time = ssd->current_time;
    }

    if(feof(ssd->tracefile))
    {
        request1=NULL;
        return 0;
    }

    request1 = (struct request*)malloc(sizeof(struct request));
    alloc_assert(request1,"request");
    memset(request1,0, sizeof(struct request));

    request1->time = time_t;
    request1->id = request_id;
    request_id++;
    request1->lsn = lsn;
    request1->size = size;
    request1->operation = ope;	
    request1->begin_time = time_t;
    request1->response_time = 0;	
    request1->energy_consumption = 0;	
    request1->next_node = NULL;
    request1->distri_flag = 0;              // indicate whether this request has been distributed already
    request1->subs = NULL;
    request1->need_distr_flag = NULL;
    request1->complete_lsn_count=0;         //record the count of lsn served by buffer
    filepoint = ftell(ssd->tracefile);		// set the file point
    if(ope == 0){
        ssd->total_write+=size;
    }else{
        ssd->total_read+=size;
    }
    // printf("erase is %d\n",ssd->erase_count);
    ssd->last_time = time_t;
    if(ssd->request_queue == NULL)          //The queue is empty
    {
        ssd->request_queue = request1;
        ssd->request_tail = request1;
        ssd->request_queue_length++;
    }
    else
    {			
        (ssd->request_tail)->next_node = request1;	
        ssd->request_tail = request1;			
        ssd->request_queue_length++;
    }

    if (request1->operation==1)             //计算平均请求大小 1为�?? 0为写
    {
        ssd->ave_read_size=(ssd->ave_read_size*ssd->read_request_count+request1->size)/(ssd->read_request_count+1);
    } 
    else
    {
        ssd->ave_write_size=(ssd->ave_write_size*ssd->write_request_count+request1->size)/(ssd->write_request_count+1);
    }


    filepoint = ftell(ssd->tracefile);	
    fgets(buffer, 200, ssd->tracefile);    //寻找下一条�?�求的到达时�?
    // sscanf(buffer,"%lld %d %d %d %d",&time_t,&device,&lsn,&size,&ope);
    sscanf(buffer,"%lld %d %d %lld %d %d",&time_t,&device,&ope,&lsn,&size,&response);
    ssd->next_request_time=time_t;
    fseek(ssd->tracefile,filepoint,0);
    if(time_t - request1->time < 100000){
        ssd->error_times++;
    }

    return 1;
}

/**********************************************************************************************************************************************
 *首先buffer�?�?写buffer，就�?为写请求服务的，因为读flash的时间tR�?20us，写flash的时间tprog�?200us，所以为写服务更能节省时�?
 *  读操作：如果命中了buffer，从buffer读，不占用channel的I/O总线，没有命中buffer，从flash读，占用channel的I/O总线，但�?不进buffer�?
 *  写操作：首先request分成sub_request子�?�求，�?�果�?动态分配，sub_request挂到ssd->sub_request上，因为不知道�?�先挂到�?个channel的sub_request�?
 *          如果�?静态分配则sub_request挂到channel的sub_request链上,同时不�?�动态分配还�?静态分配sub_request都�?�挂到request的sub_request链上
 *		   因为每�?�理完一个request，都要在traceoutput文件�?输出关于这个request的信�?。�?�理完一个sub_request,就将其从channel的sub_request�?
 *		   或ssd的sub_request链上摘除，但�?在traceoutput文件输出一条后再清空request的sub_request链�?
 *		   sub_request命中buffer则在buffer里面写就行了，并且将�?sub_page提到buffer链头(LRU)，若没有命中且buffer满，则先将buffer链尾的sub_request
 *		   写入flash(这会产生一个sub_request写�?�求，挂到这�?请求request的sub_request链上，同时�?�动态分配还�?静态分配挂到channel或ssd�?
 *		   sub_request链上),在将要写的sub_page写入buffer链头
 ***********************************************************************************************************************************************/
struct ssd_info *buffer_management(struct ssd_info *ssd)
{   
    unsigned int j,lsn,lpn,last_lpn,first_lpn,index,complete_flag=0, state,full_page;
    unsigned int flag=0,need_distb_flag,lsn_flag,flag1=1,active_region_flag=0;           
    struct request *new_request;
    struct buffer_group *buffer_node,key;
    unsigned int mask=0,offset1=0,offset2=0;

#ifdef DEBUG
    printf("enter buffer_management,  current time:%lld\n",ssd->current_time);
#endif
    ssd->dram->current_time=ssd->current_time;
    full_page=~(0xffffffff<<ssd->parameter->subpage_page);

    new_request=ssd->request_tail;
    lsn=new_request->lsn;
    if(lsn == 1712337){
        printf("here 1712337 check lpn 428092\n");
    }
    lpn=new_request->lsn/ssd->parameter->subpage_page;
    last_lpn=(new_request->lsn+new_request->size-1)/ssd->parameter->subpage_page;
    first_lpn=new_request->lsn/ssd->parameter->subpage_page;
    int capacity = sizeof(unsigned int)*((last_lpn-first_lpn+1)*ssd->parameter->subpage_page/32+1);
    new_request->need_distr_flag=(unsigned int*)malloc(capacity);
    alloc_assert(new_request->need_distr_flag,"new_request->need_distr_flag");
    memset(new_request->need_distr_flag, 0, sizeof(unsigned int)*((last_lpn-first_lpn+1)*ssd->parameter->subpage_page/32+1));

    if(new_request->operation==READ) 
    {		
        while(lpn<=last_lpn)      		
        {
            /************************************************************************************************
             *need_distb_flag表示�?否需要执行distribution函数�?1表示需要执行，buffer�?没有�?0表示不需要执�?
             *�?1表示需要分发，0表示不需要分发，对应点初始全部赋�?1
             *************************************************************************************************/
            need_distb_flag=full_page;  
             
            key.group=lpn;
            buffer_node= (struct buffer_group*)avlTreeFind(ssd->dram->buffer, (TREE_NODE *)&key);		// buffer node 
            ssd->dram->map->map_entry[lpn].read_count++;
            while((buffer_node!=NULL)&&(lsn<(lpn+1)*ssd->parameter->subpage_page)&&(lsn<=(new_request->lsn+new_request->size-1)))             			
            {             	
                lsn_flag=full_page;
                mask=1 << (lsn%ssd->parameter->subpage_page);
                if(mask>31)
                {
                    printf("the subpage number is larger than 32!add some cases");
                    getchar(); 		   
                }
                else if((buffer_node->stored & mask)==mask)
                {
                    flag=1;
                    lsn_flag=lsn_flag&(~mask);
                }

                if(flag==1)				
                {	//如果�?buffer节点不在buffer的队首，需要将这个节点提到队�?�，实现了LRU算法，这�?�?一�?双向队列�?		       		
                    if(ssd->dram->buffer->buffer_head!=buffer_node)     
                    {		
                        if(ssd->dram->buffer->buffer_tail==buffer_node)								
                        {			
                            buffer_node->LRU_link_pre->LRU_link_next=NULL;					
                            ssd->dram->buffer->buffer_tail=buffer_node->LRU_link_pre;							
                        }				
                        else								
                        {				
                            buffer_node->LRU_link_pre->LRU_link_next=buffer_node->LRU_link_next;				
                            buffer_node->LRU_link_next->LRU_link_pre=buffer_node->LRU_link_pre;								
                        }								
                        buffer_node->LRU_link_next=ssd->dram->buffer->buffer_head;
                        ssd->dram->buffer->buffer_head->LRU_link_pre=buffer_node;
                        buffer_node->LRU_link_pre=NULL;			
                        ssd->dram->buffer->buffer_head=buffer_node;													
                    }						
                    ssd->dram->buffer->read_hit++;					
                    new_request->complete_lsn_count++;											
                }		
                else if(flag==0)
                {
                    ssd->dram->buffer->read_miss_hit++;
                }

                need_distb_flag=need_distb_flag&lsn_flag;

                flag=0;		
                lsn++;						
            }	

            index=(lpn-first_lpn)/(32/ssd->parameter->subpage_page); 			
            new_request->need_distr_flag[index]=new_request->need_distr_flag[index]|(need_distb_flag<<(((lpn-first_lpn)%(32/ssd->parameter->subpage_page))*ssd->parameter->subpage_page));	
            lpn++;

        }
    }  
    else if(new_request->operation==WRITE)
    {
        while(lpn<=last_lpn)           	
        {	
            ssd->dram->map->map_entry[lpn].write_count++;
            
            need_distb_flag=full_page;
            mask=~(0xffffffff<<(ssd->parameter->subpage_page));
            state=mask;

            if(lpn==first_lpn)
            {
                offset1=ssd->parameter->subpage_page-((lpn+1)*ssd->parameter->subpage_page-new_request->lsn);
                state=state&(0xffffffff<<offset1);
            }
            if(lpn==last_lpn)
            {
                offset2=ssd->parameter->subpage_page-((lpn+1)*ssd->parameter->subpage_page-(new_request->lsn+new_request->size));
                state=state&(~(0xffffffff<<offset2));
            }

            ssd=insert2buffer(ssd, lpn, state,NULL,new_request);
            lpn++;
        }
    }
    complete_flag = 1;
    for(j=0;j<=(last_lpn-first_lpn+1)*ssd->parameter->subpage_page/32;j++)
    {
        if(new_request->need_distr_flag[j] != 0)
        {
            complete_flag = 0;
        }
    }

    /*************************************************************
     *如果请求已经�?全部由buffer服务，�?��?�求�?以�??直接响应，输出结�?
     *这里假�?�dram的服务时间为1000ns
     **************************************************************/
    if((complete_flag == 1)&&(new_request->subs==NULL))               
    {
        new_request->begin_time=ssd->current_time;
        new_request->response_time=ssd->current_time+1000;            
    }

    return ssd;
}

/*****************************
 *lpn向ppn的转�?
 ******************************/
unsigned int lpn2ppn(struct ssd_info *ssd,unsigned int lsn)
{
    int lpn, ppn;	
    struct entry *p_map = ssd->dram->map->map_entry;
#ifdef DEBUG
    printf("enter lpn2ppn,  current time:%lld\n",ssd->current_time);
#endif
    lpn = lsn/ssd->parameter->subpage_page;			//lpn
    ppn = (p_map[lpn]).pn;
    return ppn;
}

/**********************************************************************************
 *读�?�求分配子�?�求函数，这里只处理读�?�求，写请求已经在buffer_management()函数�?处理�?
 *根据请求队列和buffer命中的�?�查，将每�?请求分解成子请求，将子�?�求队列挂在channel上，
 *不同的channel有自己的子�?�求队列
 **********************************************************************************/

struct ssd_info *distribute(struct ssd_info *ssd) 
{
    unsigned int start, end, first_lsn,last_lsn,lpn,flag=0,flag_attached=0,full_page;
    unsigned int j, k, sub_size;
    int i=0;
    struct request *req;
    struct sub_request *sub;
    int* complt;

#ifdef DEBUG
    printf("enter distribute,  current time:%lld\n",ssd->current_time);
#endif
    full_page=~(0xffffffff<<ssd->parameter->subpage_page);

    req = ssd->request_tail;
    if(req->response_time != 0){
        return ssd;
    }
    if (req->operation==WRITE)
    {
        return ssd;
    }

    if(req != NULL)
    {
        if(req->distri_flag == 0)
        {
            //如果还有一些读请求需要处理
            if(req->complete_lsn_count != ssd->request_tail->size)
            {		
                first_lsn = req->lsn;	
    			// 原先的last_lsn计算错误，没有减1
                last_lsn = first_lsn + req->size - 1;
                complt = req->need_distr_flag;
                start = first_lsn - first_lsn % ssd->parameter->subpage_page;
                // 原先的end计算错误
                // end = (last_lsn/ssd->parameter->subpage_page + 1) * ssd->parameter->subpage_page;
                end = last_lsn + (ssd->parameter->subpage_page - last_lsn % ssd->parameter->subpage_page -1);
                i = (end - start)/32;	

                while(i >= 0)
                {	
                    /*************************************************************************************
                     *一个32位的整型数据的每一位代表一个子页，32/ssd->parameter->subpage_page就表示有多少页，
                     *这里的每一页的状态都存放在了 req->need_distr_flag中，也就是complt中，通过比较complt的
                     *每一项与full_page，就可以知道，这一页是否处理完成。如果没处理完成则通过creat_sub_request
                     函数创建子请求。
                     *************************************************************************************/
                    for(j=0; j<32/ssd->parameter->subpage_page; j++)
                    {	
                        k = (complt[((end-start)/32-i)] >>(ssd->parameter->subpage_page*j)) & full_page;	
                        if (k !=0)
                        {
                            lpn = start/ssd->parameter->subpage_page+ ((end-start)/32-i)*32/ssd->parameter->subpage_page + j;
                            sub_size=transfer_size(ssd,k,lpn,req);   
                            if(lpn*ssd->parameter->subpage_page > end){
                                printf("here\n");
                                break;
                            } 
                            if (sub_size==0) 
                            {
                                continue;
                            }
                            else
                            {
                                sub=creat_sub_request(ssd,lpn,sub_size,0,req,req->operation);
                                // 用于追踪请求，记录请求时间
                                sub->lsn = req->lsn;
                                sub->req_id = req->id;
                                sub->req_begin_time = req->time;
                            }	
                        }
                    }
                    i = i-1;
                }

            }
            else
            {
                req->begin_time=ssd->current_time;
                req->response_time=ssd->current_time+1000;   
            }

        }
    }
    return ssd;
}


/**********************************************************************
 *trace_output()函数�?在每一条�?�求的所有子请求经过process()函数处理完后�?
 *打印输出相关的运行结果到outputfile文件�?，这里的结果主�?�是运�?�的时间
 **********************************************************************/
void trace_output(struct ssd_info* ssd){
    int flag = 1;	
    int64_t start_time, end_time;
    struct request *req, *pre_node;
    struct sub_request *sub, *tmp;

#ifdef DEBUG
    printf("enter trace_output,  current time:%lld\n",ssd->current_time);
#endif

    pre_node=NULL;
    req = ssd->request_queue;
    start_time = 0;
    end_time = 0;

    if(req == NULL)
        return;

    while(req != NULL)	
    {
        sub = req->subs;
        flag = 1;
        start_time = 0;
        end_time = 0;
        if(req->response_time != 0)
        {
            // 表示该�?�求在dram�?执�??
            fprintf(ssd->outputfile,"%16lld %10d %6d %2d %16lld %16lld %10lld\n",req->time,req->lsn, req->size, req->operation, req->begin_time, req->response_time, req->response_time-req->time);
            fflush(ssd->outputfile);

            if(req->response_time-req->begin_time==0)
            {
                printf("the response time is 0?? \n");
                getchar();
            }
            // unsigned long long tail = req->response_time-req->begin_time;
            // if(tail > ssd->tail_latency){
            //     ssd->tail_latency = tail;
            // }
            // latency[latency_index] = tail;
            // latency_index++;
            ssd->sub_total_time+=(req->response_time - req->begin_time);
            if (req->operation==READ)
            {
                ssd->read_request_count++;
                // ssd->read_avg=ssd->read_avg+(req->response_time-req->time);
            } 
            else
            {
                ssd->write_request_count++;
                // ssd->write_avg=ssd->write_avg+(req->response_time-req->time);
                // if(ssd->write_avg > 50000000){
                //     printf("here write request time is too long\n");
                // }
            }
            if(req->response_time - req->begin_time != 1000){
                printf("response time is error %u\n",req->response_time - req->begin_time);
            }
            // 请求的前一�?节点�?能不为空
            if(pre_node == NULL)
            {
                if(req->next_node == NULL)
                {
                    free(req->need_distr_flag);
                    req->need_distr_flag=NULL;
                    free(req);
                    req = NULL;
                    ssd->request_queue = NULL;
                    ssd->request_tail = NULL;
                    ssd->request_queue_length--;
                }
                else
                {
                    ssd->request_queue = req->next_node;
                    pre_node = req;
                    req = req->next_node;
                    free(pre_node->need_distr_flag);
                    pre_node->need_distr_flag=NULL;
                    free((void *)pre_node);
                    pre_node = NULL;
                    ssd->request_queue_length--;
                }
            }
            else
            {
                if(req->next_node == NULL)
                {
                    pre_node->next_node = NULL;
                    free(req->need_distr_flag);
                    req->need_distr_flag=NULL;
                    free(req);
                    req = NULL;
                    ssd->request_tail = pre_node;
                    ssd->request_queue_length--;
                }
                else
                {
                    pre_node->next_node = req->next_node;
                    free(req->need_distr_flag);
                    req->need_distr_flag=NULL;
                    free((void *)req);
                    req = pre_node->next_node;
                    ssd->request_queue_length--;
                }
            }
        }
        else
        {
            // 说明执�?�了flash操作
            flag=1;
            unsigned long long pre_time = 0;
            unsigned long long old_sub = 0;
            unsigned long long s_time = MAX_INT64;
            unsigned long long b_time = 0;
            while(sub != NULL)
            { 
                if(start_time == 0)
                    start_time = sub->begin_time;
                if(start_time > sub->begin_time)
                    start_time = sub->begin_time;
                if(end_time < sub->complete_time)
                    end_time = sub->complete_time;
                if((sub->current_state == SR_COMPLETE)||((sub->next_state==SR_COMPLETE)&&(sub->next_state_predict_time<=ssd->current_time)))	// if any sub-request is not completed, the request is not completed
                { 
                //    if(pre_time != 0){
                //         unsigned long long time_gap = 0;
                //         if(sub->begin_time > pre_time){
                //             time_gap = sub->begin_time - pre_time;
                //         }else{
                //             time_gap = pre_time - sub->begin_time;
                //         }
                //         if(req->operation == 1 && time_gap > 100000000)
                //             printf("here sub begin time gap is too big %llu\n",time_gap);
                //    }
                //     pre_time = sub->begin_time;
                    old_sub = sub->lpn;
                    sub = sub->next_subs;
                }
                else
                {
                    flag=0;
                    break;
                }

            }

            if (flag == 1)
            {		
                // 表示大请求中的所有子请求都完成了操作
                //fprintf(ssd->outputfile,"%10I64u %10u %6u %2u %16I64u %16I64u %10I64u\n",req->time,req->lsn, req->size, req->operation, start_time, end_time, end_time-req->time);
                fprintf(ssd->outputfile,"%16lld %10d %6d %2d %16lld %16lld %10lld\n",req->time,req->lsn, req->size, req->operation, start_time, end_time, end_time-req->time);
                fflush(ssd->outputfile);

                if(end_time-start_time==0)
                {
                    printf("the response time is 0?? \n");
                    getchar();
                }
                if(end_time==0){
                    printf("here end_time is 0\n");
                }
                if(end_time < req->time){
                    printf("error\n");
                }
                unsigned long long sub_time = end_time - start_time;
                // if(req->operation ==1 && end_time - req->time > 10000000000){
                //     printf("time is %llu\n",end_time - req->time);
                //     printf("time is %llu\n",end_time - start_time);
                //     printf("gap is %llu\n",start_time - req->time);
                //     unsigned long long time_test = ssd->first_time + ssd->sub_total_time;
                //     if(time_test < req->time){
                //         printf("error %llu\n",req->time - time_test);
                //     }else{
                //         printf("real gap is %llu\n",time_test - req->time);
                //     }
                //     printf("time from begin to here %llu\n",req->time - ssd->first_time);
                //     printf("time is too long\n");
                // }
                unsigned long long req_time = end_time - req->time;
                unsigned long long gap_time = start_time - req->time;
                unsigned long long real_gap = ssd->sub_total_time - req->time;
                unsigned long long from_time = req->time - ssd->first_time;
                
                int success_flag = 0;
                // if(start_time != req->time){
                //     // printf("gap is %llu\n",gap_time);
                //     for(int i = 0;i < ssd->parameter->channel_number;i++){
                //         struct req_list* req_now = ssd->channel_head[i].req;
                //         while(req_now != NULL){
                //             if(req_now->time <= start_time && (req_now->time + req_now->here2next) >= req->time){
                //                 // printf("channel %d req id %u req time %llu\n",i,req_now->id,req_now->time + req_now->here2next - req->time);
                //                 // printf("here2next is %llu\n",req_now->here2next);
                //                 flag = 1;
                //             }
                //             req_now = req_now->next; 
                //         }
                //         for(int j = 0;j < ssd->parameter->chip_channel[0];j++){
                //             req_now = ssd->channel_head[i].chip_head[j].req;
                //             while(req_now != NULL){
                //                 if(req_now->time <= start_time && (req_now->time + req_now->here2next) >= req->time){
                //                     // printf("chip %d req id %u req time %llu\n",i*2+j,req_now->id,req_now->time + req_now->here2next - req->time);
                //                     // printf("here2next is %llu\n",req_now->here2next);
                //                     flag = 1;
                //                 }
                //                 req_now = req_now->next; 
                //             }
                //         }
                //     }
                //     if(flag == 0){
                //         printf("error flag gap is %llu\n",gap_time);
                //     }
                // }
                ssd->sub_total_time = end_time;
                
                if (req->operation==READ)
                {
                    ssd->read_request_count++;
                    ssd->read_avg=ssd->read_avg+(end_time-req->time);
                    // if(end_time - req->time > 146000000000){
                    //     ssd->error_times++;
                    //     // printf("time is %u",ssd->current_time - req->time);
                    // }
                } 
                else
                {
                    ssd->write_request_count++;
                    ssd->write_avg=ssd->write_avg+(end_time-req->time);
                    // if(ssd->write_avg > 50000000){
                    //     printf("here write request time is too long\n");
                    // }
                }
                // 在这里计算尾延时间
                unsigned long long tail = end_time - req->time;
                if(tail > ssd->tail_latency){
                    ssd->tail_latency = tail;
                }
                latency[latency_index] = tail;
                latency_index++;

                // for(int i = 0;i < ssd->parameter->channel_number;i++){
                //     struct req_list* req_pre = ssd->channel_head[i].req;
                //     struct req_list* req_now = ssd->channel_head[i].req;
                //     while(req_now != NULL){
                //         if(req_now->id == req->id){
                //             if(req_pre == ssd->channel_head[i].req){
                //                 ssd->channel_head[i].req = req_now->next;
                //             }else{
                //                 req_pre->next = req_now->next;
                //             }
                //             free(req_now);
                //             req_now = NULL;
                //         }else{
                //             req_pre = req_now;
                //         }
                //         req_now = req_pre->next; 
                //     }
                //     for(int j = 0;j < ssd->parameter->chip_channel[0];j++){
                //         req_pre = ssd->channel_head[i].chip_head[j].req;
                //         req_now = ssd->channel_head[i].chip_head[j].req;
                //         while(req_now != NULL){
                //             if(req_now->id == req->id){
                //                 if(req_pre == ssd->channel_head[i].chip_head[j].req){
                //                     ssd->channel_head[i].chip_head[j].req = req_now->next;
                //                 }else{
                //                     req_pre->next = req_now->next;
                //                 }
                //                 req_pre->next = req_now->next;
                //                 free(req_now);
                //                 req_now = NULL;
                //             }else{
                //                 req_pre = req_now;
                //             }
                //             req_now = req_pre->next; 
                //         }
                //     }
                // }

                while(req->subs!=NULL)
                {
                    tmp = req->subs;
                    req->subs = tmp->next_subs;
                    if (tmp->update!=NULL)
                    {
                        free(tmp->update->location);
                        //node change here
                        tmp->update->location=NULL;
                        free(tmp->update);
                        // if(tmp->update->lpn == 1583729184){
                        //     printf("here lpn is 1583729184\n");
                        // }
                        tmp->update=NULL;
                    }
                    free(tmp->location);
                    tmp->location=NULL;
                    free(tmp);
                    tmp=NULL;

                }

                if(pre_node == NULL)
                {
                    if(req->next_node == NULL)
                    {
                        free(req->need_distr_flag);
                        req->need_distr_flag=NULL;
                        free(req);
                        req = NULL;
                        ssd->request_queue = NULL;
                        ssd->request_tail = NULL;
                        ssd->request_queue_length--;
                    }
                    else
                    {
                        ssd->request_queue = req->next_node;
                        pre_node = req;
                        req = req->next_node;
                        free(pre_node->need_distr_flag);
                        pre_node->need_distr_flag=NULL;
                        free(pre_node);
                        pre_node = NULL;
                        ssd->request_queue_length--;
                    }
                }
                else
                {
                    if(req->next_node == NULL)
                    {
                        pre_node->next_node = NULL;
                        free(req->need_distr_flag);
                        req->need_distr_flag=NULL;
                        free(req);
                        req = NULL;
                        ssd->request_tail = pre_node;	
                        ssd->request_queue_length--;
                    }
                    else
                    {
                        pre_node->next_node = req->next_node;
                        free(req->need_distr_flag);
                        req->need_distr_flag=NULL;
                        free(req);
                        req = pre_node->next_node;
                        ssd->request_queue_length--;
                    }

                }
                
            }
            else
            {	
                pre_node = req;
                req = req->next_node;
            }
        }		
    }
}

int compareULL(const void *a, const void *b) {
    unsigned long long val_a = *(unsigned long long *)a;
    unsigned long long val_b = *(unsigned long long *)b;
    if (val_a < val_b) return -1;
    else if (val_a > val_b) return 1;
    else return 0;
}
/*******************************************************************************
 *statistic_output()函数主�?�是输出处理完一条�?�求后的相关处理信息�?
 *1，�?�算出每个plane的擦除�?�数即plane_erase和总的擦除次数即erase
 *2，打印min_lsn，max_lsn，read_count，program_count等统计信�?到文件outputfile�?�?
 *3，打印相同的信息到文件statisticfile�?
 *******************************************************************************/
void statistic_output(struct ssd_info *ssd)
{
    unsigned int lpn_count=0,i,j,k,m,erase=0,plane_erase=0;
    double gc_energy=0.0;
#ifdef DEBUG
    printf("enter statistic_output,  current time:%lld\n",ssd->current_time);
#endif

    for(i=0;i<ssd->parameter->channel_number;i++)
    {
        for(j=0;j<ssd->parameter->die_chip;j++)
        {
            for(k=0;k<ssd->parameter->plane_die;k++)
            {
                plane_erase=0;
                for(m=0;m<ssd->parameter->block_plane;m++)
                {
                    if(ssd->channel_head[i].chip_head[0].die_head[j].plane_head[k].blk_head[m].erase_count>0)
                    {
                        erase=erase+ssd->channel_head[i].chip_head[0].die_head[j].plane_head[k].blk_head[m].erase_count;
                        plane_erase+=ssd->channel_head[i].chip_head[0].die_head[j].plane_head[k].blk_head[m].erase_count;
                    }else if(ssd->channel_head[i].chip_head[1].die_head[j].plane_head[k].blk_head[m].erase_count>0){
                        erase=erase+ssd->channel_head[i].chip_head[0].die_head[j].plane_head[k].blk_head[m].erase_count;
                        plane_erase+=ssd->channel_head[i].chip_head[0].die_head[j].plane_head[k].blk_head[m].erase_count;
                    }
                }
                fprintf(ssd->outputfile,"the %d channel, %d die, %d plane, %d block has : %13d erase operations\n",i,j,k,m,plane_erase);
                fprintf(ssd->statisticfile,"the %d channel, %d die, %d plane, %d blocks has : %13d erase operations\n",i,j,k,m,plane_erase);
            }
        }
    }
    int size = 10;
    int latency_array[size];
    int tail_latency_array[1000];
    for(int i = 0;i < 1000;i ++){
        tail_latency_array[i] = 0;
    }
    for(int i = 0;i < size; i++){
        latency_array[i] = 0;
    }
    unsigned long long range = ssd->tail_latency / size;
    for(int i = 0;i <= latency_index;i++){
        // printf("range is %d\n",range);
        int j = latency[i] / range;
        // latency[i] = 0;
        latency_array[j]++;
    }
    qsort(latency,latency_index+1,sizeof(unsigned long long),compareULL);
    // int k_index = latency_index / 10 * 9;
    int k_index = 0;
    // range = (ssd->tail_latency - latency[k_index])/100;
    range = 50000000000;
    // range = 100000000;
    printf("range is %llu\n",range);
    unsigned long long min = __UINT_LEAST64_MAX__;
    for(int i = k_index;i <= latency_index;i++){
        int j = latency[i]/range + (latency[i]%range)/100000000;
        // int j = latency[i]/range;
        if(latency[i] < min){
            min = latency[i];
        }
        if(j >= 1000){
            tail_latency_array[999]++;
        }else{
            tail_latency_array[j]++;
        }
    }
    fprintf(ssd->outputfile,"---------------------------latency array distribute---------------------------\n");
    fprintf(ssd->outputfile,"total latency num is %d\n",latency_index);
    int total = 0;
    for(int i = 0;i < size; i++){
        fprintf(ssd->outputfile,"rang %d request num is %d\n",i,latency_array[i]);
        total += latency_array[i];
        if(i != 0){
            latency_array[i] += latency_array[i - 1];
        }
    }
    for(int i = 0;i < 1000;i ++){
        if(i != 0){
            tail_latency_array[i] += tail_latency_array[i - 1];
        }
    }
    if(total == latency_index){
        printf("It's right!\n");
    }else{
        printf("total is %d and latency index is %d\n",total,latency_index);
    }
    
    fprintf(ssd->outputfile,"\n");
    fprintf(ssd->outputfile,"\n");
    fprintf(ssd->outputfile,"---------------------------static data---------------------------\n");	 
    fprintf(ssd->outputfile,"min lsn: %13d\n",ssd->min_lsn);	
    fprintf(ssd->outputfile,"max lsn: %lld\n",ssd->max_lsn);
    fprintf(ssd->outputfile,"read count: %13d\n",ssd->read_count);	  	  
    fprintf(ssd->outputfile,"program count: %13d\n",ssd->program_count);
    fprintf(ssd->outputfile,"---------------------------read & program sub rate---------------------------\n");
    fprintf(ssd->outputfile,"total program sub: %13d\n",ssd->total_write);	
    fprintf(ssd->outputfile,"total read sub: %13d\n",ssd->total_read);
    fprintf(ssd->outputfile,"read rate: %f\n",(float)ssd->total_read/(ssd->total_read + ssd->total_write));
    fprintf(ssd->outputfile,"program rate: %f\n",(float)ssd->total_write/(ssd->total_read + ssd->total_write));
    // fprintf(ssd->outputfile,"                        include the flash write count leaded by read requests\n");
    // fprintf(ssd->outputfile,"the read operation leaded by un-covered update count: %13d\n",ssd->update_read_count);
    // fprintf(ssd->outputfile,"erase count: %13d\n",ssd->erase_count);
    // fprintf(ssd->outputfile,"direct erase count: %13d\n",ssd->direct_erase_count);
    // fprintf(ssd->outputfile,"copy back count: %13d\n",ssd->copy_back_count);
    // fprintf(ssd->outputfile,"multi-plane program count: %13d\n",ssd->m_plane_prog_count);
    // fprintf(ssd->outputfile,"multi-plane read count: %13d\n",ssd->m_plane_read_count);
    // fprintf(ssd->outputfile,"interleave write count: %13d\n",ssd->interleave_count);
    // fprintf(ssd->outputfile,"interleave read count: %13d\n",ssd->interleave_read_count);
    // fprintf(ssd->outputfile,"interleave two plane and one program count: %13d\n",ssd->inter_mplane_prog_count);
    // fprintf(ssd->outputfile,"interleave two plane count: %13d\n",ssd->inter_mplane_count);
    // fprintf(ssd->outputfile,"gc copy back count: %13d\n",ssd->gc_copy_back);
    // fprintf(ssd->outputfile,"write flash count: %13d\n",ssd->write_flash_count);
    // fprintf(ssd->outputfile,"interleave erase count: %13d\n",ssd->interleave_erase_count);
    // fprintf(ssd->outputfile,"multiple plane erase count: %13d\n",ssd->mplane_erase_conut);
    // fprintf(ssd->outputfile,"interleave multiple plane erase count: %13d\n",ssd->interleave_mplane_erase_count);
    fprintf(ssd->outputfile,"---------------------------read & program request rate---------------------------\n");
    fprintf(ssd->outputfile,"read request count: %13d\n",ssd->read_request_count);
    fprintf(ssd->outputfile,"write request count: %13d\n",ssd->write_request_count);
    fprintf(ssd->outputfile,"read request rate: %f\n",(float)ssd->read_request_count/(ssd->read_request_count + ssd->write_request_count));
    fprintf(ssd->outputfile,"write request rate: %f\n",(float)ssd->write_request_count/(ssd->write_request_count + ssd->read_request_count));
    fprintf(ssd->outputfile,"---------------------------read & program & total & tail latency---------------------------\n");
    fprintf(ssd->outputfile,"read request average size: %13f\n",ssd->ave_read_size);
    fprintf(ssd->outputfile,"write request average size: %13f\n",ssd->ave_write_size);
    fprintf(ssd->outputfile,"read request total response time: %llu\n",ssd->read_avg);
    fprintf(ssd->outputfile,"write request total response time: %llu\n",ssd->write_avg);
    fprintf(ssd->outputfile,"read request average response time: %llu\n",ssd->read_avg/ssd->read_request_count);
    fprintf(ssd->outputfile,"write request average response time: %llu\n",ssd->write_avg/ssd->real_written);
    fprintf(ssd->outputfile,"sub read request average response time: %llu\n",ssd->read_avg/ssd->total_read);
    fprintf(ssd->outputfile,"sub write request average response time: %llu\n",ssd->write_avg/ssd->write_request_count);
    fprintf(ssd->outputfile,"total request average response time: %lld\n",(ssd->write_avg + ssd->read_avg)/(ssd->write_request_count + ssd->read_request_count));
    fprintf(ssd->outputfile,"tail latency: %llu\n",ssd->tail_latency);
    fprintf(ssd->outputfile,"---------------------------Space utilization rate---------------------------\n");
    // TODO:�?前这里还没有空白页无�?
    fprintf(ssd->outputfile,"Space utilization rate: %f\n",(float)ssd->real_written/(ssd->real_written + ssd->free_invalid));
    fprintf(ssd->outputfile,"---------------------------WA---------------------------\n");
    fprintf(ssd->outputfile,"Write amplification: %f\n",(float)ssd->real_written/ssd->total_write);
    fprintf(ssd->outputfile,"buffer read hits: %13d\n",ssd->dram->buffer->read_hit);
    fprintf(ssd->outputfile,"buffer read miss: %13d\n",ssd->dram->buffer->read_miss_hit);
    fprintf(ssd->outputfile,"buffer write hits: %13d\n",ssd->dram->buffer->write_hit);
    fprintf(ssd->outputfile,"buffer write miss: %13d\n",ssd->dram->buffer->write_miss_hit);
    fprintf(ssd->outputfile,"erase: %13d\n",erase);
    fflush(ssd->outputfile);

    fclose(ssd->outputfile);

    fprintf(ssd->statisticfile,"---------------------------latency array distribute---------------------------\n");
    fprintf(ssd->statisticfile,"total latency num is %d\n",latency_index);
    for(int i = 0;i < size; i++){
        fprintf(ssd->statisticfile,"%f\n",i,(float)latency_array[i]/latency_index);
    }
    fprintf(ssd->statisticfile,"---------------------------tail latency---------------------------\n");	
    fprintf(ssd->statisticfile,"start is %llu rang is %llu\n",latency[k_index],range);
    for(int i = 0;i < 1000; i++){
        fprintf(ssd->statisticfile,"%f\n",(float)tail_latency_array[i]/(latency_index-k_index + 1));
    }
    fprintf(ssd->statisticfile,"\n");
    fprintf(ssd->statisticfile,"\n");
    fprintf(ssd->statisticfile,"---------------------------static data---------------------------\n");	
    fprintf(ssd->statisticfile,"min lsn: %13d\n",ssd->min_lsn);	
    fprintf(ssd->statisticfile,"max lsn: %lld\n",ssd->max_lsn);
    fprintf(ssd->statisticfile,"read count: %13d\n",ssd->read_count);	  
    fprintf(ssd->statisticfile,"program count: %13d\n",ssd->program_count);	
    fprintf(ssd->statisticfile,"---------------------------read & program sub rate---------------------------\n");
    fprintf(ssd->statisticfile,"total program sub: %13d\n",ssd->total_write);	
    fprintf(ssd->statisticfile,"total read sub: %13d\n",ssd->total_read);	
    fprintf(ssd->statisticfile,"read rate: %f\n",(float)(ssd->total_read)/(ssd->total_read + ssd->total_write));
    fprintf(ssd->statisticfile,"program rate: %f\n",(float)ssd->total_write/(ssd->total_read + ssd->total_write));  
    // fprintf(ssd->statisticfile,"                        include the flash write count leaded by read requests\n");
    // fprintf(ssd->statisticfile,"the read operation leaded by un-covered update count: %13d\n",ssd->update_read_count);
    // fprintf(ssd->statisticfile,"erase count: %13d\n",ssd->erase_count);	  
    // fprintf(ssd->statisticfile,"direct erase count: %13d\n",ssd->direct_erase_count);
    // fprintf(ssd->statisticfile,"copy back count: %13d\n",ssd->copy_back_count);
    // fprintf(ssd->statisticfile,"multi-plane program count: %13d\n",ssd->m_plane_prog_count);
    // fprintf(ssd->statisticfile,"multi-plane read count: %13d\n",ssd->m_plane_read_count);
    // fprintf(ssd->statisticfile,"interleave count: %13d\n",ssd->interleave_count);
    // fprintf(ssd->statisticfile,"interleave read count: %13d\n",ssd->interleave_read_count);
    // fprintf(ssd->statisticfile,"interleave two plane and one program count: %13d\n",ssd->inter_mplane_prog_count);
    // fprintf(ssd->statisticfile,"interleave two plane count: %13d\n",ssd->inter_mplane_count);
    // fprintf(ssd->statisticfile,"gc copy back count: %13d\n",ssd->gc_copy_back);
    // fprintf(ssd->statisticfile,"write flash count: %13d\n",ssd->write_flash_count);
    // fprintf(ssd->statisticfile,"waste page count: %13d\n",ssd->waste_page_count);
    // fprintf(ssd->statisticfile,"interleave erase count: %13d\n",ssd->interleave_erase_count);
    // fprintf(ssd->statisticfile,"multiple plane erase count: %13d\n",ssd->mplane_erase_conut);
    // fprintf(ssd->statisticfile,"interleave multiple plane erase count: %13d\n",ssd->interleave_mplane_erase_count);
    fprintf(ssd->statisticfile,"---------------------------read & program request rate---------------------------\n");
    fprintf(ssd->statisticfile,"read request count: %13d\n",ssd->read_request_count);
    fprintf(ssd->statisticfile,"write request count: %13d\n",ssd->write_request_count);
    fprintf(ssd->statisticfile,"read request rate: %f\n",(float)ssd->read_request_count/(ssd->read_request_count + ssd->write_request_count));
    fprintf(ssd->statisticfile,"write request rate: %f\n",(float)ssd->write_request_count/(ssd->write_request_count + ssd->read_request_count));
    fprintf(ssd->statisticfile,"read request average size: %13f\n",ssd->ave_read_size);
    fprintf(ssd->statisticfile,"write request average size: %13f\n",ssd->ave_write_size);
    fprintf(ssd->statisticfile,"---------------------------read & program & total & tail latency---------------------------\n");
    fprintf(ssd->statisticfile,"read request average response time: %llu\n",ssd->read_avg/ssd->read_request_count);
    fprintf(ssd->statisticfile,"write request average response time: %llu\n",ssd->write_avg/ssd->write_request_count);
    fprintf(ssd->statisticfile,"read request total response time: %llu\n",ssd->read_avg);
    fprintf(ssd->statisticfile,"write request total response time: %llu\n",ssd->write_avg);
    // fprintf(ssd->statisticfile,"sub write request average response time: %llu\n",ssd->write_avg/ssd->real_written);
    // fprintf(ssd->statisticfile,"sub read request average response time: %llu\n",ssd->read_avg/ssd->total_read);
    fprintf(ssd->statisticfile,"total request average response time: %llu\n",(ssd->write_avg + ssd->read_avg)/(ssd->write_request_count + ssd->read_request_count));
    fprintf(ssd->statisticfile,"tail latency: %llu\n",ssd->tail_latency);
    fprintf(ssd->statisticfile,"---------------------------Space utilization rate---------------------------\n");
    fprintf(ssd->statisticfile,"free invalid pagenums: %d\n",ssd->free_invalid);
    fprintf(ssd->statisticfile,"---------------------------WA---------------------------\n");
    fprintf(ssd->statisticfile,"Write amplification: %f\n",(float)((ssd->real_written + ssd->free_invalid + avlTreeCount(ssd->dram->buffer))*ssd->parameter->subpage_page )/ssd->total_write);
    fprintf(ssd->statisticfile,"update write: %13d\n",ssd->update_write);
    fprintf(ssd->statisticfile,"buffer read hits: %13d\n",ssd->dram->buffer->read_hit);
    fprintf(ssd->statisticfile,"buffer read miss: %13d\n",ssd->dram->buffer->read_miss_hit);
    fprintf(ssd->statisticfile,"buffer write hits: %13d\n",ssd->dram->buffer->write_hit);
    fprintf(ssd->statisticfile,"buffer write miss: %13d\n",ssd->dram->buffer->write_miss_hit);
    fprintf(ssd->statisticfile,"erase: %u\n",ssd->erase_count);
    fprintf(ssd->statisticfile,"direct erase: %u\n",ssd->direct_erase_count);
    fprintf(ssd->statisticfile,"rewrite page num: %13d\n",ssd->gc_rewrite);
    unsigned int page_num = ssd->parameter->page_block*ssd->parameter->block_plane*ssd->parameter->plane_die*ssd->parameter->die_chip*ssd->parameter->chip_num * BITS_PER_CELL;
    unsigned int read_array[10],prog_array[10];
    unsigned int totalnum =0;
    for(int i = 0;i < 10;i ++){
        read_array[i] = 0;
        prog_array[i] = 0;
    }
    // 已修改，统计整个ssd
    for(int i = 0;i < page_num;i ++){
        totalnum++;
        int read_count = ssd->dram->map->map_entry[i].read_count;
        int prog_count = ssd->dram->map->map_entry[i].write_count;
        if(read_count < 10){
            read_array[read_count]++;
        }else{
            read_array[9]++;
        }
        if(prog_count < 10){
            prog_array[prog_count]++;
        }else{
        prog_array[9]++;
        }
    }
    for(int i = 0;i < 10; i++){
       fprintf(ssd->statisticfile,"Read %d: count is %f\n",i,(float)read_array[i]/totalnum);
    }
    for(int i = 0;i < 10; i++){
       fprintf(ssd->statisticfile,"Write %d: count is %f\n",i,(float)prog_array[i]/totalnum);
    }
    fprintf(ssd->statisticfile,"READ\n");
    for(int i = 0;i < 10; i++){
       fprintf(ssd->statisticfile,"%u\n",read_array[i]);
    }
    fprintf(ssd->statisticfile,"WRITE\n");
    for(int i = 0;i < 10; i++){
       fprintf(ssd->statisticfile,"%u\n",prog_array[i]);
    }
    fflush(ssd->statisticfile);

    fclose(ssd->statisticfile);
}


/***********************************************************************************
 *根据每一页的状态�?�算出每一需要�?�理的子页的数目，也就是一�?子�?�求需要�?�理的子页的页数
 ************************************************************************************/
unsigned int size(unsigned int stored)
{
    unsigned int i,total=0,mask=0x80000000;

#ifdef DEBUG
    printf("enter size\n");
#endif
    for(i=1;i<=32;i++)
    {
        if(stored & mask) total++;
        stored<<=1;
    }
#ifdef DEBUG
    printf("leave size\n");
#endif
    return total;
}


/*********************************************************
 *transfer_size()函数的作用就�?计算出子请求的需要�?�理的size
 *函数�?单独处理了first_lpn，last_lpn这两�?特别情况，因为这
 *两�?�情况下很有�?能不�?处理一整页而是处理一页的一部分，因
 *为lsn有可能不�?一页的�?一�?子页�?
 *********************************************************/
unsigned int transfer_size(struct ssd_info *ssd,int need_distribute,unsigned int lpn,struct request *req)
{
    unsigned int first_lpn,last_lpn,state,trans_size;
    unsigned int mask=0,offset1=0,offset2=0;

    first_lpn=req->lsn/ssd->parameter->subpage_page;
    last_lpn=(req->lsn+req->size-1)/ssd->parameter->subpage_page;

    mask=~(0xffffffff<<(ssd->parameter->subpage_page));
    state=mask;
    if(lpn==first_lpn)
    {
        offset1=ssd->parameter->subpage_page-((lpn+1)*ssd->parameter->subpage_page-req->lsn);
        state=state&(0xffffffff<<offset1);
    }
    if(lpn==last_lpn)
    {
        offset2=ssd->parameter->subpage_page-((lpn+1)*ssd->parameter->subpage_page-(req->lsn+req->size));
        state=state&(~(0xffffffff<<offset2));
    }

    trans_size=size(state&need_distribute);

    return trans_size;
}


/**********************************************************************************************************  
 *int64_t find_nearest_event(struct ssd_info *ssd)       
 *寻找所有子请求的最早到达的下个状态时间,首先看请求的下一个状态时间，如果请求的下个状态时间小于等于当前时间，
 *说明请求被阻塞，需要查看channel或者对应die的下一状态时间。Int64是有符号 64 位整数数据类型，值类型表示值介于
 *-2^63 ( -9,223,372,036,854,775,808)到2^63-1(+9,223,372,036,854,775,807 )之间的整数。存储空间占 8 字节。
 *channel,die是事件向前推进的关键因素，三种情况可以使事件继续向前推进，channel，die分别回到idle状态，die中的
 *读数据准备好了
 ***********************************************************************************************************/
// int64_t find_nearest_event(struct ssd_info *ssd) 
// {
//     unsigned int i,j;
//     int64_t time=MAX_INT64;
//     int64_t time1=MAX_INT64;
//     int64_t time2=MAX_INT64;
//     int channel1 = NONE,chip = NONE,channel2 = NONE;
//     for (i=0;i<ssd->parameter->channel_number;i++)
//     {
//         if(time1>ssd->channel_head[i].next_state_predict_time)
//             // if (ssd->channel_head[i].next_state_predict_time>ssd->current_time)    
//             if(ssd->channel_head[i].update_time == 1){
//                 time1=ssd->channel_head[i].next_state_predict_time;
//                 channel1 = i;
//             }
//         for (j=0;j<ssd->parameter->chip_channel[i];j++)
//         {
//             // if ((ssd->channel_head[i].chip_head[j].next_state==CHIP_IDLE)||(ssd->channel_head[i].chip_head[j].next_state==CHIP_DATA_TRANSFER))
//             if(time2>ssd->channel_head[i].chip_head[j].next_state_predict_time)
//                 // if (ssd->channel_head[i].chip_head[j].next_state_predict_time>ssd->current_time)    
//                 if(ssd->channel_head[i].chip_head[j].update_time == 1){
//                     time2=ssd->channel_head[i].chip_head[j].next_state_predict_time;
//                     channel2 = i;
//                     chip = j;
//                 }
                	
//         }   
//     } 

//      /*****************************************************************************************************
//      *time为所有 A.下一状态为CHANNEL_IDLE且下一状态预计时间大于ssd当前时间的CHANNEL的下一状态预计时间
//      *           B.下一状态为CHIP_IDLE且下一状态预计时间大于ssd当前时间的DIE的下一状态预计时间
//      *		     C.下一状态为CHIP_DATA_TRANSFER且下一状态预计时间大于ssd当前时间的DIE的下一状态预计时间
//      *CHIP_DATA_TRANSFER读准备好状态，数据已从介质传到了register，下一状态是从register传往buffer中的最小值 
//      *注意可能都没有满足要求的time，这时time返回0x7fffffffffffffff 。
//      *****************************************************************************************************/
//     time=(time1>time2)?time2:time1;
//     if(time != MAX_INT64){
//         if(time == time1){
//             ssd->channel_head[channel1].update_time = 0;
//             channel_busy[channel1] = 0;
//             if(time1 == time2){
//                 ssd->channel_head[channel2].chip_head[chip].update_time = 0;
//                 int index = channel2*2+chip;
//                 chip_busy[index] = 0;
//             }
//         }else if(time == time2){
//             ssd->channel_head[channel2].chip_head[chip].update_time = 0;
//             int index = channel2*2+chip;
//             chip_busy[index] = 0;
//             if(time1 == time2){
//                 ssd->channel_head[channel1].update_time = 0;
//                 channel_busy[channel1] = 0;
//             }
//         }
//     }
    
//     return time;
// }

int64_t find_nearest_event(struct ssd_info *ssd) 
{
    unsigned int i,j;
    int64_t time=MAX_INT64;
    int64_t time1=MAX_INT64;
    int64_t time2=MAX_INT64;

    for (i=0;i<ssd->parameter->channel_number;i++)
    {
        if (ssd->channel_head[i].next_state==CHANNEL_IDLE)
            if(time1>ssd->channel_head[i].next_state_predict_time)
                if (ssd->channel_head[i].next_state_predict_time>ssd->current_time)    
                    time1=ssd->channel_head[i].next_state_predict_time;
        for (j=0;j<ssd->parameter->chip_channel[i];j++)
        {
            if ((ssd->channel_head[i].chip_head[j].next_state==CHIP_IDLE)||(ssd->channel_head[i].chip_head[j].next_state==CHIP_DATA_TRANSFER))
                if(time2>ssd->channel_head[i].chip_head[j].next_state_predict_time)
                    if (ssd->channel_head[i].chip_head[j].next_state_predict_time>ssd->current_time)    
                        time2=ssd->channel_head[i].chip_head[j].next_state_predict_time;	
        }   
    } 

    /*****************************************************************************************************
     *time为所�? A.下一状态为CHANNEL_IDLE且下一状态�?��?�时间大于ssd当前时间的CHANNEL的下一状态�?��?�时�?
     *           B.下一状态为CHIP_IDLE且下一状态�?��?�时间大于ssd当前时间的DIE的下一状态�?��?�时�?
     *		     C.下一状态为CHIP_DATA_TRANSFER且下一状态�?��?�时间大于ssd当前时间的DIE的下一状态�?��?�时�?
     *CHIP_DATA_TRANSFER读准备好状态，数据已从介质传到了register，下一状态是从register传往buffer�?的最小�? 
     *注意�?能都没有满足要求的time，这时time返回0x7fffffffffffffff �?
     *****************************************************************************************************/
    time=(time1>time2)?time2:time1;
    return time;
}


/***********************************************
 *free_all_node()函数的作用就�?释放所有申请的节点
 ************************************************/
void free_all_node(struct ssd_info *ssd)
{
    unsigned int i,j,k,l,n;
    struct buffer_group *pt=NULL;
    struct direct_erase * erase_node=NULL;
    for (i=0;i<ssd->parameter->channel_number;i++)
    {
        for (j=0;j<ssd->parameter->chip_channel[0];j++)
        {
            for (k=0;k<ssd->parameter->die_chip;k++)
            {
                for (l=0;l<ssd->parameter->plane_die;l++)
                {
                    for (n=0;n<ssd->parameter->block_plane;n++)
                    {
                        free(ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[n].page_head);
                        ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[n].page_head=NULL;
                    }
                    free(ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head);
                    ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head=NULL;
                    while(ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].erase_node!=NULL)
                    {
                        erase_node=ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].erase_node;
                        ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].erase_node=erase_node->next_node;
                        free(erase_node);
                        erase_node=NULL;
                    }
                }

                free(ssd->channel_head[i].chip_head[j].die_head[k].plane_head);
                ssd->channel_head[i].chip_head[j].die_head[k].plane_head=NULL;
            }
            free(ssd->channel_head[i].chip_head[j].die_head);
            ssd->channel_head[i].chip_head[j].die_head=NULL;
        }
        free(ssd->channel_head[i].chip_head);
        ssd->channel_head[i].chip_head=NULL;
    }
    free(ssd->channel_head);
    ssd->channel_head=NULL;

    avlTreeDestroy( ssd->dram->buffer);
    ssd->dram->buffer=NULL;

    free(ssd->dram->map->map_entry);
    ssd->dram->map->map_entry=NULL;
    free(ssd->dram->map);
    ssd->dram->map=NULL;
    free(ssd->dram);
    ssd->dram=NULL;
    free(ssd->parameter);
    ssd->parameter=NULL;

    free(ssd);
    ssd=NULL;
}


/*****************************************************************************
 *make_aged()函数的作用就死模拟真实的用过一段时间的ssd�?
 *那么这个ssd的相应的参数就�?�改变，所以这�?函数实质上就�?对ssd�?各个参数的赋值�?
 ******************************************************************************/
struct ssd_info *make_aged(struct ssd_info *ssd)
{
    unsigned int i,j,k,l,m,n,ppn;
    int threshould,flag=0;

    if (ssd->parameter->aged==1)
    {
        //threshold表示一个plane�?有�?�少页需要提前置为失�?
        threshould=(int)(ssd->parameter->block_plane*ssd->parameter->page_block*ssd->parameter->aged_ratio);  
        for (i=0;i<ssd->parameter->channel_number;i++)
            for (j=0;j<ssd->parameter->chip_channel[i];j++)
                for (k=0;k<ssd->parameter->die_chip;k++)
                    for (l=0;l<ssd->parameter->plane_die;l++)
                    {  
                        flag=0;
                        for (m=0;m<ssd->parameter->block_plane;m++)
                        {  
                            if (flag>=threshould)
                            {
                                break;
                            }
                            for (n=0;n<(ssd->parameter->page_block*ssd->parameter->aged_ratio+1);n++)
                            {  
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].page_head[n].valid_state=0;        //表示某一页失效，同时标�?�valid和free状态都�?0
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].page_head[n].free_state=0;         //表示某一页失效，同时标�?�valid和free状态都�?0
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].page_head[n].lpn=0;  //把valid_state free_state lpn都置�?0表示页失效，检测的时候三项都检测，单独lpn=0�?以是有效�?
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].free_page_num--;
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].invalid_page_num++;
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].last_write_page++;
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].free_page--;
                                flag++;

                                ppn=find_ppn_new(ssd,i,j,k,l,m,n);

                            }
                        } 
                    }	 
    }  
    else
    {
        return ssd;
    }

    return ssd;
}


/*********************************************************************************************
 *no_buffer_distribute()函数�?处理当ssd没有dram的时候，
 *这是读写请求就不必再需要在buffer里面寻找，直接利用creat_sub_request()函数创建子�?�求，再处理�?
 *********************************************************************************************/
struct ssd_info *no_buffer_distribute(struct ssd_info *ssd)
{
    unsigned int lsn,lpn,last_lpn,first_lpn,complete_flag=0, state;
    unsigned int flag=0,flag1=1,active_region_flag=0;           //to indicate the lsn is hitted or not
    struct request *req=NULL;
    struct sub_request *sub=NULL,*sub_r=NULL,*update=NULL;
    struct local *loc=NULL;
    struct channel_info *p_ch=NULL;


    unsigned int mask=0; 
    unsigned int offset1=0, offset2=0;
    unsigned int sub_size=0;
    unsigned int sub_state=0;

    ssd->dram->current_time=ssd->current_time;
    req=ssd->request_tail;       
    lsn=req->lsn;
    lpn=req->lsn/ssd->parameter->subpage_page;
    if(lpn == 1583729184){
        printf("lpn 1583729184 locaton is NULL\n");
    }
    last_lpn=(req->lsn+req->size-1)/ssd->parameter->subpage_page;
    first_lpn=req->lsn/ssd->parameter->subpage_page;

    if(req->operation==READ)        
    {		
        while(lpn<=last_lpn) 		
        {
            sub_state=(ssd->dram->map->map_entry[lpn].state&0x7fffffff);
            sub_size=size(sub_state);
            sub=creat_sub_request(ssd,lpn,sub_size,sub_state,req,req->operation);
            lpn++;
        }
    }
    else if(req->operation==WRITE)
    {
        while(lpn<=last_lpn)     	
        {	
            mask=~(0xffffffff<<(ssd->parameter->subpage_page));
            state=mask;
            if(lpn==first_lpn)
            {
                offset1=ssd->parameter->subpage_page-((lpn+1)*ssd->parameter->subpage_page-req->lsn);
                state=state&(0xffffffff<<offset1);
            }
            if(lpn==last_lpn)
            {
                offset2=ssd->parameter->subpage_page-((lpn+1)*ssd->parameter->subpage_page-(req->lsn+req->size));
                state=state&(~(0xffffffff<<offset2));
            }
            sub_size=size(state);

            sub=creat_sub_request(ssd,lpn,sub_size,state,req,req->operation);
            lpn++;
        }
    }

    return ssd;
}


