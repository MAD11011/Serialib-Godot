#include "serialib-godot.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#ifdef __unix__         
#include <fcntl.h>  
#include <errno.h>  
#include <termios.h>
#include <unistd.h> 
#include <dirent.h> 
#elif defined(_WIN32) || defined(WIN32) 

#define OS_Windows

#endif

const char* const open_fail_msg[]{"device not found"
    ,"error while opening the device"
        ,"error while getting port parameters"
        ,"Speed (Bauds) not recognized"
        ,"error while writing port parameters"
        ,"error while writing timeout parameters"
        ,"Databits not recognized"
        ,"Stopbits not recognized"
        ,"Parity not recognized"};


char buffer[128] = {0};


using namespace godot;

void SerialDevice::_bind_methods() {
    BIND_ENUM_CONSTANT(SERIAL_DATABITS_8);
    BIND_ENUM_CONSTANT(SERIAL_DATABITS_7);
    BIND_ENUM_CONSTANT(SERIAL_DATABITS_6);
    BIND_ENUM_CONSTANT(SERIAL_DATABITS_5);
    BIND_ENUM_CONSTANT(SERIAL_STOPBITS_2);
    BIND_ENUM_CONSTANT(SERIAL_STOPBITS_1);
    BIND_ENUM_CONSTANT(SERIAL_STOPBITS_1_5);
    BIND_ENUM_CONSTANT(SERIAL_PARITY_ODD);
    BIND_ENUM_CONSTANT(SERIAL_PARITY_EVEN);
    BIND_ENUM_CONSTANT(SERIAL_PARITY_SPACE);
    BIND_ENUM_CONSTANT(SERIAL_PARITY_MARK);
    BIND_ENUM_CONSTANT(SERIAL_PARITY_NONE);

    ClassDB::bind_method(D_METHOD("get_devices"),&SerialDevice::get_devices);
    /*ClassDB::add_property("SerialDevice",
            PropertyInfo(
                Variant::ARRAY,
                "list of devices",
                PROPERTY_HINT_TYPE_STRING,
                "Test"),
            "",
            "get_devices");
            */
    ClassDB::bind_method(D_METHOD("open","device","baudrate","databits","parity","stopbits"),&SerialDevice::open);
    ClassDB::bind_method(D_METHOD("close"),&SerialDevice::close);
    ClassDB::bind_method(D_METHOD("is_open"),&SerialDevice::is_open);
    ClassDB::bind_method(D_METHOD("is_available"),&SerialDevice::is_available);
    ClassDB::bind_method(D_METHOD("flush"),&SerialDevice::flush);
    ClassDB::bind_method(D_METHOD("write_string","str"),&SerialDevice::write_string);
    ClassDB::bind_method(D_METHOD("write_bytes","byte_buf"),&SerialDevice::write_bytes);
    ClassDB::bind_method(D_METHOD("read_string","delimiter","max","timeout_ms"),&SerialDevice::read_string);
 //   ClassDB::bind_method(D_METHOD("read_bytes","byte_buf","max","timeout_ms","sleepdur_us"),&SerialDevice::read_bytes);
    ClassDB::bind_method(D_METHOD("get_device_name"),&SerialDevice::get_device_name);
             
}


SerialDevice::SerialDevice() {
	// Initialize any variables here.
}

SerialDevice::~SerialDevice() {
	// Add your cleanup here.
}


bool SerialDevice::open(String device,const unsigned int baud,
            SerialDataBits Databits ,
            SerialParity Parity ,
            SerialStopBits Stopbits ){
    int err = _device.openDevice((const char*)device.to_ascii_buffer().ptr(),baud,Databits,Parity,Stopbits);
    if(err < 0){
        ERR_PRINT_ONCE(open_fail_msg[err*-1-1]);
        WARN_PRINT_ONCE((const char*)device.to_ascii_buffer().ptr());
    }

    return (err < 0)?false:true;
}

void SerialDevice::close(){
    _device_name = "";
    _device.closeDevice();
};

bool SerialDevice::is_open(){
    return  _device.isDeviceOpen(); 
};

String SerialDevice::get_device_name(){
    return String () = _device_name;
}

bool SerialDevice::write_string(String str){
    WARN_PRINT((char*)str.to_ascii_buffer().ptrw());
    return (_device.writeString((const char*)str.to_ascii_buffer().ptr()) == 1)?true:false;
}

bool SerialDevice::write_bytes(PackedByteArray byte_buf){
    return (_device.writeBytes(byte_buf.ptr(),byte_buf.size()) == 1)?true:false;
};

String SerialDevice::read_string(String delimiter,unsigned int max,unsigned int timeout_ms){
    int res = _device.readString(buffer,delimiter.to_ascii_buffer()[0],max,timeout_ms);
    String str; 
    if(res > 0)str = buffer;
    return str;
};


int SerialDevice::read_bytes(PackedByteArray byte_buf,unsigned int max,unsigned int timeout_ms,unsigned int sleepdur_us ){
    byte_buf.resize(max);
    return _device.readBytes((void*)byte_buf.ptrw(),max,timeout_ms,sleepdur_us);
}

int SerialDevice::is_available(){
    return _device.available();
}

void SerialDevice::flush(){
    _device.flushReceiver();
}

TypedArray<String> SerialDevice::get_devices(){
TypedArray<String> arr;

#ifdef OS_Windows
 /* Windows code */
#else
    DIR *d;
    dirent *dir;
    d = opendir("/sys/class/tty");
    if(d){
        while((dir = readdir(d)) != NULL){
            if(strstr(dir->d_name,"tty") != NULL ){
                String device(dir->d_name);
                device = "/dev/" + device;
                arr.append(device);
            }
        }
        closedir(d);
    };
#endif    
    return arr;
};
