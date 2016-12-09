#include"Decode/decode.h" 

SwsContext *Decode_video::scxt = sws_getContext(640,480,AV_PIX_FMT_YUV420P,640,480,AV_PIX_FMT_RGB24,2,NULL,NULL,NULL);
int Frame_Deque::push_frame(AVFrame* frame,int i_frame)
{
    mutex.lock();
    int width = frame->width;
    int height = frame->height;
    memcpy(p_frame[data_p].p_date,frame->data[0],width*height*3);
    p_frame[data_p].height = height;
    p_frame[data_p].width = width;
    p_frame[data_p].i_pt = i_frame;
    //data_i++;
    data_p=(data_p+1)%10;
    if (10==data_i)
    {//队列已经满了
        cout<<"the Pic_Date array is full"<<endl;
        data_f  = (data_f + 1) % 10;
    }else
    {
        data_i++;
    }
    mutex.unlock();
    return 0;
}
Pic_Date* Frame_Deque::get_frame(Pic_Date* pic)
{
    mutex.lock();
    if ((pic->height!=p_frame[data_f].height)||(pic->width!=p_frame[data_f].width))
    {//接受视频的尺寸变了。
		cout<<"Frame_Deque ::  pic size changed"<<endl;
        delete pic;
        pic = NULL;
        pic = new Pic_Date(p_frame[data_f].width,p_frame[data_f].height);
        pic->width = p_frame[data_f].width;
        pic->height = p_frame[data_f].height;
        pic->is_size_changed = true;
    }
    else
    {
        pic->is_size_changed = false;
    }
    memcpy(pic->p_date,p_frame[data_f].p_date,p_frame[data_f].width * p_frame[data_f].height * 3);
    pic->i_pt = p_frame[data_f].i_pt;
    data_f  = (data_f + 1) % 10;
    data_i--;
    mutex.unlock();
    return pic;
}
bool Frame_Deque::isnull()
{
    mutex.lock();
    if (0 == data_i)
    {
        mutex.unlock();
        return true;
    }else
    {
        mutex.unlock();
        return false;
    }
    return true;
}

Decode_video::Decode_video(int h)
{
    //memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

    //inbuf = (uint8_t *)malloc(sizeof(uint8_t)*(AUDIO_INBUF_SIZE));
    //inbuf = new unchar[a]; 
    decodehandle = h;
    p_nall = new CVideoData();
    p_frame_deque = new Frame_Deque();

//	scxt =(SwsContext*)malloc(sizeof(SwsContext));
//	scxt = sws_getContext(picture->width,picture->height,AV_PIX_FMT_YUV420P,
//		picture->width,picture->height,AV_PIX_FMT_RGB24,2,NULL,NULL,NULL);

    picture = av_frame_alloc();
    RGB_pic = av_frame_alloc();
    avpicture_alloc((AVPicture*)RGB_pic,AV_PIX_FMT_RGB24,640,480);
	RGB_pic->width = 640;
	RGB_pic->height = 480;
	//assert(0);
    //memset(inbuf , 0, AUDIO_INBUF_SIZE);
    avcodec_register_all();
    av_init_packet(&avpkt);
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);//原版本的是CODEC_ID_H264
    if (!codec)
    {
        fprintf(stderr, "codecnot found\n");
        return ;
    }
    c = avcodec_alloc_context3(codec);
    if (!c)
    {
        fprintf(stderr, "Could not allocate audio codec context\n");

    }
    //if(codec->capabilities&CODEC_CAP_TRUNCATED)
    //c->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
    if (avcodec_open2(c, codec, NULL) < 0)
    {
        fprintf(stderr, "couldnot open codec\n");
        exit(1);
    }

    //picture = avcodec_alloc_frame();
    //picture =av_frame_alloc();

    frame = 0;
   // buf_size = 0;
}
Decode_video::~Decode_video()
{
    delete p_nall;
    p_nall = NULL;
    free(scxt);
    scxt = NULL;
    avcodec_close(c);
    av_free(c);
    av_frame_free(&picture);
    av_frame_free(&RGB_pic);
    printf("\n");
}
void Decode_video::run()
{
    while (1)
    {
        if (!MyRTPSession::m_ReceiveArray[decodehandle].isnull())
        {
			av_init_packet(&avpkt);
            MyRTPSession::m_ReceiveArray[decodehandle].get(p_nall);
            avpkt.size =p_nall->m_lLength;
            avpkt.data =p_nall->m_pBuffer;
            //cout<<"get data "<<avpkt.data<<endl;
            while (avpkt.size > 0)
            {
                //cout<<"a"<<endl;
                len = avcodec_decode_video2(c,picture, &got_picture, &avpkt);//解码每一帧
                //cout<<"b"<<endl;

                if (len < 0)
                {
                    fprintf(stderr, "Error while decoding frame %d\n",frame);
                    break;
                }
                else
                {
                    cout<<"decode success  "<<c->width<<"  "<<c->height<<endl;
                }
                if (got_picture)
                {
                   Call_Show();
                    cout<<"get frame"<<endl;
                    frame++;
                }
                avpkt.size -= len;
                avpkt.data += len;
            }
            //av_packet_free(&(&avpkt));
            av_free_packet(&avpkt);
        }
        else
        {//if

            msleep(10);
        }
    }//while
}
void Decode_video::Call_Show()
{
    if (scxt!=NULL)
    {	
		if((RGB_pic->width!=c->width)||(RGB_pic->height!=c->height))
		{
			cout<<"picture size changed"<<endl;
			av_frame_free(&RGB_pic);
			RGB_pic = av_frame_alloc();
			avpicture_alloc((AVPicture*)RGB_pic,AV_PIX_FMT_RGB24,c->width,c->height);
			RGB_pic->width = c->width;
			RGB_pic->height = c->height;
			scxt = sws_getContext(picture->width,picture->height,AV_PIX_FMT_YUV420P,
		picture->width,picture->height,AV_PIX_FMT_RGB24,2,NULL,NULL,NULL);
		}else{
			cout<<"picture size isn't change"<<endl;
		}
		sws_scale(scxt,picture->data,picture->linesize,0,c->height,RGB_pic->data,RGB_pic->linesize);
		cout<<picture->width<<"  "<<picture->height<<endl;
		cout<<RGB_pic->width<<"  "<<RGB_pic->height<<endl;
	}
	p_frame_deque->push_frame(RGB_pic,frame);

}
