#ifndef CAMERA_H
#define CAMERA_H

#include "config.h"
#include "pd.h"
#include "videosrc.h"
#include "videoprocessor.h"
class Camera
{
      typedef CameraConfiguration::camera_config_t camera_config;
    VideoSrc *p_src;
    VideoProcessor *_prc;
public:
    Camera(camera_config cfg)
    {
        p_src=new VideoSrc(cfg.ip.toStdString().data());
    }
    ~Camera()
    {
        delete p_src;
    }
};

class CameraManager{
    int test_int;
public:
#if 1
    static CameraManager &GetInstance()
    {
        static CameraManager m;

        return m;
    }
private:
#endif
    CameraManager()
    {
#ifdef DISPLAY_VIDEO
#ifdef IS_UNIX
        XInitThreads();
#endif
#endif
        p_cfg=new CameraConfiguration("config.json-server");
        start_all();
    }

public:
    void test()
    {
        test_int=123;
    }
    void prt_test()
    {
        prt(info,"%d",test_int);
    }

    ~CameraManager()
    {
        //        stop_all();
        //        delete p_cfg;

    }
    int handle_cmd(char *src_buf,char*dst_buf,int size)
    {
        prt(info,"handle cmd");
        int client_cmd=Protocol::get_operation(src_buf);
        int pkg_len=Protocol::get_length(src_buf);
        int cam_index=Protocol::get_cam_index(src_buf);
        QByteArray bta;
        int ret_size=0;
        switch (client_cmd) {
        case Protocol::ADD_CAMERA:
            prt(info,"protocol :add    cam  ");
            bta.clear();
            bta.append(src_buf+Protocol::HEAD_LENGTH,pkg_len);
            add_camera(bta.data());
            memcpy(dst_buf,src_buf,size);
            ret_size= Protocol::HEAD_LENGTH;
            break;
        case  Protocol::GET_CONFIG:
            prt(info,"protocol :get cofnig  ");
            memcpy(dst_buf,src_buf,Protocol::HEAD_LENGTH);
            memcpy(dst_buf+Protocol::HEAD_LENGTH,p_cfg->get_config().data(),p_cfg->get_config().size());
            ret_size=p_cfg->get_config().size()+Protocol::HEAD_LENGTH;
            break;
        case Protocol::DEL_CAMERA:
            prt(info,"protocol :deleting    cam %d ",cam_index);
            del_camera(cam_index);
            memcpy(dst_buf,src_buf,Protocol::HEAD_LENGTH);
            ret_size= Protocol::HEAD_LENGTH;
            break;
        case Protocol::MOD_CAMERA:
            prt(info,"protocol : modify   cam %d ",cam_index);
            break;
        default:
            break;
        }
        return ret_size;

    }

    void start_all()
    {
        foreach (CameraConfiguration::camera_config_t tmp, p_cfg->cfg.camera) {
            Camera *c=new Camera(tmp);
            cameras.push_back(c);
        }
    }
    void stop_all()
    {
        foreach (Camera *tmp, cameras) {
            delete tmp;
            cameras.removeOne(tmp);
        }
    }

    void add_camera(const char *cfg_buf)
    {
        p_cfg->set_config(cfg_buf);
        Camera *c=new Camera(p_cfg->cfg.camera[p_cfg->cfg.camera_amount-1]);
        //    cameras.push_back(c);
        cameras.append(c);
    }
    void del_camera(const char *cfg_buf,const int index)
    {
        p_cfg->set_config(cfg_buf);
        delete cameras[index-1];
        cameras.removeAt(index-1);
    }
    void del_camera(const int index)
    {
        // p_cfg->set_config(cfg_buf);

        p_cfg->del_camera(index);
        Camera *cm=cameras[index-1];
     //   prt(info,"delete %s",cm->d.p_src->get_url());
        delete cm;//////////////////////////TODO
        cameras.removeAt(index-1);
        //   delete cm;
    }
    void mod_camera(const char *cfg_buf,const int index)
    {
        p_cfg->set_config(cfg_buf);
        //        while(true){
        //            if(0==cameras[index-1]->try_restart(p_cfg->cfg.camera[p_cfg->cfg.camera_amount-1]))
        //                break;
        //            else
        //            {
        //                prt(info,"restarting camera %d",index);
        //            }
        //        }
    }

private:
    CameraConfiguration *p_cfg;
    QList<Camera *> cameras;

};

#endif // CAMERA_H
