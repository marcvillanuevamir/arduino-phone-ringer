# arduino-phone-ringer
Ring the bell of old telephones with an Arduino UNO and a L298N

For this project, I wanted to trigger the bell of 3 old cord telephones. I had great support from the Arduino community, which pointed to many hints on how to achieve this. You can retrieve them from this thread: https://forum.arduino.cc/t/ringing-old-phones-from-amplified-audio-amplification-issues/1407291/15

The challenge with these old phones is that they (at least in Europe) expect a wave of 20Hz at approximately 48VAC in order to go on. I used a wall wart mount AC-AC adapter (rated 230VAC - 9VAC ** please note that the adapter should be AC, not DC **) to step up my audio output to a voltage that the telephone could react on. Whereas this is straight-forward with a computer, using a DAW for the wave generation and just passing that audio signal to the telephone through the AC-AC adapter, it gets more tricky if you want to use an Arduino instead, because of the significantly lower output voltage of the Arduino pins.

I found the simplest solution using a L298N driver, which acts as a H-Bridge and transforms the DC input voltage of the Arduino into an AC wave. The shifting speed of pins INA1 and INA2 determines the frequency of the wave, so you can achieve a 20Hz square wave quite easily. The L298N output can go then through the adapter to reach a voltage that is enough for the bells to sound. ** Once picked up, the voltage drops significantly, so you might need to adjust your own setup depending on whether you want to pick them up or not **

I attach a simple code for ringing a single phone, and the code I used in a theatre performance, where I wanted to control 3 different phones and had 3 scenes. I controlled them with a solid state relay, which was perfectly silent and did not interfere at all with the sound of the phones.

<img width="1399" height="859" alt="Screenshot 2025-09-25 at 10 41 58" src="https://github.com/user-attachments/assets/2e498377-9be7-4e92-877c-bd86c7c2fece" />
