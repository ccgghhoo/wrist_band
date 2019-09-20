

#include "role_config.h"

#if (BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE)

  #include <stdint.h>
  #include <string.h>

  #include "nrf.h"
  #include "ble.h"
  #include "ble_hci.h"
  #include "ble_gap.h"
  #include "ble_advdata.h"
  #include "ble_advertising.h"
  #include "peer_manager.h"
  #include "ble_radio_notification.h"

  #include "ble_conn_params.h"
  #include "ble_conn_state.h"
  #include "nrf_ble_gatt.h"
  #include "softdevice_handler.h"
  #include "app_error.h"
  #include "fstorage.h"
  #include "fds.h"
  #include "app_timer.h"

  #define NRF_LOG_MODULE_NAME "APP"
  #include "nrf_log.h"
  #include "nrf_log_ctrl.h"

  #include "ble_dis.h"
  #include "app_bas.h"
  #include "app_nus.h"

  #include "peripheral_role.h"
  #include "gsflag.h"
  #include "dev_config.get.h"
  #include "crc16.h"
  #include "battery.h"
  #include "indicator.h"
  #include "ble_conn_state.h"
  #include "peer_manager.h"
  #include "scan_Adv.h"

  #if 1
    #include "nrf_log.h"
    #include "nrf_log_ctrl.h"
    #define DEBUG       NRF_LOG_INFO
  #else
    #define DEBUG(...)
  #endif


  #define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}


  #define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

 // #define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
//  #define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/
//  #define APP_CONN_CFG_TAG                1                                           /**< A tag that refers to the BLE stack configuration we set with @ref sd_ble_cfg_set. Default tag is @ref BLE_CONN_CFG_TAG_DEFAULT. */

//#define CONN_CFG_TAG                    1                                           /**< A tag that refers to the BLE stack configuration we set with @ref sd_ble_cfg_set. Default tag is @ref BLE_CONN_CFG_TAG_DEFAULT. */

  #define CENTRAL_LINK_COUNT              1                                           /**< We will connect to two peers as a central (one link to a heart rate sensor and one link to running speed sensor). */
  #define PERIPHERAL_LINK_COUNT           1                                           /**< We will only connect to one peer as a peripheral. */
  #define LINK_TOTAL                      PERIPHERAL_LINK_COUNT + CENTRAL_LINK_COUNT
  #define APP_CONN_CFG_TAG                1                                           /**< A tag that refers to the BLE stack configuration we set with @ref sd_ble_cfg_set. Default tag is @ref BLE_CONN_CFG_TAG_DEFAULT. */


  #if	defined( BOARD_EV07B  )
    #define DEVICE_NAME                     "EV07B"                           /**< Name of device. Will be included in the advertising data. */
  #elif defined( BOARD_EV201  )
    #define DEVICE_NAME						"EV201"
  #else
    #define DEVICE_NAME						"unknow"
  #endif

  #define MANUFACTURER_NAME               "www.abcd.com"                       /**< Manufacturer. Will be passed to Device Information Service. */
  #define APP_ADV_INTERVAL                160                                        /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
  #define APP_ADV_TIMEOUT_IN_SECONDS      15                                         /**< The advertising timeout in units of seconds. */
  #define APP_ADV_INTERVAL_SLOW			800		//1 seconds
  #define APP_ADV_TIMEOUT_IN_SECONDS_SLOW	30


  #define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */

  #define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.1 seconds). */
  #define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.2 second). */
  #define SLAVE_LATENCY                   0                                           /**< Slave latency. */
  #define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(2000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

  #define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
  #define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
  #define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

  #define SEC_PARAM_BOND                  0                                           /**< Perform bonding. */
  #define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
  #define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
  #define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
  #define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
  #define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
  #define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
  #define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */



  #define SCAN_INTERVAL                   0x00A0                                      /**< Determines scan interval in units of 0.625 millisecond. */
  #define SCAN_WINDOW                     0x00A0                                      /**< Determines scan window in units of 0.625 millisecond. */
  #define SCAN_TIMEOUT                    0

static uint16_t 			m_conn_handle 		= BLE_CONN_HANDLE_INVALID;                            /**< Handle of the current connection. */
static nrf_ble_gatt_t     	m_gatt;                       /**< GATT module instance. */
static ble_uuid_t 			m_adv_uuids[] = { { BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE } }; /**< Universally unique service identifiers. */
static bl_user_data_t       m_user_data = { 0 };
static bool                 m_is_fast_conn_mode = false;
static uint16_t             m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;  /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

  #define MANUFACTURE_NAME				"Eview"
  #define FIRMWARE_REVISION				"v0.1"

const ble_gap_conn_params_t fast_conn_params =
{
  .min_conn_interval = MSEC_TO_UNITS(8, UNIT_1_25_MS),
  .max_conn_interval = MSEC_TO_UNITS(12, UNIT_1_25_MS),
  .slave_latency = 0,
  .conn_sup_timeout = MSEC_TO_UNITS(1000, UNIT_10_MS)
};

const ble_gap_conn_params_t slow_conn_param =
{
  .min_conn_interval = MSEC_TO_UNITS(100, UNIT_1_25_MS),
  .max_conn_interval = MSEC_TO_UNITS(200, UNIT_1_25_MS),
  .slave_latency = 0,
  .conn_sup_timeout = MSEC_TO_UNITS(4000, UNIT_10_MS)
};


/**@brief Parameters used when scanning.
 */
static ble_gap_scan_params_t const m_scan_params =
{
  .active   = 1,
  .interval = SCAN_INTERVAL,
  .window   = SCAN_WINDOW,
  .timeout  = SCAN_TIMEOUT,
#if (NRF_SD_BLE_API_VERSION <= 2)
  .selective   = 0,
  .p_whitelist = NULL,
#endif
#if (NRF_SD_BLE_API_VERSION >= 3)
  .use_whitelist = 0,
#endif
};



/**
 * scan adv
 */
  #define SCAN_ADV_TIME (200)
  #define SCAN_ADV_PRIOD (60000)


// ========================================================================================


void ble_slow_connection(void)
{
  (void)ble_conn_params_change_conn_params((ble_gap_conn_params_t *)&slow_conn_param);
  m_is_fast_conn_mode = false;
  DEBUG("[BLE]: slow connection\r\n");
}
void ble_fast_connection(void)
{
  if (m_is_fast_conn_mode == false)
  {
    m_is_fast_conn_mode = true;
    (void)ble_conn_params_change_conn_params((ble_gap_conn_params_t *)&fast_conn_params);
    DEBUG("[BLE]: fast connection\r\n");
  }
}


static void ble_dis_init_func(void)
{
  ble_dis_init_t 		dis_init_obj;

  memset(&dis_init_obj, 0, sizeof(dis_init_obj));
  dis_init_obj.manufact_name_str.p_str 	= (uint8_t *)MANUFACTURE_NAME;
  dis_init_obj.manufact_name_str.length 	= strlen(MANUFACTURE_NAME);

  dis_init_obj.fw_rev_str.p_str			= (uint8_t *)FIRMWARE_REVISION;
  dis_init_obj.fw_rev_str.length			= strlen(FIRMWARE_REVISION);

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init_obj.dis_attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init_obj.dis_attr_md.write_perm);

  uint32_t err_code = ble_dis_init(&dis_init_obj);
  if (err_code != NRF_SUCCESS)
  {
    APP_ERROR_CHECK(err_code);
  }
}


static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
  switch (ble_adv_evt)
  {
      default:
        break;

      case BLE_ADV_EVT_IDLE:    //set slow advertising
        APP_ERROR_CHECK(ble_advertising_start(BLE_ADV_MODE_SLOW));
        break;
  }
}

static void ble_services_init(void)
{
  ble_dis_init_func();

  app_bas_init();

  app_nus_init();
}

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t *p_ble_evt)
{
  uint32_t err_code = NRF_SUCCESS;

  switch (p_ble_evt->header.evt_id)
  {
      case BLE_GAP_EVT_DISCONNECTED:

        DEBUG("Disconnected, reason = %d.\r\n", p_ble_evt->evt.gap_evt.params.disconnected.reason);

        APP_ERROR_CHECK(err_code);
        loc_set_ble_connect_flag(false);

        m_is_fast_conn_mode = false;
        break; // BLE_GAP_EVT_DISCONNECTED

      case BLE_GAP_EVT_CONNECTED:
        DEBUG("Connected.\r\n");
        APP_ERROR_CHECK(err_code);
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        loc_set_ble_connect_flag(true);

        //(void)ble_advertising_start( BLE_ADV_MODE_SLOW );
        break; // BLE_GAP_EVT_CONNECTED

      case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        // Pairing not supported
        err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
        APP_ERROR_CHECK(err_code);
        break; // BLE_GAP_EVT_SEC_PARAMS_REQUEST

      case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
        {
          ble_gap_data_length_params_t dl_params;

          // Clearing the struct will effectivly set members to @ref BLE_GAP_DATA_LENGTH_AUTO
          memset(&dl_params, 0, sizeof(ble_gap_data_length_params_t));
          err_code = sd_ble_gap_data_length_update(p_ble_evt->evt.gap_evt.conn_handle, &dl_params, NULL);
          APP_ERROR_CHECK(err_code);
        }
        break;

      case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        // No system attributes have been stored.
        err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
        APP_ERROR_CHECK(err_code);
        break; // BLE_GATTS_EVT_SYS_ATTR_MISSING

      case BLE_GATTC_EVT_TIMEOUT:
        // Disconnect on GATT Client timeout event.
        DEBUG("GATT Client Timeout.\r\n");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break; // BLE_GATTC_EVT_TIMEOUT

      case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server timeout event.
        DEBUG("GATT Server Timeout.\r\n");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break; // BLE_GATTS_EVT_TIMEOUT

      case BLE_EVT_USER_MEM_REQUEST:
        err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
        APP_ERROR_CHECK(err_code);
        break; // BLE_EVT_USER_MEM_REQUEST

      case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
          ble_gatts_evt_rw_authorize_request_t  req;
          ble_gatts_rw_authorize_reply_params_t auth_reply;

          req = p_ble_evt->evt.gatts_evt.params.authorize_request;

          if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
          {
            if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ) ||
                (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
            {
              if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
              {
                auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
              } else
              {
                auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
              }
              auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
              err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                         &auth_reply);
              APP_ERROR_CHECK(err_code);
            }
          }
        }
        break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

      default:
        // No implementation needed.
        break;
  }
}

/**@brief Function for handling BLE Stack events concerning central applications.
 *
 * @details This function keeps the connection handles of central applications up-to-date. It
 * parses scanning reports, initiating a connection attempt to peripherals when a target UUID
 * is found, and manages connection parameter update requests. Additionally, it updates the status
 * of LEDs used to report central applications activity.
 *
 * @note        Since this function updates connection handles, @ref BLE_GAP_EVT_DISCONNECTED events
 *              should be dispatched to the target application before invoking this function.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_central_evt(const ble_evt_t *const p_ble_evt)
{
  const ble_gap_evt_t   *const p_gap_evt = &p_ble_evt->evt.gap_evt;
  ret_code_t                    err_code=0;
  err_code =err_code;

  switch (p_ble_evt->header.evt_id)
  {
    // Upon connection, check which peripheral has connected (HR or RSC), initiate DB
    // discovery, update LEDs status and resume scanning if necessary.
      case BLE_GAP_EVT_CONNECTED:
        {
          NRF_LOG_INFO("Central Connected \r\n");


        }
        break; // BLE_GAP_EVT_CONNECTED

        // Upon disconnection, reset the connection handle of the peer which disconnected,
        // update the LEDs status and start scanning again.
      case BLE_GAP_EVT_DISCONNECTED:
        {}
        break; // BLE_GAP_EVT_DISCONNECTED

      case BLE_GAP_EVT_ADV_REPORT:
        {
          ble_gap_evt_adv_report_t const *pAdv = &p_gap_evt->params.adv_report;
          ScanAdv_CallBack(pAdv);
        }
        break; // BLE_GAP_ADV_REPORT



      default:
        // No implementation needed.
        break;
  }
}
/**@brief Function for checking if a bluetooth stack event is an advertising timeout.
 *
 * @param[in] p_ble_evt Bluetooth stack event.
 */
static bool ble_evt_is_advertising_timeout(ble_evt_t * p_ble_evt)
{
  if((p_ble_evt->header.evt_id == BLE_GAP_EVT_TIMEOUT) && (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISING))
  {
      return true;
  }
  return false;
}
/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t *p_ble_evt)
{

#if 0
//    ble_conn_state_on_ble_evt(p_ble_evt);
  ble_conn_params_on_ble_evt(p_ble_evt);
  nrf_ble_gatt_on_ble_evt(&m_gatt, p_ble_evt);

  app_bas_on_ble_evt(p_ble_evt);
  app_nus_on_ble_evt(p_ble_evt);

  on_ble_evt(p_ble_evt);
  ble_advertising_on_ble_evt(p_ble_evt);

#else
   uint16_t conn_handle;
    uint16_t role;

    ble_conn_state_on_ble_evt(p_ble_evt);
   // pm_on_ble_evt(p_ble_evt);
    nrf_ble_gatt_on_ble_evt(&m_gatt, p_ble_evt);

    // The connection handle should really be retrievable for any event type.
    conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    role        = ble_conn_state_role(conn_handle);

    // Based on the role this device plays in the connection, dispatch to the right modules.
    if (role == BLE_GAP_ROLE_PERIPH || ble_evt_is_advertising_timeout(p_ble_evt))
    {
        // Manages peripheral LEDs.
        ble_conn_params_on_ble_evt(p_ble_evt);
        //nrf_ble_gatt_on_ble_evt(&m_gatt, p_ble_evt);

        app_bas_on_ble_evt(p_ble_evt);
        app_nus_on_ble_evt(p_ble_evt);

        on_ble_evt(p_ble_evt);
        ble_advertising_on_ble_evt(p_ble_evt);

    }
    else if ((role == BLE_GAP_ROLE_CENTRAL) || (p_ble_evt->header.evt_id == BLE_GAP_EVT_ADV_REPORT))
    {
        // on_ble_central_evt() will update the connection handles, so we want to execute it
        // after dispatching to the central applications upon disconnection.
        if (p_ble_evt->header.evt_id != BLE_GAP_EVT_DISCONNECTED)
        {
            on_ble_central_evt(p_ble_evt);
        }

     
        // If the peer disconnected, we update the connection handles last.
        if (p_ble_evt->header.evt_id == BLE_GAP_EVT_DISCONNECTED)
        {
            on_ble_central_evt(p_ble_evt);
        }
    }
#endif



}

/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
  // Dispatch the system event to the fstorage module, where it will be
  // dispatched to the Flash Data Storage (FDS) module.
  fs_sys_event_handler(sys_evt);

  // Dispatch to the Advertising module last, since it will check if there are any
  // pending flash operations in fstorage. Let fstorage process system events first,
  // so that it can report correctly to the Advertising module.
  ble_advertising_on_sys_evt(sys_evt);
}
/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void)
{
  ret_code_t err_code;

  // Initialize the SoftDevice handler module.
  nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

  // Initialize the SoftDevice handler module.
  SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

  // Fetch the start address of the application RAM.
  uint32_t ram_start = 0;
  err_code = softdevice_app_ram_start_get(&ram_start);
  APP_ERROR_CHECK(err_code);

  // Overwrite some of the default configurations for the BLE stack.
  ble_cfg_t ble_cfg;

  // Configure the number of custom UUIDS.
  memset(&ble_cfg, 0, sizeof(ble_cfg));
  ble_cfg.common_cfg.vs_uuid_cfg.vs_uuid_count = 2;
  err_code = sd_ble_cfg_set(BLE_COMMON_CFG_VS_UUID, &ble_cfg, ram_start);
  APP_ERROR_CHECK(err_code);

  // Configure the maximum number of connections.
  memset(&ble_cfg, 0, sizeof(ble_cfg));
  ble_cfg.gap_cfg.role_count_cfg.periph_role_count  = PERIPHERAL_LINK_COUNT;
  ble_cfg.gap_cfg.role_count_cfg.central_role_count = CENTRAL_LINK_COUNT;
  ble_cfg.gap_cfg.role_count_cfg.central_sec_count  = BLE_GAP_ROLE_COUNT_CENTRAL_SEC_DEFAULT;
  err_code = sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &ble_cfg, ram_start);
  APP_ERROR_CHECK(err_code);

  
  // Configure the maximum ATT MTU.
  memset(&ble_cfg, 0x00, sizeof(ble_cfg));
  ble_cfg.conn_cfg.conn_cfg_tag                 = APP_CONN_CFG_TAG;
  ble_cfg.conn_cfg.params.gatt_conn_cfg.att_mtu = NRF_BLE_GATT_MAX_MTU_SIZE;
  err_code = sd_ble_cfg_set(BLE_CONN_CFG_GATT, &ble_cfg, ram_start);
  APP_ERROR_CHECK(err_code);
  
  
  memset(&ble_cfg, 0, sizeof(ble_cfg));
  ble_cfg.conn_cfg.params.gap_conn_cfg.conn_count     = PERIPHERAL_LINK_COUNT + CENTRAL_LINK_COUNT;
  ble_cfg.conn_cfg.params.gap_conn_cfg.event_length   = BLE_GAP_EVENT_LENGTH_DEFAULT;
  ble_cfg.conn_cfg.conn_cfg_tag                       = APP_CONN_CFG_TAG;
  err_code = sd_ble_cfg_set(BLE_CONN_CFG_GAP, &ble_cfg, ram_start);
  APP_ERROR_CHECK(err_code);

  // Enable BLE stack.
  err_code = softdevice_enable(&ram_start);
  APP_ERROR_CHECK(err_code);

  // Register with the SoftDevice handler module for BLE events.
  err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
  APP_ERROR_CHECK(err_code);

  // Register with the SoftDevice handler module for System events.
  err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
  APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
  uint32_t               err_code;
  ble_advdata_t          advdata;
  ble_adv_modes_config_t options;

  // Build advertising data struct to pass into @ref ble_advertising_init.
  memset(&advdata, 0, sizeof(advdata));

  advdata.name_type               = BLE_ADVDATA_FULL_NAME;
  advdata.include_appearance      = true;
  advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  advdata.uuids_complete.p_uuids  = m_adv_uuids;

  memset(&options, 0, sizeof(options));
  options.ble_adv_fast_enabled  = true;
  options.ble_adv_fast_interval = APP_ADV_INTERVAL;
  options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;
  options.ble_adv_slow_enabled  = true;
  options.ble_adv_slow_interval = APP_ADV_INTERVAL_SLOW;
  options.ble_adv_slow_timeout  = APP_ADV_TIMEOUT_IN_SECONDS_SLOW;


  err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
  APP_ERROR_CHECK(err_code);

  ble_advertising_conn_cfg_tag_set(APP_CONN_CFG_TAG);
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
  APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt)
{
//    uint32_t err_code;

  if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
  {
    DEBUG("[BLE]: fail in update conn_params\r\n");
//        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
//        APP_ERROR_CHECK(err_code);
  }
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
  uint32_t               err_code;
  ble_conn_params_init_t cp_init;

  memset(&cp_init, 0, sizeof(cp_init));

  cp_init.p_conn_params                  = NULL;
  cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail             = false;
  cp_init.evt_handler                    = on_conn_params_evt;
  cp_init.error_handler                  = conn_params_error_handler;

  err_code = ble_conn_params_init(&cp_init);
  APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
  uint32_t                err_code;
  ble_gap_conn_params_t   gap_conn_params;
  ble_gap_conn_sec_mode_t sec_mode;

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode,
                                        (const uint8_t *)DEVICE_NAME,
                                        strlen(DEVICE_NAME));
  APP_ERROR_CHECK(err_code);

  memset(&gap_conn_params, 0, sizeof(gap_conn_params));

  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency     = SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

  err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
  APP_ERROR_CHECK(err_code);

  err_code = sd_ble_gap_tx_power_set(0);
  APP_ERROR_CHECK(err_code);
}


void ble_manual_setup_adv(void)
{
  (void)sd_ble_gap_adv_stop();

  APP_ERROR_CHECK(ble_advertising_start(BLE_ADV_MODE_FAST));
}


/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, const nrf_ble_gatt_evt_t *p_evt)
{
  if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
  {
    m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
    NRF_LOG_INFO("Data len is set to 0x%X(%d)\r\n", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
  }
  NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x\r\n", p_gatt->att_mtu_desired_central, p_gatt->att_mtu_desired_periph);
}
/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
  ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, 64);
  APP_ERROR_CHECK(err_code);
}

void ble_stop(void)
{
  uint32_t err_code;

  if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
  {
    err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
    APP_ERROR_CHECK(err_code);
  } else
  {
    (void)sd_ble_gap_adv_stop();
  }
}

/**@brief Function for initiating scanning.
 */
void scan_start(void)
{
  ret_code_t err_code;

  (void)sd_ble_gap_scan_stop();

  err_code = sd_ble_gap_scan_start(&m_scan_params);
  // It is okay to ignore this error since we are stopping the scan anyway.
  if (err_code != NRF_ERROR_INVALID_STATE)
  {
    APP_ERROR_CHECK(err_code);
  }

}


/**@brief Function for initiating scanning.
 */
void scan_stop(void)
{
  (void)sd_ble_gap_scan_stop();
}

static void ble_advdata_update_evt(void)
{
  if (m_user_data.change != 1) return;

  m_user_data.change = 0;

  uint32_t err_code;
  uint8_t adv_data[31];
  uint8_t chunk_len = 0;

  // 3bytes
  adv_data[chunk_len + 0] = 0x02;
  adv_data[chunk_len + 1] = BLE_GAP_AD_TYPE_FLAGS;
  adv_data[chunk_len + 2] = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
  chunk_len += 3;

  // 7 bytes
  uint32_t dev_len = strlen(DEVICE_NAME);
  adv_data[chunk_len + 0] = dev_len + 1;
  adv_data[chunk_len + 1] = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
  memcpy(adv_data + chunk_len + 2, DEVICE_NAME, dev_len);
  chunk_len += dev_len + 2;

  // 15 bytes
  adv_data[chunk_len + 0] = 13;
  adv_data[chunk_len + 1] = BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA;

  adv_data[chunk_len + 2] = (uint8_t)(m_user_data.company_id >> 0);
  adv_data[chunk_len + 3] = (uint8_t)(m_user_data.company_id >> 8);
  adv_data[chunk_len + 4] = m_user_data.version;
  adv_data[chunk_len + 5] = (uint8_t)(m_user_data.module_id >> 0);
  adv_data[chunk_len + 6] = (uint8_t)(m_user_data.module_id >> 8);
  adv_data[chunk_len + 7] = (uint8_t)(m_user_data.module_id >> 16);
  adv_data[chunk_len + 8] = (uint8_t)(m_user_data.module_id >> 24);
  adv_data[chunk_len + 9] = (uint8_t)(m_user_data.battery);
  adv_data[chunk_len + 10] = (uint8_t)(m_user_data.state >> 0);
  adv_data[chunk_len + 11] = (uint8_t)(m_user_data.state >> 8);

  m_user_data.crc = crc16_compute(adv_data + chunk_len + 2, 10, 0);
  adv_data[chunk_len + 12] = (uint8_t)(m_user_data.crc >> 0);
  adv_data[chunk_len + 13] = (uint8_t)(m_user_data.crc >> 8);

  chunk_len += 14;

  err_code = sd_ble_gap_adv_data_set(adv_data, chunk_len, NULL, 0);
  if (err_code != NRF_SUCCESS)
  {
    APP_ERROR_CHECK(err_code);
  }

  //DEBUG("[BLE]: update advdata success. state = 0x%X\r\n", m_user_data.state );
}


// battery update
// flag update
void bl_user_data_change(bl_user_data_type type, uint16_t value, bool enable)
{
  switch (type)
  {
      case BL_ADV_BATTERY:
        if (m_user_data.battery != value)
        {
          m_user_data.battery = (uint8_t)value;
          m_user_data.change = 1;
        }
        break;

      case BL_ADV_STATE:
        if (enable)
        {
          m_user_data.state |= value;
        } else
        {
          m_user_data.state &= (~value);
        }
        m_user_data.change = 1;
        break;
  }
}

static void ble_adv_init(void)
{
  m_user_data.company_id = 0x1234;
  m_user_data.version = 0x00;

  m_user_data.module_id = dev_config_get_module_id();
  m_user_data.battery = 100;
  m_user_data.state = 0;
  if (battery_is_charging())
  {
    m_user_data.state |= BA_BATT_IN_CHARGING;
  }

  m_user_data.change = 1;

  ble_advdata_update_evt();
}


static void bl_evt_on_radio_notification(bool active)
{
  ble_advdata_update_evt();
  app_nus_res_handler();
}

static void bl_radio_init(void)
{
  uint32_t err_code;
  err_code = ble_radio_notification_init(APP_IRQ_PRIORITY_LOW,
                                         NRF_RADIO_NOTIFICATION_DISTANCE_800US,
                                         bl_evt_on_radio_notification);
  if (err_code != NRF_SUCCESS)
  {
    APP_ERROR_CHECK(err_code);
  }
}

void ble_role_init(void)
{
  ble_services_init();
  gap_params_init();
  gatt_init();
  advertising_init();
  conn_params_init();
  ble_adv_init();
  bl_radio_init();
  ScanAdv_Init(SCAN_ADV_TIME, SCAN_ADV_PRIOD, scan_start, scan_stop);
  ScanAdv_Start();
}



#endif // PERIPHERAL_MODE


