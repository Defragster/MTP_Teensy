/*
  LittleFS MTP Simple Example.

  Illustrates setting up MTP drives for LittleFS_Program, LittleFS_RAM and
  flash on SPI with CS = 6 for a Teensy 4, 4.1 and micromod.

  T3.x's do not support LittleFS_Program so the example just sets up MTP disk
  for using Ram and Flash.

  The key points in settup up to run MTP using LittleFS are:
  1. Specifing #include <MTP_Teensy.h> in the header
  2. Add the MTP objects:
        MTPStorage storage;
        MTPD mtpd(&storage);
  3. Telling the sketch to start MTP in setup before doing anything else with MTP 
     storage objects with
        mtpd.begin();
  4. After starting an instance of LittleFS for the device as shown in detail in
     the LittleFS examples using storage.addFilesystem method to tell MTP to associate
     a file system with the LittleFS drive:
        storage.addFilesystem(LittleFS Object, Name you assign to drive)
  5. Adding metp.loop() as part of the sketch loop
  
  This example code is in the public domain.
*/

#include <MTP_Teensy.h>
#include <LittleFS.h>


// Add in MTPD objects
MTPStorage storage;
MTPD mtpd(&storage);

// Select Memory Storage for LittleFS
/* RAM and Program Disk requires user to specify amount of memory 
 * available for the disk.
 * 
 * RAM DISK:
 *   On the Teensy 4.1 RAM can be assigned to QSPI PSRAM using the following format
 *   ramDisk.begin(1000000);
 *   which assigns 1Mbyte of PSRAM for the RAM disk on Teensy 3.x and Teensy Micromod/Teensy 4.0
 *   so would highly recommend using Program Memory only.
 *
 *
 * PROGRAM DISK:
 *   On the Teensy 4.0, 4.1 and Micromod program can persist across restarts, however, the total available 
 *   memory varies depending whether you are running them in normal or secure mode.
 *    Board            Normal  Secure
 *    -----            ------  ------
 *    Teensy 4.0       1472K   960K
 *    Teensy 4.1       7424K   6912K  
 *    MicroMod Teensy  15616K  15104K
 *
 *  Note:  If you are using Program memory for other things the maximum available will be reduced.
 */
LittleFS_RAM        ramdisk;
LittleFS_Program    progdisk;

LittleFS_SPIFlash   sFlash;  
//LittleFS_SPINAND    sNand;
//LittleFS_SPIFram    sFram;

// QSPI devices are only available on the Teensy 4.1
//LittleFS_QSPIFlash  qFlash;
//LittleFS_QPINAND    qNand;


/*** Chip Select for SPI Memory chips ***/
/* Note if you have multiple devices attached you would
 * specify different chip select pins for each device.
 * CS pin can be any digital pin.
 */
const int csFlash = 6;
//const int csNand  = 6;
//const int csFram  = 6;

void setup()
{

  while (!Serial && millis() < 5000)  {}

  delay(5000);
  Serial.print(CrashReport);
  Serial.println("\n" __FILE__ " " __DATE__ " " __TIME__);  

  //This is mandatory to begin the mtpd session.
  mtpd.begin();

#if defined(__IMXRT1062__)
  //***** RAM Disk   ****/
  // If you are using a Teensy 4.1 with PSRAM you can set up a single ram disk:
  // ramdisk.begin(2000000);  //Setups a 1MB ram disk in PSRAM
  // storage.addFilesystem(ramdisk, "ramDisk");
  
  //*****  Program disk   ****/
  // similar to Ramdisk:
   progdisk.begin(1024*256);   // minimum program disk size is 256k.
   storage.addFilesystem(progdisk, "progdisk");
#else
 // if you are using a T3.x a RAM disk can only be set up a disk using
 // BUFFER in RAM1, can also be used for Teensy 4.x and TMM
 char buf[ 120 * 1024 ]; // BUFFER in RAM1
   ramdisk.begin(buf, sizeof(buf));
   storage.addFilesystem(ramdisk, "ramdisk");
#endif
  
  /*****  SPI Flash  ****/
  // note the initialization pattern
   sFlash.begin(csFlash, SPI);
   storage.addFilesystem(sFlash, "sflash");
   
  /***** SPI NAND ****/
  // sNand.begin(csNand, SPI);
  // storage.addFilesystem(sNand, "sNand");
  
  /***** SPI FRAM ****/
  // sFram.begin(csFram, SPI);
  // storage.addFilesystem(sFram, "sFram");
  
  // QSPI follows the same pattern of starting the device with the appropriate begin 
  // just as you would for a LittleFS device without MTP.  
  // Next for the device you identified you tell it to assign a file system with the
  // addFilesystem command.
  
  /***** QSPI Flash ****/
  // qFlash.begin();
  // storage.addFilesystem(qFlash, "sNand");
  
  /***** QSPI Nand ****/
  // sNand.begin();
  // storage.addFilesystem(sNand, "sNand");

  // sets the storage for the index file
  storage.setIndexStore(0);
  Serial.println("\nSetup done");

}

void loop() {
  mtpd.loop();  //This is mandatory to be placed in the loop code.
}
