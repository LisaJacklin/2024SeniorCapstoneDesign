//EE 482: ECE Senior Capstone Design
//Last Updated: 2/12/2024
//Team Name: MAD
//Team Members: Elizabeth Lee, Sheridan Blasey, Daniel Kling, Lisa Jacklin

/* 
Musculoskeletal Skeletal Device 

Overview:The goal of this project is to develop a device that will allow people to accurately assess their muscle 
 strength with and without the help of a healthcare provider. The importance of this project is to provide people who
 need muscle rehabilitation with an easier way to track progress. This will be accomplished via an electromyography (EMG) 
 sensor. An EMG sensor senses the electrical signal sent to the muscles from the brain and quantifies it. The user will attach 
 the EMG sensor and contract their bicep while holding a resistance band. If the person has greater muscle strength the brain
 will have to “recruit” less muscle fibers and will thus result in a smaller signal on the EMG which will give us our muscle 
 rating score. The expected outcomes of the project are to create a mobile device that can be deployed in the field or in 
 patient homes that can measure muscle and joint strength for use in tracking growth, aging, and disease progression.

Objectives:

Notes:

*/

////////LIBRARIES////////////////
#include <msp430.h>


void main(void) {


 _EINT(); //this allows for interrupts! vv important!
 LPM0; //low power modes if needed...leaving for now
}

//skeleton service request, need to change port number to what we are actually using!
/////////INTERRUPTS///////////////
void InterruptService_Name(void __interrupt [PORT2_VECTOR] {


}