
#include <stdint.h>
#include <string.h>
#include "config.h"

#if (FEATURE_BLEC)
#include "Datetime.h"
#include "HalBLEC.h"

#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "ble_nus.h"


#include "app_util_platform.h"
#include "nrf_ble_scan.h"
#include "ble_db_discovery.h"
#include "dev_config.get.h"

#include "ble_nus_c.h"
#include "ble_bas_c.h"
#include "ble_meshtechas_c.h"
#include "nrf_ble_gatt.h"



#define HALBLEC_LOG HZH_LOG




#define ECHOBACK_BLE_UART_DATA  1                                       /**< Echo the UART data that is received over the Nordic UART Service (NUS) back to the sender. */


#define APP_BLE_CONN_CFG_TAG    1
#define NUS_SERVICE_UUID_TYPE   BLE_UUID_TYPE_VENDOR_BEGIN              /**< UUID type for the Nordic UART Service (vendor specific). */


//BLE_NUS_C_DEF(m_ble_nus_c);                                             /**< BLE Nordic UART Service (NUS) client instance. */
BLE_NUS_C_ARRAY_DEF(m_ble_nus_c, NRF_SDH_BLE_CENTRAL_LINK_COUNT);
BLE_BAS_C_ARRAY_DEF(m_bas_c, NRF_SDH_BLE_CENTRAL_LINK_COUNT);                                             /**< Structure used to identify the Battery Service client module. */
BLE_MESHTECHAS_C_ARRAY_DEF(m_ble_meshtechas_c, NRF_SDH_BLE_CENTRAL_LINK_COUNT);
//BLE_DB_DISCOVERY_DEF(m_db_disc);                                        /**< Database discovery module instance. */
BLE_DB_DISCOVERY_ARRAY_DEF(m_db_discovery, NRF_SDH_BLE_CENTRAL_LINK_COUNT);
NRF_BLE_SCAN_DEF(m_scan);                                               /**< Scanning Module instance. */



static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH; /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */



pfnHalBLECNotif_t pfnHalBLECNotif = NULL;


void HalBLEC_RegisterNotif(pfnHalBLECNotif_t notif)
{
    pfnHalBLECNotif = notif;
}

static void HalNotif(HalBLECNotifAct_en act, void *param)
{
    if (pfnHalBLECNotif)
    {
        pfnHalBLECNotif(act, param);
    }
}



/**@brief Function for starting scanning. */
static void scan_start(void)
{
    ret_code_t ret;

    ret = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(ret);

}
/**@brief Function for starting scanning. */
static void scan_stop(void)
{
    nrf_ble_scan_stop();
    // APP_ERROR_CHECK(ret);

}


/**@brief Function for handling Scanning Module events.
 */
static void scan_evt_handler(scan_evt_t const *p_scan_evt)
{
    ret_code_t err_code;
    NRF_LOG_INFO("Evt %d\r\n", p_scan_evt->scan_evt_id);
    switch (p_scan_evt->scan_evt_id)
    {

    case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
        {
            err_code = p_scan_evt->params.connecting_err.err_code;
            APP_ERROR_CHECK(err_code);
        }
        break;

    case NRF_BLE_SCAN_EVT_CONNECTED:
        {
            ble_gap_evt_connected_t const *p_connected =
               p_scan_evt->params.connected.p_connected;
            // Scan is automatically stopped by the connection.
            NRF_LOG_INFO("Connecting to target %02x%02x%02x%02x%02x%02x",
                         p_connected->peer_addr.addr[0],
                         p_connected->peer_addr.addr[1],
                         p_connected->peer_addr.addr[2],
                         p_connected->peer_addr.addr[3],
                         p_connected->peer_addr.addr[4],
                         p_connected->peer_addr.addr[5]
                        );
        }
        break;

    case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
        {
            NRF_LOG_INFO("Scan timed out.");
            scan_start();
        }
        break;

    default:
        break;
    }
}

static ble_gap_scan_params_t m_scan_param =                 /**< Scan parameters requested for scanning and connection. */
{
    .active        = 0x01,
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW,
    .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,
    .timeout       = NRF_BLE_SCAN_SCAN_DURATION,
    .scan_phys     = BLE_GAP_PHY_AUTO,
    .extended      = true,
};

/**@brief Function for initializing the scanning and setting the filters.
 */
static void scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.p_scan_param = &m_scan_param;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling database discovery events.
 *
 * @details This function is a callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function forwards the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t *p_evt)
{
    NRF_LOG_DEBUG("call to ble_lbs_on_db_disc_evt for instance %d and link 0x%x!",
                  p_evt->conn_handle,
                  p_evt->conn_handle);


    ble_meshtechas_c_on_db_disc_evt(&m_ble_meshtechas_c[p_evt->conn_handle], p_evt);

    ble_nus_c_on_db_disc_evt(&m_ble_nus_c[p_evt->conn_handle], p_evt);
    ble_bas_on_db_disc_evt(&m_bas_c[p_evt->conn_handle], p_evt);
}




/**@brief Function for handling characters received by the Nordic UART Service (NUS).
 *
 * @details This function takes a list of characters of length data_len and prints the characters out on UART.
 *          If @ref ECHOBACK_BLE_UART_DATA is set, the data is sent back to sender.
 */
static void ble_nus_chars_received(ble_nus_c_t *p_ble_nus_c, uint8_t *p_data, uint16_t data_len)
{
    HalReceiveData_t rec;
    rec.conn_handle = p_ble_nus_c->conn_handle;
    rec.p_data = p_data;
    rec.data_len = data_len;
    HalNotif(HalBLECNotifAct_ReceiveData, (void *)&rec);
}
/**@brief Function for handling characters received by the Nordic UART Service (NUS).
 *
 * @details This function takes a list of characters of length data_len and prints the characters out on UART.
 *          If @ref ECHOBACK_BLE_UART_DATA is set, the data is sent back to sender.
 */
static void ble_meshtechas_chars_received(ble_meshtechas_c_t *p_ble_meshtechas_c, uint8_t *p_data, uint16_t data_len)
{
    HalReceiveData_t rec;
    rec.conn_handle = p_ble_meshtechas_c->conn_handle;
    rec.p_data = p_data;
    rec.data_len = data_len;
    HalNotif(HalBLECNotifAct_ReceiveData, (void *)&rec);
}

/**@brief Callback handling Nordic UART Service (NUS) client events.
 *
 * @details This function is called to notify the application of NUS client events.
 *
 * @param[in]   p_ble_nus_c   NUS client handle. This identifies the NUS client.
 * @param[in]   p_ble_nus_evt Pointer to the NUS client event.
 */

/**@snippet [Handling events from the ble_nus_c module] */
static void ble_nus_c_evt_handler(ble_nus_c_t *p_ble_nus_c, ble_nus_c_evt_t const *p_ble_nus_evt)
{
    ret_code_t err_code;
    switch (p_ble_nus_evt->evt_type)
    {
    case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
        NRF_LOG_INFO("Discovery complete.");
        err_code = ble_nus_c_handles_assign(p_ble_nus_c, p_ble_nus_evt->conn_handle, &p_ble_nus_evt->handles);
        APP_ERROR_CHECK(err_code);

        err_code = ble_nus_c_tx_notif_enable(p_ble_nus_c);
        APP_ERROR_CHECK(err_code);
        NRF_LOG_INFO("Connected to device with Nordic UART Service.");
        HalNotif(HalBLECNotifAct_Connected, (void *)&p_ble_nus_c->conn_handle);
        break;

    case BLE_NUS_C_EVT_NUS_TX_EVT:
        //AB 10 03 00 13 4C 37 03 03 01 31
        ble_nus_chars_received(p_ble_nus_c, p_ble_nus_evt->p_data, p_ble_nus_evt->data_len);
        break;

    case BLE_NUS_C_EVT_DISCONNECTED:
        NRF_LOG_INFO("Disconnected.");
        break;
    }
}




/**@brief Callback handling Nordic UART Service (NUS) client events.
 *
 * @details This function is called to notify the application of NUS client events.
 *
 * @param[in]   p_ble_meshtechas_c   NUS client handle. This identifies the NUS client.
 * @param[in]   p_ble_meshtechas_evt Pointer to the NUS client event.
 */

/**@snippet [Handling events from the ble_meshtechas_c module] */
static void ble_meshtechas_c_evt_handler(ble_meshtechas_c_t *p_ble_meshtechas_c, ble_meshtechas_c_evt_t const *p_ble_meshtechas_evt)
{
    ret_code_t err_code;
    switch (p_ble_meshtechas_evt->evt_type)
    {
    case BLE_MESHTECHAS_C_EVT_DISCOVERY_COMPLETE:
        HALBLEC_LOG("Discovery complete.");
        err_code = ble_meshtechas_c_handles_assign(p_ble_meshtechas_c, p_ble_meshtechas_c->conn_handle, &p_ble_meshtechas_evt->handles);
        APP_ERROR_CHECK(err_code);

        err_code = ble_meshtechas_c_tx_notif_enable(p_ble_meshtechas_c);
        APP_ERROR_CHECK(err_code);
        HALBLEC_LOG("Connected to device with MeschTech AS Service.");
        HalNotif(HalBLECNotifAct_Connected, (void *)&p_ble_meshtechas_c->conn_handle);
        break;

    case BLE_MESHTECHAS_C_EVT_MESHTECHAS_TX_EVT:
        //AB 10 03 00 13 4C 37 03 03 01 31
        ble_meshtechas_chars_received(p_ble_meshtechas_c, p_ble_meshtechas_evt->p_data, p_ble_meshtechas_evt->data_len);
        break;

    case BLE_MESHTECHAS_C_EVT_DISCONNECTED:
        HALBLEC_LOG("Disconnected.");
        break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void ble_central_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
    ret_code_t            err_code;
    ble_gap_evt_t const *p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_ADV_REPORT:
        HalNotif(HalBLECNotifAct_Adversting, (void *)&p_gap_evt->params.adv_report);
        break;
    case BLE_GAP_EVT_CONNECTED:
        if (p_gap_evt->conn_handle >= NRF_SDH_BLE_CENTRAL_LINK_COUNT)
        {
            break;
        }
        HALBLEC_LOG("Connection 0x%x established, starting DB discovery.", p_gap_evt->conn_handle);

        err_code = ble_nus_c_handles_assign(&m_ble_nus_c[p_ble_evt->evt.gap_evt.conn_handle],
                                            p_ble_evt->evt.gap_evt.conn_handle,
                                            NULL);
        APP_ERROR_CHECK(err_code);

        err_code = ble_meshtechas_c_handles_assign(&m_ble_meshtechas_c[p_ble_evt->evt.gap_evt.conn_handle],
                                                   p_ble_evt->evt.gap_evt.conn_handle,
                                                   NULL);
        APP_ERROR_CHECK(err_code);

        // start discovery of services. The NUS Client waits for a discovery result
        err_code = ble_db_discovery_start(&m_db_discovery[p_ble_evt->evt.gap_evt.conn_handle], p_ble_evt->evt.gap_evt.conn_handle);
        APP_ERROR_CHECK(err_code);


        if (ble_conn_state_central_conn_count() == NRF_SDH_BLE_CENTRAL_LINK_COUNT)
        {
            // bsp_board_led_off(CENTRAL_SCANNING_LED);
        }
        else
        {
            // Resume scanning.
            //    bsp_board_led_on(CENTRAL_SCANNING_LED);
            scan_start();
        }
        break;
    case BLE_GAP_EVT_RSSI_CHANGED:
        HalNotif(HalBLECNotifAct_RssiChanged, (void *)p_gap_evt);
        break;
    case BLE_GAP_EVT_DISCONNECTED:


        HALBLEC_LOG("Disconnected. conn_handle: 0x%x, reason: 0x%x",
                    p_gap_evt->conn_handle,
                    p_gap_evt->params.disconnected.reason);


        HalNotif(HalBLECNotifAct_Disconnected, (void *)p_gap_evt);

        scan_start();
        break;

    case BLE_GAP_EVT_TIMEOUT:
        if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
        {
            HALBLEC_LOG("Connection Request timed out.");
        }
        break;

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        // Pairing not supported.
        err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
        HALBLEC_LOG("BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST.");
        // Accepting parameters requested by peer.
        err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                                &p_gap_evt->params.conn_param_update_request.conn_params);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            HALBLEC_LOG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        }
        break;

    case BLE_GATTC_EVT_TIMEOUT:
        // Disconnect on GATT Client timeout event.
        HALBLEC_LOG("GATT Client Timeout.");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server timeout event.
        HALBLEC_LOG("GATT Server Timeout.");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    default:
        break;
    }
}


/**@brief Function for initializing the Nordic UART Service (NUS) client. */
static void nus_c_init(void)
{
    ret_code_t       err_code;
    ble_nus_c_init_t init;

    init.evt_handler = ble_nus_c_evt_handler;
    for (uint32_t i = 0; i < NRF_SDH_BLE_CENTRAL_LINK_COUNT; i++)
    {
        err_code = ble_nus_c_init(&m_ble_nus_c[i], &init);
        APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for initializing the Nordic UART Service (NUS) client. */
static void meshtechas_c_init(void)
{
    ret_code_t       err_code;
    ble_meshtechas_c_init_t init;

    init.evt_handler = ble_meshtechas_c_evt_handler;
    for (uint32_t i = 0; i < NRF_SDH_BLE_CENTRAL_LINK_COUNT; i++)
    {
        err_code = ble_meshtechas_c_init(&m_ble_meshtechas_c[i], &init);
        APP_ERROR_CHECK(err_code);
    }
}
/**@brief Battery level Collector Handler.
 */
static void bas_c_evt_handler(ble_bas_c_t *p_bas_c, ble_bas_c_evt_t *p_bas_c_evt)
{
    ret_code_t err_code;

    switch (p_bas_c_evt->evt_type)
    {
    case BLE_BAS_C_EVT_DISCOVERY_COMPLETE:
        {
            err_code = ble_bas_c_handles_assign(p_bas_c,
                                                p_bas_c_evt->conn_handle,
                                                &p_bas_c_evt->params.bas_db);
            APP_ERROR_CHECK(err_code);

            // Battery service discovered. Enable notification of Battery Level.
            HALBLEC_LOG("Battery Service discovered. Reading battery level.");

            err_code = ble_bas_c_bl_read(p_bas_c);
            APP_ERROR_CHECK(err_code);

            HALBLEC_LOG("Enabling Battery Level Notification.");
            err_code = ble_bas_c_bl_notif_enable(p_bas_c);
            APP_ERROR_CHECK(err_code);

        }
        break;

    case BLE_BAS_C_EVT_BATT_NOTIFICATION:
        HALBLEC_LOG("Battery Level received %d %%.", p_bas_c_evt->params.battery_level);
        break;

    case BLE_BAS_C_EVT_BATT_READ_RESP:
        HALBLEC_LOG("Battery Level Read as %d %%.", p_bas_c_evt->params.battery_level);
        break;

    default:
        break;
    }
}

/**
 * @brief Battery level collector initialization.
 */
static void bas_c_init(void)
{
    ble_bas_c_init_t bas_c_init_obj;
    ret_code_t err_code;

    bas_c_init_obj.evt_handler = bas_c_evt_handler;
    for (uint32_t i = 0; i < NRF_SDH_BLE_CENTRAL_LINK_COUNT; i++)
    {
        err_code = ble_bas_c_init(&m_bas_c[i], &bas_c_init_obj);
        APP_ERROR_CHECK(err_code);
    }
}


/** @brief Function for initializing the database discovery module. */
static void db_discovery_init(void)
{
    ret_code_t err_code = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(err_code);
}




void HalBLEC_Init()
{
    db_discovery_init();
    //  nus_c_init();
    meshtechas_c_init();
    bas_c_init();
    scan_init();
}




void HalBLEC_ScanStart()
{
    scan_start();
}

void HalBLEC_ScanStop()
{
    scan_stop();
}

#if 0
int HalBLEC_Send(uint16_t conn, uint8_t *data, uint16_t len)
{
    NRF_LOG_INFO("Send  Send............");
    NRF_LOG_HEXDUMP_INFO(data, len);
    uint16_t length = (uint16_t)len;
    return  ble_nus_c_string_send(&m_ble_nus_c[conn], data, length);
}
#endif

int HalBLEC_Send(uint16_t conn, uint8_t *data, uint16_t len)
{
    NRF_LOG_INFO("Send  Send............");
    NRF_LOG_HEXDUMP_INFO(data, len);
    uint16_t length = (uint16_t)len;
    return  ble_meshtechas_c_string_send(&m_ble_meshtechas_c[conn], data, length);
}

int HalBLEC_Connect(uint8_t const  mac[6])
{
    ble_gap_addr_t p_addr;
    p_addr.addr_id_peer = 0;
    p_addr.addr_type = 1;
    memcpy(p_addr.addr, mac, 6);
    uint32_t  err_code = sd_ble_gap_connect(&p_addr,
                                            &m_scan.scan_params,
                                            &m_scan.conn_params,
                                            APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
    return 0;
}

#endif
