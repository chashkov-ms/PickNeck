#include "fatfs.h"
#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define FILE_NAME   "NORDIC.TXT"
#define TEST_STRING "SD card example."
#define FILE_NAMES   "nRF52832.TXT"


static FATFS    fs;
static DIR      dir;
static FILINFO  fileInfo;
static FIL      file;
static FRESULT         ff_result;
static DSTATUS         disk_state = STA_NOINIT;
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


/**
 * @brief Function for scan directory
 */
void listing_dir(void)
{
	// char *fn;
	NRF_LOG_INFO("Listing directory: /");
	ff_result = f_opendir(&dir, "/");
	if (ff_result)
	{
		NRF_LOG_INFO("Directory listing failed!");
		return;
	}

	do
	{
		ff_result = f_readdir(&dir, &fileInfo);
		if (ff_result != FR_OK)
		{
			NRF_LOG_INFO("Directory read failed.");
			return;
		}
		// fn = fileInfo.fname;
		if (fileInfo.fname[0])
		{
			if (fileInfo.fattrib & AM_DIR)
			{
				NRF_LOG_INFO("   <DIR>   %s", (uint32_t)fileInfo.fname);
			}
			else
			{
				NRF_LOG_INFO("%9lu  %s", fileInfo.fsize, (uint32_t)fileInfo.fname);
			}
		}
	}
	while (fileInfo.fname[0]);
	NRF_LOG_INFO("");
}


/**
 * @brief Function for demonstrating FAFTS usage.
 */
void fatfs_example(void)
{

	uint32_t bytes_written;

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

	listing_dir();

	NRF_LOG_INFO("Writing to file " FILE_NAME "...");
	ff_result = f_open(&file, FILE_NAME, FA_READ | FA_WRITE | FA_OPEN_APPEND);
	if (ff_result != FR_OK)
	{
		NRF_LOG_INFO("Unable to open or create file: " FILE_NAME ".");
		return;
	}

	ff_result = f_write(&file, TEST_STRING, sizeof(TEST_STRING) - 1, (UINT *) &bytes_written);
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


