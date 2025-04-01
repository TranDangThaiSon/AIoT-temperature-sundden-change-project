#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <queue>
#include <math.h>
#include <queue>
#include <cstring>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

//For DHT22
using namespace std; 

queue<float> nhiet_do;
float input_buf[] = {
    0, 0, 0, 0, 0
};
char tft[] = "0";

void temp_push(int times){
	for(int i = 0; i<times; i++){
		FILE *fp;
		char output[1035];
		float temp = 0;
		// Open a pipe to execute the Python script
		fp = popen("python dht22.py", "r");
		if (fp == NULL) {
			cout << "Failed to run Python script\n";
		}

		// Read the output from Python
		while (fgets(output, sizeof(output), fp) != NULL) {
			strcpy(tft,output);
			temp = std::stof(output);
			temp = roundf(temp * 10) / 10;
		}
		if (temp != 100){
			nhiet_do.push(temp);
		}
		else{i--;}
		// Close the pipe
		pclose(fp);
	}
}
void queue_print(){
	queue<float> temp = nhiet_do;
	while (!temp.empty()) {
        cout << temp.front() << " ";
        temp.pop();
    }
	cout << "\n";
}
void queue_cpy(){
	queue<float> temp = nhiet_do;
	for(int i = 0; i<5; i++) {
        input_buf[i] = temp.front();
        temp.pop();
    }
}
// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

// Raw features copied from test sample (Edge Impulse > Model testing)
/*static float input_buf[] = {
    
};*/

//FOR ILI9341
void normal_display(){
	string python= "python tft.py ";
	python = python.append(tft);
	char cmd[python.length() + 1];
	auto first = python.begin();
	auto last = python.end();
	copy(first, last, cmd);
	cmd[python.length()] = '\0';
	system(cmd);
}
void anomaly_display(){
	string python= "python tft_anomaly.py ";
	python = python.append(tft);
	char cmd[python.length() + 1];
	auto first = python.begin();
	auto last = python.end();
	copy(first, last, cmd);
	cmd[python.length()] = '\0';
	system(cmd);
}
int main(int argc, char **argv) {
	temp_push(5);
    while(1){
		temp_push(1);
		nhiet_do.pop();
		queue_cpy();
		signal_t signal;            // Wrapper for raw input buffer
		ei_impulse_result_t result; // Used to store inference output
		EI_IMPULSE_ERROR res;       // Return code from inference
		// Calculate the length of the buffer
		size_t buf_len = sizeof(input_buf) / sizeof(input_buf[0]);

		// Make sure that the length of the buffer matches expected input length
		if (buf_len != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
			printf("ERROR: The size of the input buffer is not correct.\r\n");
			printf("Expected %d items, but got %d\r\n", 
					EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, 
					(int)buf_len);
			return 1;
		}

		// Assign callback function to fill buffer used for preprocessing/inference
		signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
		signal.get_data = &get_signal_data;

		// Perform DSP pre-processing and inference
		res = run_classifier(&signal, &result, false);

		// Print return code and how long it took to perform inference
		/*printf("run_classifier returned: %d\r\n", res);
		printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n", 
				result.timing.dsp, 
				result.timing.classification, 
				result.timing.anomaly);*/

		// Print the prediction results (object detection)
	#if EI_CLASSIFIER_OBJECT_DETECTION == 1
		printf("Object detection bounding boxes:\r\n");
		for (uint32_t i = 0; i < EI_CLASSIFIER_OBJECT_DETECTION_COUNT; i++) {
			ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
			if (bb.value == 0) {
				continue;
			}
			printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n", 
					bb.label, 
					bb.value, 
					bb.x, 
					bb.y, 
					bb.width, 
					bb.height);
		}

		// Print the prediction results (classification)
	#else
		printf("Predictions: ");
		if(result.classification[0].value > result.classification[1].value){
			printf("%s\n", ei_classifier_inferencing_categories[0]);
			anomaly_display();
		}
		else{
			printf("%s\n", ei_classifier_inferencing_categories[1]);
			normal_display();
		}
	#endif

		// Print anomaly result (if it exists)
	#if EI_CLASSIFIER_HAS_ANOMALY == 1
		printf("Anomaly prediction: %.3f\r\n", result.anomaly);
	#endif
		sleep(5);
	}
	return 0;
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (input_buf + offset)[i];
    }

    return EIDSP_OK;
}
