



//由于使用的是lvgl8.0初的版本所以，时间轴不支持lv_anim_timeline_pause() 或 lv_anim_timeline_resume()，无法暂停和恢复进度条，只能重启进度条
//lvgl低于7.0版本的可以支持直接在lv_anim_t的动画暂停

#include "lvgl/lvgl.h"
#include <time.h>
#include "my/my_gui.h"
#include <lvgl/src/misc/lv_timer.h>
#include <sdl.h>
#include "SDL_mixer.h"
#include "pthread.h"


pthread_mutex_t mutex_task1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_task1 = PTHREAD_COND_INITIALIZER;
int paused_task1 = 1; // 0 - 不暂停，1 - 暂停


//9cc compiler了，更适合你的学习情况
 lv_anim_t a;
Mix_Music* music;
lv_anim_t b;
lv_obj_t* obj; /* Pointer to the object you want to animate */
static lv_obj_t* s1;//s1主界面背景
static lv_obj_t* s2;//s2是音乐图片按钮
static lv_obj_t* s3;//s3是按钮点进后的下屏框
static lv_obj_t* s0;//s3是按钮点进去后的上屏主体框
static lv_obj_t* s4;//s4是按钮点进去后的整个背景
static lv_obj_t* s5;//s5是上框中的外框
static lv_obj_t* s6;//s5是上框中的二次元头像
static   lv_obj_t* s7;//s7是音量按钮
static lv_obj_t* s10;//s10是播放按钮的变化
static lv_obj_t* imgbtn_sound;
static lv_anim_t music_ainm_bar;
lv_anim_timeline_t* music_bar;
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())
//LV_IMG_DECLARE(test4)
LV_IMG_DECLARE(musicbox_frame)
LV_IMG_DECLARE(musicbox_col)
LV_IMG_DECLARE(musicbox_img)
LV_IMG_DECLARE(test)
LV_IMG_DECLARE(OIP)
LV_IMG_DECLARE(lcv)
LV_IMG_DECLARE(tec)
LV_IMG_DECLARE(tes)
LV_IMG_DECLARE(music_sound)
LV_IMG_DECLARE(music_player)
LV_IMG_DECLARE(music_paly)
LV_IMG_DECLARE(music_pause)
static int16_t  value_var = 0;
lv_anim_t anim;
lv_anim_timeline_t *music_timeline_bar;

void pause_task1() {
    pthread_mutex_lock(&mutex_task1);
    paused_task1 = 1;  // 设置暂停标志
    printf("Task 1 paused.\n");
    Mix_PauseMusic(); // 暂停音乐
    pthread_mutex_unlock(&mutex_task1);
    
}

// 恢复任务1
void resume_task1() {
    pthread_mutex_lock(&mutex_task1);
    paused_task1 = 0;  // 清除暂停标志
    printf("Task 1 resumed.\n");
    pthread_cond_signal(&cond_task1);  // 唤醒等待的线程
    Mix_ResumeMusic(); // 恢复音乐
    pthread_mutex_unlock(&mutex_task1);
}

// 动画回调函数：使圆形旋转
static void rotate_cb(void* obj, int32_t angle) {
    lv_obj_set_style_transform_angle((lv_obj_t*)obj, angle, LV_PART_MAIN);
}
static my_event() {
    lv_obj_del(s1);
    lv_obj_del(s2);
    s4 = lv_obj_create(NULL);                                                
    lv_obj_set_style_bg_color(s4, lv_color_hex(0x000000), LV_PART_MAIN);       
    lv_scr_load(s4);                                                       

    // ------渐变效果[性能问题(抗锯齿)]舍弃---------------------------------------------------------

    //    static lv_style_t style;
    //    lv_style_init(&style);
    //    lv_style_set_bg_color(&style, lv_color_make(79,52,52));                     // 设置起始颜色为黑色
    //    lv_style_set_bg_grad_color(&style, lv_color_make(24, 25, 19));                // 设置渐变结束颜色为白色
    //    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);                            // 设置垂直渐变
    // -----------------------------------------------------------------

    s0 = lv_obj_create(s4);                                         // 主体框
    lv_obj_set_size(s0, 800,300);                                           // 默认4.3寸屏幕
    lv_obj_set_style_bg_color(s0, lv_color_make(40, 38, 38), LV_PART_MAIN);    // 设置背景颜色
    lv_obj_align(s0, LV_ALIGN_TOP_MID, 0, 0);                                 // 设置对齐位置
    lv_obj_set_style_border_opa(s0, 0, LV_PART_MAIN);                        // 设置无边框
    lv_obj_set_style_radius(s0, 0, 0);                                       // 设置无锐角
  //  lv_obj_add_event_cb(s0, music_main_event_cb, LV_EVENT_CLICKED, NULL);     // 触发事件回调函数（被点击）

    //    lv_obj_add_style(s0,&style,LV_PART_MAIN);                               //渐变效果要设置的对象

    s3 = lv_obj_create(s4);                                         // 次要框
    lv_obj_set_size(s3, 800,200);                                           // 默认4.3寸屏幕
    lv_obj_align_to(s3,s0, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);                             // 设置对齐位置
    lv_obj_set_style_border_opa(s3, 0, LV_PART_MAIN);                       // 设置无边框
    lv_obj_set_style_bg_color(s3, lv_color_hex(0x272524), LV_PART_MAIN);    // 设置背景颜色
    lv_obj_set_style_radius(s3, 0, 0);                                      // 设置无锐角
    lv_obj_set_style_bg_opa(s3, 248, LV_STATE_DEFAULT);                     // 设置背景透明度


    LV_IMG_DECLARE(musicbox_img);                                           //声明要使用的图片
  lv_obj_t*  music_box_in_img = lv_img_create(s0);                          //在主框创建一个图片对象控件
    lv_img_set_src(music_box_in_img, &musicbox_img);                         //对象设置图片来源
    lv_obj_align(music_box_in_img, LV_ALIGN_CENTER, 0, 0);     //位置对齐

    lv_obj_t* progress_bar = lv_bar_create(s3);
    lv_obj_set_align(progress_bar, LV_ALIGN_TOP_MID);
    lv_obj_set_size(progress_bar, 750, 5);
    lv_bar_set_range(progress_bar, 0, 100); // 设置进度条范围为 0 到 100
    lv_bar_set_value(progress_bar, 0, LV_ANIM_ON);  // 设置初始值为 0

    music_bar = lv_anim_timeline_create();
    lv_anim_timeline_add(music_bar, 0, &progress_bar);

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, progress_bar); // 设置动画的目标对象
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_bar_set_value); // 进度条的执行函数
    lv_anim_set_values(&anim, 0, 100);   // 动画从 0 到 100
    lv_anim_set_time(&anim, 120000);      // 动画持续 120,000 毫秒，即 2 分钟
    lv_anim_set_path_cb(&anim, lv_anim_path_linear); // 线性路径
    
    music_bar = lv_anim_timeline_create();
    lv_anim_timeline_add(music_bar, 0, &anim);
   
}
void my_event3(lv_event_t* e) {

  
    static bool is_slider_visible = false; // 静态变量用于存储滑块显示状态
    lv_event_code_t s13 = lv_event_get_code(e);

    if (s13 == LV_EVENT_CLICKED) {
        if (is_slider_visible) {
            // 隐藏滑块
            lv_obj_add_flag(s7, LV_OBJ_FLAG_HIDDEN);
           
        }
        else {
            // 创建并显示滑块
            s7 = lv_slider_create(s3);
            lv_obj_set_style_bg_color(s7, lv_color_hex(0xff0000), LV_PART_INDICATOR);
            lv_obj_align_to(s7, s3, LV_ALIGN_RIGHT_MID, -20, 0);
            lv_obj_set_size(s7, 10, 70);
            lv_obj_align_to(s7, imgbtn_sound,LV_ALIGN_TOP_MID,320,20);
            //lv_obj_add_flag(s7, LV_OBJ_FLAG_HIDDEN);
          

        }

        // 切换滑块显示状态
        is_slider_visible = !is_slider_visible;
    }
}
void* task1(void* arg) {
    while (1) {

        while (paused_task1) {
            printf("Task 2 paused, waiting...\n");
            pthread_cond_wait(&cond_task1, &mutex_task1); // 等待恢复信号
        }
        pthread_mutex_unlock(&mutex_task1);
        static int num = 0;
        if (num == 0) {
            SDL_Init(SDL_INIT_AUDIO);
            Mix_Init(MIX_INIT_MP3);
            Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);


            //音乐.MP3文件的路径
            const char* filePath = "C:\\Users\\34397\\Desktop\\a.mp3";  // 绝对路径

            music = Mix_LoadMUS(filePath);

            Mix_PlayMusic(music, 1);
            while (Mix_PlayingMusic()) {
                SDL_Delay(100);  // 稍微延迟以避免占用过多 CPU
            }
            //  释放资源
            Mix_FreeMusic(music);
            Mix_CloseAudio();
            Mix_Quit();
            SDL_Quit();
        }
        else
            return NULL;
        num = 1;
    }
}





void anim_callback(void* var, int32_t value) {
    //lv_obj_set_angle((lv_obj_t*)var, value); // 设置对象的角度
    
}

static bool is_palying= true;
void my_event4(lv_event_t* e) {
    static num = 0;
    if (num == 0) {
        resume_task1();
        lv_anim_timeline_start(music_timeline_bar);
    }
    lv_event_code_t s12 = lv_event_get_code(e);

    if (s12 == LV_EVENT_CLICKED) {
        if (is_palying) {
            // 隐藏滑块
            lv_imgbtn_set_src(s10, LV_IMGBTN_STATE_RELEASED, NULL, &music_pause, NULL);
           
            lv_anim_set_values(&b, 0, 300);
           // lv_obj_add_child(s5,s6);
            //lv_obj_set_layer(s5,s6);
            lv_anim_timeline_start(music_timeline_bar);
                   lv_anim_start(&b);
            lv_obj_clear_flag(s5, LV_OBJ_FLAG_HIDDEN);
            lv_anim_timeline_start(music_bar);
            resume_task1();
        }
        else {
            lv_imgbtn_set_src(s10, LV_IMGBTN_STATE_RELEASED, NULL, &music_paly, NULL);
           // lv_anim_del(&a,NULL);
            lv_anim_del(s5, &a);
            lv_anim_set_values(&b, 300, 0);
             lv_anim_start(&b);
            //lv_obj_add_flag(s5, LV_OBJ_FLAG_HIDDEN); // 隐藏外框，或根据需要进行其他静态显示
             lv_anim_timeline_stop(music_timeline_bar);
             lv_anim_timeline_stop(music_bar);
           
            pause_task1();
        }

        // 切换滑块显示状态
        is_palying = !is_palying;
    }
    num = 1;
}

void box_anming() {
   
     s5 = lv_img_create(s0);
    lv_img_set_src(s5, &musicbox_frame);
    s6 = lv_img_create(s0);
    lv_img_set_src(s6, &musicbox_col);
     lv_obj_center(s5);
     lv_obj_align_to(s6, s5, LV_ALIGN_OUT_TOP_RIGHT, 0, 10);
     lv_obj_set_style_transform_pivot_x(s5, lv_obj_get_width(s5) / 2, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_y(s5, lv_obj_get_height(s5) / 2, LV_PART_MAIN);
    lv_anim_init(&a); // 初始化动画
    lv_anim_set_var(&a, s5); // 设置目标对象
    lv_anim_set_exec_cb(&a, rotate_cb); // 设置动画回调
    lv_anim_set_values(&a, 0, 3600); // 设置旋转范围 (0 到 3600 = 360°)
    lv_anim_set_time(&a, 2000); // 动画时长 2000ms (2秒)
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // 无限循环  
    music_timeline_bar = lv_anim_timeline_create();
    lv_anim_timeline_add(music_timeline_bar, 0, &a);
    
    s10 = lv_imgbtn_create(s3);
   lv_imgbtn_set_src(s10, LV_IMGBTN_STATE_RELEASED,NULL,&music_paly,NULL);
    lv_obj_set_size(s10, 70, 70);
    lv_imgbtn_set_state(s10, LV_IMGBTN_STATE_RELEASED);
    lv_obj_align_to(s10,s3,LV_ALIGN_TOP_MID,0,50);
    lv_obj_add_event_cb(s10, my_event4, LV_EVENT_CLICKED, NULL);
     lv_anim_init(&b);
    
    lv_anim_set_var(&b, s6); // 指定动画作用的对象
    // 动画从 0 到 30 度旋转
    lv_anim_set_time(&b, 2000); // 动画持续时间为 2 秒
    lv_anim_set_exec_cb(&b, rotate_cb);
}
void fu_kuang() {


    LV_IMG_DECLARE(music_sound); //声音图标
    lv_obj_t* imgbtn_sound = lv_imgbtn_create(s3);
    lv_imgbtn_set_src(imgbtn_sound, LV_IMGBTN_STATE_RELEASED, NULL, &music_sound, NULL);
    lv_obj_set_size(imgbtn_sound, 25, 25);
    lv_imgbtn_set_state(imgbtn_sound, LV_IMGBTN_STATE_RELEASED);
    lv_obj_align_to(imgbtn_sound, s3, LV_ALIGN_RIGHT_MID, -40, 40);
    //lv_obj_add_event_cb(imgbtn_sound, sound_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(imgbtn_sound, my_event3, LV_EVENT_CLICKED, NULL);


}
static  my_event1(lv_event_t* e) {
    my_event();  //设置背景
   box_anming();
   fu_kuang();
  
  
}
void* task2(void* arg) {

    s1 = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(s1, lv_color_hex(0xefefef), LV_PART_MAIN);
    lv_obj_set_size(s1, scr_act_width() * 5 / 6, scr_act_height() * 4 / 5);
    lv_obj_center(s1);

    s2 = lv_imgbtn_create(lv_scr_act());
    lv_imgbtn_set_src(s2, LV_IMGBTN_STATE_RELEASED, NULL, &test, NULL);
    // 设置按钮大小和位置
    lv_obj_add_event_cb(s2, my_event1, LV_EVENT_CLICKED, NULL);
    lv_obj_set_align(s2, LV_ALIGN_CENTER);

    while (1) {
        lv_task_handler();  // 确保定期调用 LVGL 任务
        SDL_Delay(5);      // 防止主线程阻塞
    }

    return NULL;
}
void create_colorful_clock() {
  
  
   
    pthread_t thread1, thread2;

    // 创建两个线程，分别执行 task1 和 task2
    pthread_create(&thread2, NULL, task2, NULL);

   pthread_create(&thread1, NULL, task1, NULL);
  
    // 等待两个线程完成
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;

  
   
   
    }




//void bofang() {
//    SDL_Init(SDL_INIT_AUDIO);
//    Mix_Init(MIX_INIT_MP3);
//    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
//
//    const char* filePath = "C:\\Users\\34397\\Desktop\\a.mp3";  // 绝对路径
//
//    music = Mix_LoadMUS(filePath);
//
//    Mix_PlayMusic(music, 1);
//    while (Mix_PlayingMusic()) {
//        SDL_Delay(100);  // 稍微延迟以避免占用过多 CPU
//    }
// //  释放资源
//    Mix_FreeMusic(music);
//    Mix_CloseAudio();
//    Mix_Quit();
//    SDL_Quit();
//    return 0;
//}





//s1 = lv_obj_create(lv_scr_act());
////   lv_obj_set_style_bg_color(s1, lv_color_hex(0xefefef), LV_PART_MAIN);
////   lv_obj_set_size(s1, scr_act_width() * 5 / 6, scr_act_height() * 4 / 5);
////   lv_obj_center(s1);
////   
////    s2 = lv_imgbtn_create(lv_scr_act());
////   lv_imgbtn_set_src(s2 ,LV_IMGBTN_STATE_RELEASED, NULL, &test, NULL);
////   // 设置按钮大小和位置
////   lv_obj_add_event_cb(s2, my_event1, LV_EVENT_CLICKED, NULL);
////   lv_obj_set_align(s2,LV_ALIGN_CENTER);
