#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/char_string.hpp>
#include "../serialib/lib/serialib.h"

VARIANT_ENUM_CAST(SerialStopBits);
VARIANT_ENUM_CAST(SerialParity);
VARIANT_ENUM_CAST(SerialDataBits);


namespace godot {

    

class SerialDevice : public Node {
	GDCLASS(SerialDevice,Node);
    private:
    serialib _device;
    String _device_name;

    protected:
    static void _bind_methods();

    public:
    SerialDevice();
    ~SerialDevice();

    TypedArray<String> get_devices();
    bool open(String device,const unsigned int baud,
            SerialDataBits Databits = SERIAL_DATABITS_8,
            SerialParity Parity = SERIAL_PARITY_NONE,
            SerialStopBits Stopbits = SERIAL_STOPBITS_1);
    bool is_open();
    bool write_string(String str);
    bool write_bytes(PackedByteArray byte_buf);
    String read_string(String delimiter,unsigned int max,unsigned int timeout_ms);
#ifdef __unix__         
    int  read_bytes(PackedByteArray byte_buf,unsigned int max,unsigned int timeout_ms,unsigned int sleepdur_us = 100);
#elif defined(_WIN32) || defined(WIN32) 
    int  read_bytes(PackedByteArray &byte_buf,unsigned int max,unsigned int timeout_ms);
#endif
    int  is_available();
    void flush();
    void close();
    String get_device_name();
};

}

