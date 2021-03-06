#include <list>
#include "tagging_decoder.h"
#include "ngram_feature.h"
namespace daidai{



TaggingDecoder::TaggingDecoder(){
    this->separator='_';
    this->max_length=10000;
    this->len=0;
    this->sequence=new int[this->max_length];
    this->allowed_label_lists=new int*[this->max_length];
    pocs_to_tags=NULL;

    ngram_feature=NULL;

    

    dat=NULL; 
    is_old_type_dat=false;
    
    nodes=new permm::Node[this->max_length];
    
    this->label_trans=NULL;
    label_trans_pre=NULL;
    label_trans_post=NULL;
    this->threshold=0;
    
//    this->allow_sep=new int[this->max_length];
    this->allow_com=new int[this->max_length];
    
    this->tag_size=0;
    //this->is_good_choice=NULL;
    
    this->model=NULL;
    
    alphas=NULL;
    betas=NULL;
    
}
TaggingDecoder::~TaggingDecoder(){
    delete[]sequence;
    delete[]allowed_label_lists;
    
    

    
    for(int i=0;i<max_length;i++){
        delete[](nodes[i].predecessors);
        delete[](nodes[i].successors);
    }
    delete[](nodes);
    free(values);
    free(alphas);
    free(betas);
    free(result);
    if(model!=NULL)for(int i=0;i<model->l_size;i++){
        if(label_info)delete[](label_info[i]);
    };
    delete[](label_info);
    
        
    
    free(label_trans);
    if(model!=NULL)for(int i=0;i<model->l_size;i++){
        if(label_trans_pre)free(label_trans_pre[i]);
        if(label_trans_post)free(label_trans_post[i]);
    }
    free(label_trans_pre);
    free(label_trans_post);
    
//    delete[](allow_sep);
    delete[](allow_com);
    
    if(model!=NULL)for(int i=0;i<model->l_size;i++){
        if(label_looking_for)delete[](label_looking_for[i]);
    };
    delete[](label_looking_for);
    delete[](is_good_choice);
    
    if(pocs_to_tags){
        for(int i=1;i<16;i++){
            delete[]pocs_to_tags[i];
        }
    }
    delete[]pocs_to_tags;
    
    if(model!=NULL)delete model;
    delete dat;
}

void TaggingDecoder::init(
        const char* model_file="model.bin",
        const char* dat_file="dat.bin",
        const char* label_file="label.txt",
        char* label_trans
        ){
    /**模型*/
    model=new permm::Model(model_file);
    
    /**解码用*/
    values=(int*)calloc(sizeof(int),max_length*model->l_size);
    alphas=(permm::Alpha_Beta*)calloc(sizeof(permm::Alpha_Beta),max_length*model->l_size);
    betas=(permm::Alpha_Beta*)calloc(sizeof(permm::Alpha_Beta),max_length*model->l_size);
    result=(int*)calloc(sizeof(int),max_length*model->l_size);
    label_info=new char*[model->l_size];
    
    for(int i=0;i<max_length;i++){
        int* pr=new int[2];
        pr[0]=i-1;
        pr[1]=-1;
        nodes[i].predecessors=pr;
        
        pr=new int[2];
        pr[0]=i+1;
        pr[1]=-1;
        nodes[i].successors=pr;
    };
    
    //DAT
    dat=new DAT(dat_file,is_old_type_dat);

    //Ngram Features
    ngram_feature=new NGramFeature(dat,model,values);

    std::list<int> poc_tags[16];
    char* str=new char[16];
    FILE *fp;
    fp = fopen(label_file, "r");
    int ind=0;
    while( fscanf(fp, "%s", str)==1){
        label_info[ind]=str;
        int seg_ind=str[0]-'0';
        for(int j=0;j<16;j++){
            if((1<<seg_ind)&(j)){
                poc_tags[j].push_back(ind);
            }
        }
        str=new char[16];
        ind++;
    }
    delete[]str;
    fclose(fp);
    
    /*pocs_to_tags*/
    pocs_to_tags=new int*[16];
    for(int j=1;j<16;j++){
        pocs_to_tags[j]=new int[(int)poc_tags[j].size()+1];
        int k=0;
        for(std::list<int>::iterator plist = poc_tags[j].begin();
                plist != poc_tags[j].end(); plist++){
            pocs_to_tags[j][k++]=*plist;
        };
        pocs_to_tags[j][k]=-1;
    }
    
    
    label_looking_for=new int*[model->l_size];
    for(int i=0;i<model->l_size;i++)
        label_looking_for[i]=NULL;
    for(int i=0;i<model->l_size;i++){
        if(label_info[i][0]==kPOC_B || label_info[i][0]==kPOC_S)continue;
        
        for(int j=0;j<=i;j++){
            if((strcmp(label_info[i]+1,label_info[j]+1)==0)&&(label_info[j][0]==kPOC_B)){
                if(label_looking_for[j]==NULL){
                    label_looking_for[j]=new int[2];
                    label_looking_for[j][0]=-1;label_looking_for[j][1]=-1;
                    tag_size++;
                }
                label_looking_for[j][label_info[i][0]-'1']=i;
                break;
            }
        }
    }
    //printf("tagsize %d",tag_size);
    

    
    /**label_trans*/
    if(label_trans){
        load_label_trans(label_trans);
    }
    
   for(int i=0;i<max_length;i++)
       allowed_label_lists[i]=NULL;
    
    is_good_choice=new int[max_length*model->l_size];
    
}
void TaggingDecoder::dp(){
    best_score=dp_decode(
            model->l_size,//check
            model->ll_weights,//check
            len,//check
            nodes,
            values,
            alphas,
            result,
            label_trans_pre,
            allowed_label_lists
        );
}
void TaggingDecoder::cal_betas(){
    int tmp=nodes[len-1].successors[0];
    nodes[len-1].successors[0]=-1;
    dp_cal_betas(
            model->l_size,
            model->ll_weights,
            len,
            nodes,
            values,
            betas
    );
    nodes[len-1].successors[0]=tmp;
}

void TaggingDecoder::set_label_trans(){
    int l_size=this->model->l_size;
    std::list<int> pre_labels[l_size];
    std::list<int> post_labels[l_size];
    for(int i=0;i<l_size;i++)
        for(int j=0;j<l_size;j++){
            // 0:B 1:M 2:E 3:S
            int ni=this->label_info[i][0]-'0';
            int nj=this->label_info[j][0]-'0';
            int i_is_end=((ni==2)//i is end of a word
                    ||(ni==3));
            int j_is_begin=((nj==0)//i is end of a word
                    ||(nj==3));
            int same_tag=strcmp(this->label_info[i]+1,this->label_info[j]+1);
            
            if(same_tag==0){
                if((ni==0&&nj==1)||
                        (ni==0&&nj==2)||
                        (ni==1&&nj==2)||
                        (ni==1&&nj==1)||
                        (ni==2&&nj==0)||
                        (ni==2&&nj==3)||
                        (ni==3&&nj==3)||
                        (ni==3&&nj==0)){
                    pre_labels[j].push_back(i);
                    post_labels[i].push_back(j);
                    //printf("ok\n");
                }
            }else{
                //printf("%s <> %s\n",this->label_info[i],this->label_info[j]);
                if(i_is_end&&j_is_begin){
                    pre_labels[j].push_back(i);
                    post_labels[i].push_back(j);
                    //printf("ok\n");
                }
            }
        }
    label_trans_pre=new int*[l_size];
    for(int i=0;i<l_size;i++){
        label_trans_pre[i]=new int[(int)pre_labels[i].size()+1];
        int k=0;
        //printf("i=%d>>\n",i);
        for(std::list<int>::iterator plist = pre_labels[i].begin();
                plist != pre_labels[i].end(); plist++){
            label_trans_pre[i][k]=*plist;
            //printf("%d ",*plist);
            k++;
        };
        //printf("\n");
        label_trans_pre[i][k]=-1;
    }
    
};

void TaggingDecoder::load_label_trans(char*filename){
    //打开文件
    FILE * pFile=fopen ( filename , "rb" );
    /*得到文件大小*/
    int remain_size=0;
    int rtn=fread (&remain_size,sizeof(int),1,pFile);
    /*得到矩阵数据*/
    label_trans=new int[remain_size];
    rtn=fread (label_trans,sizeof(int),remain_size,pFile);
    
    /*计算标签个数*/
    int label_size=0;
    for(int i=0;i<remain_size;i++){
        if(label_trans[i]==-1)label_size++;
    }
    label_size/=2;
    /*设定各个标签的指针*/
    label_trans_pre=new int*[label_size];
    label_trans_post=new int*[label_size];
    int ind=0;
    for(int i=0;i<label_size;i++){
        label_trans_pre[i]=label_trans+ind;
        while(label_trans[ind]!=-1)ind++;ind++;
        label_trans_post[i]=label_trans+ind;
        while(label_trans[ind]!=-1)ind++;ind++;
    }
    fclose (pFile);
    return;
}

void TaggingDecoder::put_values(){
    if(!len)return;
    /*nodes*/
    for(int i=0;i<len;i++){
        nodes[i].type=0;
    }
    nodes[0].type+=1;
    nodes[len-1].type+=2;
    /*values*/
    memset(values,0,sizeof(*values)*len*model->l_size);
    ngram_feature->put_values(sequence,len);
}


void TaggingDecoder::output_raw_sentence(){
    int c;
    for(int i=0;i<len;i++){
        daidai::put_character(sequence[i]);
        
    }
}
void TaggingDecoder::output_sentence(){
    int c;
    for(int i=0;i<len;i++){
        daidai::put_character(sequence[i]);
        
        if((label_info[result[i]][0]==kPOC_E)||(label_info[result[i]][0]==kPOC_S)){//分词位置
            if(*(label_info[result[i]]+1)){//输出标签（如果有的话）
                putchar(separator);
                printf("%s",label_info[result[i]]+1);
            }
            if((i+1)<len)putchar(' ');//在分词位置输出空格
        }
    }
}

void TaggingDecoder::find_good_choice(){
    /*找出可能的标注*/
    for(int i=0;i<len*model->l_size;i++)
        is_good_choice[i]=alphas[i].value+betas[i].value-values[i]+threshold>=best_score;
};

    
void TaggingDecoder::output_allow_tagging(){
    find_good_choice();
    /*找出可能的词*/
    int this_score=0;
    int left_part=0;
    int last_id=0;
    for(int i=0;i<len;i++){
        //printf("i=%d\n",i);
        for(int b_label_i=0;b_label_i<model->l_size;b_label_i++){
            if(!is_good_choice[i*model->l_size+b_label_i]){
                continue;
            }
            if(label_info[b_label_i][0]==kPOC_S){
                //输出单个字的词
                this_score=alphas[i*model->l_size+b_label_i].value
                        +betas[i*model->l_size+b_label_i].value
                        -values[i*model->l_size+b_label_i];
                printf("%d,%d,%s,%d ",i,i+1,label_info[b_label_i]+1,best_score-this_score);
                //printf("%d,",i);
                //put_character(this->sequence[i]);
                //putchar(',');
                //printf("%s,%d ",label_info[b_label_i]+1,best_score-this_score);
            }else if(label_info[b_label_i][0]==kPOC_B){
                int mid_ind=label_looking_for[b_label_i][0];
                int right_ind=label_looking_for[b_label_i][1];
                left_part=alphas[i*model->l_size+b_label_i].value;
                last_id=b_label_i;
                for(int j=i+1;j<len;j++){
                    if(j==len)break;
                    if(right_ind==-1)break;

                    if(is_good_choice[j*model->l_size+right_ind]){
                        //check，是不是合格的词
                        this_score=left_part
                                +model->ll_weights[last_id*model->l_size+right_ind]
                                +betas[j*model->l_size+right_ind].value;
                        if(best_score-this_score<=threshold){
                            printf("%d,%d,%s,%d ",i,j+1,label_info[b_label_i]+1,best_score-this_score);
                            //printf("%d,",i);
                            //for(int k=i;k<=j;k++)
                            //    put_character(this->sequence[k]);
                            //putchar(',');
                            //printf("%s,%d ",label_info[b_label_i]+1,best_score-this_score);
                        }
                    }
                    if(mid_ind==-1)break;
                    if(!is_good_choice[(j*(model->l_size))+mid_ind])
                        break;
                    left_part+=values[j*model->l_size+mid_ind]
                            +model->ll_weights[last_id*model->l_size+mid_ind];
                    last_id=mid_ind;
                }
                
            }
        }
    }
}

int TaggingDecoder::segment(int* input,int length,int* tags){
    if(not length)return 0;
    for(int i=0;i<length;i++){
        sequence[i]=input[i];
    }
    len=length;
    
    put_values();//检索出特征值并初始化放在values数组里
    dp();//动态规划搜索最优解放在result数组里
    
    for(int i=0;i<len;i++){
        if((label_info[result[i]][0]==kPOC_B)||(label_info[result[i]][0]==kPOC_S)){//分词位置
            tags[i]=1;
        }else{
            tags[i]=0;
        }
    }
}

int TaggingDecoder::segment(RawSentence& raw,SegmentedSentence& segged){
    segged.clear();
    if(raw.size()==0)return 0;
    for(int i=0;i<(int)raw.size();i++){
        sequence[i]=raw[i];
    }
    len=(int)raw.size();
    put_values();//检索出特征值并初始化放在values数组里
    dp();//动态规划搜索最优解放在result数组里
    for(int i=0;i<len;i++){
        if((i==0)||(label_info[result[i]][0]==kPOC_B)||(label_info[result[i]][0]==kPOC_S)){
            segged.push_back(Word());
        }
        segged.back().push_back(raw[i]);
    }
}

int TaggingDecoder::segment(RawSentence& raw,
        POCGraph&old_graph,
        SegmentedSentence& segged){
    for(int i=0;i<(int)raw.size();i++){
        int pocs=0;
        for(int j=0;j<(int)old_graph[i].size();j++){
            pocs+=1<<(old_graph[i][j]);
        }
        if(pocs){
            allowed_label_lists[i]=pocs_to_tags[pocs];
        }else{
            allowed_label_lists[i]=pocs_to_tags[15];
        }
    }
    segment(raw,segged);
    for(int i=0;i<(int)raw.size();i++){
        allowed_label_lists[i]=NULL;
    }
}

int TaggingDecoder::segment(RawSentence&raw,
        POCGraph&new_graph){
    if(raw.size()==0)return 0;
    for(int i=0;i<(int)raw.size();i++){
        sequence[i]=raw[i];
    }
    len=(int)raw.size();
    put_values();//检索出特征值并初始化放在values数组里
    dp();//动态规划搜索最优解放在result数组里
    cal_betas();
    find_good_choice();
    new_graph.clear();
    for(int i=0;i<len;i++){
        new_graph.push_back(std::vector<POC>());
        for(int j=0;j<model->l_size;j++){
            if(is_good_choice[i*model->l_size+j])
                new_graph.back().push_back((POC)j);
        }
    }
};


}
