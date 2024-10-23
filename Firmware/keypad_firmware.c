/* This code is for the keypad firmware for the Hampod Program
* Written by Brendan Perez
* Last modified on 10/23/2023
*/

extern pid_t controller_pid;

char numpad_symbols[] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};//Symbols on the numpad

unsigned char keypad_running = 1;

pthread_mutex_t keypad_queue_lock;
pthread_mutex_t keypad_queue_available;

void *keypad_io_thread(void* arg);

int readNumPad(){
	/* This function reads the numpad and returns what button was pressed.
	*  This function assumes that the numpad is configured with the rows as outputs and columns as inputs.
	*  The columns are tied HIGH through an internal pull-up resistor, so if a column reads as low, then that row-column pair button was pressed
	*/
	int pressed = 0;//Used to keep track of how many buttons are pressed
	int pressedNum = '-';//Value to return. -1 corresponds to an invalid input/no input
	int rows[4] = {R1,R2,R3,R4};
	int cols[4] = {C1,C2,C3,C4};
	for(int i = 0; i < 4; i++){
		digitalWrite(rows[i], LOW);//Make a row logic low
		for(int j = 0; j < 4; j++){
			if(digitalRead(cols[j]) == LOW){//if a column is a logic low, then the button on that row, column was pressed
				pressed++;
				pressedNum = 4*i + j;//Map the button to a number
			}
		}
		digitalWrite(rows[i], HIGH);//Set the row back to high to not interfere with other rows
	}
	if(pressed > 1 || pressedNum == '-'){
		return '-';//Return invalid if multiple buttons are pressed
	}else{
		return numpad_symbols[pressedNum];
	}
}

// Debug print statements from this process are White (\033[0;m)
void keypad_process(){

    KEYPAD_PRINTF("Keypad reader process launched\n");

	wiringPiSetup();
	pinMode(7, OUTPUT);//LED for Testing, on Pin GPIOG11
	pinMode(R1, OUTPUT);
	pinMode(R2, OUTPUT);
	pinMode(R3, OUTPUT);
	pinMode(R4, OUTPUT);
	pinMode(C1, INPUT);
	pinMode(C2, INPUT);
	pinMode(C3, INPUT);
	pinMode(C4, INPUT);
	pullUpDnControl(C1, PUD_UP);//Set pull-up resistors
	pullUpDnControl(C2, PUD_UP);
	pullUpDnControl(C3, PUD_UP);
	pullUpDnControl(C4, PUD_UP);

    KEYPAD_PRINTF("Connecting to input/output pipes\n");


    int input_pipe_fd = open(KEYPAD_I, O_RDONLY);
    if(input_pipe_fd == -1) {
        perror("open");
        // kill(controller_pid, SIGINT);
        exit(0);
    }

    int output_pipe_fd = open(KEYPAD_O, O_WRONLY);
    if(output_pipe_fd == -1) {
        perror("open");
        // kill(controller_pid, SIGINT);
        exit(0);
    }
    
	KEYPAD_PRINTF("Pipes successfully connected\n");
    KEYPAD_PRINTF("Creating input queue\n");

    Packet_queue* input_queue = create_packet_queue();

    KEYPAD_PRINTF("Creating queue mutex lock\n");

    if(pthread_mutex_init(&keypad_queue_lock, NULL) != 0) {
        perror("pthread_mutex_init");
        // kill(controller_pid, SIGINT);
        exit(1);
    }
    KEYPAD_PRINTF("Creating queue availibility mutex lock\n");

    if(pthread_mutex_init(&keypad_queue_available, NULL) != 0) {
        perror("pthread_mutex_init");
        // kill(controller_pid, SIGINT);
        exit(1);
    }
    
    pthread_t keypad_io_buffer;

    keypad_io_packet thread_input;
    thread_input.pipe_fd = input_pipe_fd;
    thread_input.queue = input_queue;

    KEYPAD_PRINTF("Launching IO thread\n");

    if(pthread_create(&keypad_io_buffer, NULL, keypad_io_thread, (void*)&thread_input) != 0){
        perror("Keypad IO thread failed");
        // kill(controller_pid, SIGINT);
        exit(1);
    }
    usleep(500000); //Half sec sleep to let child thread take control
	while(keypad_running){
		pthread_mutex_lock(&keypad_queue_available);
        pthread_mutex_lock(&keypad_queue_lock);
        if(is_empty(input_queue)) {    
		    pthread_mutex_unlock(&keypad_queue_available);
            pthread_mutex_unlock(&keypad_queue_lock);
            usleep(500);
            continue;
        }
        Inst_packet* received_packet = dequeue(input_queue);
        pthread_mutex_unlock(&keypad_queue_lock);
        pthread_mutex_unlock(&keypad_queue_available);
        char read_value = -1;
        if (received_packet->data[0] == 'r') {
            read_value = readNumPad();
        }

        Inst_packet* packet_to_send = create_inst_packet(KEYPAD, 1, (unsigned char*)&read_value, received_packet->tag);
        KEYPAD_PRINTF("Sending back value of %x ('%c')\n", read_value, (char) read_value);
        write(output_pipe_fd, packet_to_send, 8);
        write(output_pipe_fd, packet_to_send->data, 1);

        destroy_inst_packet(&packet_to_send);
	}
    pthread_join(keypad_io_buffer, NULL);
    destroy_queue(input_queue);
    close(input_pipe_fd);
    close(output_pipe_fd); //Graceful closing is always nice :)
    return;
}

//Debug print statements from this thread are Cyan (\033[0;96m)
void *keypad_io_thread(void* arg) {
    
    KEYPAD_IO_PRINTF("Keypad IO thread created\n");

    keypad_io_packet* new_packet =  (keypad_io_packet*)arg;
    int i_pipe = new_packet->pipe_fd;
    Packet_queue* queue = new_packet->queue;
    unsigned char buffer[256];

    KEYPAD_IO_PRINTF("Input pipe = %d, queue ptr = %p\n", i_pipe, queue);

    while(keypad_running) {
        pthread_mutex_lock(&keypad_queue_lock);
        int queue_empty = is_empty(queue);
        pthread_mutex_unlock(&keypad_queue_lock);
        if(queue_empty) {
        KEYPAD_IO_PRINTF("Making queue inaccessible\n");
        pthread_mutex_lock(&keypad_queue_available);
        }

        Packet_type type;
        unsigned short size;
        unsigned short tag;
        read(i_pipe, &type, sizeof(Packet_type));
        read(i_pipe, &size, sizeof(unsigned short));
        read(i_pipe, &tag, sizeof(unsigned short));
        read(i_pipe, buffer, size);

        KEYPAD_IO_PRINTF("Found packet with type %d, size %d\n", type, size);
        KEYPAD_IO_PRINTF("Buffer holds: %s: with size %d\n", buffer, size);

        if(type != KEYPAD) {
            KEYPAD_IO_PRINTF("Packet not supported for Keypad firmware\n");
            continue;
        }

        Inst_packet* new_packet = create_inst_packet(type, size, buffer, tag);

        KEYPAD_IO_PRINTF("Locking queue\n");
        pthread_mutex_lock(&keypad_queue_lock);

        KEYPAD_IO_PRINTF("Queueing packet\n");
        enqueue(queue, new_packet);

        KEYPAD_IO_PRINTF("Releasing queue & making it accessible\n");
        pthread_mutex_unlock(&keypad_queue_lock);
        if(queue_empty) {
            pthread_mutex_unlock(&keypad_queue_available);
        }
        usleep(100);
    }
    return NULL;
}

void keypadTurnon(){
    wiringPiSetup();
	pinMode(7, OUTPUT);//LED for Testing, on Pin GPIOG11
	pinMode(R1, OUTPUT);
	pinMode(R2, OUTPUT);
	pinMode(R3, OUTPUT);
	pinMode(R4, OUTPUT);
	pinMode(C1, INPUT);
	pinMode(C2, INPUT);
	pinMode(C3, INPUT);
	pinMode(C4, INPUT);
	pullUpDnControl(C1, PUD_UP);//Set pull-up resistors
	pullUpDnControl(C2, PUD_UP);
	pullUpDnControl(C3, PUD_UP);
	pullUpDnControl(C4, PUD_UP);
}