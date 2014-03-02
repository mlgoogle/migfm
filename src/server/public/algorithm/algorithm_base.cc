#include "algorithm_base.h"
#include "log/mig_log.h"
namespace algorithm{

void AlgorithmBase::Init(){
	//radom_num_ = new base::MigRadomIn();
}

void AlgorithmBase::Dest(){
	/*if (radom_num_){
		delete radom_num_;
		radom_num_ = NULL;
	}*/
}


int32 AlgorithmBase::GetTotalForNum(int32 num,int32 n){
    return num*n;
}



void AlgorithmBase::AllocationLatitudeMusicNum(const int32 mood_index, int32 &mood_num,
										const int32 scenes_index, int32 &scenes_num, 
									    const int32 channel_index, int32 &channel_num,
										const int32 num){
   double overall_num = mood_index+scenes_index+channel_index;
   double dl_mood_num = 0;
   double dl_scenes_num = 0;
   double dl_channel_num = 0;
   int temp_num;
   double temp;
   
   if(overall_num==0)
	   overall_num = 1;
   double proportion_mood = mood_index/overall_num;
   double proportion_scenes = scenes_index/overall_num;
   double proportion_channel = channel_index/overall_num;
   
   //比较大小返回的标准
   int32 flag = 0;//1.心情 2.场景 3.频道
   //
   dl_mood_num = proportion_mood*num;
   dl_scenes_num = proportion_scenes*num;
   dl_channel_num = proportion_channel*num;

   //默认顺序 心情大于场景大于频道
   flag = (dl_mood_num>=dl_scenes_num)?((dl_mood_num>=dl_channel_num)?1:3):(dl_scenes_num>=      dl_channel_num?2:3);

   //若不够总和 比重最大的获取剩余的歌曲个数
   double surplus_num = num - dl_mood_num - dl_scenes_num  - dl_channel_num;
   if (surplus_num==0)//若够的话 最大加0.5,
	   surplus_num = 0.5;

   switch (flag){
	     case 1:
	     {
		    dl_mood_num+=surplus_num;
			if (dl_scenes_num>dl_channel_num){
			    dl_scenes_num+=0.5;
				dl_channel_num-=0.5;
			}else if(dl_scenes_num<dl_channel_num){
				dl_channel_num+=0.5;
				dl_scenes_num-=0.5;
			}
		 }
		    break;
	     case 2:
			 {
				 dl_scenes_num+=surplus_num;
				 if (dl_mood_num>dl_channel_num){
					 dl_mood_num+=0.5;
					 dl_channel_num-=0.5;
				 }else if(dl_mood_num<dl_channel_num){
					 dl_channel_num+=0.5;
					 dl_mood_num-=0.5;
				 }
			 }
		    break;
	     case 3:
			 {
				 dl_channel_num+=surplus_num;
				 if (dl_mood_num>dl_scenes_num){
					 dl_mood_num+=0.5;
					 dl_scenes_num-=0.5;
				 }else if(dl_mood_num<dl_scenes_num){
					 dl_scenes_num+=0.5;
					 dl_mood_num-=0.5;
				 }
			 }
		    break;
	     default:
		     break;
   }

#define MIG_PLUGIS_RETURN(X,Y)\
    temp_num = (int)X;\
	temp = X - temp_num;\
	Y = (temp*10>5)?1:0;\

   int Y =0;

    //个数为0 说明所占的比重过小 只推荐一首
   MIG_PLUGIS_RETURN(dl_mood_num,Y);
   mood_num = dl_mood_num + Y;
   if (mood_num==0)
	   mood_num = 1;

   MIG_PLUGIS_RETURN(dl_scenes_num,Y);
   scenes_num = dl_scenes_num + Y;
   if (scenes_num==0)
	   scenes_num = 1;

   MIG_PLUGIS_RETURN(dl_channel_num,Y);
   channel_num = dl_channel_num + Y;
   if (channel_num==0)
	   channel_num = 1;

#undef  MIG_PLUGIS_RETURN

  

   MIG_DEBUG(USER_LEVEL,"mood_num[%d] scenes_num[%d] channel_mum[%d]",
	           mood_num,scenes_num,channel_num);
}

void AlgorithmBase::NotRepeatedNum(int total_num,int* num,int& n){
	int ticket = 0;
	int m = 0;
	/*while(m<n){
		int temp_num = radom_num_->GetPrize()%total_num;
	}*/
}
}