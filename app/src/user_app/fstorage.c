#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "nrf.h"
#include "nrf_soc.h"
#include "nordic_common.h"
#include "boards.h"
#include "app_timer.h"
#include "app_util.h"
#include "nrf_fstorage.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_sd.h"
#else
#include "nrf_drv_clock.h"
#include "nrf_fstorage_nvmc.h"
#endif

#define NRF_LOG_MODULE_NAME fstorage
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
NRF_LOG_MODULE_REGISTER();

static void fstorage_evt_handler(nrf_fstorage_evt_t *p_evt);

// #define WRITE_START_ADR  (BOOTLOADER_ADDRESS-0x1000)
// #define WRITE_END_ADR    (BOOTLOADER_ADDRESS-1)
#define WRITE_START_ADR 0xF7000
#define WRITE_END_ADR 0xF7FFF

static uint32_t start_addr = WRITE_START_ADR, end_addr = WRITE_END_ADR;
static bool isWriteOperation = false;

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
    {
        /* Set a handler for fstorage events. */
        .evt_handler = fstorage_evt_handler,

        /* These below are the boundaries of the flash space assigned to this instance of fstorage.
         * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
         * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
         * last page of flash available to write data. */
        .start_addr = WRITE_START_ADR,
        .end_addr = WRITE_END_ADR,
};

/* Dummy data to write to flash. */
static uint32_t m_data = 0xBADC0FFE;

/**@brief   Helper function to obtain the last address on the last page of the on-chip flash that
 *          can be used to write user data.
 */
static uint32_t nrf5_flash_end_addr_get()
{
  uint32_t const bootloader_addr = BOOTLOADER_ADDRESS;
  uint32_t const page_sz = NRF_FICR->CODEPAGESIZE;
  uint32_t const code_sz = NRF_FICR->CODESIZE;

  NRF_LOG_INFO("bootloader_addr: %x  code addr   %x ", bootloader_addr, (code_sz * page_sz));
  return (bootloader_addr != 0xFFFFFFFF ? bootloader_addr : (code_sz * page_sz));
}

/**@brief   Sleep until an event is received. */
//static void power_manage(void)
//{
//#ifdef SOFTDEVICE_PRESENT
//  (void)sd_app_evt_wait();
//#else
//  __WFE();
//#endif
//}
extern void device_code_write_finished_proc(bool result);
static void fstorage_evt_handler(nrf_fstorage_evt_t *p_evt)
{

  if (p_evt->result != NRF_SUCCESS)
  {
    NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
    if (isWriteOperation)
    {
      device_code_write_finished_proc(false);
      isWriteOperation = false;
    }
    return;
  }

  switch (p_evt->id)
  {
  case NRF_FSTORAGE_EVT_WRITE_RESULT:
  {
    isWriteOperation = false;
    device_code_write_finished_proc(true);

    NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",
                 p_evt->len, p_evt->addr);
  }
  break;

  case NRF_FSTORAGE_EVT_ERASE_RESULT:
  {
    NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",
                 p_evt->len, p_evt->addr);
  }
  break;

  default:
    break;
  }
}

static void print_flash_info(nrf_fstorage_t *p_fstorage)
{
  NRF_LOG_INFO("========| flash info |========");
  NRF_LOG_INFO("erase unit: \t%d bytes", p_fstorage->p_flash_info->erase_unit);
  NRF_LOG_INFO("program unit: \t%d bytes", p_fstorage->p_flash_info->program_unit);
  NRF_LOG_INFO("==============================");
}

//static void wait_for_flash_ready(nrf_fstorage_t const *p_fstorage)
//{
//  /* While fstorage is busy, sleep and wait for an event. */
//  while (nrf_fstorage_is_busy(p_fstorage))
//  {
//    power_manage();
//  }
//}

static nrf_fstorage_api_t *p_fs_api;

void fstorge_init(void)
{
  ret_code_t rc;
  p_fs_api = &nrf_fstorage_sd;

  start_addr = (BOOTLOADER_ADDRESS - 0x1000);
  start_addr = (0xF8000 - 0x1000);
  end_addr = (start_addr + 0xfff);

  fstorage.start_addr = start_addr;
  fstorage.end_addr = end_addr;

  NRF_LOG_INFO("fstorage init start adr= %0x end adr=%0x", fstorage.start_addr, fstorage.end_addr);
  rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);

  if (rc != NRF_SUCCESS)
  {
    NRF_LOG_INFO("nrf_fstorage_init error errcode =%0x", rc);
    APP_ERROR_CHECK(rc);
  }
  else
  {

    print_flash_info(&fstorage);
    (void)nrf5_flash_end_addr_get();
  }
}

uint32_t fstorge_read_device_code(void)
{
  ret_code_t rc;
  rc = nrf_fstorage_read(&fstorage, start_addr, &m_data, sizeof(m_data));

  if (rc != NRF_SUCCESS)
  {
    NRF_LOG_INFO("read device code error id \"%x\" form flash.", rc);
    return 0;
  }
  else
  {

    NRF_LOG_INFO("read device code value \"%x\" form flash.", m_data);
    return m_data;
  }
}

bool fstorge_write_device_code(uint32_t device_code)
{
  ret_code_t rc;
  m_data = device_code;
  /* Let's write to flash. */

  NRF_LOG_INFO("Erase...flash.");
  rc = nrf_fstorage_erase(&fstorage, start_addr, 1, NULL);

  if (rc != NRF_SUCCESS)
  {

    NRF_LOG_INFO("Erase...flash error id \"%x\" to flash.", rc);
    return false;
  }
  // APP_ERROR_CHECK(rc);
  isWriteOperation = true;
  NRF_LOG_INFO("Writing \"%x\" to flash.", m_data);
  rc = nrf_fstorage_write(&fstorage, start_addr, &m_data, sizeof(m_data), NULL);

  if (rc != NRF_SUCCESS)
  {
    //	APP_ERROR_CHECK(rc);
    NRF_LOG_INFO("write device code error id \"%x\" to flash.", rc);
    isWriteOperation = false;
    return false;
  }
  else
  {

    // wait_for_flash_ready(&fstorage);
    NRF_LOG_INFO("Done.");
    return true;
  }
}
