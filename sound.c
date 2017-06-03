#include <rtmidi_c.h>
#include <math.h>
#include <stdio.h>
#include "sound.h"
#define A_HERTZ 440
#define CLOCK_FREQ 50000000.0/64.0
void exit(int);
uint16_t dividers[4] = {0};
uint8_t notes[4] = {0};
RtMidiOutPtr out;
uint8_t get_note_from_divider(uint16_t divider){
    float frequency = CLOCK_FREQ/(float) divider;
    //printf("Frequency %f\n",frequency);
    return log2f(frequency/A_HERTZ)*12.0+69.0;

}
uint16_t get_bend_from_divider(uint16_t divider, uint8_t note){
    float f_note = A_HERTZ * powf(2,(note-69.0)/12.0);
    float frequency = CLOCK_FREQ/(float) divider;
    float cents = (float)(log2f(frequency/f_note) *1200.0);
    //printf("cents: %f\n",cents);
    return (8192.0 * (cents/200.0)) + 0x2000;
}
void init_sound(){
    out = rtmidi_out_create_default();
    int ports = rtmidi_get_port_count(out);
    if(ports >= 2)
        rtmidi_open_port(out,1,"port");
    else
        rtmidi_open_port(out,0,"port");
    unsigned char message[4];
    /*message[0] = 192;*/
    /*message[1] = 5;*/
    /*rtmidi_out_send_message(out,message,2);*/
    /*message[0] = 176;*/
    /*message[1] = 7;*/
    /*message[2] = 100;*/
    /*rtmidi_out_send_message(out,message,3);*/

}
void cleanup_sound(){
    uint8_t message[4] = {0xb0,120,0x00};
    for(int i=0;i<4;i++){
        message[0] = 0xb0+i;
        rtmidi_out_send_message(out,message,3);
    }
    rtmidi_out_free(out); 
    
}
void sound_set(uint16_t val){
    //printf("0x%04x\n",val);
    unsigned char message[4];
    uint8_t note = 0;
    uint16_t bend = 0;
    int index = val >> 14;
    dividers[index] = val & 0x3fff;
    if(val&0x3fff){
        note = get_note_from_divider(val&0x3fff);
        //printf("note: %d\n",note);
        bend = get_bend_from_divider(val&0x3fff,note);
    }
    
    if(notes[index] != note){
        message[0] = 128+index;
        message[1] = notes[index];
        message[2] = 0;
        rtmidi_out_send_message(out,message,3);
        if((val&0x3fff) != 0 && note < 90){
            message[0] = 144+index;
            message[1] = note;
            message[2] = 127;
            rtmidi_out_send_message(out,message,3);
            /*bend = 0x2000;*/
            notes[index] = note;
            message[0] = 0xe0+index;
            message[1] = bend & 0x7f;
            message[2] = (bend>>7) & 0x7f;
            rtmidi_out_send_message(out,message,3);
        }
    }
}
