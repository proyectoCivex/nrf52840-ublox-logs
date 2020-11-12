
#include "CommonHeaders.h"
#include "definitions.h"
#include "SDcard.h"



/**
 * @brief  SDC block device definition
 * */
NRF_BLOCK_DEV_SDC_DEFINE(
        m_block_dev_sdc,
        NRF_BLOCK_DEV_SDC_CONFIG(
                SDC_SECTOR_SIZE,
                APP_SDCARD_CONFIG(SDC_MOSI_PIN, SDC_MISO_PIN, SDC_SCK_PIN, SDC_CS_PIN)
         ),
         NFR_BLOCK_DEV_INFO_CONFIG("Nordic", "SDC", "1.00")
);




void initializeSD ()
{
    static FATFS fs;    //Work area (filesystem object) for logical drivers
    static DIR dir;   //Direcotry object
    static FILINFO fno;   //File information structure
 
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    // Initialize FATFS disk I/O interface by providing the block device.
    static diskio_blkdev_t drives[] =
    {
            DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    NRF_LOG_INFO("Initializing disk 0 (SDC)...");
    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        initializeSD();
        NRF_LOG_INFO("Disk initialization failed.");
        //return;
    }

    NRF_LOG_INFO("Mounting volume...");
    ff_result = f_mount(&fs, "", 1);
    if (ff_result)
    {
        initializeSD();
        NRF_LOG_INFO("Mount failed.");
        return;
    }

}


void openWriteANDCloseBinary (SENSORS* data)
{
    static FIL file;  //File object
    FRESULT ff_result;
    uint32_t bytes_written;
   


    NRF_LOG_INFO("Writing to file " FILE_NAME "...");
    ff_result = f_open(&file, FILE_NAME, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Unable to open or create file: " FILE_NAME ".");
        return;
    }

    ff_result = f_write(&file, (const uint8_t *)data, sizeof(*data), (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Write failed\r\n.");
    }
    else
    {
        NRF_LOG_INFO("%d bytes written.", bytes_written);
    }

    f_close(&file);
}





void openSDFile (FIL* file, uint16_t numOfFile)
{
    FRESULT ff_result;

  //name is : "data" + "xxxxx" + ".bin" where xxxxx is numOfFile
  //example: numOfFile = 125    xxxxxx= 00125
  //4+5+4
    char fileName [14];
    char numOfFile_char [5];
    itoa(numOfFile,numOfFile_char,10);   // here 10 means decimal;

    strcpy (fileName, "data");
    strcat (fileName, numOfFile_char);
    strcat (fileName,".bin");

    NRF_LOG_INFO("Openning file "  "...");
    ff_result = f_open(file, fileName, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (ff_result != FR_OK)
    {
        openSDFile(file, numOfFile);
        NRF_LOG_INFO("Unable to open or create file: %s ", fileName);
        //return;
    }
}


void writeSDBinary (FIL* file, M_SENSORS* data)
{
    FRESULT ff_result;
    uint32_t bytes_written;
  
    //NRF_LOG_INFO("Writting to file...\r\n.");
    ff_result = f_write(file, (const uint8_t *)data, sizeof(*data), (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        writeSDBinary(file, data);
        NRF_LOG_INFO("Write failed\r\n.");
    }
    else
    {
        //NRF_LOG_INFO("%d bytes written.", bytes_written);
    }
}


void closeSDFile (FIL* file)
{
  f_close(file);
   NRF_LOG_INFO("FIle closed.");
}


void infoSD ()
{
    static DIR dir;   //Direcotry object
    static FILINFO fno;   //File information structure
    FRESULT ff_result;

    uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;
    NRF_LOG_INFO("Capacity: %d MB", capacity);

    NRF_LOG_INFO("\r\n Listing directory: /");

    //Open directory
    ff_result = f_opendir(&dir, "/");
    if (ff_result)
    {
        NRF_LOG_INFO("Directory listing failed!");
        return;
    }

    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            NRF_LOG_INFO("Directory read failed.");
            return;
        }

        if (fno.fname[0])
        {
            if (fno.fattrib & AM_DIR)
            {
                NRF_LOG_RAW_INFO("   <DIR>   %s",(uint32_t)fno.fname);
            }
            else
            {
                NRF_LOG_RAW_INFO("%9lu  %s", fno.fsize, (uint32_t)fno.fname);
            }
        }
    }
    while (fno.fname[0]);
    NRF_LOG_RAW_INFO("");
}



void OpenReadANDCloseBinary (SENSORS* data)
{

    static FIL file;  //File object
    uint32_t bytes_written;
    FRESULT ff_result;

   
    NRF_LOG_INFO("Reading from file " FILE_NAME "...");

    ff_result = f_open(&file, FILE_NAME, FA_READ);    //Only FA_READ
    //If its is FA_READ | FA_WRITE | FA_OPEN_APPEND, the point will be at the end of the file and not an the beginning
    //You could also use f_lseek function
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Unable to open or create file: " FILE_NAME ".");
        return;
    }


     /*
  FRESULT f_read (
    FIL* fp,     // [IN] File object 
    void* buff,  // [OUT] Buffer to store read data 
    UINT btr,    // [IN] Number of bytes to read
    UINT* br     // [OUT] Number of bytes read
  );*/

  UINT br;

  ff_result= f_read (&file, (uint8_t *)data, sizeof(*data), &br);


  if  ( ff_result == FR_OK ) 
  {
     NRF_LOG_RAW_INFO("Reading ok");
  }
  else if  (br == 0) 
  {
      NRF_LOG_RAW_INFO("End of file");
  }
  else
  {
      NRF_LOG_RAW_INFO(" Reading from file filed!");
  }


    f_close(&file);
}



void setFilePointatHeader (FIL* file)
{
  FRESULT ff_result;
  ff_result = f_lseek(file, 0);  //Set file header
}


int8_t readSDBinary (FIL* file, SENSORS* data)
{

    FRESULT ff_result;
     /*
  FRESULT f_read (
    FIL* fp,     // [IN] File object 
    void* buff,  // [OUT] Buffer to store read data 
    UINT btr,    // [IN] Number of bytes to read
    UINT* br     // [OUT] Number of bytes read
  );*/

  UINT br;

  ff_result= f_read (file, (uint8_t *)data, sizeof(*data), &br);


  if  (br == 0) 
  {
      NRF_LOG_RAW_INFO("End of file");
      return SD_READING_END_FILE;
  }
  
  else if  ( ff_result == FR_OK ) 
  {
     NRF_LOG_RAW_INFO("Reading ok \n");
     return SD_READING_OK;
  }
  else
  {
      NRF_LOG_RAW_INFO(" Reading from file filed!");
      return SD_READING_ERROR;

  }


}


/******************************************************************************/
// NO BINARY
// Save data with a human-readable format
/******************************************************************************/

void writeVariableSD(variable, variableLenght)
{

    static FATFS fs;    //Work area (filesystem object) for logical drivers
    static DIR dir;   //Direcotry object
    static FILINFO fno;   //File information structure
    static FIL file;      //File objects

    uint32_t bytes_written;
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;


    // Initialize FATFS disk I/O interface by providing the block device.
    static diskio_blkdev_t drives[] =
    {
            DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    //Initializing disk 0 (SDC)...
    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        NRF_LOG_INFO("Disk initialization failed.");
        return;
    }

    //Gives a work area to the default drive
    ff_result = f_mount(&fs, "", 1);
    if (ff_result)
    {
        NRF_LOG_INFO("Mount failed.");
        return;
    }


    //open or creating the file (FA_WRITE)
    //set the pointer to the end of the file (FA_OPEN_APPEND)
    ff_result = f_open(&file, FILE_NAME, FA_WRITE | FA_OPEN_APPEND | FA_READ );
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Unable to open or create file: " FILE_NAME ".");
        return;
    }

  
    //First, it is necessary to convert the (int)variable to a string
    char buffer[variableLenght+1];
    itoa(variable,buffer,10);   // here 10 means decimal;


    ff_result = f_write(&file, buffer, sizeof(buffer) - 1, (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Write failed\r\n.");
    }
    else
    {
        NRF_LOG_INFO("%d bytes written.", bytes_written);
    }


    ff_result = f_write(&file, ENDLINE, sizeof(ENDLINE) - 1, (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Write failed\r\n.");
    }
    else
    {
        NRF_LOG_INFO("%d bytes written.", bytes_written);
    }

    (void) f_close(&file);
    return;
}








void readLineSD()
{
    static FATFS fs;    //Work area (filesystem object) for logical drivers
    static DIR dir;   //Direcotry object
    static FILINFO fno;   //File information structure
    static FIL file;      //File objects

    uint32_t bytes_written;
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    char line[100]; /* Line buffer */

    // Initialize FATFS disk I/O interface by providing the block device.
    static diskio_blkdev_t drives[] =
    {
            DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    NRF_LOG_INFO("Initializing disk 0 (SDC)...");
    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        NRF_LOG_INFO("Disk initialization failed.");
        return;
    }

/*
    uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;
    NRF_LOG_INFO("Capacity: %d MB", capacity);*/

    NRF_LOG_INFO("Mounting volume...");
    ff_result = f_mount(&fs, "", 1);
    if (ff_result)
    {
        NRF_LOG_INFO("Mount failed.");
        return;
    }

/*
    NRF_LOG_INFO("\r\n Listing directory: /");

    //Open directory
    ff_result = f_opendir(&dir, "/");
    if (ff_result)
    {
        NRF_LOG_INFO("Directory listing failed!");
        return;
    }

    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            NRF_LOG_INFO("Directory read failed.");
            return;
        }

        if (fno.fname[0])
        {
            if (fno.fattrib & AM_DIR)
            {
                NRF_LOG_RAW_INFO("   <DIR>   %s",(uint32_t)fno.fname);
            }
            else
            {
                NRF_LOG_RAW_INFO("%9lu  %s", fno.fsize, (uint32_t)fno.fname);
            }
        }
    }
    while (fno.fname[0]);
    NRF_LOG_RAW_INFO("");
*/


    //Open file for reading 
    ff_result = f_open(&file, FILE_NAME, FA_READ);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Unable to open or create file: " FILE_NAME ".");
        return;
    }

    //Read a line (Read operation until a ' \n')
     f_gets(line, sizeof line, &file);


    (void) f_close(&file);

    return;

}




void readSD(int lenght)
{
    static FATFS fs;    //Work area (filesystem object) for logical drivers
    static DIR dir;   //Direcotry object
    static FILINFO fno;   //File information structure
    static FIL file;      //File objects

    uint32_t bytes_written;
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    // Initialize FATFS disk I/O interface by providing the block device.
    static diskio_blkdev_t drives[] =
    {
            DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    NRF_LOG_INFO("Initializing disk 0 (SDC)...");
    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        NRF_LOG_INFO("Disk initialization failed.");
        return;
    }

    uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;
    NRF_LOG_INFO("Capacity: %d MB", capacity);

    NRF_LOG_INFO("Mounting volume...");
    ff_result = f_mount(&fs, "", 1);
    if (ff_result)
    {
        NRF_LOG_INFO("Mount failed.");
        return;
    }

    NRF_LOG_INFO("\r\n Listing directory: /");

    //Open directory
    ff_result = f_opendir(&dir, "/");
    if (ff_result)
    {
        NRF_LOG_INFO("Directory listing failed!");
        return;
    }

    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            NRF_LOG_INFO("Directory read failed.");
            return;
        }

        if (fno.fname[0])
        {
            if (fno.fattrib & AM_DIR)
            {
                NRF_LOG_RAW_INFO("   <DIR>   %s",(uint32_t)fno.fname);
            }
            else
            {
                NRF_LOG_RAW_INFO("%9lu  %s", fno.fsize, (uint32_t)fno.fname);
            }
        }
    }
    while (fno.fname[0]);
    NRF_LOG_RAW_INFO("");


    NRF_LOG_INFO("Reading from file " FILE_NAME "...");

    ff_result = f_open(&file, FILE_NAME, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Unable to open or create file: " FILE_NAME ".");
        return;
    }


     /*
  FRESULT f_read (
    FIL* fp,     // [IN] File object 
    void* buff,  // [OUT] Buffer to store read data 
    UINT btr,    // [IN] Number of bytes to read
    UINT* br     // [OUT] Number of bytes read
  );*/

  char buffer[lenght]; 
  UINT br;

  ff_result= f_read (&file, &buffer, sizeof(buffer), &br);


  if ( ff_result == FR_OK ) 
  {
     NRF_LOG_RAW_INFO("Reading ok");
  }
  else
  {
      NRF_LOG_RAW_INFO(" Reading from file filed!");
  }


    (void) f_close(&file);

    return;


}

