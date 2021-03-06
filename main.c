#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#include "grab.h"
#include "ffts.h"
#include "windows.h"
#include "global.h"
#include "image.h"
#include "animation.h"
#include "commands.h"
#include "compressor.h"
#include "Connection.h"
#include "serial/serialtalk.h"
struct image *img;
int arduino;
int devider =0;
void 
calculate_fbs(int samplerate, int buffer_length)
{
  int i;
  for (i = 0; i < buffer_length /2; i++){
    double fq = i * samplerate / buffer_length;
    printf("%d\t%f Hz\n",i,fq);
  }
}
void cb_mic(struct audio_stream *as)
{
  double *freq = malloc(sizeof(double) * as->buffer_length);
  apply_window (as->audio_data, as->buffer_length);
  int tl = getfreq(as->audio_data, freq, as->buffer_length);
  double *magd = malloc(sizeof(double)* as->buffer_length);
  double max = calc_magnitude(freq, as->buffer_length, magd);
  //  printf("Got %d frequencies at %d of %d audio frames\n", tl/2, as->sample_rate, as->buffer_length);
  int mag[8];
  mag[0] = 8 + (int) round(log10(magd[1]));
  mag[1] = 8 + (int) round(log10(magd[2]));
  mag[2] = 8 + (int) round(log10(magd[4]));
  mag[3] = 8 + (int) round(log10(magd[9]));
  mag[4] = 8 + (int) round(log10(magd[21]));
  mag[5] = 8 + (int) round(log10(magd[53]));
  mag[6] = 8 + (int) round(log10(magd[171]));
  mag[7] = 8 + (int) round(log10(magd[213]));
  //  printf("%d,%d,\n%d,%d,\n%d,%d,\n%d,%d\n",mag[0],mag[1],mag[2],mag[3],mag[4],mag[5],mag[6],mag[7]);
  //  printf("--------------------------------------------------------------\n\n");

  // make animation here
  unsigned char* idata;
  devider++;
  if (devider % 2 == 0){
    devider = 0;
  switch(cmdnr){
  case 0: //do not display
    
    break;
  case 1: //anim1 + display
    fallingEdge(img, mag, 8);
    testimgdisplay(img); //<- or send to arduino (by kev)
    idata = compressData(img);
    sendData(idata, 8, arduino);
    free(idata);
    break;
  case 2: //anim2 + display
    plainDb(img, mag, 8);
    idata = compressData(img);
    sendData(idata, 8, arduino);
    free(idata);
    testimgdisplay(img); //<- or send to arduino (by kev)
    break;
  }
  

  // send img somewhere here

  printf("\n");
 
  free(freq);
  }
  
}
int
main (int argc, char *argv[])
{
  cmdnr = 2; //remove once cgi is ready (or set it to 0 better)
  img = malloc( sizeof(struct image));
  img->width = 8;
  img->height = 8;
  img->frames = 8;
  int datasize = img->width * img->height * img->frames;
  img->imgdata = calloc(datasize, sizeof(char));
  arduino = openSerial("/dev/ttyACM0");
  fftw_setup(1024);
  mic_setup (&cb_mic);
  //mic_grab;
  //calculate_fbs(48000, 1024);
  
  while (1){
    sleep (1);
    //readCommand("Kamikaze_Server", "Kamikaze_Client");
  }

  closeSerial(arduino);
  free(img);
  return 0;
}
