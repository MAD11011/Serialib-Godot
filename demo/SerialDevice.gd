extends SerialDevice


# Called when the node enters the scene tree for the first time.
func _ready():
	var devices = get_devices()
	var device
	for dev in devices:
		if dev.contains("USB"):
			device = dev
			break
	print(device)	
	var err = open(device,115200,SERIAL_DATABITS_8,SERIAL_PARITY_NONE,SERIAL_STOPBITS_1)
	print(err)
	OS.delay_msec(250)
	print(write_string("Hello from Godot"))
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.

func _process(delta):

	if is_available():
		print(read_string("\n",32,100))

	pass
	
